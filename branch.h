//
// 2014/12/02 and  2017/12/31 
// 
// Branch Opcodes

     case BCS_REL: // branch carry set 
                if(GETFLAG(C_FLAG)) {
                    branch_offset = mread(cpu.pc+1);
                    cpu.pc+=2;
                    cpu.pc=cpu.pc+branch_offset;
                } else {
                    cpu.pc+=2;
                }
                break;

     case BCC_REL: // branch carry clear 
                if(!GETFLAG(C_FLAG)) {
                    branch_offset = mread(cpu.pc+1);
                    cpu.pc+=2;
                    cpu.pc=cpu.pc+branch_offset;
                } else {
                    cpu.pc+=2;
                }
                break;

     case BEQ_REL: // branch equal zero 
                if(GETFLAG(Z_FLAG)) {
                    branch_offset = mread(cpu.pc+1);
                    cpu.pc+=2;
                    cpu.pc=cpu.pc+branch_offset;
                } else {
                    cpu.pc+=2;
                }
                break;

     case BNE_REL: // branch not equal zero 
                if(!GETFLAG(Z_FLAG)) {
                    branch_offset = mread(cpu.pc+1);
                    cpu.pc+=2;
                    cpu.pc=cpu.pc+branch_offset;
                } else {
                    cpu.pc+=2;
                }
                break;

     case BMI_REL: // branch minus 
                if(GETFLAG(N_FLAG)) {
                    branch_offset = mread(cpu.pc+1);
                    cpu.pc+=2;
                    cpu.pc=cpu.pc+branch_offset;
                } else {
                    cpu.pc+=2;
                }
                break;

     case BPL_REL: // branch positive 
                if(!GETFLAG(N_FLAG)) {
                    branch_offset = mread(cpu.pc+1);
                    cpu.pc+=2;
                    cpu.pc=cpu.pc+branch_offset;
                } else {
                    cpu.pc+=2;
                }
                break;

     case BVS_REL: // branch overflow set
                if(GETFLAG(V_FLAG)) {
                    branch_offset = mread(cpu.pc+1);
                    cpu.pc+=2;
                    cpu.pc=cpu.pc+branch_offset;
                } else {
                    cpu.pc+=2;
                }
                break;

     case BVC_REL: // branch overflow clear
                if(!GETFLAG(V_FLAG)) {
                    branch_offset = mread(cpu.pc+1);
                    cpu.pc+=2;
                    cpu.pc=cpu.pc+branch_offset;
                } else {
                    cpu.pc+=2;
                }
                break;

// ++++++++++++++++++++++++++++++++++++++++++++++++++
// jump and call opcodes


     case JMP_AB: // jump absolute 
                cpu.pc = AB(0);
                break;

     case JMP_IN: // jump indirect 
                cpu.pc = IN;
                break;

     case JSR_AB: // call -- jump subroutine 
                  //  save PC hi, dec sp, save PC low, dec sp
                mwrite(STACK + cpu.sp--, (cpu.pc)>>8 & 0xff);
                mwrite(STACK + cpu.sp--, cpu.pc & 0xff);
                cpu.pc = AB(0);
                break;

     case RTS: // return from subroutine 
                cpu.pc = mread(++cpu.sp+STACK) + read(++cpu.sp+STACK)<<8 ;
                cpu.pc++; 
                break;

     case RTI: // return from interrupt 
                cpu.p = mread(++cpu.sp+STACK);
                cpu.pc = mread(++cpu.sp+STACK) + read(++cpu.sp+STACK)<<8 ;
                break;

