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

//****************************************************************************
// Main
//****************************************************************************

/*	TODO REMOVE
#define BUF_LEN	50
#include "flexsea_generic.h"
#include <QDebug>
void TEMPORARY_test_ricnu_fcts(void)
{
	//FlexSEA_Generic myFlexSEA_Generic;
	uint16_t numb = 0;
	uint8_t offset = 0;
	int32_t result = 0;
	uint8_t myBuf1[BUF_LEN], myBuf2[BUF_LEN], myBuf3[BUF_LEN], myBuf4[BUF_LEN];

	memset(myBuf1, 0, BUF_LEN);
	memset(myBuf2, 0, BUF_LEN);
	memset(myBuf3, 0, BUF_LEN);
	memset(myBuf4, 0, BUF_LEN);

	//Legacy style (with macros):
	//===========================

	qDebug() << "Old:";
	numb = tx_cmd_ricnu(FLEXSEA_EXECUTE_1, CMD_READ, myBuf1,\
						PAYLOAD_BUF_LEN, 0, 1, 2, 3, 4, 5, 6, 7);
	qDebug() << "tx_cmd_ricnu returned " << numb;
	//COMM_GEN_STR_DEFAULT
	numb = comm_gen_str(myBuf1, myBuf2, numb);
	qDebug() << "comm_gen_str returned " << numb;
	numb = COMM_STR_BUF_LEN;

	//Reset:
	memset(myBuf1, 0, BUF_LEN);

	//New approach:
	//=============

	uint8_t cmd = 0, cmdType = 0;
	uint16_t len = 0;

	qDebug() << "New:";
	tx_cmd_ricnu_rw(myBuf1, &cmd, &cmdType, &len, 0, 1, 2, 3, 4, 5, 6, 7);
	qDebug() << "tx_cmd_ricnu_rw returned " << len;
	numb = tx_cmd(myBuf1, cmd, cmdType, len, FLEXSEA_EXECUTE_1, myBuf3);
	qDebug() << "tx_cmd returned " << numb;
	numb = comm_gen_str(myBuf3, myBuf4, numb);
	numb = COMM_STR_BUF_LEN;

	//Test:
	//=====
	result = memcmp(myBuf2, myBuf4, len);

	//myFlexSEA_Generic.packetVisualizer(numb, myBuf2);
}
*/

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
