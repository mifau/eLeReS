

#include "servo.h"


volatile unsigned int total_ppm_time=0;
//unsigned int Servo_Buffer[10] = {3000,3000,3000,3000,3000,3000,3000,3000};	//servo position values from RF
volatile unsigned int Servo_Position[20] = {3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000};	//real servo position values
volatile static unsigned char Servo_Number = 0;


//volatile static unsigned char receiver_mode = 0;

void Servo_Init()
{
		DDRD |= (1<<PD3); //Servo1
		DDRD |= (1<<PD5); //Servo2
		DDRD |= (1<<PD6); //Servo3
		DDRD |= (1<<PB7); //Servo4
		DDRB |= (1<<PB0); //Servo5
		DDRB |= (1<<PB1); //Servo6
		DDRB |= (1<<PB2); //Servo7
		DDRB |= (1<<PB3); //Servo8
	
		//Timer1 configuration - servo pwm/ppm
	   	TCCR1B	=   0x00;   //stop timer
	   	TCNT1H	=   0x00;   //setup
	   	TCNT1L	=   0x00;
	   	ICR1	=   40000;   // used for TOP, makes for 50 hz
   
	   	TCCR1A	=   0x02;   
	   	TCCR1B	=   0x1A; //start timer with 1/8 prescaler for 0.5us PPM resolution

	   	TIMSK1	|= (1<<TOIE1); 

}


ISR(TIMER1_OVF_vect)
{
  unsigned int us; // this value is not real microseconds, we are using 0.5us resolution (2048 step), this is why the all values 2 times more than real microseconds.

	Servo_Ports_LOW;

	Servo_Number++; // jump to next servo
	if (Servo_Number>8) // back to the first servo 
	{
		total_ppm_time = 0; // clear the total servo ppm time
		Servo_Number=0;
	}

	if (Servo_Number == 8)  // Check the servo number. 
	{
	    //Servos accepting 50hz ppm signal, this is why we are waiting for 20ms before second signal brust. 
	    us = 40000 - total_ppm_time; //wait for total 20ms loop.  waiting time = 20.000us - total servo times
	}
	else
	    us = Servo_Position[Servo_Number]; // read the servo timing from buffer

	total_ppm_time += us; // calculate total servo signal times.

	//if (receiver_mode==0) // Parallel PPM
	//  {  
	switch (Servo_Number) {
	  case 0:
	    Servo1_OUT_HIGH;
	    break;
	  case 1:
	    Servo2_OUT_HIGH;
	    break;
	  case 2:
	    Servo3_OUT_HIGH;
	    break;
	  case 3:
	    Servo4_OUT_HIGH;
	    break;
	  case 4:
	   	Servo5_OUT_HIGH;
	    break;
	  case 5:
	    Servo6_OUT_HIGH;
	    break;
	  case 6:
	    Servo7_OUT_HIGH;
	    break;
	  case 7:
	    Servo8_OUT_HIGH;
	    break;
	  //case 8:
	  //  Servo9_OUT_HIGH;
	  //  break;  
	}     
 //   }
   
  //else  
  //  {
   	 Serial_PPM_OUT_HIGH; // Serial PPM over 9th channel
  //  }
  
    
	TCNT1 = 40000 - us; // configure the timer interrupt for X micro seconds     
}




