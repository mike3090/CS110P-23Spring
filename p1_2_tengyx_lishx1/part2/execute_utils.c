#include <stdio.h>
#include <stdlib.h>
#include "execute_utils.h"


/* You may find implementing this function helpful */

/* Signed extension of a number to 32 bits. 
   size is the number of valid bits of field.
   e.g. field=0x8a, size=8, return 0xFFFFFF8a.
        field=0x7a, size=8, return 0x0000007a.
 */
int bitSigner(unsigned int field, unsigned int size)
{
    /* YOUR CODE HERE */
    unsigned int result = 0;
    /* get the first bit of the field. If it's 1 then it's negative, 0 otherwise.*/
    /* actually you could use ` field>>(size-1) ` ... */
    /* unsigned int is_negative = (field&(1<<(size-1)))>>(size-1); */
    unsigned int is_negative = field>>(size-1); 
    if(is_negative==1){
        result = (0xFFFFFFFE)<<(size-1); /* get something like 111...10...00*/
        result |= field; /* replace the 0s with the field */
    }
    else{
        result = field; /* unsigned->signed convertion: no problem */
    }
    return result;
}

/* Return the imm from instruction, for details, please refer to I type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_imm_operand(Instruction instruction)
{
    /* YOUR CODE HERE */
    /* srri, srli, srai only requires 5 bits shamt */
    /* well let's handle these manually */
    /*
    if(instruction.itype.opcode==0x13){
        if(instruction.itype.funct3==0x1 || instruction.itype.funct3==0x5){
            return instruction.itype.imm & 0x1f;
        }
    }
    */
    /* other instructions */
    return bitSigner(instruction.itype.imm,12);
}
/* Return the offset from instruction, for details, please refer to SB type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_branch_offset(Instruction instruction)
{
    /* YOUR CODE HERE */
    return bitSigner((int32_t)((((int32_t)instruction.sbtype.imm7>>6)&0x1)<<12) + (int32_t)(((int32_t)instruction.sbtype.imm5&0x1)<<11) + 
            (int32_t)(((int32_t)instruction.sbtype.imm7&0x3f)<<5) + (int32_t)((((int32_t)instruction.sbtype.imm5>>1)&0xf)<<1) + 0, 13);
            
}

/* Return the offset from instruction, for details, please refer to UJ type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_jump_offset(Instruction instruction)
{
    /* YOUR CODE HERE */
    int i = instruction.ujtype.imm;
    return bitSigner((int32_t)(((i>>19)&0x1)<<20) + (int32_t)((i&0xff)<<12) + (int32_t)(((i>>8)&0x1)<<11) + (int32_t)(((i>>9)&0x3ff)<<1) + 0, 21);
}

/* Return the offset from instruction, for details, please refer to S type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_store_offset(Instruction instruction)
{
    /* YOUR CODE HERE */
    return bitSigner(((int32_t)instruction.stype.imm7<<5)+((int32_t)instruction.stype.imm5), 12);
}
