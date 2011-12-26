#ifndef _RFM22B_H_
#define _RFM22B_H_

#include "common.h"

#define  nIRQ_1 (PIND & 0x04)==0x04 //D2
#define  nIRQ_0 (PIND & 0x04)==0x00 //D2
      
#define  nSEL_on PORTD |= 0x10 //D4
#define  nSEL_off PORTD &= 0xEF //D4

#define  SCK_on PORTC |= 0x04 //C2
#define  SCK_off PORTC &= 0xFB //C2

#define  SDI_on PORTC |= 0x02 //C1
#define  SDI_off PORTC &= 0xFD //C1

#define  SDO_1 (PINC & 0x01) == 0x01 //C0
#define  SDO_0 (PINC & 0x01) == 0x00 //C0


extern volatile unsigned char RF_Mode;
#define Available 0
#define Transmit 1
#define Transmitted 2
#define Receive 3
#define Received 4


extern unsigned char hopping_channel;

unsigned char _spi_read(unsigned char address);

void RF22B_Init();
void RF22B_init_parameter(void);
//void fast_init(void);
void send_read_address(unsigned char i);
void send_8bit_data(unsigned char i);
unsigned char read_8bit_data(void);
void rx_reset(void);
void to_tx_mode(void);
void to_rx_mode(void); 
void to_ready_mode(void);
void to_sleep_mode(void);
void frequency_configurator(long frequency);

void Hopping(void);
void ChannelHopping(uint8_t hops);
void ChannelHoppingFrom(uint8_t start_channel, uint8_t hops);
void ChannelSet(uint8_t channel);

#endif
