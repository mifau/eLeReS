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

#ifndef _BIND_H_
#define _BIND_H_

#include "common.h"

uint8_t bind;

void Bind_RX_Init();
uint8_t Bind_RX();
void Bind_TX();
void Bind_TX_Init();
void Bind_Channels(unsigned char* RF_HEAD);

#endif
