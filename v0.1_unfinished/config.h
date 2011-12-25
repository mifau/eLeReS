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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "common.h"

#define VERSION "eLeReS 0.1"


#define TX_POWER 0x07   //0 = 1mW, 1 = 1.6mW, 2 = 3mW, 3 = 6mW, 4 = 12.6mW, 5 = 25mW, 6 = 50mW , 7 = 100mW
#define CARRIER_FREQUENCY 435000  	// 435Mhz startup frequency
#define FREQUENCY_HOPPING	 		


//###### HOPPING CHANNELS #######
//Select the hopping channels between 0-255 (435000 .. 450300)
// Default values are 13,54 and 23 for all transmitters and receivers, you should change it before your first flight for safety.
//Frequency = CARRIER_FREQUENCY + (StepSize(40khz)* Channel_Number) 

#define HOP_CHANNELS 		10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68
#define HOP_CHANNELS_COUNT	30


#define DEFAULT_RF_HEADER 'O','L','R','S'   //do not change that!

#define BAUD_RATE_0 57600



#define NO_BINDING


#endif
