#include <stdio.h>
#include "execute.h"
#include "execute_utils.h"
#include "mem.h"


/*******************************
 * Helper Functions
 *******************************/

/* Function handle with error machine code. You should skip the error 
   machine code after call this funcion
*/
void raise_machine_code_error(Instruction instruction){
    write_to_log("Invalid Machine Code: 0x%08x\n", instruction.bits);
}

/* Function handle with undefined ecall parameter.
*/
void raise_undefined_ecall_error(Register a0){
    write_to_log("Undefined Ecall Number: %d\n", a0);
}

/* Function to exit current program */
void exit_program(Processor *p) {
    if (p->stat==1) {
        /* set cpu state unwork */
        p->stat=0;
        write_to_log("Exited with error code 0.\n");
    }
    else {
        write_to_log("Error: program already exit.\n");
    }
}

/* 
    In this section you need to complete execute instruction. This should modify 
    the appropriate registers, make any necessary calls to memory, and updatge the program
    counter to refer You will find the functions in execute_utils.h very useful. So it's 
    better to finish execute_utils.c first. The following provides you with a general 
    framework. You can follow this framework or add your own functions, but please 
    don't change declaration or feature of execute(). Case execute() is the entry point
    to the execution phase. The execute_*() is specific implementations for each 
    instruction type. You had better read types.h carefully. Think about the usage 
    of the union defined in types.h
 */

void execute(Instruction inst, Processor *p, Byte *memory)
{
    switch (inst.opcode.opcode)
    { 
    /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
    /* DON'T FORGET TO WRITE BREAK!!!!!! */
    /* Should we always let p->PC+=4 ? */

    /* r-type */
    case 0x33:
        execute_rtype(inst, p);
        p->PC += 4;
        break;
    /* i-type lb, lh, lw */
    case 0x03:
        execute_load(inst, p, memory);
        p->PC += 4;
        break;
    /* i-type */
    case 0x13:
        execute_itype_except_load(inst, p);
        p->PC += 4;
        break;
    /* jalr */
    case 0x67:
        execute_jalr(inst, p);
        break;
    /* ecall */
    case 0x73:
        execute_ecall(p, memory);
        p->PC += 4;
        break;
    /* s-type */
    case 0x23:
        execute_store(inst, p, memory);
        p->PC += 4;
        break;
    /* sb-type brunch: beq, ... */
    case 0x63:
        execute_branch(inst, p);
        break;
    /* auipc. Note that the 2 u-type instructions don't share the same opcode */
    case 0x17:
        execute_utype(inst, p);
        p->PC += 4; /* it's worth keeping this line...? */
        break;
    /* lui */
    case 0x37:
        execute_utype(inst, p);
        p->PC += 4;
        break;
    /* jal */
    case 0x6f:
        execute_jal(inst, p);
        break;

    default: /* undefined opcode */
        raise_machine_code_error(inst);
        p->PC += 4;
        break;
    }
}

void execute_rtype(Instruction inst, Processor *p)
{
    /* YOUR CODE HERE */
    unsigned int rd = inst.rtype.rd;
    unsigned int rs1 = inst.rtype.rs1;
    unsigned int rs2 = inst.rtype.rs2;
    /*  OK I give up.
        Reading the framework provided, now i treat:
        - `Register` type as the VALUE they stored in the register.
          It's actually Word/uint32_t.
          So, ` Register rd ` is not expected,
          and ` Register p->reg[rd] ` is as expected.
          The fun thing here is that, you can MESS UP ALL OF THEM
          since they're all uint32_t.

        Fine.
    */

    /* add, mul, sub */
    /* THESE SHOULD BE TREATED AS SIGNED! */
    if(inst.rtype.funct3==0x0){
        /* add */
        if(inst.rtype.funct7 == 0x00){
            /* We don't need to care about overflow here. */
            /* eg. (3 uint32_t here) 0xfffffffc + 0x8 = 0x4, which is true in C-language and venus. */
            p->reg[rd]= (uint32_t)((int32_t)(p->reg[rs1]) + (int32_t)(p->reg[rs2]));
            return;
        }
        /* mul */
        else if(inst.rtype.funct7 == 0x01){
            /* convert rs1, rs2 to Double */
            /*
            sDouble rs1d = (sDouble) (p->reg[rs1]);
            sDouble rs2d = (sDouble) (p->reg[rs2]);
            sDouble res = rs1d*rs2d;
            */
            
            /* the lower 32 bits is needed */
            /*p->reg[rd] = (Word)(res & 0xffffffff);*/
            p->reg[rd] = (uint32_t)((sDouble)p->reg[rs1] * (sDouble)p->reg[rs2]);
            return;
        }
        /* sub */
        else if(inst.rtype.funct7 == 0x20){
            /* We don't need to care about overflow here. */
            p->reg[rd]= (uint32_t)((int32_t)(p->reg[rs1]) - (int32_t)(p->reg[rs2]));
            return;
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
            return;
        }
        /* A cute comment! */
    }
    /* sll, mulh */
    else if(inst.rtype.funct3==0x1){
        /* sll */
        if(inst.rtype.funct7 == 0x00){
            p->reg[rd]= (p->reg[rs1]) << (p->reg[rs2]);
            return;
        }
        /* mulh */
        else if(inst.rtype.funct7 == 0x01){
            /* the higher 32 bits is needed */
            /*
                It's the higher 32 bits, however, I guess it does not make 2's-complement conversion.

                sDouble rs1d = (sDouble) (p->reg[rs1]);
                sDouble rs2d = (sDouble) (p->reg[rs2]);
                sDouble res = rs1d*rs2d;
                p->reg[rd] = (Word)(((Double)res >> 32) & 0xffffffff);
            */
            
            /* 
                At first, only God and me know why such type conversion is correct;
                Now, only God knows why such type conversion is correct.
            */
            p->reg[rd] = (Word)((Double)((sDouble)(sWord)(p->reg[rs1])*(sDouble)(sWord)(p->reg[rs2]))>>32);
            return;
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
            return;
        }
        /* A cute comment! */
    }
    /* slt */
    else if (inst.rtype.funct3 == 0x2)
    {
        /*  difference between slt and sltu:
            let t1 = 0x80000004, t2 = 0x7ffffffc, 
            then run:
                sltu t3 t1 t2
                slt t4 t1 t2
            the result is:
                t3 = 1,
                t4 = 0.
        */
        if(inst.rtype.funct7 == 0x00){
            /* the registers are already unsigned. */
            p->reg[rd]= (int32_t)(p->reg[rs1])<(int32_t)(p->reg[rs2]) ? 1 : 0 ;
            return;
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
            return;
        }
        
        /* A cute comment! */
    }
    /* sltu */
    else if (inst.rtype.funct3 == 0x3)
    {
        /* see slt. */
        if(inst.rtype.funct7 == 0x00){
            /* the registers are already unsigned. */
            p->reg[rd]= (p->reg[rs1]<p->reg[rs2]) ? 1 : 0 ;
            return;
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
            return;
        }
        /* A cute comment! */
    }
    /* xor, div */
    else if(inst.rtype.funct3==0x4){
        /* xor */
        if(inst.rtype.funct7 == 0x00){
            p->reg[rd]= (p->reg[rs1]) ^ (p->reg[rs2]);
            return;
        }
        /* div */
        else if(inst.rtype.funct7 == 0x01){
            /* should not be divided by 0 */
            if(p->reg[rs2]==0){
                raise_machine_code_error(inst);
                return;
            }
            p->reg[rd]= (uint32_t)((int32_t)(p->reg[rs1]) / (int32_t)(p->reg[rs2]));
            return;
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
            return;
        }
        /* A cute comment! */
    }
    /* srl, sra */
    /*  Note that C-language treat operator ">>" intelligently.
        If the operand is signed, then it will do srA;
        if the operand is unsigned, then it will do srL.
    */
    else if(inst.rtype.funct3==0x5){
        /* srl */
        if(inst.rtype.funct7 == 0x00){
            p->reg[rd]= ((p->reg[rs1]) >> (p->reg[rs2]));
            return;
        }
        /* sra */
        else if(inst.rtype.funct7 == 0x20){
            p->reg[rd]= ((int32_t)(p->reg[rs1]))>>(p->reg[rs2]);
            return;
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
            return;
        }
        /* A cute comment! */
    }
    /* or, rem*/
    else if(inst.rtype.funct3==0x6){
        /* or */
        if(inst.rtype.funct7 == 0x00){
            p->reg[rd]= (p->reg[rs1]) | (p->reg[rs2]);
            return;
        }
        /* rem */
        else if(inst.rtype.funct7 == 0x01){
            /* should not be divide by 0 */
            if(p->reg[rs2]==0){
                raise_machine_code_error(inst);
                return;
            }
            p->reg[rd]= (uint32_t)((int32_t)(p->reg[rs1]) % (int32_t)(p->reg[rs2]));
            return;
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
            return;
        }
        /* A cute comment! */
    }
    /* and */
    else if (inst.rtype.funct3 == 0x7){
        /* funct7!!!!! 7!!!!!! not3!!!!!!!*/
        if(inst.rtype.funct7 == 0x00){
            p->reg[rd]= (p->reg[rs1]) & (p->reg[rs2]);
            return;
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
            return;
        }
        /* A cute comment! */
    }
    /* invalid */
    else{
        raise_machine_code_error(inst);
        return;
    }
}

void execute_itype_except_load(Instruction inst, Processor *p)
{
    /* YOUR CODE HERE */
    int imm = get_imm_operand(inst);
    unsigned int rd = inst.itype.rd;
    unsigned int rs1 = inst.itype.rs1;
    /* registers should be converted into signed when add, sub, mul,
        and convert back to unsigned */
    /* addi */
    if(inst.itype.funct3==0x0){
        p->reg[rd] = (uint32_t)((int32_t)(p->reg[rs1])+imm);
        return;
    }
    /* slli */
    else if(inst.itype.funct3==0x1){
        #if 0
        /* well, let's not check them */
        /* check if imm lies in [0,31] */
        if(imm>=32 || imm<0){
            raise_machine_code_error(inst);
            return;
        }
        
        p->reg[rd] = (p->reg[rs1])<<imm;
        #endif
        /* haha */
        if((imm>>5)==0x00){
            p->reg[rd]=p->reg[rs1]<<(imm&0x1f);
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
        }
        
        return;
    }
    /* slti */
    else if(inst.itype.funct3==0x2){
        /* please note that the registers are already unsigned. */
        p->reg[rd] = ((int32_t)(p->reg[rs1]) < (int32_t)imm)? 1: 0;
        return;
    }
    /* sltiu */
    else if(inst.itype.funct3==0x3){
        /* please note that the registers are already unsigned. */
        p->reg[rd] = (p->reg[rs1] < (uint32_t)imm)? 1: 0;
        /* convert imm to unsigned? */

        return;
    }
    /* xori */
    else if(inst.itype.funct3==0x4){
        p->reg[rd] = (p->reg[rs1])^imm;
        return;
    }
    /* srli, srai */
    else if(inst.itype.funct3==0x5){
        /*  the 30th bit (the last but one, [-2] in python):
            if it's 0: srli;
            if it's 1: srai.
        */
        if(((get_imm_operand(inst))>>5) == 0x20){ /* use r-type to get funct7 section */
            /* srai */
            #if 0
            /* well, let's not check them */
            /* check if shamt lies in [0,31] */
            if((uint32_t)imm>0x71f || (uint32_t)imm<0x700){
                /* range: 0100 0000 0000 (0x700) to 0100 0001 1111(0x71f) */
                raise_machine_code_error(inst);
                return;
            }
            /* if in the range */
            #endif
            /*  imm&0x1f!!!!! not 0xf1!!!! md
            */
            p->reg[rd] = (int32_t)p->reg[rs1]>>(imm & 0x1f);
            return;
        }
        else if(((get_imm_operand(inst))>>5) == 0x00){ 
            /* if((inst.bits>>30)&(0x1)==0) */
            /* srli */
            #if 0
            /* well, let's not check them */
            /* check if shamt lies in [0,31] */
            /* if((uint32_t)imm>31 || (uint32_t)imm<0){ */
            if((uint32_t)imm>31){
                raise_machine_code_error(inst);
                return;
            }
            /* if in the range */
            #endif
            /*  imm&0x1f!!!!! not 0xf1!!!! md
            */
            p->reg[rd] = (p->reg[rs1])>>(imm&0x1f);
            return;
        }
        /* invalid */
        else{
            raise_machine_code_error(inst);
            return;
        }
    }
    /* ori */
    else if(inst.itype.funct3==0x6){
        p->reg[rd] = (p->reg[rs1]) | imm;
        return;
    }
    /* andi */
    else if(inst.itype.funct3==0x7){
        p->reg[rd] = (p->reg[rs1]) & imm;
        return;
    }
    /* invalid */
    else{
        raise_machine_code_error(inst);
        return;
    }
}

/* You may need to use exit_program() */
void execute_ecall(Processor *p, Byte *memory)
{
    /* YOUR CODE HERE */
    /* register a0: x10; a1: x11 */
    uint32_t a0 = 10;
    uint32_t a1 = 11;
    Word value = p->reg[a0];
    Word value_a1 = p->reg[a1]; 
    Word addr_physical_a1;
    /* 1,4,10,11 are in decimal. */
    /* 1: print the value of a1 as an integer. */
    if(value==1){
        /* no need to consider \n here */
        write_to_log("%d", value_a1);
    }
    /* 4: print the string at address a1.*/
    else if(value==4){
        addr_physical_a1 = convert(value_a1);
        while(memory[addr_physical_a1]!=0){
            write_to_log("%c", memory[addr_physical_a1]);
            addr_physical_a1++;
        }
        /* Still need to check if error here! please check */
    }
    /* 10: exit the program */
    else if(value==10){
        exit_program(p);
    }
    /* 11: print value of a1 as a character */
    else if(value==11){
        write_to_log("%c", value_a1);
    }
    /* invalid */
    else{
        raise_undefined_ecall_error(value);
    }
    return;

}

void execute_branch(Instruction inst, Processor *p)
{
    /* YOUR CODE HERE */
    uint32_t rs1 = inst.sbtype.rs1;
    uint32_t rs2 = inst.sbtype.rs2;
    /* beq */
    if(inst.sbtype.funct3==0x0){
        if((int32_t)(p->reg[rs1]) == (int32_t)(p->reg[rs2])){
            p->PC+=(get_branch_offset(inst));/* no need to offset*4 i guess */
        }
        /* DON'T FORGET TO WRITE ELSE! */
        else{
            p->PC+=4;
        }
        return;
    }
    /* bne */
    else if(inst.sbtype.funct3==0x1){
        if((int32_t)(p->reg[rs1]) != (int32_t)(p->reg[rs2])){
            p->PC+=(get_branch_offset(inst));
        }
        /* DON'T FORGET TO WRITE ELSE! */
        else{
            p->PC+=4;
        }
        return;
    }
    /* blt */
    else if(inst.sbtype.funct3==0x4){
        if((int32_t)(p->reg[rs1]) < (int32_t)(p->reg[rs2])){
            p->PC+=(get_branch_offset(inst));
        }
        /* DON'T FORGET TO WRITE ELSE! */
        else{
            p->PC+=4;
        }
        return;
    }
    /* bge */
    else if(inst.sbtype.funct3==0x5){
        if((int32_t)(p->reg[rs1]) >= (int32_t)(p->reg[rs2])){
            p->PC+=(get_branch_offset(inst));
        }
        /* DON'T FORGET TO WRITE ELSE! */
        else{
            p->PC+=4;
        }
        return;
    }
    /* bltu */
    else if(inst.sbtype.funct3==0x6){
        if(p->reg[rs1] < p->reg[rs2]){
            p->PC+=(get_branch_offset(inst));
        }
        /* DON'T FORGET TO WRITE ELSE! */
        else{
            p->PC+=4;
        }
        return;
    }
    /* bgeu */
    else if(inst.sbtype.funct3==0x7){
        if(p->reg[rs1] >= p->reg[rs2]){
            p->PC+=(get_branch_offset(inst));
        }
        /* DON'T FORGET TO WRITE ELSE! */
        else{
            p->PC+=4;
        }
        return;
    }
    /* invalid */
    else{
        p->PC+=4;
        raise_machine_code_error(inst);
        return;
    }
}

void execute_load(Instruction inst, Processor *p, Byte *memory)
{
    /* YOUR CODE HERE */
    uint32_t rd = inst.itype.rd;
    uint32_t rs1 = inst.itype.rs1;
    int offset = get_imm_operand(inst);
    
    /*  Let's imagine, there's:
            .data
            m: .word 0xeeddccbb
        then
            la a2 m
        and the data will be stored as:
            +0  +1  +2  +3
            BB  CC  DD  EE
        So this is what you will get:
            lb a1 0(a2) --> a1 = 0xFFFFFFBB
            lh a1 0(a2) --> a1 = 0xFFFFCCBB
            lw a1 0(a2) --> a1 = 0xEEDDCCBB
            lbu a1 0(a2) -> a1 = 0x000000BB
            lhu a1 0(a2) -> a1 = 0x0000CCBB
        
        And you can see this post!
        https://piazza.com/class/ld018f5gur54h4/post/247
    */
    /* lb */
    if(inst.itype.funct3==0x0){
        p->reg[rd] = (uint32_t)bitSigner(load(memory,p->reg[rs1]+offset,LENGTH_BYTE,1),8);
        /* a cute comment */
        return;
    }
    /* lh */
    else if(inst.itype.funct3==0x1){
        p->reg[rd] = (uint32_t)bitSigner(load(memory,p->reg[rs1]+offset,LENGTH_HALF_WORD,1),16);
        /* a cute comment */
        return;
    }
    /* lw */
    else if(inst.itype.funct3==0x2){
        p->reg[rd] = load(memory,p->reg[rs1]+offset,LENGTH_WORD,1);
        return;
    }
    /* lbu */
    else if(inst.itype.funct3==0x4){
        p->reg[rd] = load(memory,p->reg[rs1]+offset,LENGTH_BYTE,1);
        return;
    }
    /* lhu */
    else if(inst.itype.funct3==0x5){
        p->reg[rd] = load(memory,p->reg[rs1]+offset,LENGTH_HALF_WORD,1);
        return;
    }
    /* invalid */
    else{
        raise_machine_code_error(inst);
        return;
    }
}

void execute_store(Instruction inst, Processor *p, Byte *memory)
{
    /* YOUR CODE HERE */
    uint32_t rs1 = inst.stype.rs1;
    uint32_t rs2 = inst.stype.rs2;
    int offset = get_store_offset(inst);
    /*  Try understand following codes,
        and understand the result:
            .data
            m: .half 0xAABB
            .text
            la a2 m
            li a1 0x11223344
            sb a1 2(a2)
            li a1 0xEEEEEEEE
            sw a1 3(a2)
            li a1 0x55667788
            sh a1 7(a2)
        Then the (virtual) memory:
                        +0  +1  +2  +3
            0x10000008: 77            
            0x10000004: EE  EE  EE  88
            0x10000000: AA  BB  44  EE
        
        However we require alignment this time!

        Or you can see this post!
        https://piazza.com/class/ld018f5gur54h4/post/247
    */
    /* sb */
    if(inst.stype.funct3==0x0){
        store(memory, p->reg[rs1]+offset, LENGTH_BYTE, p->reg[rs2], 1);
        return;
    }
    /* sh */
    else if(inst.stype.funct3==0x1){
        store(memory, p->reg[rs1]+offset, LENGTH_HALF_WORD, p->reg[rs2], 1);
        return;
    }
    /* sw */
    else if(inst.stype.funct3==0x2){
        store(memory, p->reg[rs1]+offset, LENGTH_WORD, p->reg[rs2], 1);
        return;
    }
    /* invalid */
    else{
        raise_machine_code_error(inst);
        return;
    }

}

void execute_jal(Instruction inst, Processor *p)
{
    /* YOUR CODE HERE */
    /* Should we consider invalid here? */
    uint32_t rd = inst.ujtype.rd;
    p->reg[rd] = p->PC+4;
    /* No need to offset<<1 */
    p->PC = (uint32_t)((int32_t)p->PC + (get_jump_offset(inst)));
    /* A cute comment here! */
    return;
}

void execute_jalr(Instruction inst, Processor *p)
{
    /* YOUR CODE HERE */
    uint32_t rd = inst.itype.rd;
    uint32_t rs1 = inst.itype.rs1;
    /* int! int! int! int! NEVER uint!*/
    int imm = get_imm_operand(inst);
    /* valid */
    if(inst.itype.funct3 == 0x0){
        p->reg[rd] = p->PC + 4;
        /* do type conversion here! */
        p->PC = (uint32_t)((int32_t)(p->reg[rs1]) + imm);
    }
    /* invalid */
    else{
        p->PC+=4;
        raise_machine_code_error(inst);
    }
    /* A cute comment here! */
    return;
}

void execute_utype(Instruction inst, Processor *p)
{
    /* YOUR CODE HERE */
    uint32_t rd = inst.utype.rd;
    unsigned int offset = inst.utype.imm;
    /* auipc */
    if(inst.utype.opcode==0x17){
        p->reg[rd] = (uint32_t)((int32_t)p->PC + (offset<<12));
        return;
    }
    /* lui */
    else if(inst.utype.opcode==0x37){
        p->reg[rd] = (offset << 12);
        return;
    }
    /* invalid */
    else{
        raise_machine_code_error(inst);
        return;
    }
}