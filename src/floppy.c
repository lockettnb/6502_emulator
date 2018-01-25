//
// 2018/01/20 created
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "floppy.h"

#define LOG(fmt, ...)  printf(fmt, ##__VA_ARGS__ )


//******************************************************************
//
//  floppy device Routines
//
void floppy_init(void) {
    LOG("floppy init\n");
}
uint8_t floppy_read(uint16_t address) {
    LOG("floppy read\n");
    return (0);
}
void floppy_write(uint16_t address, uint8_t byte) {
    LOG("floppy write\n");
}
void floppy_halt(void) {
    LOG("floppy halt\n");
}

