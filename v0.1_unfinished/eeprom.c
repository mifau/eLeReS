/*
 This file is part of eLeReS.
 
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
#include "eeprom.h"

unsigned char EEProm_unsafe_location[2] EEMEM;		//address 0 is unsafe according to Atmel datasheet
unsigned char EEPromArray[E2END-2] EEMEM;    		//aquire all EEPROM space



uint8_t GetByteEE(uint16_t offset)
{
	eeprom_busy_wait();
	uint8_t value = eeprom_read_byte(&EEPromArray[offset]);
	return value;
}


void SetByteEE(uint16_t offset, uint8_t value)
{
	eeprom_busy_wait();
	eeprom_write_byte(&EEPromArray[offset], value);
}


void GetBlockEE(char *destination, uint16_t offset,uint16_t length)
{
	eeprom_busy_wait();
	eeprom_read_block(destination, &EEPromArray[offset], length);
}


void SetBlockEE(char *source, uint16_t offset,uint16_t length)
{
	eeprom_busy_wait();
	eeprom_write_block(source, &EEPromArray[offset], length);
}

long GetLongEE(uint16_t offset)
{
	long value;
	value = (GetByteEE(offset+1)<<8) + GetByteEE(offset);	
	return value;
}

void SetLongEE(uint16_t offset,long value)
{
	char buff[2];	
	memcpy(buff,&value,sizeof(value));
	SetBlockEE(buff,offset,sizeof(value));
}


void ReadConfigFromEEPROM()
{
	GetBlockEE((char *)SETTINGS.RF_HEADER, RF_HEADER_EE_OFFSET, sizeof(SETTINGS.RF_HEADER));
	SETTINGS.POWER = GetByteEE(TX_POWER_EE_OFFSET);
	SETTINGS.FREQ = GetLongEE(CARRIER_FREQ_EE_OFFSET);
}




void SetDefaults()
{
	unsigned char rf_header[4] = {DEFAULT_RF_HEADER};
	
	SETTINGS.RF_HEADER[0] = rf_header[0];
	SETTINGS.RF_HEADER[1] = rf_header[1];
	SETTINGS.RF_HEADER[2] = rf_header[2];
	SETTINGS.RF_HEADER[3] = rf_header[3];
	SetBlockEE((char *)SETTINGS.RF_HEADER, RF_HEADER_EE_OFFSET, sizeof(SETTINGS.RF_HEADER));

	SETTINGS.POWER = TX_POWER;
	SetByteEE(TX_POWER_EE_OFFSET,SETTINGS.POWER);

	SETTINGS.FREQ = CARRIER_FREQUENCY;
	SetLongEE(CARRIER_FREQ_EE_OFFSET,SETTINGS.FREQ);
}
