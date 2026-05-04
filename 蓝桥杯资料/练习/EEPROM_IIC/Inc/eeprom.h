/* e2prom.h */
 
#ifndef __EEPROM_H
#define __EEPROM_H
 
#include "main.h"
#include "i2c - hal.h"

 
void e2prom_write(unsigned char address, unsigned char info);
unsigned char e2prom_read(unsigned char address);
 
#endif /* __E2PROM_H */
