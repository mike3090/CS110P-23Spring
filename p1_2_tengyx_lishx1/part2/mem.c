#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mem.h"


/*******************************
 * Helper Functions
 *******************************/

/* Call this function in the following cases:
        1. Attempts to access a memory location that it is not allowed to access.
        2. Attempts to access a memory location in a way that is not allowed.
 */
static void raise_segmentation_fault(Address addr) {
    write_to_log("Error - segmentation fault at address %08x\n", addr);
}

/* Call this function if a data access is performed to an address 
   that is not aligned for the size of the access.
 */
static void raise_alignment_fault(Address addr) {
    write_to_log("Error - alignment fault at address %08x\n", addr);
}

/* This function is helpful to your alignment check.
 */
int check_alignment(Address address, Alignment alignment) {
    if (alignment == LENGTH_BYTE)
    {
        return 1;
    }
    /* HALF_WORD: The last bit is not used. */
    else if (alignment == LENGTH_HALF_WORD)
    {
        return address % 2 == 0;
    }
    /* WORD: The last two bits are not used. */
    else if (alignment == LENGTH_WORD)
    {
        return address % 4 == 0;
    }
    else {
        return 0;
    }
}

/*  @note The conversion function, convert virtual memory address to physical memory address.
    Conversion rule:

    Virtual Memory <-> Physical Memory

    Stack:
    ( STACK_BASE <-> MEMORY_SIZE-4; )
    0x 7FFF FFFC <-> 0x F FFFC; // Actually 0x7FFFFFFC == 0x80000000 - 4, 0xFFFFC == 0x100000 - 0x4;
    ( STACK_BASE-0x20000 <-> MEMORY_TEXT-0x20000 == MEMORY_DATA )
    0x 7FF8 0000 <-> 0x 8 0000; // Actually 0x7FFDFFFC == 0x7FFFFFFC - 0x20000;
    
    Data:
    ( Which is easy to understand )
    0x 1000 0000 <-> 0x 4 0000;
    0x 1004 0000 <-> 0x 8 0000;

    Text:
    ( Which is easy to understand )
    0x 0000 0000 <-> 0x 0 0000;
    0x 0004 0000 <-> 0x 4 0000;

    With the edge:
    0x80000: give to stack;
    0x40000: give to data.

    And anything beyond these ranges(virtual memory) should be treated as segmentation fault.

    @input: the virtual memory address.
    @param: 0x100001, if seg fault; 
                (This 0x100001 address is beyond the physical memory's range, and it's signed, unlike -1)
            the address on physical memory, if not seg fault.
*/
Address convert(Address virtual){
    /* stack */
    if((uint32_t)0x7FF80000 <= virtual && virtual <= (uint32_t)0x7FFFFFFC){
        return virtual-(uint32_t)0x7FF00000;
    }
    /* data */
    else if((uint32_t)0x10000000 <= virtual && virtual < (uint32_t)0x10040000){
        return virtual-(uint32_t)0x0FFC0000;
    }
    /* text */
    /* else if(0x00000000 <= virtual && virtual < 0x00040000){ */
    /*  Since virtual is unsigned, virtual>=0x00000000 is always true,
        which would CAUSE WARNINGS->ERRORS! F**K YOU -Werror
    */
    else if(virtual < (uint32_t)0x00040000){
        return virtual;
    }
    /* seg fault */
    else{
        return (uint32_t)0x00100001;
    }
}

/* To store the data in the corresponding address in memory, 
   please note the following points:
        1. When check_align==1, check if the memory address is aligned
         with the data type. You need to use raise_alignment_fault().
        2. Think about when you need to use raise_segmentation_fault().
        3. Please perform an address conversion before actually store to memory.
 */
/*
    memory: eg. emulator->memory;
    address: virtual address (before mapping to physical memory);
    check_align: 1/0;
    
    what we actually do here is: memory[f(address)]=value,
    where f() is the conversion/mapping function.
*/
void store(Byte *memory, Address address, Alignment alignment, Word value,
           int check_align)
{
    /* YOUR CODE HERE */
    Address physical;
    Word* word_pos;
    Half* half_pos;
    Byte* byte_pos;
    Byte * tmp;

    /* conversion */
    physical = convert(address);
    if(physical == 0x00100001){
        /* if seg fault. Why choose this magic number: see in the function convert(). */
        raise_segmentation_fault(address);
        return;
    }
    /*  WTF...
        We shouldn't store anything in the text section.
        Text section is read-only.
        If we try to store anything in the text section,
        raise segmentation fault.
    */
    /* else if(0x0<=physical && physical < 0x40000){ */
    else if(physical < 0x40000){
        raise_segmentation_fault(address);
        return;
    }
    /* correct */
    else{
        /* check alignment; no need to say anything here*/
        if(check_align==1){
            if(!check_alignment(physical, alignment)){
                raise_alignment_fault(physical);
                return;
            }
        }
        /*  note that the memory is an array of Byte-s, 
            and we're storing a Word.
            So we need to split the word into 4 bits.
            
            Note the order here. (little endian maybe? nevermind)
            eg: .data 
                .word 0x12345678
            then in the data section:
            0x10000000: 78
            0x10000001: 56
            0x10000002: 34
            0x10000003: 12

            in the stack/text section it's similar.
        */

        /* word */
        if(alignment == LENGTH_WORD){
            ;
        }
        /* half */
        else if(alignment == LENGTH_HALF_WORD){
            value&=0xffff;
        }
        /* byte */
        else{
            value&=0xff;
        }

        #if 0
        /* +0 */
        memory[physical] = (Byte)((value)&0xff);
        /* +1 */
        memory[physical+1] = (Byte)((value>>8)&0xff);
        /* +2 */
        memory[physical+2] = (Byte)((value>>16)&0xff);
        /* +3 */
        memory[physical+3] = (Byte)((value>>24)&0xff);
        #endif

        /* let's use something new */
        tmp = memory + physical;
        /* word */
        if(alignment == LENGTH_WORD){
            word_pos = (Word*)tmp;
            *word_pos = value;
        }
        /* half */
        else if(alignment == LENGTH_HALF_WORD){
            half_pos = (Half*)tmp;
            *half_pos = (Half)value;
        }
        /* byte */
        else{
            byte_pos= (Byte*)tmp;
            *byte_pos = (Byte)value;
        }

        return;
    }
}

/* To load the data in the corresponding address in memory, 
   please note the following points:
        1. When check_align==1, check if the memory address is aligned
         with the data type. You need to use raise_alignment_fault().
        2. Think about when you need to use raise_segmentation_fault().
        3. Please perform an address conversion before actually load from memory.
 */
Word load(Byte *memory, Address address, Alignment alignment, int check_align)
{
    /* YOUR CODE HERE */
    Address physical;
    Word* word_pos;
    Half* half_pos;
    Byte* byte_pos;
    Byte * tmp;

    /* conversion */
    physical = convert(address);
    if(physical == 0x00100001){
        /* if seg fault. Why choose this magic number: see in the function convert(). */
        raise_segmentation_fault(address);
        return 0;
    }
    else{
        /* check alignment; no need to say anything here*/
        if(check_align==1){
            if(!check_alignment(physical, alignment)){
                raise_alignment_fault(physical);
                return 0;
            }
        }
        /*  Note the order here. It's little endian.
            eg: .data 
                .word 0x12345678
            then in the data section:
            0x10000000: 78
            0x10000001: 56
            0x10000002: 34
            0x10000003: 12

            in the stack/text section it's similar.
        */
        #if 0
        /* word */
        if(alignment == LENGTH_WORD){
            return ((Word)(memory[physical+3])<<24)+((Word)(memory[physical+2])<<16)+
                    ((Word)(memory[physical+1])<<8)+(Word)(memory[physical]);
        }
        /* half */
        else if(alignment == LENGTH_HALF_WORD){
            return ((Word)(memory[physical+1])<<8)+(Word)(memory[physical]);
        }
        /* byte */
        else{
            return (Word)(memory[physical]);
        }
        #endif
        tmp = memory + physical;
        /* word */
        if(alignment == LENGTH_WORD){
            word_pos = (Word*)tmp;
            return (Word)(*word_pos);
        }
        /* half */
        else if(alignment == LENGTH_HALF_WORD){
            half_pos = (Half*)tmp;
            return (Word)(*half_pos);
        }
        /* byte */
        else{
            byte_pos = (Byte*)tmp;
            return (Word)(*byte_pos);
        }
    }
}