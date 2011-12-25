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

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <math.h>
#include <avr/wdt.h>

#include "config.h"
#include "eeprom.h"
#include "rfm22b.h"
#include "timer.h"
#include "servo.h"
#include "ppm.h"
#include "uart.h"
#include "service_mode_rx.h"
#include "base64.h"

#define byte uint8_t
#define sbi(port,pin)   port |=   _BV(pin)	//High
#define cbi(port,pin)   port &= ~(_BV(pin)) //Low

#define SOFT_RESET { asm("cli"); asm("jmp 0"); } 


extern unsigned char RF_Header[4];  
extern unsigned char hop_list[HOP_CHANNELS_COUNT];



#define DATA_PACKAGE_SIZE 17
unsigned char RF_Tx_Buffer[DATA_PACKAGE_SIZE]; 
unsigned char RF_Rx_Buffer[DATA_PACKAGE_SIZE]; 


#define Red_LED_ON  PORTC |= _BV(PC3);
#define Red_LED_OFF  PORTC &= ~_BV(PC3);

#define Green_LED_ON  PORTB |= _BV(PB5);
#define Green_LED_OFF  PORTB &= ~_BV(PB5);


volatile unsigned int transmitted;






#endif
