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

#include "common.h"

	unsigned int temp_int;
	unsigned long time;
	unsigned long last_pack_time ;
	unsigned long last_hopping_time;
	
	unsigned short last_ok_channel = 1;


void Init()
{
		ReadConfigFromEEPROM();

		//LEDs
		DDRC |= (1<<PC3); //Red LED
		DDRB |= (1<<PB5); //Green LED


		//check for service mode
		CheckServiceMode();


#ifndef NO_BINDING
		Bind_RX_Init();
#endif
		


		Red_LED_ON;

		Timer0_Init();

		USART_Init();

		RF22B_Init();
		
		Servo_Init();

		to_rx_mode();

		sei();

}






//---------- IF SIGNAL LOST --------------------------
void SignalLostProcedure()
{
#ifdef FREQUENCY_HOPPING
		static uint8_t czekamy = 0;
		int ile;

		if (time-last_pack_time > 3500) //>No singal for 3,5 sek
		{					
			if (!czekamy)
			{
					ile = (time-(last_pack_time-1))/20; //how many channels tx changed since last recieved packet
					ChannelHoppingFrom(last_ok_channel+1,ile); //4 channels ahead (to get rid of time sync errors)
					last_hopping_time = time;
					czekamy = 1;

			}else{
					if (time-last_hopping_time > (HOP_CHANNELS_COUNT*20/*ms*/+1)) czekamy = 0; //wait for all channels to hop
			}

		}
		else
		if (time-last_pack_time > 41) //more than one frame lost (works well till 4 sec)
		{
				
				Red_LED_ON;

				if (!czekamy)
				{
					ile = (time-(last_pack_time-1))/20;//how many channels tx changed since last recieved packet
					ChannelHoppingFrom(last_ok_channel,ile);
					last_hopping_time = time;
					czekamy = 1;
				}else{
					if (time-last_hopping_time > 19) czekamy = 0;
				}
		}
		else
		if (time-last_hopping_time > 21) //single frame lost
		{
			Red_LED_ON;
			
			ChannelHopping(1);
			
			last_hopping_time = time-1;
			Red_LED_OFF;
			
		}		
#endif

}
//------------------------------------------------------

int main()
{
	uint8_t i;

	Init();

	if (service_mode_rx) ServiceMode(); //enter service mode


  	//Hop to first frequency from Carrier
  	#ifdef FREQUENCY_HOPPING
    	Hopping();
  	#endif  
	
	RF_Mode = Receive;


	Red_LED_OFF;

	time = millis();

	last_pack_time = time; // reset the last pack receiving time for first usage
	last_hopping_time = time; // reset hopping timer
//	quality_check_time = time;


//--------------  MAIN LOOP  -------------------------------------------------------------------------------------------
	while(1)
	{
		time = millis();

		if (_spi_read(0x0C)==0) // detect the locked module and reboot	
		{
			RF22B_init_parameter(); 
			to_rx_mode(); 
		}

		SignalLostProcedure();

		if(RF_Mode == Received)   // RFM22B INT pin Enabled by received Data
		{		
			last_pack_time = time; // record last package time
							 
			Red_LED_OFF;
			Green_LED_ON;

			send_read_address(0x7f); // Send the package read command

			//read all buffer
			for(i = 0; i<DATA_PACKAGE_SIZE; i++) RF_Rx_Buffer[i] = read_8bit_data(); 
			rx_reset();

			if (RF_Rx_Buffer[0] == 'S') // servo control data
			{	
				for(i = 0; i<8; i++) //Write into the Servo Buffer
				{                                                          
					temp_int = (256*RF_Rx_Buffer[1+(2*i)]) + RF_Rx_Buffer[2+(2*i)];
					if ((temp_int>1500) && (temp_int<4500)) Servo_Position[i] = temp_int; 
				}
			}
		
//			sum_rssi += _spi_read(0x26); // Read the RSSI value
			
			//binding mode
			#ifndef NO_BINDING
				if (bind) if (Bind_RX()) break;
			#endif
			
			#ifdef FREQUENCY_HOPPING
				ChannelHopping(1);
			#endif  
			last_ok_channel = hopping_channel;

			
			last_hopping_time = time;

			RF_Mode = Receive;
                                   
            

		}
		else Green_LED_OFF;
			


	}
//----------------------------------------------------------------------------------------------------------------------
	
	if (bind) //binding finished, now you have to restart RX
	{
		 while(1);
	}


}
