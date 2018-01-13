//
// 2014/11/30
// Compare Opcodes

     case CMP_IMM: // CMP #n -- immediate
                db = mread(IMM_DATA);
                SETFLAG(Z_FLAG, cpu.a == db) ;
                SETFLAG(N_FLAG, (cpu.a-db) & 0x80);
                SETFLAG(C_FLAG, cpu.a >= db);
                cpu.pc+=2;
                break;

     case CMP_AB: //CMP nn -- absolute
                addr = AB(0); 
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.a == db) ;
                SETFLAG(N_FLAG, (cpu.a-db) & 0x80);
                SETFLAG(C_FLAG, cpu.a >= db);
                cpu.pc+=3;
                break;

     case CMP_ZP: //CMP n -- zero page 
                addr = ZP(0);
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.a == db) ;
                SETFLAG(N_FLAG, (cpu.a-db) & 0x80);
                SETFLAG(C_FLAG, cpu.a >= db);
                cpu.pc+=2;
                break;

     case CMP_INX: //CMP (n,X) -- indexed indirect
                addr = INX; 
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.a == db) ;
                SETFLAG(N_FLAG, (cpu.a-db) & 0x80);
                SETFLAG(C_FLAG, cpu.a >= db);
                cpu.pc+=2;
                break;

     case CMP_INY: //CMP (n),Y -- indirect indexed 
                addr = INY; 
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.a == db) ;
                SETFLAG(N_FLAG, (cpu.a-db) & 0x80);
                SETFLAG(C_FLAG, cpu.a >= db);
                cpu.pc+=2;
                break;

     case CMP_ZPX: //CMP n,X -- zero page X 
                addr = ZP(cpu.x); 
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.a == db) ;
                SETFLAG(N_FLAG, (cpu.a-db) & 0x80);
                SETFLAG(C_FLAG, cpu.a >= db);
                cpu.pc+=2;
                break;

     case CMP_ABX: //CMP nn,X -- absolute X 
                addr = AB(cpu.x) ;
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.a == db) ;
                SETFLAG(N_FLAG, (cpu.a-db) & 0x80);
                SETFLAG(C_FLAG, cpu.a >= db);
                cpu.pc+=3;
                break;

     case CMP_ABY: //CMP nn,Y -- absolute Y 
                addr = AB(cpu.y); 
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.a == db) ;
                SETFLAG(N_FLAG, (cpu.a-db) & 0x80);
                SETFLAG(C_FLAG, cpu.a >= db);
                cpu.pc+=3;
                break;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     case CPX_IMM: // CPX #n -- immediate
                db = mread(IMM_DATA);
                SETFLAG(Z_FLAG, cpu.x == db) ;
                SETFLAG(N_FLAG, (cpu.x-db) & 0x80);
                SETFLAG(C_FLAG, cpu.x >= db);
                cpu.pc+=2;
                break;

     case CPX_AB: //CPX nn -- absolute
                addr = AB(0); 
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.x == db) ;
                SETFLAG(N_FLAG, (cpu.x-db) & 0x80);
                SETFLAG(C_FLAG, cpu.x >= db);
                cpu.pc+=3;
                break;

     case CPX_ZP: //CPX n -- zero page 
                addr = ZP(0);
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.x == db) ;
                SETFLAG(N_FLAG, (cpu.x-db) & 0x80);
                SETFLAG(C_FLAG, cpu.x >= db);
                cpu.pc+=2;
                break;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     case CPY_IMM: // CPY #n -- immediate
                db = mread(IMM_DATA);
                SETFLAG(Z_FLAG, cpu.y == db) ;
                SETFLAG(N_FLAG, (cpu.y-db) & 0x80);
                SETFLAG(C_FLAG, cpu.y >= db);
                cpu.pc+=2;
                break;

     case CPY_AB: //CPY nn -- absolute
                addr = AB(0); 
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.y == db) ;
                SETFLAG(N_FLAG, (cpu.y-db) & 0x80);
                SETFLAG(C_FLAG, cpu.y >= db);
                cpu.pc+=3;
                break;

     case CPY_ZP: //CPY n -- zero page 
                addr = ZP(0);
                db = mread(addr);
                SETFLAG(Z_FLAG, cpu.y == db) ;
                SETFLAG(N_FLAG, (cpu.y-db) & 0x80);
                SETFLAG(C_FLAG, cpu.y >= db);
                cpu.pc+=2;
                break;

