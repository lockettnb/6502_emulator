//
// 2014/11/30
// Math Opcodes

#define STACK 0x0100

      case PHA: //
            mwrite(STACK + cpu.sp--, cpu.a);
            cpu.pc+=1;
            break;

      case PHP: //
            mwrite(STACK + cpu.sp--, cpu.p);
            cpu.pc+=1;
            break;

      case PLA: //
            cpu.a = mread(++cpu.sp + STACK);
            cpu.pc+=1;
            break;

      case PLP: //
            cpu.p = mread(++cpu.sp + STACK);
            cpu.pc+=1;
            break;
