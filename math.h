//
// 2014/12/03
// 2017/12/19 added bunch of inc and dec and subtract codes
// Math Opcodes

// Overflow flag is set if two signed values result in a value > +127 or < -128
// Overflow occurs if the sign of both inputs is different from
// the sign of the result.  (M^result)&(N^result)&0x80 is nonzero.
//
// Another C formula is !((M^N) & 0x80) && ((M^result) & 0x80)
// This means there is overflow if the inputs do not have different signs and 
// the input sign is different from the output sign



case ADC_IMM: // ADC #n -- immediate
        addr=IMM_DATA;
        cpu.pc+=2;
        goto ADD_WITH_CARRY;

case ADC_AB: //ADC nn -- absolute
        addr = AB(0);
        cpu.pc+=3;
        goto ADD_WITH_CARRY;

case ADC_ZP: //ADC n -- zero page 
        addr = ZP(0);
        cpu.pc+=2;
        goto ADD_WITH_CARRY;

case ADC_INX: //ADC (n,X) -- indexed indirect
        addr = INX;
        cpu.pc+=2;
        goto ADD_WITH_CARRY;

case ADC_INY: //ADC (n),Y -- indirect indexed 
        addr = INY;
        cpu.pc+=2;
        goto ADD_WITH_CARRY;

case ADC_ZPX: //ADC n,X -- zero page X 
        addr = ZP(cpu.x);
        cpu.pc+=2;
        goto ADD_WITH_CARRY;

case ADC_ABX: //ADC nn,X -- absolute X 
        addr = AB(cpu.x);
        cpu.pc+=3;
        goto ADD_WITH_CARRY;

case ADC_ABY: //ADC nn,Y -- absolute Y 
        addr = AB(cpu.y);
        cpu.pc+=3;
        goto ADD_WITH_CARRY;

ADD_WITH_CARRY:
        db = mread(addr);
        dw = cpu.a + db + GETFLAG(C_FLAG);
        SETFLAG(C_FLAG, dw & 0xff00);
        SETFLAG(V_FLAG, (cpu.a^dw)&(db^dw)&0x80);
        cpu.a = dw & 0x00ff;
        SETFLAG(Z_FLAG, cpu.a==0) ;
        SETFLAG(N_FLAG, cpu.a & 0x80);
    break;

// Subtraction
// When the subtraction result is 0 to 255, the carry is set.
// When the subtraction result is less than 0, the carry is cleared.
//  V indicates whether the result of an addition or subraction is outside the range -128 to 127

// The overflow flag indicates when the sign of the number has changed
// improperly. So there is overflow if you add two positive numbers and get a
// negative result. There is also overflow if you add two negative numbers and
// get a positive result. Overflow is set if two inputs with the same sign
// produce a result with a different sign. Otherwise it is clear.
// 
// Suppose you've got a + b = c.
// (a ^ b)  will have a 1 in the sign bit if the signs were different.
// You want to know the opposite of that. So:  ~(a ^ b)  gives a 1 in the
// sign bit if the signs were the same.  
// 
// Supposing a and b had the same sign, you can test c
// against either of them. This time you do want to test for difference, so
// that's just:  a ^ c
// You need the tested bit to be set in both parts of the test, so you can
// combine those with: (~(a ^ b))&(a ^ c)
//
// You only want the sign bit, so mask off that: (~(a ^ b))&(a ^ c)&0x80
// Will evaluate to 0x00 if the overflow flag should be clear and 0x80 if it
// should be set.


case SBC_IMM: // SBC #n -- immediate
        addr=IMM_DATA;
        cpu.pc+=2;
        goto SUBTRACT_WITH_CARRY;

case SBC_AB:
        addr = AB(0);
        cpu.pc+=3;
        goto SUBTRACT_WITH_CARRY;

case SBC_ZP:
        addr = ZP(0);
        cpu.pc+=2;
        goto SUBTRACT_WITH_CARRY;

case SBC_INX:
        addr = INX;
        cpu.pc+=2;
        goto SUBTRACT_WITH_CARRY;

case SBC_INY:
        addr = INY;
        cpu.pc+=2;
        goto SUBTRACT_WITH_CARRY;

case SBC_ZPX:
        addr = ZP(cpu.x);
        cpu.pc+=2;
        goto SUBTRACT_WITH_CARRY;

case SBC_ABX:
        addr = AB(cpu.x);
        cpu.pc+=3;
        goto SUBTRACT_WITH_CARRY;

case SBC_ABY:
        addr = AB(cpu.y);
        cpu.pc+=3;
        goto SUBTRACT_WITH_CARRY;


SUBTRACT_WITH_CARRY: // v flag is still wrong
        db = mread(addr);
        dw = cpu.a + ~db + GETFLAG(C_FLAG) ;
        SETFLAG(C_FLAG, dw & 0xff00);
        SETFLAG(V_FLAG, (cpu.a^dw)&((~db)^dw)&0x80);
        cpu.a = dw & 0x00ff;
        SETFLAG(Z_FLAG, cpu.a==0) ;
        SETFLAG(N_FLAG, cpu.a & 0x80);
        cpu.a = dw & 0x00ff;
        break;

// Decrement Opcodes
case DEC_AB:
        addr = AB(0); 
        cpu.pc+=3;
        db=mread(addr);
        mwrite(addr, --db);
        SETFLAG(Z_FLAG, mread(addr)==0) ;
        SETFLAG(N_FLAG, mread(addr) & 0x80);
    break;

case DEC_ABX:
        addr = AB(cpu.x); 
        cpu.pc+=3;
        db=mread(addr);
        mwrite(addr, --db);
        SETFLAG(Z_FLAG, mread(addr)==0) ;
        SETFLAG(N_FLAG, mread(addr) & 0x80);
    break;

case DEC_ZP:
        addr = ZP(0); 
        cpu.pc+=2;
        db=mread(addr);
        mwrite(addr, --db);
        SETFLAG(Z_FLAG, mread(addr)==0) ;
        SETFLAG(N_FLAG, mread(addr) & 0x80);
    break;

case DEC_ZPX:
        addr = ZP(cpu.x); 
        cpu.pc+=2;
        db=mread(addr);
        mwrite(addr, --db);
        SETFLAG(Z_FLAG, mread(addr)==0) ;
        SETFLAG(N_FLAG, mread(addr) & 0x80);
    break;

case DEX:
        cpu.x--;
        cpu.pc+=1;
        SETFLAG(Z_FLAG, cpu.x==0) ;
        SETFLAG(N_FLAG, cpu.x & 0x80);
    break;

case DEY:
        cpu.y--;
        cpu.pc+=1;
        SETFLAG(Z_FLAG, cpu.y==0) ;
        SETFLAG(N_FLAG, cpu.y & 0x80);
    break;



// Increment Opcodes
case INC_AB:
        addr = AB(0); 
        cpu.pc+=3;
        db=mread(addr);
        mwrite(addr, ++db);
        SETFLAG(Z_FLAG, mread(addr)==0) ;
        SETFLAG(N_FLAG, mread(addr) & 0x80);
    break;

case INC_ABX:
        addr = AB(cpu.x); 
        cpu.pc+=3;
        db=mread(addr);
        mwrite(addr, ++db);
        SETFLAG(Z_FLAG, mread(addr)==0) ;
        SETFLAG(N_FLAG, mread(addr) & 0x80);
    break;

case INC_ZP:
        addr = ZP(0); 
        cpu.pc+=2;
        db=mread(addr);
        mwrite(addr, ++db);
        SETFLAG(Z_FLAG, mread(addr)==0) ;
        SETFLAG(N_FLAG, mread(addr) & 0x80);
    break;

case INC_ZPX:
        addr = ZP(cpu.x); 
        cpu.pc+=2;
        db=mread(addr);
        mwrite(addr, ++db);
        SETFLAG(Z_FLAG, mread(addr)==0) ;
        SETFLAG(N_FLAG, mread(addr) & 0x80);
    break;

case INX:
        cpu.x++;
        cpu.pc+=1;
        SETFLAG(Z_FLAG, cpu.x==0) ;
        SETFLAG(N_FLAG, cpu.x & 0x80);
    break;

case INY:
        cpu.y++;
        cpu.pc+=1;
        SETFLAG(Z_FLAG, cpu.y==0) ;
        SETFLAG(N_FLAG, cpu.y & 0x80);
    break;
