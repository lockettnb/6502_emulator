//
// 2014/11/30
// Logical Opcodes

     case AND_IMM: //AND #n -- immediate
                cpu.a = cpu.a & IMM_DATA;
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case AND_AB: //AND nn -- absolute
                addr = AB(0); 
                cpu.a = cpu.a & mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

     case AND_ZP: //AND n -- zero page 
                addr = ZP(0);
                cpu.a = cpu.a & mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case AND_IDX: //AND (n,X) -- indexed indirect
                addr = IDX; 
                cpu.a = cpu.a & mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case AND_IDY: //AND (n),Y -- indirect indexed 
                addr = IDY; 
                cpu.a = cpu.a & mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case AND_ZPX: //AND n,X -- zero page X 
                addr = ZP(cpu.x); 
                cpu.a = cpu.a & mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case AND_ABX: //AND nn,X -- absolute X 
                addr = AB(cpu.x) ;
                cpu.a = cpu.a & mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

     case AND_ABY: //AND nn,Y -- absolute Y 
                addr = AB(cpu.y); 
                cpu.a = cpu.a & mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++
     case EOR_IMM: //EOR #n -- immediate
                cpu.a = cpu.a ^ IMM_DATA;
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case EOR_AB: //EOR nn -- absolute
                addr = AB(0); 
                cpu.a = cpu.a ^ mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

     case EOR_ZP: //EOR n -- zero page 
                addr = ZP(0);
                cpu.a = cpu.a ^ mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case EOR_IDX: //EOR (n,X) -- indexed indirect
                addr = IDX; 
                cpu.a = cpu.a ^ mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case EOR_IDY: //EOR (n),Y -- indirect indexed 
                addr = IDY; 
                cpu.a = cpu.a ^ mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case EOR_ZPX: //EOR n,X -- zero page X 
                addr = ZP(cpu.x); 
                cpu.a = cpu.a ^ mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case EOR_ABX: //EOR nn,X -- absolute X 
                addr = AB(cpu.x) ;
                cpu.a = cpu.a ^ mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

     case EOR_ABY: //EOR nn,Y -- absolute Y 
                addr = AB(cpu.y); 
                cpu.a = cpu.a ^ mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;


// +++++++++++++++++++++++++++++++++++++++++++++++++++++
     case ORA_IMM: //ORA #n -- immediate
                cpu.a = cpu.a | IMM_DATA;
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case ORA_AB: //ORA nn -- absolute
                addr = AB(0); 
                cpu.a = cpu.a | mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

     case ORA_ZP: //ORA n -- zero page 
                addr = ZP(0);
                cpu.a = cpu.a | mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case ORA_IDX: //ORA (n,X) -- indexed indirect
                addr = IDX; 
                cpu.a = cpu.a | mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case ORA_IDY: //ORA (n),Y -- indirect indexed 
                addr = IDY; 
                cpu.a = cpu.a | mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case ORA_ZPX: //ORA n,X -- zero page X 
                addr = ZP(cpu.x); 
                cpu.a = cpu.a | mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=2;
                break;

     case ORA_ABX: //ORA nn,X -- absolute X 
                addr = AB(cpu.x) ;
                cpu.a = cpu.a | mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

     case ORA_ABY: //ORA nn,Y -- absolute Y 
                addr = AB(cpu.y); 
                cpu.a = cpu.a | mread(addr);
                SETFLAG(Z_FLAG, cpu.a==0) ;
                SETFLAG(N_FLAG, cpu.a & 0x80);
                cpu.pc+=3;
                break;

