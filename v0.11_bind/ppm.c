
#include "ppm.h"


void SetServoPos (unsigned char channel,int value)
{
  unsigned char ch = channel*2;
  Servo_Position[ch] = value/256;
  Servo_Position[ch+1] = value%256;
}


void PPM_Init()
{
		channel_no = 0;
		channel_count = 0;

		PCMSK2 = 0x08;
		PCICR|=(1<<PCIE2);


// set default servo position values.
		for (unsigned char i=0;i<8;i++)  SetServoPos(i,3000); // set the center position

		TCCR1B   =   0x00;   //stop timer
		TCNT1H   =   0x00;   //setup
		TCNT1L   =   0x00;
		ICR1     =   60005;   // used for TOP, makes for 50 hz
		TCCR1A   =   0x02;   
		TCCR1B   =   0x1A; //start timer with 1/8 prescaler for measuring 0.5us PPM resolution

}



ISR(PCINT2_vect)
{
	unsigned int time_temp;

	if ((PIND & 0x08)==0x08) // Only works with rising edge of the signal
	{
		time_temp = TCNT1; // read the timer1 value
		TCNT1 = 0; // reset the timer1 value for next
		if (channel_no<14) channel_no++; 
            		
		if (time_temp > 8000) // new frame detection : >4ms LOW
		{	
				channel_count = channel_no;
				channel_no = 0;
				transmitted = 0;                               
		}
	    else
	    {
			if ((time_temp>1500) && (time_temp<4500)) // check the signal time and update the channel if it is between 750us-2250us
			{
				//Servo_Buffer[(2*channel_no)-1] = (byte) (time_temp >> 8); // write the high byte of the value into the servo value buffer.
				//Servo_Buffer[2*channel_no] =  (byte) (time_temp); // write the low byte of the value into the servo value buffer.                                    
				SetServoPos(channel_no-1,time_temp);
			}
	    }
	}

}

