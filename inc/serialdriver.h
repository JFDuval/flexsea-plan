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
#include <QObject>
#include <QString>
#include <QSerialPort>
#include <vector>
#include <flexseaDevice.h>
#include <QTimer>

//USB driver:
#define CHUNK_SIZE				48
#define MAX_SERIAL_RX_LEN		(CHUNK_SIZE*10 + 10)

//Timer:
#define CLOCK_TIMER_PERIOD		500	//ms
#define CLOCK_TIMER_MAX_COUNT	30	//30*500ms = 15s

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui {
class SerialDriver;
}

typedef enum SerialPortStatus {
	Idle = -1,
	PortOpeningFailed = 0,
	WhileOpening = 1,
	PortOpeningSucceed = 2,
	PortClosed = 3
}SerialPortStatus;
Q_DECLARE_METATYPE(SerialPortStatus)

class SerialDriver : public QObject
{
	Q_OBJECT



public:
	explicit SerialDriver(QObject *parent = 0);
	virtual ~SerialDriver();

public slots:

	void open(QString name, int tries, int delay, bool* success);
	void close(void);
	void tryReadWrite(uint8_t bytes_to_send, uint8_t *serial_tx_data, int timeout);
	int write(uint8_t bytes_to_send, uint8_t *serial_tx_data);
	void flush(void);

	bool isOpen() { return USBSerialPort->isOpen(); }
	void clear(void);
	void addDevice(FlexseaDevice* device);

private:

	QSerialPort* USBSerialPort;
	bool comPortOpen;
	unsigned char usb_rx[256];
	uint8_t largeRxBuffer[MAX_SERIAL_RX_LEN];
	uint16_t timerCount;

	std::vector<FlexseaDevice*> devices;
	FlexseaDevice* getDeviceByIdCmd(uint8_t slaveId, int cmd);

	void signalSuccessfulParse();
	void debugStats(int,int);

private slots:
	void handleReadyRead();
	void serialPortErrorEvent(QSerialPort::SerialPortError error);

signals:
	void openStatus(SerialPortStatus status,int nbTries);
	void newDataReady(void);
	void dataStatus(int idx, int status);
	void newDataTimeout(bool rst);
	void setStatusBarMessage(QString msg);
	void writeToLogFile(FlexseaDevice*);
	void aboutToClose(void);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

//Data in status indicator:
#define DATAIN_STATUS_GREY		0
#define DATAIN_STATUS_GREEN		1
#define DATAIN_STATUS_YELLOW	2
#define DATAIN_STATUS_RED		3
#define INDICATOR_TIMEOUT		15	//1.5s

#endif // SERIALDRIVER_H
