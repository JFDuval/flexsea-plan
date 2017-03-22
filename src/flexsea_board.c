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

//****************************************************************************
// External variable(s)
//****************************************************************************

//plan_spi:
uint8_t spi_rx[COMM_STR_BUF_LEN];
uint8_t usb_rx[COMM_STR_BUF_LEN];

//****************************************************************************
// Function(s)
//****************************************************************************

//Wrapper for the specific serial functions. Useful to keep flexsea_network
//plateform independant (for example, we don't need need puts_rs485() for Plan)
void flexsea_send_serial_slave(PacketWrapper* p)
{
  (void) p;
	//The current implementation of FlexSEA-Plan doesn't require this function,
	//the different windows simply emit a signal to the serial driver. We are
	//Keeping this here for compatibility reasons.

	return;
}

void flexsea_send_serial_master(PacketWrapper *p)
{
	//Not implemented for this board
	(void)p;

}

uint8_t getBoardID(void)
{
	return board_id;
}

uint8_t getBoardUpID(void)
{
	return board_up_id;
}

uint8_t getBoardSubID(uint8_t sub, uint8_t idx)
{
	if(sub == 0){return board_sub1_id[idx];}
	else if(sub == 1){return board_sub2_id[idx];}

	return 0;
}

uint8_t getSlaveCnt(uint8_t sub)
{
	if(sub == 0){return SLAVE_BUS_1_CNT;}
	else if(sub == 1){return SLAVE_BUS_2_CNT;}

	return 0;
}

#ifdef __cplusplus
}
#endif
