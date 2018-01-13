6502 EMULATOR
=============

This program provides a 6502 emulation for both a generic 6502 and for
Ohio Scientific C3 computer.  The generic mode has a flat 64k for memory
space with no devices or rom.  A monitor is provided to load object files
into memory and run them.

The monitor also provides:

    + disassemble
    + set/display memory
    + set/display cpu registers
    + single step
    + breakpoints
    + trace of execution

The OSI mode provides a basic OSI Challenger 3 system emulation.
The system has 48k of memory, serial console, and  8" floppy with OS65D.
This is the first check-in of the system, the floppy control and serial
console are not finished..... yet.

2018/01/13 
