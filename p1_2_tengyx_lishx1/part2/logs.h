#ifndef LOGS_H
#define LOGS_H

#include "emulator.h"
#include "mem.h"


/* Complete the following definition of Logs and implement the following
   functions. You are free to add structs or functions, but you must keep
   structure Logs. And you cannot change a function that has already been given.
 */

 /* Add structure definition here if you need */
 
/* For how ugly this implement is, you can go logs.c for my complains. */

typedef struct
{
  /* newly defined struct, not in the original part of the template */
  Byte* memory;
  Processor cpu;
  Word current_value;
  /* 1 for if memory is changed, 0 for not */
  int flag;
  Alignment align;
  Address addr;
} Node;

typedef struct
{
    /* YOUR CODE HERE */
    Node* nodes;
    uint32_t size;
    uint32_t cap;
} Logs;

/* IMPLEMENT ME - see documentation in logs.c */
Logs *create_logs();

/* IMPLEMENT ME - see documentation in logs.c */
void free_logs(Logs *logs);

/* IMPLEMENT ME - see documentation in logs.c */
void record(Instruction inst, Processor *cpu, Byte *mem, Logs *logs);

/* IMPLEMENT ME - see documentation in logs.c */
int undo(Processor *cpu, Byte *mem, Logs *logs);

/* Add function declaration here if you need */

#endif