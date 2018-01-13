//
// 2014/11/30
// bit manipulation Opcodes

    case BIT_AB: // BIT nn 
        addr = AB(0);
        db= mread(addr);
        SETFLAG(Z_FLAG, (db&cpu.a)==0 );
        SETFLAG(N_FLAG, db & 0x80);
        SETFLAG(V_FLAG, db & 0x40 );
        cpu.pc+=3;
        break;

    case BIT_ZP: // BIT n 
        addr = ZP(0);
        db = mread(addr);
        SETFLAG(Z_FLAG, (db&cpu.a)==0 );
        SETFLAG(N_FLAG, db & 0x80);
        SETFLAG(V_FLAG, db & 0x40 );
        cpu.pc+=2;
        break;

    case CLC: // Clear Carry
        SETFLAG(C_FLAG, FALSE);
        cpu.pc+=1;
        break;

    case CLD: // Clear Decimal 
        SETFLAG(D_FLAG, FALSE);
        cpu.pc+=1;
        break;

    case CLI: // Clear IRQ Disable
        SETFLAG(I_FLAG, FALSE);
        cpu.pc+=1;
        break;

    case CLV: // Clear Overflow
        SETFLAG(V_FLAG, FALSE);
        cpu.pc+=1;
        break;

    case SEC: // Set Carry
        SETFLAG(C_FLAG, TRUE);
        cpu.pc+=1;
        break;

    case SED: // Set Decimal 
        SETFLAG(D_FLAG, TRUE);
        cpu.pc+=1;
        break;

    case SEI: // Set IRQ Disable
        SETFLAG(I_FLAG, TRUE);
        cpu.pc+=1;
        break;

