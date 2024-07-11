/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia 
   to be part of the project in Feb 2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tables.h"

const int SYMBOLTBL_NON_UNIQUE = 0;
const int SYMBOLTBL_UNIQUE_NAME = 1;

/*******************************
 * Helper Functions
 *******************************/

void addr_alignment_incorrect()
{
    write_to_log("Error: address is not a multiple of 4.\n");
}

void name_already_exists(const char *name)
{
    write_to_log("Error: name '%s' already exists in table.\n", name);
}

void write_sym(FILE *output, uint32_t addr, const char *name)
{
    fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containg 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time.
   If memory allocation fails, you should call allocation_failed().
   Mode will be either SYMBOLTBL_NON_UNIQUE or SYMBOLTBL_UNIQUE_NAME. You will
   need to store this value for use during add_to_table().
 */
SymbolTable *create_table(int mode)
{
    /* YOUR CODE HERE */
    SymbolTable * tmp = (SymbolTable*)malloc(sizeof(SymbolTable));
    if(tmp==NULL){allocation_failed();return NULL;}

    /* Other members of struct could malloc() later ...? */
    tmp->tbl = (Symbol*) malloc(sizeof(Symbol));
    tmp->len=0;
    /* store the Mode value */
    tmp->mode=mode;
    return tmp;

}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable *table)
{   
    /* YOUR CODE HERE */
    uint32_t i;
    Symbol * tmp;
    /* if table is NULL*/
    if(table==NULL){
        return;
    }
    /* free the names */
    for(i=0;i<(table->len);i++){
        /* if len==0 then it would not jump into this loop?*/
        tmp = &(table->tbl)[i];
        free(tmp->name);
    }
    /* no need to write so many comments for this*/
    free(table->tbl);
    free(table);
    table=NULL;
    

}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE. 
   1. ADDR is given as the byte offset from the first instruction. 
   2. The SymbolTable must be able to resize itself as more elements are added. 

   3. Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   4. If ADDR is not word-aligned and check_align==1, you should call 
   addr_alignment_incorrect() and return -1. 

   5. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists 
   in the table, you should call name_already_exists() and return -1. 

   6.If memory allocation fails, you should call allocation_failed(). 

   Otherwise, you should store the symbol name and address and return 0. 
 */
int add_to_table(SymbolTable *table, const char *name, uint32_t addr, int check_align)
{
    /* YOUR CODE HERE */
    /* Some declarations */
    uint32_t i;
    /* char* name_tmp; */
    /* Symbol* sbl_tmp; */

    /* if NULL */
    if(table==NULL){
        return -1;
    }

    /* check if name is duplicated */
    if(table->mode==SYMBOLTBL_UNIQUE_NAME){
        for(i=0;i<table->len;i++){
            if(strcmp(name,(table->tbl)[i].name)==0){ /* if duplicated */
                name_already_exists(name);
                return -1;
            }
        }        
    }
    /* addr check */
    if(addr%4!=0 && check_align==1){addr_alignment_incorrect();return -1;}

    /* resize */
    table->tbl = (Symbol*)realloc(table->tbl,((table->len)+1)*sizeof(Symbol));
    if(table->tbl==NULL){allocation_failed();return -1;}
    
    
    /* name part */
    (table->tbl)[table->len].name = (char*)malloc((strlen(name)+1)*sizeof(char));/* +1 for '\0' */
    if((table->tbl)[table->len].name == NULL){allocation_failed();return -1;}
    strcpy((table->tbl)[table->len].name,name);

    /* addr part */
    (table->tbl)[table->len].addr = addr;

    (table->len)++; /* For simplicity we addup len at the end */
    return 0;
}

/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable *table, const char *name)
{
    /* YOUR CODE HERE */
    uint32_t i;
    if(table==NULL||name==NULL){
        return -1;
    }
    /* search for the symbol*/
    for(i=0;i<table->len;i++){
        if(strcmp(name,(table->tbl)[i].name)==0){ 
            /* No need to explain such easy codes but we're required to write some comments */
            return (table->tbl)[i].addr;
        }
    }
    return -1;
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_sym() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable *table, FILE *output)
{
    /* YOUR CODE HERE */

    /*
        I'm still DONT know what this function is supposed to do.
        Now I'm writing it as:
        Output every Symbol in the SymbolTable.
    */
    uint32_t i;
    if(table==NULL||output==NULL){
        return;
    }
    /* write */
    for(i=0;i<table->len;i++){
        write_sym(output,(table->tbl)[i].addr,(table->tbl)[i].name);
    }
}

/* DO NOT MODIFY THIS LINE BELOW */
/* ===================================================================== */
/* Read the symbol table file written by the `write_table` function, and
   restore the symbol table.
 */
SymbolTable *create_table_from_file(int mode, FILE *file)
{
    SymbolTable *symtbl = create_table(mode);
    char buf[1024];
    while (fgets(buf, sizeof(buf), file))
    {
        char *name, *addr_str, *end_ptr;
        uint32_t addr;
        addr_str = strtok(buf, "\t");
        /* create table based on these tokens */
        addr = strtol(addr_str, &end_ptr, 0);
        name = strtok(NULL, "\n");
        /* add the name to table, one by one */
        add_to_table(symtbl, name, addr, 0);
    }
    return symtbl;
}