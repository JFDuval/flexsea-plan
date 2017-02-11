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
#include "flexsea_payload.h"
#include <flexsea_comm.h>
#include "main.h"
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

//Parse the usb_rx buffer
uint8_t tmp_rx_command_usb[PACKAGED_PAYLOAD_LEN];
uint8_t decode_usb_rx(unsigned char *newdata)
{
	int i = 0, result = 0;
	uint8_t cmd_ready_usb = 0;
	//uint8_t tmp_rx_command_usb[PACKAGED_PAYLOAD_LEN];
	uint8_t ret = 0;
	uint8_t info[2] = {0,0};

	//Try to decode
	cmd_ready_usb = unpack_payload_usb();
	if(cmd_ready_usb != 0)
	{
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

	//Try to parse
	if(cmd_ready_usb != 0)
	{
		cmd_ready_usb = 0;

		PacketWrapper* p = fm_pool_allocate_block();
		if (p == NULL)
			return -1;

                memcpy(p->unpaked, &rx_command_usb, COMM_STR_BUF_LEN);
                //memcpy(p->packed, rx_buf_2, COMM_STR_BUF_LEN);
                // parse the command and execute it
                p->port = PORT_USB;
                result = payload_parse_str(p);

		//One or more new USB commands
		ret = 3;
	}
	else
	{
		//No new USB command
		ret = 4;
	}

	return ret;
}

#ifdef __cplusplus
}
#endif
