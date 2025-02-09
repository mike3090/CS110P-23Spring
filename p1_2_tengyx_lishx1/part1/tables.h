/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia 
   to be part of the project in Feb 2023.
*/
#ifndef TABLES_H
#define TABLES_H

#include "../types.h"
#include "../utils.h"

extern const int SYMBOLTBL_NON_UNIQUE;  /* allows duplicate names in table */
extern const int SYMBOLTBL_UNIQUE_NAME; /* duplicate names not allowed */

/* Complete the following definition of SymbolTable and implement the following
   functions. You can use the structures we provide. Or you can use your own 
   structure if you wish. You are free to declare additional structs or functions, 
   but you must build this data structure yourself.
 */

/* STRUCTURES CODE BELOW */
typedef struct
{
    char *name;
    uint32_t addr;
} Symbol;

typedef struct
{
    Symbol *tbl;
    uint32_t len;
    uint32_t cap; /* Why you're existing here? */
    int mode;
} SymbolTable;

/* Helper functions: */
void allocation_failed();

void addr_alignment_incorrect();

void name_already_exists(const char *name);

void write_sym(FILE *output, uint32_t addr, const char *name);

/* IMPLEMENT ME - see documentation in tables.c */
SymbolTable *create_table(int mode);

/* IMPLEMENT ME - see documentation in tables.c */
void free_table(SymbolTable *table);

/* IMPLEMENT ME - see documentation in tables.c */
int add_to_table(SymbolTable *table, const char *name, uint32_t addr, int check_align);

/* IMPLEMENT ME - see documentation in tables.c */
int64_t get_addr_for_symbol(SymbolTable *table, const char *name);

/* IMPLEMENT ME - see documentation in tables.c */
void write_table(SymbolTable *table, FILE *output);

/* DO NOT CHANGE THIS - see documentation in tables.c */
SymbolTable *create_table_from_file(int mode, FILE *symtbl_file);

#endif
