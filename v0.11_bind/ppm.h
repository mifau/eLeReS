#ifndef _PPM_H_
#define _PPM_H_

#include "common.h"


volatile unsigned char channel_count;
volatile unsigned char channel_no;

void PPM_Init();
void SetServoPos (unsigned char channel,int value);


#endif
