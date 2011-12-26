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

#ifndef UART_H
#define UART_H

#include "common.h"

#define UART_CMD_EEPROM_DATA	'w'
#define UART_CMD_EEPROM_END		'e'
#define UART_CMD_EEPROM_REQ		'r'
#define UART_CMD_ERROR			'x'
#define UART_CMD_OK				's'
#define UART_CMD_PING			'p'
#define UART_CMD_VERSION		'i'
#define UART_CMD_DEBUG_MESSAGE	'm'	
#define UART_CMD_RSSI			'a'
#define UART_CMD_DEFAULTS		'D'
#define UART_EOT				'#'




#define UART_buffer_size 		200
extern volatile char UART0_SendBuffer[UART_buffer_size];
extern volatile uint8_t UART0_wyslano;

volatile char UART_command[UART_buffer_size];
volatile uint8_t UART_command_type;
volatile uint8_t UART_command_size;
volatile uint8_t UART_lock;

void USART_Init();
void USART_Transmit( unsigned char data );
void USART_Transmit_String(char *str);
void USART_Transmit_String2(char *str);
void USART_Msg(char *text);	


#endif
