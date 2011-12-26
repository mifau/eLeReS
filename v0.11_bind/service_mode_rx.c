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

#include "service_mode_rx.h"

uint8_t service_mode_rx = 0;

	unsigned int rcv_packets = 0;
	unsigned long QUALITY = 100;
	unsigned short RSSI = 0;


int16_t GetCRC(char *buf, uint16_t len)
{
	int16_t crc = 0;
	for (uint8_t i=0;i<len;i++) crc += buf[i];
	return crc;
}

void Service_send_OK() //wyslanie informacji o sukcesie przeprowadzonej operacji
{
	USART_Transmit(UART_CMD_OK);
	USART_Transmit(UART_EOT);
}


void Service_send_ERROR(char *text) //wyslanie informacji o niepowodzeniu przeprowadzonej operacji
{
	USART_Transmit(UART_CMD_ERROR);
	USART_Transmit_String(text);
	USART_Transmit(UART_EOT);
}

void Service_ReadEEPROMData(uint8_t command_type, uint8_t command_size, char *command)
{
	char buffer[101];

	char buffer_coded[101];

	int16_t crc;
	int16_t crc_data;

	uint16_t offset = (command[1]<<8) + command[0]; //odczyt offsetu

	uint8_t len = command[2]; //odczyt dlugosci
	if (len>100) len = 100;

	crc = GetCRC(command,command_size-2); //wygenerowanie CRC
	crc_data = (command[command_size-1]<<8) + command[command_size-2]; //odczyt CRC
	if (crc!=crc_data)
	{
		Service_send_ERROR("CRC Error");
		return;
	}

	//odczyt paczki z eepromu
	GetBlockEE((char *)(buffer+2), offset,len);

	buffer[0] = command[0];			  //offset	(ten sam, ktory przyszedl w zadaniu)
	buffer[1] = command[1];

	//obliczanie crc
	crc = GetCRC(buffer,len+2);
	memcpy(buffer+len+2,&crc,2);

	//zakodowanie informacji
	base64_encode((unsigned char*)buffer, len+4, (unsigned char*)buffer_coded);
	len = strlen((char *)buffer_coded);

	
	//odeslanie paczki przez uart
	USART_Transmit(UART_CMD_EEPROM_DATA); //przesylamy dane 
	for (int i=0;i<len;i++)	USART_Transmit(buffer_coded[i]);
	USART_Transmit(UART_EOT);
}

void Service_SaveEEPROMData(uint8_t command_type, uint8_t command_size, char *command) 
{

	int16_t crc = GetCRC(command,command_size-2);
	int16_t offset = (command[1]<<8) + command[0]; //odczyt offsetu
	int16_t crc_data = (command[command_size-1]<<8) + command[command_size-2]; // CRC danych przychodzacych

	if (crc_data!=crc)
	{
		Service_send_ERROR("CRC Error");
		return;
	}


	//przepisanie danych z paczki do EEPROM'u
	SetBlockEE((char *)(command+2), offset,command_size-4);

	Service_send_OK();

}

void Service_VersionInfo()
{
	USART_Transmit(UART_CMD_VERSION); //przesylamy dane 
	for (int i=0;i<10;i++) USART_Transmit(VERSION[i]);
	USART_Transmit(UART_EOT);
}

void Service_Recieve(uint8_t command_type, uint8_t command_size, char *command)
{
	char command_decoded[101];
	uint8_t new_size;

	switch(command_type)
	{
	
		case UART_CMD_EEPROM_DATA:	//write EEPROM block
			new_size = base64_decode((unsigned char*)command, command_size, (unsigned char*)command_decoded);
			Service_SaveEEPROMData(command_type,new_size,command_decoded);
			break;
	
		case UART_CMD_EEPROM_REQ: //read EEPROM block
			new_size = base64_decode((unsigned char*)command, command_size, (unsigned char*)command_decoded);
			Service_ReadEEPROMData(command_type,new_size,command_decoded);
			break;

		case UART_CMD_EEPROM_END: //after all eeprom packets
			//ReadConfigFromEEPROM();
			SOFT_RESET;
			break;

		case UART_CMD_PING: //ping
			USART_Transmit(UART_CMD_PING); 
			USART_Transmit(UART_EOT);
			break;

		case UART_CMD_VERSION: //return version number
			Service_VersionInfo();
			break;

		case UART_CMD_DEFAULTS: //load default values to EEPROM
			if ((command[0]=='E')&&(command[1]=='F')&&(command[2]=='A')&&(command[3]=='U')&&(command[4]=='L')&&(command[5]=='T')&&(command[6]=='S'))
			{
				SetDefaults();
				SOFT_RESET;
			}
			break;
	}

}



void QualityControl()
{
	unsigned long time = millis();
	static unsigned long quality_check_time = 0;
	static unsigned int sum_rssi = 0;

	//after 200ms (every 10th frame) we check averange packet loss and RSSI
	//po 0,2sek sprawdzamy procentowy stosunek pakietow wyslanych do odebranych
	if (time - quality_check_time > 199)
	{
		QUALITY = ((rcv_packets) * 100) / timer0_packets; //100 = 100% received packets
		RSSI = sum_rssi/rcv_packets;
		if (RSSI<0) RSSI = 0;
		
/*
  		char tmp[100]; 
		sprintf(tmp,"   %3d   %3d\n",(int16_t)QUALITY,(int16_t)RSSI);
		USART_Msg(tmp);
*/

		rcv_packets = 0;
		timer0_packets = 0;
		sum_rssi = 0;
		quality_check_time = time;
	}
	timer0_start_transmission = 0;

}




void Service_Send(unsigned char* rssi_table)
{
	char buffer[68];
	static uint8_t rssi_index = 0;
	uint16_t crc;
	uint8_t len;
	static unsigned long last_pack_time = 0;

	//paczka z danymi RSSI (kazda paczka wysylana jest co 50ms)
	if ((millis()-last_pack_time>50)&&(UART0_wyslano))
	{
		memset(UART0_SendBuffer,0,sizeof(UART0_SendBuffer));
		UART0_SendBuffer[0]=UART_CMD_RSSI;
		buffer[0]=rssi_index;
		memcpy(buffer+1,rssi_table+(rssi_index*64),64);
		crc = GetCRC(buffer, 65);
		memcpy(buffer+65,&crc,2);
		base64_encode((unsigned char*)buffer, 67, (unsigned char*)UART0_SendBuffer+1);
		len = strlen((char *)UART0_SendBuffer);
		UART0_SendBuffer[len] = UART_EOT;
		UART0_wyslano = 0;
		UDR0 = UART0_SendBuffer[0];
		last_pack_time = millis();
	}

}





uint8_t CheckServiceMode()
{
	//check for service mode
	DDRB &= ~(1<<PB4);
	PORTB |= (1<<PB4);
	_delay_ms(1);
	if (bit_is_clear(PINB,PB4))
	{
		_delay_ms(1000);
		if (bit_is_clear(PINB,PB4)) service_mode_rx = 1;

	}else service_mode_rx = 0;
	
	return service_mode_rx;
}

void ServiceMode()
{
	unsigned char rssi_table[256];
	uint8_t service_channel;
	unsigned long time = millis();
	static unsigned long last_hop_time;

	Red_LED_ON;
	Green_LED_ON;
	

	service_channel = 0;
	last_hop_time = time;
	ChannelSet(service_channel);
	while(1)
	{
		time = millis();
		
		if (_spi_read(0x0C)==0) // detect the locked module and reboot	
		{
			RF22B_init_parameter(); 
			to_rx_mode(); 
		}
		
		
		//channel change
		if (time-last_hop_time>12)
		{
			rssi_table[service_channel] = _spi_read(0x26);
			service_channel++;
			last_hop_time = time;
		}

		if (UART_lock) //awaiting message from UART
		{
			Service_Recieve(UART_command_type,UART_command_size,(char *)UART_command);
			UART_lock = 0;		//unlock uart - allowing to recieve next message
		}

		Service_Send(rssi_table);

	}
}
