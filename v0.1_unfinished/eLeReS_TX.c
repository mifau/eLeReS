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

#include "common.h"



uint8_t bind = 0;


void Bind()
{
	unsigned char RF_Header_tmp[4]; 

	Red_LED_ON;
	Green_LED_ON;

	//generate new RF_Header
	srandom(234);
	RF_Header_tmp[0] = random() % 255;
	RF_Header_tmp[1] = random() % 255;
	RF_Header_tmp[2] = random() % 255;
	RF_Header_tmp[3] = random() % 255;



	//save RF_Header to EEPROM
	SetBlockEE((char *)RF_Header_tmp, RF_HEADER_EE_OFFSET, sizeof(RF_Header_tmp));


	rx_reset();

	while(1)
	{
		if (_spi_read(0x0C)==0) // detect the locked module and reboot
        {
			Red_LED_ON;  
			RF22B_init_parameter();
			frequency_configurator(CARRIER_FREQUENCY);
			rx_reset();
			Red_LED_OFF;
        }
		
		
		RF_Tx_Buffer[0] = 'B';
		RF_Tx_Buffer[1] = RF_Header_tmp[0];
		RF_Tx_Buffer[2] = RF_Header_tmp[1];
		RF_Tx_Buffer[3] = RF_Header_tmp[2];
		RF_Tx_Buffer[4] = RF_Header_tmp[3];
		RF_Tx_Buffer[5] = 0x00;
		
		to_tx_mode();

		_delay_ms(1);

		#ifdef FREQUENCY_HOPPING
          	Hopping();
        #endif 

	}
	while(1);
}


void Init()
{
		//LEDs
		DDRC |= (1<<PC3); //Red LED
		DDRB |= (1<<PB5); //Green LED


		DDRD &= ~(1<<PD3);	//PPM IN
		DDRB |= (1<<PB2);	//BUZZER
		DDRB &= ~(1<<PB3);	//Button - original OpenLRS TX button


#ifndef NO_BINDING
		//check BIND button state
		DDRD &= ~(1<<PD3);		
		PORTD |= (1<<PD3);
		_delay_ms(1);
		if (bit_is_clear(PIND,PD3))
			bind = 1;
		else 
			bind = 0;


		if (bind)
		{
			RF_Header[0] = 'B';
			RF_Header[1] = 'I';
			RF_Header[2] = 'N';
			RF_Header[3] = 'D';
		} else {
			GetBlockEE((char *)RF_Header, RF_HEADER_EE_OFFSET, sizeof(RF_Header));
		}
#endif

		transmitted = 0;

		Timer0_Init();

		USART_Init();
	
		PPM_Init();

		RF22B_Init();

		sei();
}




int main()
{
	unsigned long time,old_time; //system timer
	uint8_t i;

	Init();

	//binding
	if (bind) Bind();
	
	rx_reset();

	time = millis();
	old_time = time;


	//ramka inicjalizacyjna - dopoki nie pojawi sie wlasciwy sygnal, to ona wlasnie bedzie wysylana co 20ms
	RF_Tx_Buffer[0] = 'S';	//"S" header for Servo Channel data
	for(i = 0; i<16; i++) RF_Tx_Buffer[i+1] = Servo_Position[i];


//--------------  MAIN LOOP  -------------------------------------------------------------------------------------------
	while(1)
	{
		//time = millis(); 

		 
		if (_spi_read(0x0C)==0) // detect the locked module and reboot
        {
			Red_LED_ON;  
			RF22B_init_parameter();
			frequency_configurator(CARRIER_FREQUENCY);
			rx_reset();
			Red_LED_OFF;
        }

		if (timer0_start_transmission) //wysylamy dokladnie co 20ms
		{
			timer0_start_transmission = 0;

	        Green_LED_ON;	//Green LED will be on during transmission  
						
			if ((transmitted==0) && (channel_count>3) && (channel_count<13)) //aktualizujemy stan pakietu tylko po odebranej ramce PPM
			{	
				RF_Tx_Buffer[0] = 'S';	//"S" header for Servo Channel data
				for(i = 0; i<16; i++) RF_Tx_Buffer[i+1] = Servo_Position[i]; // fill the rf-tx buffer with 8 channel (2x8 byte) servo signal
			    transmitted = 1;
			}
			
			to_tx_mode();
			
			Green_LED_OFF; 

			//Hop to the next frequency
            #ifdef FREQUENCY_HOPPING
				Hopping();
            #endif  

			


		}

	}
//----------------------------------------------------------------------------------------------------------------------





}
