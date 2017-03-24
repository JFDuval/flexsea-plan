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
	[This file] peripherals: misc. drivers for HW and SW peripherals
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-10-19 | jfduval | Initial release
	*
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//****************************************************************************
// Include(s)
//****************************************************************************

#include <flexsea_buffers.h>
#include "../flexsea-comm/inc/flexsea_payload.h"
#include <flexsea_comm.h>
#include "peripherals.h"

//****************************************************************************
// Local variable(s)
//****************************************************************************

//****************************************************************************
// External variable(s)
//****************************************************************************

//****************************************************************************
// Function(s)
//****************************************************************************

//Prepares the structures:
void initLocalComm(void)
{
	//Default state:
	initCommPeriph(&commPeriph[PORT_USB], PORT_USB, MASTER, rx_buf_1, \
			comm_str_1, rx_command_1, &rx_buf_circ_1, \
			&packet[PORT_USB][INBOUND], &packet[PORT_USB][OUTBOUND]);

	//Personalize specific fields:
	//...
}

//Parse the usb_rx buffer
uint8_t tmp_rx_command_usb[PACKAGED_PAYLOAD_LEN];
uint8_t decode_usb_rx(unsigned char *newdata)
{
	int result = 0;
	uint8_t ret = 0;
	(void)newdata;

	//Try to decode
	tryUnpacking(&commPeriph[PORT_USB], &packet[PORT_USB][INBOUND]);

	//Valid communication from USB?
	if(commPeriph[PORT_USB].rx.unpackedPacketsAvailable > 0)
	{
		commPeriph[PORT_USB].rx.unpackedPacketsAvailable = 0;
		result = payload_parse_str(&packet[PORT_USB][INBOUND]);
		#ifdef USE_PRINTF
		//printf("[Received a valid comm_str!]\n");
		ret = 0;
		#endif
	}
	else
	{
		#ifdef USE_PRINTF
		// printf("[No intelligent data received]\n");
		ret = 2;
		#endif
	}

	const int SUCCESS = 3;
	const int FAILURE = 4;
	ret = (result == 2) ? SUCCESS : FAILURE;

	return ret;
}

#ifdef __cplusplus
}
#endif
