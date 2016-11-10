/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2016 Dephy, Inc. <http://dephy.com/>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] main: FlexSEA Plan project: GUI app to control FlexSEA slaves
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "mainwindow.h"
#include <QApplication>

/*
//Compress 6x uint16 to 9 bytes (12bits per sensor).
//Needed to send all data with RIC/NU Read All function
uint8_t compressAndSplit6ch(uint8_t *buf, uint16_t ch0, uint16_t ch1, uint16_t ch2, \
							uint16_t ch3, uint16_t ch4, uint16_t ch5)
{
	uint8_t tmp0 = 0, tmp1 = 0;
	uint16_t tmp[6] = {0,0,0,0,0,0};
	uint16_t combo[5] = {0,0,0,0,0};

	//Compress to 12bits
	tmp[0] = (ch0 >> 4) & 0x0FFF;
	tmp[1] = (ch1 >> 4) & 0x0FFF;
	tmp[2] = (ch2 >> 4) & 0x0FFF;
	tmp[3] = (ch3 >> 4) & 0x0FFF;
	tmp[4] = (ch4 >> 4) & 0x0FFF;
	tmp[5] = (ch5 >> 4) & 0x0FFF;

	//We want:
	//combo[0]: 0000 0000 0000 1111
	//combo[1]: 1111 1111 2222 2222
	//combo[2]: 2222 3333 3333 3333
	//combo[3]: 4444 4444 4444 5555
	//combo[4]: ____ ____ 5555 5555

	//Combine:
	combo[0] = (tmp[0] << 4) | ((tmp[1] >> 8) & 0xFF);
	combo[1] = (tmp[1] << 8) | ((tmp[2] >> 4) & 0xFFFF);
	combo[2] = (tmp[2] << 12) | (tmp[3]);
	combo[3] = (tmp[4] << 4) | ((tmp[5] >> 8) & 0xFF);
	combo[4] = (tmp[5] & 0xFF);


	//Stock in uint8_t buffer:
	uint16_to_bytes((uint16_t)combo[0], &tmp0, &tmp1);
	*(buf) = tmp0;
	*(buf+1) = tmp1;
	uint16_to_bytes((uint16_t)combo[1], &tmp0, &tmp1);
	*(buf+2) = tmp0;
	*(buf+3) = tmp1;
	uint16_to_bytes((uint16_t)combo[2], &tmp0, &tmp1);
	*(buf+4) = tmp0;
	*(buf+5) = tmp1;
	uint16_to_bytes((uint16_t)combo[3], &tmp0, &tmp1);
	*(buf+6) = tmp0;
	*(buf+7) = tmp1;
	*(buf+8) = (uint8_t)combo[4];

	return 0;
}

//Unpack from buffer
void unpackCompressed6ch(uint8_t *buf, uint16_t *v0, uint16_t *v1, uint16_t *v2, \
							uint16_t *v3, uint16_t *v4, uint16_t *v5)
{
	*v0 = ((*(buf+0) << 8 | *(buf+1)) >> 4) << 4;
	*v1 = (((*(buf+1) << 8 | *(buf+2))) & 0xFFF) << 4;
	*v2 = ((*(buf+3) << 8 | *(buf+4)) >> 4) << 4;
	*v3 = (((*(buf+4) << 8 | *(buf+5))) & 0xFFF) << 4;
	*v4 = ((*(buf+6) << 8 | *(buf+7)) >> 4) << 4;
	*v5 = (((*(buf+7) << 8 | *(buf+8))) & 0xFFF) << 4;
}

void compress6chTestCodeBlocking(void)
{
	uint8_t buffer[20];
	uint16_t strainValues[6] = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666};
	uint16_t results[6] = {0,0,0,0,0,0};

	for(int i = 0; i < 20; i++)
	{
		buffer[i] = 0;
	}

	while(1)
	{
		compressAndSplit6ch(&buffer[5], strainValues[0], strainValues[1], \
							strainValues[2],strainValues[3],\
							strainValues[4],strainValues[5]);

		unpackCompressed6ch(&buffer[5], &results[0], &results[1], &results[2], \
							&results[3], &results[4], &results[5]);

		//CyDelay(10);
	}
}
*/

//****************************************************************************
// Main
//****************************************************************************

int main(int argc, char *argv[])
{
	//compress6chTestCodeBlocking();

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
