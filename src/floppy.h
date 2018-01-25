//
// 2018/01/20 created
//
#include "hardware.h"


void    floppy_init(void);
uint8_t floppy_read(uint16_t address);
void    floppy_write(uint16_t address, uint8_t data);
void    floppy_halt(void);

// void    floppy_status(void);
void    floppy_status(Pia *a6820);


// 6820 PIA 
// 
// pa0 -- (not) ready drive1
// pa1 -- (not) track 00
// pa3 -- (not) fault
// pa3 -- (not) sector
// pa4 -- (not) ready drive2
// pa5 -- (not) write protect
// pa6 -- (not) something to do with drive select??
// pa7 -- (not) index
//
// pb0 -- (not) write enable
// pb1 -- (not) erase enable
// pb2 -- (not) step in
// pb3 -- (not) step
// pb4 -- (not)fault reset
// pb5 --
// pb6 -- (not) low current
// pb7 -- (not) head load
// 
