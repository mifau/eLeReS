/*
 This file is part of eLeReS (by Michal Maciakowski - Cyberdrones.com).
 
 Foobar is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 Foobar is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Foobar; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _EEPROM_H_
#define _EEPROM_H_ 

#include "common.h"


#define RF_HEADER_EE_OFFSET		0
#define TX_POWER_EE_OFFSET  	4
#define CARRIER_FREQ_EE_OFFSET	5 

struct Settings
{
	unsigned char RF_HEADER[4];
	unsigned char POWER;
	unsigned long FREQ;
};
struct Settings SETTINGS;





uint8_t GetByteEE(uint16_t offset);
void SetByteEE(uint16_t offset, uint8_t value);
void GetBlockEE(char *destination, uint16_t offset,uint16_t length);
void SetBlockEE(char *source, uint16_t offset,uint16_t length);
long GetLongEE(uint16_t offset);
void SetLongEE(uint16_t offset,long value);

void ReadConfigFromEEPROM();
void SetDefaults();


#ifndef EEMEM
#define EEMEM __attribute__ ((section (".eeprom")))
#endif

#endif
