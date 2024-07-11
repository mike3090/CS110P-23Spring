#include <stdlib.h>
#include <string.h>
#include "logs.h"
#include "execute_utils.h"

/*  Update:
    It's a better implementation now.
    The previous ugly one is abolished, due to TLE.
*/

/*  Well, I have to admit that this is a very UGLY implementation of logs,
    since it's so inefficient for it's HUUUUUUUGE space complexity.
    Basically, it stores the physical memory (100000 Bytes) for EVERY step.

    A better way to implement this: for every step, record the last instruction,
    and what registers and what parts of memory it used.
    The s-type (sw,sh,sb) changes the memory and you can pay attention here.

    But the ugly way works! For my mental health, let's do it.

    The instructors told us in piazza that the largest testcase 
    covers "thousands of instructions". 
    With the log for each instruction takes ~1Mb, it could be accepted,
    but longer running time is needed.
*/

/*******************************
 * Helper Functions
 *******************************/

/* Creates a new empty Logs and returns a pointer to that Logs. 
   If memory allocation fails, you should call allocation_failed().
 */
Logs *create_logs() {
    /* YOUR CODE HERE */
    Logs* new_log = (Logs*)malloc(sizeof(Logs));
    if(new_log == NULL){
        allocation_failed();
    }
    /* LET'S DO SOMETHING BIGGGGGGGG! */
    new_log->cap = 10000;
    new_log->nodes = (Node*)malloc(sizeof(Node)*(new_log->cap));
    if(new_log->nodes == NULL){
        /* A cute comment! */
        allocation_failed();
    }
    new_log->size=0;
    return new_log;
}

/* Frees the given Logs and all associated memory. */
void free_logs(Logs *logs) {
    /* YOUR CODE HERE */
    if(!logs){
        return;
    }
    /* if nodes needs to be freed */
    if(logs->nodes!=NULL){
        free(logs->nodes);
    }
    free(logs);
    /* A cute comment! */
    return;
}

/* Record the current status and save in a logs component.
   Think about what information needs to be recorded.
   Logs can resize to fit an arbitrary number of entries. 
   (so you should use dynamic memory allocation).
 */
void record(Instruction inst, Processor *cpu, Byte *mem, Logs *logs){
    /* YOUR CODE HERE */
    /* we will update the logs->size at last.*/
    uint32_t addr;
    
    /* error preventing */
    if(!cpu || !mem || !logs){
        return;
    }
    /* if cap is not big enough, but for size 10000 I don't believe it would happen  */
    if((logs->size) >= (logs->cap)){
        logs->cap += (logs->cap); /* multiply by 2*/
        logs->nodes = (Node*)realloc(logs->nodes, sizeof(Node)*(logs->cap));
        /* error preventing */
        if(!(logs->nodes)){
            allocation_failed();
        }
    }
    /* copy PC, stat, regs into our CPU in the nodes */
    logs->nodes[logs->size].cpu.PC = cpu->PC;
    logs->nodes[logs->size].cpu.stat = cpu->stat;
    memcpy(logs->nodes[logs->size].cpu.reg, cpu->reg, 32*sizeof(Word));
    /* by default we set the flag (recording if the memory is changed) to 0 */
    logs->nodes[logs->size].flag = 0;

    /* caution for s-type: sw, sh, sb(which is me). */
    /* nevermind what type it is, since opcode is always at the first 7 bits.*/
    if(inst.stype.opcode==0x23){/* if is s-type */
        /* get the address: where the memory is changed */
        addr = cpu->reg[inst.stype.rs1] + get_store_offset(inst);
        if(inst.stype.funct3==0x0){
            /* sb */
            /* always check alignment, which is shown in execute.c */
            /* if not aligned, the inside instructions wouldn't be executed. */
            if(check_alignment(convert(addr),LENGTH_BYTE)==1){
                logs->nodes[logs->size].current_value = load(mem, addr, LENGTH_BYTE, 1);
                logs->nodes[logs->size].addr = addr;
                logs->nodes[logs->size].align = LENGTH_BYTE;
                /* we have changed the memory */
                logs->nodes[logs->size].flag = 1;
            }
        }
        else if(inst.stype.funct3==0x1){
            /* sh */
            /* always check alignment, which is shown in execute.c */
            /* if not aligned, the inside instructions wouldn't be executed. */
            if(check_alignment(convert(addr),LENGTH_HALF_WORD)==1){
                logs->nodes[logs->size].current_value = load(mem, addr, LENGTH_HALF_WORD, 1);
                logs->nodes[logs->size].addr = addr;
                logs->nodes[logs->size].align = LENGTH_HALF_WORD;
                /* we have changed the memory */
                logs->nodes[logs->size].flag = 1;
            }
        }
        else if(inst.stype.funct3==0x2){
            /* sh */
            /* always check alignment, which is shown in execute.c */
            /* if not aligned, the inside instructions wouldn't be executed. */
            if(check_alignment(convert(addr),LENGTH_WORD)==1){
                logs->nodes[logs->size].current_value = load(mem, addr, LENGTH_WORD, 1);
                logs->nodes[logs->size].addr = addr;
                logs->nodes[logs->size].align = LENGTH_WORD;
                /* we have changed the memory */
                logs->nodes[logs->size].flag = 1;
            }
        }
        /* no need to write the final else, since we don't raise machine_code_error here.*/
    }
    /* finally size++ */
    logs->size++;
    return;
}

/* Back to the previous state recorded in a logs component.
   if success, return 0.
   if fail, for example, no previous state, return -1.
 */
/*  what is "previous state"?
    I suppose: it's the last state. 
    eg: if we have size=3, and now the last one is [2].
    Then the last state is [2]. what we do is to use the content of [2] to 
"*/
int undo(Processor *cpu, Byte *mem, Logs *logs) {
    /* YOUR CODE HERE */
    if(!cpu || !mem || !logs){
        return -1;
    }
    /* no previous state */
    if((logs->size)<=0){
        return -1;
    }
    /* we decrease the size first*/
    logs->size--;
    /* rollback cpu related items */
    cpu->stat = logs->nodes[logs->size].cpu.stat;
    cpu->PC = logs->nodes[logs->size].cpu.PC;
    memcpy(cpu->reg, logs->nodes[logs->size].cpu.reg, 32*sizeof(Word));

    /* change back the memory if we've changed memory before (flag==1) */
    if(logs->nodes[logs->size].flag==1){
        store(mem,logs->nodes[logs->size].addr, logs->nodes[logs->size].align,
                logs->nodes[logs->size].current_value, 1);
    }

    /* no need to free anything since we've malloc-ed a huge place at first! */
    return 0;
   
}

/* Add function definition here if you need */