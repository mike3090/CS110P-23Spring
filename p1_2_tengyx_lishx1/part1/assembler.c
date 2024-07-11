/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia 
   to be part of the project in Feb 2023.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "tables.h"
#include "translate.h"
#include "translate_utils.h"

#define MAX_ARGS 3
#define BUF_SIZE 1024
const char *IGNORE_CHARS = " \f\n\r\t\v,()";

/*******************************
 * Helper Functions
 *******************************/

/* you should not be calling this function yourself. */
static void raise_label_error(uint32_t input_line, const char *label)
{
    write_to_log("Error - invalid label at line %d: %s\n", input_line, label);
}

/* call this function if more than MAX_ARGS arguments are found while parsing
   arguments.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.

   EXTRA_ARG should contain the first extra argument encountered.
 */
static void raise_extra_argument_error(uint32_t input_line,
                                       const char *extra_arg)
{
    write_to_log("Error - extra argument at line %d: %s\n", input_line,
                 extra_arg);
}

/* You should call this function if write_original_code() or translate_inst()
   returns 0.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.
 */
static void raise_instruction_error(uint32_t input_line, const char *name,
                                    char **args, int num_args)
{

    write_to_log("Error - invalid instruction at line %d: ", input_line);
    log_inst(name, args, num_args);
}

/* Truncates the string at the first occurrence of the '#' character. */
static void skip_comments(char *str)
{
    char *comment_start = strchr(str, '#');
    if (comment_start)
    {
        *comment_start = '\0';
    }
}

/* Reads STR and determines whether it is a label (ends in ':'), and if so,
   whether it is a valid label, and then tries to add it to the symbol table.

   INPUT_LINE is which line of the input file we are currently processing. Note
   that the first line is line 1 and that empty lines are included in this
   count.

   BYTE_OFFSET is the offset of the NEXT instruction (should it exist).

   Four scenarios can happen:
    1. STR is not a label (does not end in ':'). Returns 0.
    2. STR ends in ':', but is not a valid label. Returns -1.
    3a. STR ends in ':' and is a valid label. Addition to symbol table fails.
        Returns -1.
    3b. STR ends in ':' and is a valid label. Addition to symbol table succeeds.
        Returns 1.
 */
static int add_if_label(uint32_t input_line, char *str, uint32_t byte_offset,
                        SymbolTable *symtbl, int check_align)
{
    
    size_t len = strlen(str);
    if (str[len - 1] == ':')
    {
        str[len - 1] = '\0';
        if (is_valid_label(str))
        {
            /* add to table and return number of tokens, otherwise return -1 */
            if (add_to_table(symtbl, str, byte_offset, check_align) == 0)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            /* we raise error if no valid label is found */
            raise_label_error(input_line, str);
            return -1;
        }
    }
    else
    {
        return 0;
    }
}

/*******************************
 * Implement the Following
 *******************************/

/* First pass of the assembler. You should implement pass_two() first.

   This function should read each line, strip all comments, scan for labels,
   and pass instructions to write_original_code(), pass data to write_static_data(). 
   The symbol table and data segment should also been built and written to 
   specified file. The input file may or may not be valid. Here are some guidelines:

   1. Only one of ".data" or ".text" may be present per line. It must be the 
      first token present. Once you see a ".data" or ".text", the rest part
      will be treat as data or text segment until the next ".data" or ".text"
      Appears. Ignore the rest token of this line. Default to text segment 
      if not indicated.
    
   2. For data segment, we only considering ".word", ".half", ".Byte" types.
      These types must be the first token of a line. The rest of the tokens 
      in this line will be stored as variables of that type in the data segment.
      You can assume that these numbers are legal with their types.

   3. Only one label may be present per line. It must be the first token
      present. Once you see a label, regardless of whether it is a valid label
      or invalid label, You can treat the NEXT token in this line as the 
      beginning of an instruction or a static data.

   4. If the first token is not a label, treat it as the name of an
      instruction. DO NOT try to check it is a valid instruction in this pass.
   
   5. Everything after the instruction name in the same line should be treated 
      as arguments to that instruction. If there are more than MAX_ARGS 
      arguments, call raise_extra_argument_error() and pass in the first extra 
      argument. Do not write that instruction to the original file (eg. don't call
      write_original_code())
   
   6. Only one instruction should be present per line. You do not need to do
       anything extra to detect this - it should be handled by guideline 5.
   
   8. A line containing only a label is valid. The address of the label should
      be the address of the next instruction or static data regardless of 
      whether there is a next instruction or static data or not.
   
   9. If an instruction contains an immediate, you should output it AS IS.
   
   10. Comments should always be skipped before any further process.

   11. Note that the starting address of the text segment is defined as TEXT_BASE
   in "../types.", and the starting address of the data segment is defined as 
   DATA_BASE in "../types.".

   12. Think about how you should use write_original_code() and write_static_data().
   
   Just like in pass_two(), if the function encounters an error it should NOT
   exit, but process the entire file and return -1. If no errors were
   encountered, it should return 0.
 */

int pass_one(FILE *input, FILE *original, FILE *symtbl, FILE *data) {
    /* YOUR CODE HERE */
    int errors = 0;
    /* QUESTION: how to decide the mode of the table? */
    SymbolTable * tbl = create_table_from_file(SYMBOLTBL_UNIQUE_NAME, symtbl); 

    /* store the input line */
    char buf[BUF_SIZE];
    /* store the args */
    char* args[MAX_ARGS];
    /* as the name describes! */
    int line_number=0;
    int text_byte_offset = TEXT_BASE;
    int data_byte_offset = DATA_BASE;
    int num_args=0;
    /* int translate_success; Unused*/
    int is_label;
    /* delimiters, or, Fen1 Ge2 Fu2 */
    const char * delim = IGNORE_CHARS;
    /* each string after spliting */
    char * token;
    /* name of a instruction */
    char * name;
    /* 1 if data, 0 if text */
    int is_data = 0;
    /* buffer of .data, in Byte. is BUF_SIZE enough? */
    Byte data_buf[BUF_SIZE]={0};
    /* how many bytes has been stored*/
    int data_buf_len = 0;
    /* corresponds to the data_buf */
    Word tmp_data;
    /* a tmp char* for the strtol() */
    char* tmp_tol;
    /* records how many instructions are written. Use write_original_code() */
    int inst_num;

    /* A Cute Comment */
    while(fgets(buf,sizeof(buf),input)!=NULL){
        line_number++;
        /* skip the comments */
        skip_comments(buf);
        /* detect empty lines */
        if(strlen(buf)==0){continue;}

        /* read the first token */
        token = strtok(buf, delim);
        if(token==NULL){continue;}
        
        /* check if start with .data/.text */
        if(strcmp(token,".data")==0){
            is_data=1;
            
            continue;
        }
        /* start with .text */
        else if(strcmp(token, ".text")==0){
            is_data=0;
            
            continue;
        }
        /* sections */
        else{
            if(is_data){ /* .data */
                /* just let check_align=0, which is, don't check, in the .data section */
                is_label = add_if_label(line_number,token,data_byte_offset,tbl,0);
                if(is_label==0){
                    /* not a label */
                    if(strcmp(token,".byte")==0){
                        while((token = strtok(NULL,delim))!=NULL){
                            
                            /* the result must be legal with their types.*/
                            tmp_data = (Byte) (strtol(token,&tmp_tol,0))&0xff; 
                            /* &0xff is a double error prevention. I believe no need to write it */                            
                            /*printf("reading: %d\n", tmp_data);
                            printf("before:%d ",data_byte_offset);*/
                            data_byte_offset+=1;
                            /*printf("after:%d\n",data_byte_offset);*/
                            data_buf[data_buf_len] = (Byte)tmp_data;
                            data_buf_len++;
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            
                        }
                    }
                    else if(strcmp(token,".half")==0){
                        while((token = strtok(NULL,delim))!=NULL){
                            /* the result must be legal with their types.*/
                            tmp_data = (Half) (strtol(token,&tmp_tol,0))&0xffff;
                            /*printf("reading: %d\n", tmp_data);
                            printf("before:%d ",data_byte_offset);*/
                            data_byte_offset+=2;
                            /*printf("after:%d\n",data_byte_offset);*/
                            
                            data_buf[data_buf_len] = (Byte) (tmp_data&0xff);
                            data_buf_len++;
                            
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* note that Venus don't require memory alignment.
                                So we have to do this check for every byte. */
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            

                            data_buf[data_buf_len]= (Byte) ((tmp_data>>8)&0xff); /* it's >>8, not >>2!!!!*/
                            data_buf_len++;
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }     
                            
                        }
                        
                    }
                    else if(strcmp(token,".word")==0){
                        while((token = strtok(NULL,delim))!=NULL){
                            /* the result must be legal with their types.*/
                            tmp_data = (Word) (strtol(token,&tmp_tol,0))&0xffffffff;
                            /*printf("reading: %d\n", tmp_data);
                            printf("before:%d ",data_byte_offset);*/
                            data_byte_offset+=4;
                            /*printf("after:%d\n",data_byte_offset);*/
                            /* printf("%d %d %d %d",((tmp_data>>24)&0xff),((tmp_data>>16)&0xff),((tmp_data>>8)&0xff),(tmp_data&0xff));*/
                            data_buf[data_buf_len] = (Byte) (tmp_data&0xff);
                            data_buf_len++;
                            
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* note that Venus don't require memory alignment.
                                So we have to do this check for every byte. */
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            

                            data_buf[data_buf_len] = (Byte) ((tmp_data>>8)&0xff); /* it's >>8, not >>2!!!!*/
                            data_buf_len++;
                            
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            

                            data_buf[data_buf_len] = (Byte) ((tmp_data>>16)&0xff); /* it's >>16, not >>4!!!!*/
                            data_buf_len++;
                            
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            
                            
                            data_buf[data_buf_len] = (Byte) ((tmp_data>>24)&0xff); /* it's >>24, not >>6!!!!*/
                            data_buf_len++;
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }    
                            
                        }
                    }
                    /* no other possibilities I guess? (like having some instructions here)*/
                }
                else if(is_label==-1){
                    /* label, but invalid / addition failed*/
                    errors++;
                    continue;
                }
                else{
                    /* is_label == 1: valid label */
                    /* Note: 
                        Although ".byte"/".half"/".word" should be the first token of a line,
                        Still, the label and ".byte"/".half"/".word",
                        could stay in the same line, and we should store the data.

                        For example, this is valid and we should store the data:

                        label_test: .word 3 6 7 8
                        
                    */
                    token=strtok(NULL,delim);
                    if(token==NULL){
                        continue;
                    }
                    if(strcmp(token,".byte")==0){
                        while((token = strtok(NULL,delim))!=NULL){
                            
                            /* the result must be legal with their types.*/
                            tmp_data = (Byte) (strtol(token,&tmp_tol,0))&0xff; 
                            /* &0xff is a double error prevention. I believe no need to write it */                            
                            /*printf("reading: %d\n", tmp_data);
                            printf("before:%d ",data_byte_offset);*/
                            data_byte_offset+=1;
                            /*printf("after:%d\n",data_byte_offset);*/
                            data_buf[data_buf_len] = (Byte)tmp_data;
                            data_buf_len++;
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            
                        }
                    }
                    else if(strcmp(token,".half")==0){
                        while((token = strtok(NULL,delim))!=NULL){
                            /* the result must be legal with their types.*/
                            tmp_data = (Half) (strtol(token,&tmp_tol,0))&0xffff;
                            /*printf("reading: %d\n", tmp_data);
                            printf("before:%d ",data_byte_offset);*/
                            data_byte_offset+=2;
                            /*printf("after:%d\n",data_byte_offset);*/
                            
                            data_buf[data_buf_len] = (Byte) (tmp_data&0xff);
                            data_buf_len++;
                            
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* note that Venus don't require memory alignment.
                                So we have to do this check for every byte. */
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            

                            data_buf[data_buf_len]= (Byte) ((tmp_data>>8)&0xff); /* it's >>8, not >>2!!!!*/
                            data_buf_len++;
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }     
                            
                        }
                        
                    }
                    else if(strcmp(token,".word")==0){
                        while((token = strtok(NULL,delim))!=NULL){
                            /* the result must be legal with their types.*/
                            tmp_data = (Word) (strtol(token,&tmp_tol,0))&0xffffffff;
                            /*printf("reading: %d\n", tmp_data);
                            printf("before:%d ",data_byte_offset);*/
                            data_byte_offset+=4;
                            /*printf("after:%d\n",data_byte_offset);*/
                            /* printf("%d %d %d %d",((tmp_data>>24)&0xff),((tmp_data>>16)&0xff),((tmp_data>>8)&0xff),(tmp_data&0xff));*/
                            data_buf[data_buf_len] = (Byte) (tmp_data&0xff);
                            data_buf_len++;
                            
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* note that Venus don't require memory alignment.
                                So we have to do this check for every byte. */
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            

                            data_buf[data_buf_len] = (Byte) ((tmp_data>>8)&0xff); /* it's >>8, not >>2!!!!*/
                            data_buf_len++;
                            
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            

                            data_buf[data_buf_len] = (Byte) ((tmp_data>>16)&0xff); /* it's >>16, not >>4!!!!*/
                            data_buf_len++;
                            
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }
                            
                            
                            data_buf[data_buf_len] = (Byte) ((tmp_data>>24)&0xff); /* it's >>24, not >>6!!!!*/
                            data_buf_len++;
                            /* if len>=8 (actually when reaches 8),
                                then call write_static_data() to write 4 bytes of data*/
                            /* Update: sorry, len>=4 is enough. Actually 4 bytes is a full line; 
                                so we have to output it. In other words, 4 bytes is 8 digits.*/
                            if(data_buf_len>=4){
                                write_static_data(data, data_buf);
                                data_buf_len-=4;
                            }    
                            
                        }
                    }
                    /* else: after label there are still some content */
                    
                }
            }
            else{
                /* .text */
                /* let check_align = 1 , which is, check, since it's .text section */
                is_label = add_if_label(line_number,token,text_byte_offset,tbl,1);
                if(is_label==0){
                    /* not a label; it's an instruction */
                    /* go out this if-else is enough */
                    ;
                }
                else if(is_label==-1){
                    /* label, but invalid / addition failed*/
                    errors++;
                    continue;
                }
                else{
                    /* is_label == 1: valid label */
                    /* get the instruction name */
                    token = strtok(NULL, delim);
                }
                /* assign the instruction name */
                if(token==NULL){continue;}
                name=token;
                

                /* parsing */
                num_args=0;
                while((token=strtok(NULL,delim))!=NULL){
                    /* A cute comment */
                    num_args++;
                    /* printf("%s ",token); */
                    if(num_args>MAX_ARGS){
                        /* A cute comment */
                        raise_extra_argument_error(line_number, token);
                        errors++;
                        break;
                        /* A cute comment */
                    }
                    args[num_args-1]=token;
                }
                if(num_args>MAX_ARGS){
                    /* this line should not be written */
                    continue;
                }
                /* How many instructions are written */
                inst_num = write_original_code(original, name, args, num_args);
                if(inst_num==0){
                    /* if not written */
                    raise_instruction_error(line_number, name, args, num_args);
                    errors++;
                    continue;
                }

                text_byte_offset += (inst_num*4);

            }
        }
    }
    /* finally write out the rest of data_buf */
    while(data_buf_len>0){
        write_static_data(data, data_buf);
        data_buf_len -= 4;
    }

    /* final steps on table */
    write_table(tbl,symtbl);
    free_table(tbl);
    return (errors>0) ? -1 : 0 ;
}

/* Second pass of the assembler.

   This function should read an original file and the corresponding symbol table
   file, translates it into basic code and machine code. You may assume:
    1. The input file contains no comments
    2. The input file contains no labels
    3. The input file contains at maximum one instruction per line
    4. All instructions have at maximum MAX_ARGS arguments
    5. The symbol table file is well formatted
    6. The symbol table file contains all the symbol required for translation
    7. The line may contain only one "-" which is placeholder you can ignore.
   If an error is reached, DO NOT EXIT the function. Keep translating the rest of
   the document, and at the end, return -1. Return 0 if no errors were encountered. */


int pass_two(FILE *original, FILE *symtbl, FILE *basic, FILE *machine) {

    /* YOUR CODE HERE */
    int errors = 0;
    /* QUESTION: how to decide the mode of the table? */
    SymbolTable * tbl = create_table_from_file(SYMBOLTBL_UNIQUE_NAME, symtbl); 

    /* store the input line */
    char buf[BUF_SIZE];
    /* store the args */
    char* args[MAX_ARGS];
    /* as the name describes! */
    int line_number=0;
    int byte_offset = 0;
    int num_args=0;
    int translate_success;
    /* delimiters, or, Fen1 Ge2 Fu2 */
    const char * delim = IGNORE_CHARS;
    /* each string after spliting */
    char * token;
    /* name of a instruction */
    char * name;


    /* read the each line of the file */
    while(fgets(buf,sizeof(buf),original)!=NULL){
        line_number++;
        /* The first splitting result is the instruction name */
        name = strtok(buf, delim);
        /* a blank line also contribute 1 to the line_number */
        if(name==NULL){continue;}
        /* reading a "-". ignore it. BUT WHAT IS IGNORE? */
        if(strcmp(name, "-")==0){continue;}

        /* split out the args */
        num_args=0;
        while(1){
            token = strtok(NULL,delim);
            if(token!=NULL){
                /* This is a cute comment! */
                args[num_args]=token;
                num_args++;
            }
            else{
                /* break the inner loop */
                goto end_split_args;
            }
        }
        /* jump out of the inner loop */
        end_split_args: ;
        /* translate the instruction*/
        translate_success = translate_inst(basic, machine, name, args, num_args, byte_offset, tbl);
        
        if(translate_success == 0){ /* if error: not -1! it's 0! */
            /* if error */
            raise_instruction_error(line_number, name, args, num_args);
            errors++;
            /* the assembling progress needs to go on */
            continue;
        }
        else{
            byte_offset += translate_success * 4;
        }
    }
    /* end of the outer while loop */
    free_table(tbl);
    return (errors>0) ? -1 : 0 ;
}


/*******************************
 * Do Not Modify Code Below
 *******************************/


/* Runs the two-pass assembler. Most of the actual work is done in pass_one()
   and pass_two().
 */
int assemble(int mode, FILE *input, FILE *data, FILE *symtbl, FILE *orgin, FILE *basic, FILE *text)
{
    int err = 0;

    /* Execute pass one if mode two is not specified */
    if (mode != 2)
    {
        rewind(input);
        if (pass_one(input, orgin, symtbl, data) != 0)
        {
            err = 1;
        }
    }

    /* Execute pass two if mode one is not specified */
    if (mode != 1)
    {
        rewind(orgin);
        rewind(symtbl);
        if (pass_two(orgin, symtbl, basic, text) != 0)
        {
            err = 1;
        }
    }

    /* Error handling */
    if (err) {
        write_to_log("One or more errors encountered during assembly operation.\n");
    } else {
        write_to_log("Assembly operation completed successfully!\n");
    }

    return err;
}