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
	[This file] flexsea_board: configuration and functions for this
	particular board
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "flexsea_board.h"
#include "../../flexsea-system/inc/flexsea_system.h"

//****************************************************************************
// Local variable(s)
//****************************************************************************

//Board ID (this board) - pick from Board list in /common/inc/flexsea.h
uint8_t board_id = FLEXSEA_PLAN_1;
uint8_t board_up_id = FLEXSEA_DEFAULT;
uint8_t board_sub1_id[SLAVE_BUS_1_CNT ? SLAVE_BUS_1_CNT : 1] = {FLEXSEA_MANAGE_1};
uint8_t board_sub2_id[SLAVE_BUS_2_CNT ? SLAVE_BUS_2_CNT : 1];

unsigned char comm_str_payload1[16];

//ToDo: This used to be in flexsea_rx_cmd... not clean.
uint8_t read_offset = 0;

//Slave Read Buffer:
unsigned char slave_read_buffer[SLAVE_READ_BUFFER_LEN];

//****************************************************************************
// External variable(s)
//****************************************************************************

//plan_spi:
unsigned char spi_rx[COMM_STR_BUF_LEN];
unsigned char usb_rx[COMM_STR_BUF_LEN];

//flexsea_comm.c:
//extern uint8_t rx_command_spi[PAYLOAD_BUF_LEN][PACKAGED_PAYLOAD_LEN];

//****************************************************************************
// Function(s)
//****************************************************************************

//Wrapper for the specific serial functions. Useful to keep flexsea_network
//plateform independant (for example, we don't need need puts_rs485() for Plan)
void flexsea_send_serial_slave(unsigned char port, unsigned char *str, unsigned char length)
{
	//Test:
	unsigned char stri[] = {"jfduval    "};

	length = COMM_STR_BUF_LEN;    //Fixed length for now	//Steven: without that line the success rate depends on the # of bytes

	if(port == PORT_SPI)
	{
		#ifdef USE_PRINTF
		//printf("Sending %i bytes.\n", length+1);
		#endif

		if(length > 0)
		{
			//flexsea_spi_transmit(length , str, 0);
		}
	}
	else if(port == PORT_USB)
	{
		//flexsea_serial_transmit(length, str, 0);
		#warning "Re-enable this!!!"
		//***ToDo***
	}

	return;
}

void flexsea_send_serial_master(unsigned char port, unsigned char *str, unsigned char length)
{
	//...
}

//TODO remove this
//Parse the usb_rx buffer
uint8_t tmp_rx_command_usb[PACKAGED_PAYLOAD_LEN];
uint8_t decode_usb_rx(unsigned char *newdata)
{
	int i = 0, result = 0, n = 0;
	uint8_t cmd_ready_usb = 0;
	//uint8_t tmp_rx_command_usb[PACKAGED_PAYLOAD_LEN];
	uint8_t ret = 0;

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

		//Cheap trick to get first line	//ToDo: support more than 1
		for(i = 0; i < PAYLOAD_BUF_LEN; i++)
		{
			tmp_rx_command_usb[i] = rx_command_usb[0][i];
		}

		result = payload_parse_str(tmp_rx_command_usb);

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
