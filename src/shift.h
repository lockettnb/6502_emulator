//
// 2014/11/30
// bit manipulation Opcodes

// +++++++++++++++++++++++++++++++++++++++++++++++++++++
     case ASL_AB: // ASL nn
            addr = AB(0);
            db = mread(addr) << 1;
            mwrite(addr, db);
            SETFLAG(C_FLAG, mread(addr) & 0x80);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr) & 0x80);
            cpu.pc+=3;
            break;


     case ASL_ZP: // ASL n
            addr = ZP(0);
            db = mread(addr) << 1;
            mwrite(addr, db);
            SETFLAG(C_FLAG, mread(addr) & 0x80);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr) & 0x80);
            cpu.pc+=2;
            break;

     case ASL_ACC: // ASL A
            cpu.a = cpu.a << 1;
            SETFLAG(C_FLAG, cpu.a & 0x80);
            SETFLAG(Z_FLAG, cpu.a==0 );
            SETFLAG(N_FLAG, cpu.a & 0x80);
            cpu.pc+=1;
            break;

     case ASL_ZPX: // ASL n,X
            addr = ZP(cpu.x);
            db = mread(addr) << 1;
            mwrite(addr, db);
            SETFLAG(C_FLAG, mread(addr) & 0x80);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr) & 0x80);
            cpu.pc+=2;
            break;

     case ASL_ABX: // ASL nn,X
            addr = AB(cpu.x);
            db = mread(addr) << 1;
            mwrite(addr, db);
            SETFLAG(C_FLAG, mread(addr) & 0x80);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr) & 0x80);
            cpu.pc+=3;
            break;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++
     case LSR_AB: // LSR nn
            addr = AB(0);
            SETFLAG(C_FLAG, mread(addr) & 0x01);
            db = mread(addr) >> 1;
            mwrite(addr, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, FALSE);
            cpu.pc+=3;
            break;

     case LSR_ZP: // LSR n
            addr = ZP(0);
            SETFLAG(C_FLAG, mread(addr) & 0x01);
            db = mread(addr) >> 1;
            mwrite(addr, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, FALSE);
            cpu.pc+=2;
            break;

     case LSR_ACC: // LSR A 
            SETFLAG(C_FLAG, cpu.a & 0x01);
            cpu.a = cpu.a >> 1;
            SETFLAG(Z_FLAG, cpu.a==0 );
            SETFLAG(N_FLAG, FALSE);
            cpu.pc+=1;
            break;

     case LSR_ZPX: // LSR n,X
            addr = ZP(cpu.x);
            SETFLAG(C_FLAG, mread(addr) & 0x01);
            db = mread(addr) >> 1;
            mwrite(addr, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, FALSE);
            cpu.pc+=2;
            break;

     case LSR_ABX: // LSR nn,X
            addr = AB(cpu.x);
            SETFLAG(C_FLAG, mread(addr) & 0x01);
            db = mread(addr) >> 1;
            mwrite(addr, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, FALSE);
            cpu.pc+=3;
            break;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++
     case ROL_AB: // ROL nn
            addr = AB(0);
            db = mread(addr) & 0x80;  //save the new carry bit
            mwrite(addr, ((mread(addr) << 1 ) & 0xfe | cpu.p & C_FLAG));
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr)&0x80);
            cpu.pc+=3;
            break;


     case ROL_ZP: // ROL n
            addr = ZP(0);
            db = mread(addr) & 0x80;  //save the new carry bit
            mwrite(addr, ((mread(addr) << 1 ) & 0xfe | cpu.p & C_FLAG));
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr)&0x80);
            cpu.pc+=2;
            break;

     case ROL_ACC: // ROL A
            db = cpu.a & 0x80;  //save the new carry bit
            cpu.a = (cpu.a << 1 ) & 0xfe | cpu.p & C_FLAG;
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, cpu.a==0 );
            SETFLAG(N_FLAG, cpu.a&0x80);
            cpu.pc+=1;
            break;

     case ROL_ZPX: // ROL n,X
            addr = ZP(cpu.x);
            db = mread(addr) & 0x80;  //save the new carry bit
            mwrite(addr, ((mread(addr) << 1 ) & 0xfe | cpu.p & C_FLAG));
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr)&0x80);
            cpu.pc+=2;
            break;

     case ROL_ABX: // ROL nn,X
            addr = AB(cpu.x);
            db = mread(addr) & 0x80;  //save the new carry bit
            mwrite(addr, ((mread(addr) << 1 ) & 0xfe | cpu.p & C_FLAG));
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr)&0x80);
            cpu.pc+=3;
            break;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++
     case ROR_AB: // ROR nn
            addr = AB(0);
            db = mread(addr) & 0x01;  //save the new carry bit
            mwrite(addr, ((mread(addr) >> 1 ) & 0x7f | cpu.p & C_FLAG) << 7);
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr)&0x80);
            cpu.pc+=3;
            break;


     case ROR_ZP: // ROR n
            addr = ZP(0);
            db = mread(addr) & 0x80;  //save the new carry bit
            mwrite(addr, ((mread(addr) >> 1 ) & 0x7f | cpu.p & C_FLAG) << 7);
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr)&0x80);
            cpu.pc+=2;
            break;

     case ROR_ACC: // ROR A
            db = cpu.a & 0x80;  //save the new carry bit
            cpu.a = (cpu.a >> 1 ) & 0x7f | cpu.p & C_FLAG << 7;
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, cpu.a==0 );
            SETFLAG(N_FLAG, cpu.a&0x80);
            cpu.pc+=1;
            break;

     case ROR_ZPX: // ROR n,X
            addr = ZP(cpu.x);
            db = mread(addr) & 0x80;  //save the new carry bit
            mwrite(addr, ((mread(addr) >> 1 ) & 0x7f | cpu.p & C_FLAG) << 7);
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr)&0x80);
            cpu.pc+=2;
            break;

     case ROR_ABX: // ROR nn,X
            addr = AB(cpu.x);
            db = mread(addr) & 0x80;  //save the new carry bit
            mwrite(addr, ((mread(addr) >> 1 ) & 0x7f | cpu.p & C_FLAG) << 7);
            SETFLAG(C_FLAG, db);
            SETFLAG(Z_FLAG, mread(addr)==0 );
            SETFLAG(N_FLAG, mread(addr)&0x80);
            cpu.pc+=3;
            break;


