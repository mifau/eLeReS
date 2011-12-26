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

#include "bind.h"

uint8_t bind = 0;
unsigned char RF_Header_tmp[4]; 



void Bind_RX_Init()
{
	//check BIND button state
	DDRB &= ~(1<<PB3);		
	PORTB |= (1<<PB3);
	_delay_ms(1);
	if (bit_is_clear(PINB,PB3))
	{
		_delay_ms(1000);
		if (bit_is_clear(PINB,PB3)) bind = 1;

	} else bind = 0;

	if (bind)
	{
		RF_Header[0] = 'B';
		RF_Header[1] = 'I';
		RF_Header[2] = 'N';
		RF_Header[3] = 'D';
	} else {
		RF_Header[0] = SETTINGS.RF_HEADER[0];
		RF_Header[1] = SETTINGS.RF_HEADER[1];
		RF_Header[2] = SETTINGS.RF_HEADER[2];
		RF_Header[3] = SETTINGS.RF_HEADER[3];
	}
	Bind_Channels(RF_Header);
}

uint8_t Bind_RX()
{
	unsigned char RF_Header_tmp[4]; 
	static unsigned char RF_Header_tmp_old[4] = {0,0,0,0};
	static uint8_t RF_Header_OK_count = 0;
	static uint8_t RF_Header_STEP = 0;


	Green_LED_ON;
	Red_LED_ON;

	if (RF_Rx_Buffer[0] == 'B') // servo control data
	{	
		RF_Header_tmp[0] = RF_Rx_Buffer[1];
		RF_Header_tmp[1] = RF_Rx_Buffer[2];
		RF_Header_tmp[2] = RF_Rx_Buffer[3];
		RF_Header_tmp[3] = RF_Rx_Buffer[4];
		
		if (RF_Header_STEP>0)
		{
				if (RF_Header_tmp[0]==RF_Header_tmp_old[0]) RF_Header_OK_count ++; else {RF_Header_OK_count = 0; RF_Header_STEP = 0;}
				if (RF_Header_tmp[1]==RF_Header_tmp_old[1]) RF_Header_OK_count ++; else {RF_Header_OK_count = 0; RF_Header_STEP = 0;}
				if (RF_Header_tmp[2]==RF_Header_tmp_old[2]) RF_Header_OK_count ++; else {RF_Header_OK_count = 0; RF_Header_STEP = 0;}
				if (RF_Header_tmp[3]==RF_Header_tmp_old[3]) RF_Header_OK_count ++; else {RF_Header_OK_count = 0; RF_Header_STEP = 0;}
		}
		RF_Header_STEP = 1;

		RF_Header_tmp_old[0] = RF_Header_tmp[0];
		RF_Header_tmp_old[1] = RF_Header_tmp[1];
		RF_Header_tmp_old[2] = RF_Header_tmp[2];
		RF_Header_tmp_old[3] = RF_Header_tmp[3];

		//if enough good binding packets arrived then write new RF_Header to EEPROM
		if (RF_Header_OK_count>200)
		{
			Red_LED_OFF;
			Green_LED_ON;
			SetBlockEE((char *)RF_Header_tmp, RF_HEADER_EE_OFFSET, sizeof(RF_Header_tmp));
			return 1;
		}
	}

	return 0;

}

void Bind_TX_Init()
{
		//check BIND button state
		DDRB &= ~(1<<PB3);		
		PORTB |= (1<<PB3);
		_delay_ms(1);
		if (bit_is_clear(PINB,PB3))
		{
			_delay_ms(1000);
			if (bit_is_clear(PINB,PB3)) bind = 1;

		} else bind = 0;

		if (bind)
		{
			RF_Header[0] = 'B';
			RF_Header[1] = 'I';
			RF_Header[2] = 'N';
			RF_Header[3] = 'D';
		} else {
			GetBlockEE((char *)RF_Header, RF_HEADER_EE_OFFSET, sizeof(RF_Header));
		}
		Bind_Channels(RF_Header);
}

void Bind_TX()
{
	

	Red_LED_ON;
	Green_LED_ON;

	while(bit_is_clear(PIND,PD5)); //wait till button is released
	
	Red_LED_OFF;

	//generate new RF_Header
	srand(millis());
	RF_Header_tmp[0] = rand() % 255;
	RF_Header_tmp[1] = rand() % 255;
	RF_Header_tmp[2] = rand() % 255;
	RF_Header_tmp[3] = rand() % 255;



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

}


//check if channel is already assigned, then choose another
unsigned char CheckChannel(unsigned char channel)
{
	unsigned char new_channel;
	uint8_t count = 0;
	unsigned char high = 0;

	for (int i = 0;i<HOP_CHANNELS_COUNT;i++)
	{
		if (high<hop_list[i]) high = hop_list[i];
		if (channel==hop_list[i])count++;	
	}
	if (count>0) new_channel = high + 2;
	else new_channel = channel;

	return new_channel%255;
}

//assign channels by binding information
void Bind_Channels(unsigned char* RF_HEAD)
{
	unsigned char A = RF_HEAD[0]%128;
	unsigned char B = RF_HEAD[1]%128;
	unsigned char C = RF_HEAD[2]%128;
	unsigned char D = RF_HEAD[3]%128;
	int i;

	for (i = 0;i<HOP_CHANNELS_COUNT;i++) hop_list[i] = 0;	

	hop_list[0]  = CheckChannel(A);
	hop_list[1]  = CheckChannel(B);
	hop_list[2]  = CheckChannel(C);
	hop_list[3]  = CheckChannel(D);
	hop_list[4]  = CheckChannel(A/2);
	hop_list[5]  = CheckChannel(B/2);
	hop_list[6]  = CheckChannel(C/2);
	hop_list[7]  = CheckChannel(D/2);
	hop_list[8]  = CheckChannel(A/3);
	hop_list[9]  = CheckChannel(B/3);
	hop_list[10] = CheckChannel(C/3);
	hop_list[11] = CheckChannel(D/3);
	hop_list[12] = CheckChannel(A/5);
	hop_list[13] = CheckChannel(B/5);
	hop_list[14] = CheckChannel(C/5);
	hop_list[15] = CheckChannel(D/5);
}


