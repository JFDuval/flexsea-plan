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
	[This file] WinSlaveComm: Slave Communication Window (Wrapper)
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "WinSlaveComm.h"
#include <QMdiSubWindow>
#include <QDebug>
#include "w_slavecomm.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

WinSlaveComm::WinSlaveComm(QWidget *parent) :
  QMdiSubWindow(parent)
{
    my_w_slavecomm = new W_SlaveComm(this);
    this->setWidget(my_w_slavecomm);

    //Refresh 2D Plot:
    connect(my_w_slavecomm, SIGNAL(refresh2DPlot()), \
            this, SLOT(receiveRefresh2DPlot()));

    //Logging:
    connect(my_w_slavecomm, SIGNAL(writeToLogFile(int)), \
            this, SLOT(receiveWriteToLogFile(int)));
    connect(my_w_slavecomm, SIGNAL(closeLogFile()), \
            this, SLOT(receiveCloseLogFile()));

    //SerialPort:
    connect(my_w_slavecomm, SIGNAL(slaveReadWrite(uint, uint8_t*, uint8_t)), \
            this, SLOT(receiveSlaveReadWrite(uint, uint8_t*, uint8_t)));
}

WinSlaveComm::~WinSlaveComm()
{
    emit windowClosed();
    my_w_slavecomm->~W_SlaveComm();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void WinSlaveComm::receiveRefresh2DPlot(void)
{
    emit refresh2DPlot();
}

void WinSlaveComm::receiveNewDataReady(void)
{
    //my_w_slavecomm->
}

void WinSlaveComm::receiveDataStatus(int idx, int status)
{
    my_w_slavecomm->displayDataReceived(idx, status);
}

void WinSlaveComm::receiveNewDataTimeout(bool rst)
{
    my_w_slavecomm->indicatorTimeout(rst);
}

void WinSlaveComm::receiveWriteToLogFile(int slaveId)
{
    emit writeToLogFile(slaveId);
}

void WinSlaveComm::receiveCloseLogFile(void)
{
    emit closeLogFile();
}

void WinSlaveComm::receiveComOpenStatus(bool status)
{
    my_w_slavecomm->receiveComPortStatus(status);
}

void WinSlaveComm::receiveSlaveReadWrite(uint numb, uint8_t *dataPacket, uint8_t r_w)
{
    emit slaveReadWrite(numb, dataPacket, r_w);
}

//A 3rd party is using SlaveComm to write to a slave (ex.: Control, Any Command)
void WinSlaveComm::receiveExternalSlaveWrite(char numb, unsigned char *tx_data)
{
    my_w_slavecomm->externalSlaveWrite(numb, tx_data);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

//****************************************************************************
// Private slot(s):
//****************************************************************************
