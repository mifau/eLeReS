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

#include "uart.h"


volatile uint8_t UART_licznik;
volatile char UART0_SendBuffer[UART_buffer_size];
volatile uint8_t UART0_wyslano = 1;


ISR(USART_RX_vect)
{

	uint8_t znak = UDR0;
	
	sei();

	if (UART_lock==1) return; //nie odczytujemy gdy nie zdarzylismy jeszcze przetworzyc danych

	if (UART_licznik==0) //przyszla komenda sterujaca
	{

		UART_command_type = znak;	
		UART_licznik++;
	}
	else	//pozostala czesc transmisji
	{

		if (znak==UART_EOT) //zakonczenie transmisji

		{

			UART_command_size = UART_licznik-1; //nie liczmy znaku typu, ani znaku konca transmisji

			UART_licznik = 0;

			UART_lock = 1;

		}

		else //srodek transmisji

		{

			UART_command[UART_licznik-1] = znak;

			UART_licznik++;

		}

	}

}



ISR(USART_TX_vect)
{
	 static unsigned int ptr = 0;
	 unsigned char tmp_tx;
	 
	 sei();

	 if(!UART0_wyslano) //jesli jeszcze nie wyslano
	  {
	   ptr++;                    
	   tmp_tx = UART0_SendBuffer[ptr];
	   if((tmp_tx == UART_EOT) || (ptr == UART_buffer_size))
	    {
	     ptr = 0;
	     UART0_wyslano = 1;
	    }
	   UDR0 = tmp_tx;
	  }
	  else ptr = 0;
}






void USART_Init()
{
	UART_licznik = 0;
	UART_lock = 0;
	UART_command_size = 0;

	UCSR0B=(1 << TXEN0) | (1 << RXEN0);
    UCSR0A   |= (1<<U2X0); // UART Double Speed (U2X)
	
	UCSR0B |= (1<<RXCIE0); // przerwanie RX
	UCSR0B |= (1<<TXCIE0); //przerwanie TX

	UBRR0L=(F_CPU / (BAUD_RATE_0 * 8L) - 1); //ustalenie predkosci

}



void USART_Transmit( unsigned char data )
{
	//if (data == '\n') USART_Transmit('\r');
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = data; 
}

void USART_Transmit_String(char *str)
{
	for (uint8_t i=0;i<strlen(str);i++) USART_Transmit(str[i]);
}



//send string using interrupt
void USART_Transmit_String2(char *str)
{
	int len = strlen(str);
	memcpy((char *)UART0_SendBuffer,str,len);
	UART0_SendBuffer[len] = UART_EOT;
	UART0_wyslano = 0;
	UDR0 = UART0_SendBuffer[0];
}

//debug message using interrupt
void USART_Msg(char *text)
{
	if (!UART0_wyslano) return; //do not try to send if usart is still busy sending recent packet

	int len = strlen(text);
	UART0_SendBuffer[0] = UART_CMD_DEBUG_MESSAGE;
	memcpy((char *)UART0_SendBuffer+1,text,len);
	UART0_SendBuffer[len+1] = UART_EOT;
	UART0_wyslano = 0;
	UDR0 = UART0_SendBuffer[0];
}

