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
#ifndef _SERVO_H_
#define _SERVO_H_

#include "common.h"

#define Servo_Ports_LOW PORTB &= 0xE0; PORTD &= 0x17; // pulling down the servo outputs

#define Servo1_OUT_HIGH PORTD |= _BV(PD3) //Servo1
#define Servo2_OUT_HIGH PORTD |= _BV(PD5) //Servo2
#define Servo3_OUT_HIGH PORTD |= _BV(PD6) //Servo3 
#define Servo4_OUT_HIGH PORTD |= _BV(PD7) //Servo4
#define Servo5_OUT_HIGH PORTB |= _BV(PB0) //Servo5
#define Servo6_OUT_HIGH PORTB |= _BV(PB1) //Servo6
#define Servo7_OUT_HIGH PORTB |= _BV(PB2) //Servo7
#define Servo8_OUT_HIGH PORTB |= _BV(PB3) //Servo8
#define Servo9_OUT_HIGH PORTB |= _BV(PB4) //Servo9 

#define Serial_PPM_OUT_HIGH PORTB |= _BV(PB4) //Serial PPM out on Servo 9

extern volatile unsigned int total_ppm_time;
//extern unsigned int Servo_Buffer[10];	//servo position values from RF
extern volatile unsigned int Servo_Position[20];	//real servo position values

void Servo_Init();


#endif
