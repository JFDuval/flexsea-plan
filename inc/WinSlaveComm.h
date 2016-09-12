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

#ifndef WIN_SLAVECOMM_H
#define WIN_SLAVECOMM_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QtGui>
#include <QWidget>
#include <QMdiSubWindow>
#include "w_slavecomm.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui
{
    class WinSlaveComm;
}

class WinSlaveComm : public QMdiSubWindow
{
    Q_OBJECT

public:
	//Constructor & Destructor:
    explicit WinSlaveComm(QWidget *parent = 0);
    ~WinSlaveComm();

public slots:
    void receiveComOpenStatus(bool status);
    void receiveNewDataReady(void);
    void receiveDataStatus(int idx, int status);
    void receiveNewDataTimeout(bool rst);
    void receiveRefresh2DPlot(void);
    void receiveWriteToLogFile(int slaveId);
    void receiveCloseLogFile(void);
    void receiveSlaveReadWrite(uint numb, uint8_t *dataPacket, uint8_t r_w);
    void receiveExternalSlaveWrite(char numb, unsigned char *tx_data);

signals:
    void refresh2DPlot(void);
    void windowClosed(void);
    void writeToLogFile(int slaveId);
    void closeLogFile(void);
    void slaveReadWrite(uint numb, uint8_t *dataPacket, uint8_t r_w);

private:
	//Variables & Objects:
    W_SlaveComm *my_w_slavecomm;
};

#endif // WIN_SLAVECOMM_H
