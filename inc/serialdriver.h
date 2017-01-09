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
	[This file] serialdriver: Serial Port Driver
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef SERIALDRIVER_H
#define SERIALDRIVER_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include <QString>
#include <QSerialPort>

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui {
class SerialDriver;
}

class SerialDriver : public QWidget
{
	Q_OBJECT

public:
	explicit SerialDriver(QWidget *parent = 0);
	void init(void);

public slots:
	void open(QString name, int tries, int delay, bool* success);
	void close(void);
	int read(unsigned char *buf);
	int write(char bytes_to_send, unsigned char *serial_tx_data);
	void readWrite(uint numb, uint8_t *dataPacket, uint8_t r_w);

private slots:

private:
	//Variables & Objects:
	QSerialPort USBSerialPort;
	bool comPortOpen;
	unsigned char usb_rx[256];

	//Function(s):

signals:
	void openProgress(int val);
	void openStatus(bool status);
	void newDataReady(void);
	void dataStatus(int idx, int status);
	void newDataTimeout(bool rst);
	void setStatusBarMessage(QString msg);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

//Data in status indicator:
#define DATAIN_STATUS_GREY		0
#define DATAIN_STATUS_GREEN		1
#define DATAIN_STATUS_YELLOW	2
#define DATAIN_STATUS_RED		3
#define INDICATOR_TIMEOUT		110

//USB driver:
#define USB_READ_TIMEOUT		10		//ms

#endif // SERIALDRIVER_H
