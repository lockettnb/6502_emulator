//
// 2014/11/28
// LDA -- Load Accumulator


     case LDA_IMM: //LDA #n -- immediate
                cpu.a = mread(IMM_DATA);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case LDA_AB: //LDA nn -- absolute
                addr = AB(0); 
                cpu.a = mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

     case LDA_ZP: //LDA n -- zero page 
                addr = ZP(0);
                cpu.a = mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case LDA_INX: //LDA (n,X) -- indexed indirect
                addr = INX; 
                cpu.a = mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case LDA_INY: //LDA (n),Y -- indirect indexed 
                addr = INY; 
                cpu.a = mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case LDA_ZPX: //LDA n,X -- zero page X 
                addr = ZP(cpu.x); 
                cpu.a = mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case LDA_ABX: //LDA nn,X -- absolute X 
                addr = AB(cpu.x) ;
                cpu.a = mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

     case LDA_ABY: //LDA nn,Y -- absolute Y 
                addr = AB(cpu.y); 
                cpu.a = mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// LDX -- Load X register

     case LDX_IMM: //LDX #n -- immediate
                cpu.x = mread(IMM_DATA); 
                SETFLAG(Z_FLAG, cpu.x==0) ;
                SETFLAG(N_FLAG, cpu.x & 0x80);
                cpu.pc+=2;
                break;

     case LDX_AB: //LDX nn -- absolute
                addr = AB(0); 
                cpu.x = mread(addr);
                SETFLAG(Z_FLAG, cpu.x==0) ;
                SETFLAG(N_FLAG, cpu.x & 0x80);
                cpu.pc+=3;
                break;

     case LDX_ZP: //LDX n -- zero page 
                addr = ZP(0); 
                cpu.x = mread(addr);
                SETFLAG(Z_FLAG, cpu.x==0) ;
                SETFLAG(N_FLAG, cpu.x & 0x80);
                cpu.pc+=2;
                break;

     case LDX_ABY: //LDX nn,Y -- absolute Y 
                addr = AB(cpu.y);
                cpu.x = mread(addr);
                SETFLAG(Z_FLAG, cpu.x==0) ;
                SETFLAG(N_FLAG, cpu.x & 0x80);
                cpu.pc+=3;
                break;

     case LDX_ZPY: //LDX n,Y -- zero page Y 
                addr = ZP(cpu.y); 
                cpu.x = mread(addr);
                SETFLAG(Z_FLAG, cpu.x==0) ;
                SETFLAG(N_FLAG, cpu.x & 0x80);
                cpu.pc+=2;
                break;


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// LDY -- Load Y register

     case LDY_IMM: //LDY #n -- immediate
                cpu.y = mread(IMM_DATA); 
                SETFLAG(Z_FLAG, cpu.y==0) ;
                SETFLAG(N_FLAG, cpu.y & 0x80);
                cpu.pc+=2;
                break;

     case LDY_AB: //LDY nn -- absolute
                addr = AB(0); 
                cpu.y = mread(addr);
                SETFLAG(Z_FLAG, cpu.y==0) ;
                SETFLAG(N_FLAG, cpu.y & 0x80);
                cpu.pc+=3;
                break;

     case LDY_ZP: //LDY n -- zero page 
                addr = ZP(0); 
                cpu.y = mread(addr);
                SETFLAG(Z_FLAG, cpu.y==0) ;
                SETFLAG(N_FLAG, cpu.y & 0x80);
                cpu.pc+=2;
                break;

     case LDY_ABX: //LDY nn,Y -- absolute X 
                addr = AB(cpu.x) ;
                cpu.y = mread(addr);
                SETFLAG(Z_FLAG, cpu.y==0) ;
                SETFLAG(N_FLAG, cpu.y & 0x80);
                cpu.pc+=3;
                break;

     case LDY_ZPX: //LDY n,Y -- zero page X 
                addr = ZP(cpu.x); 
                cpu.y = mread(addr);
                SETFLAG(Z_FLAG, cpu.y==0) ;
                SETFLAG(N_FLAG, cpu.y & 0x80);
                cpu.pc+=2;
                break;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Store Accumulator

     case STA_AB: //STA nn -- absolute
                addr = AB(0); 
                mwrite(addr, cpu.a);
                cpu.pc+=3;
                break;

     case STA_ZP: //STA n -- zero page 
                addr = ZP(0);
                mwrite(addr, cpu.a);
                cpu.pc+=2;
                break;

     case STA_INX: //STA (n,X) -- indexed indirect
                addr = INX; 
                mwrite(addr, cpu.a);
                cpu.pc+=2;
                break;

     case STA_INY: //STA (n),Y -- indirect indexed 
                addr = INY; 
                mwrite(addr, cpu.a);
                cpu.pc+=2;
                break;

     case STA_ZPX: //STA n,X -- zero page X 
                addr = ZP(cpu.x); 
                cpu.pc+=2;
                break;

     case STA_ABX: //STA nn,X -- absolute X 
                addr = AB(cpu.x) ;
                mwrite(addr, cpu.a);
                cpu.pc+=3;
                break;

     case STA_ABY: //STA nn,Y -- absolute Y 
                addr = AB(cpu.y); 
                mwrite(addr, cpu.a);
                cpu.pc+=3;
                break;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Store Register X 

     case STX_AB: //STX nn -- absolute
                addr = AB(0); 
                mwrite(addr, cpu.x);
                cpu.pc+=3;
                break;

     case STX_ZP: //STX n -- zero page 
                addr = ZP(0);
                mwrite(addr, cpu.x);
                cpu.pc+=2;
                break;

     case STX_ZPY: //STX n,Y -- zero page Y 
                addr = ZP(cpu.y) ;
                mwrite(addr, cpu.x);
                cpu.pc+=2;
                break;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Store Register X 

     case STY_AB: //STY nn -- absolute
                addr = AB(0); 
                mwrite(addr, cpu.y);
                cpu.pc+=3;
                break;

     case STY_ZP: //STY n -- zero page 
                addr = ZP(0);
                mwrite(addr, cpu.y);
                cpu.pc+=2;
                break;

     case STY_ZPX: //STY n,X -- zero page X 
                addr = ZP(cpu.x) ;
                mwrite(addr, cpu.y);
                cpu.pc+=2;
                break;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Transfer Instructions 
     case TAX:
                cpu.x = cpu.a;
                SETFLAG(Z_FLAG, cpu.x==0) ;
                SETFLAG(N_FLAG, cpu.x & 0x80);
                cpu.pc+=1;
                break;
     case TAY:
                cpu.y = cpu.a;
                SETFLAG(Z_FLAG, cpu.y==0) ;
                SETFLAG(N_FLAG, cpu.y & 0x80);
                cpu.pc+=1;
                break;
     case TSX:
                cpu.x = cpu.sp;
                SETFLAG(Z_FLAG, cpu.x==0) ;
                SETFLAG(N_FLAG, cpu.x & 0x80);
                cpu.pc+=1;
                break;
     case TXA:
                cpu.a = cpu.x;
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=1;
                break;
     case TXS: 
                cpu.sp = cpu.x;
                cpu.pc+=1;
                break;
     case TYA: 
                cpu.a = cpu.y;
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=1;
                break;
