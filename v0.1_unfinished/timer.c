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

#include "timer.h"

volatile unsigned long timer0_ms = 0;
volatile uint8_t timer0_start_transmission = 0;
volatile uint8_t timer0_20ms = 0;
volatile unsigned long timer0_packets = 0;

SIGNAL(TIMER0_OVF_vect)
{
	TCNT0=6;
	timer0_ms++;
	

	//wskaznik rozpoczecia transmisji modulu RF
	if (timer0_start_transmission) timer0_20ms = 0;
	else timer0_20ms++;

	if (timer0_20ms>19)
	{
		timer0_20ms = 0;
		timer0_start_transmission = 1;
		timer0_packets++; //ilosc potencjalnie wyslanych przez TX pakietow
	}

}




void Timer0_Init()
{
	TCCR0A = 0;
	TCCR0B = (1<<CS01)|(1<<CS00); //prescaler 64
	TCNT0 = 6;
	TIMSK0 |= (1<<TOIE0);
}


unsigned long millis()
{
	unsigned long m;
	//uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	//cli();
	m = timer0_ms;
	//SREG = oldSREG;

	return m;
}
