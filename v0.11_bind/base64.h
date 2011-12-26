#ifndef BASE64_H
#define BASE64_H

#include "common.h"

void base64_encode(const unsigned char * srcp, int len, unsigned char * dstp);
int base64_decode(const unsigned char * srcp, int len, unsigned char * dstp);


#endif
