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

//****************************************************************************
// Include(s)
//****************************************************************************

#include "serialdriver.h"
#include <flexsea_buffers.h>
#include <flexsea_comm.h>
#include "main.h"
#include <QDebug>
#include <QTime>
#include <flexsea_comm.h>
#include <flexsea_payload.h>
#include <ctime>
//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

SerialDriver::SerialDriver(QObject *parent) : QObject(parent)
{
	comPortOpen = false;

	clockTimer = new QTimer();
	clockTimer->setSingleShot(false);
	clockTimer->setInterval(1);
	clockTimer->setTimerType(Qt::PreciseTimer);
	connect(clockTimer, &QTimer::timeout, this, &SerialDriver::handleTimeout);
}

SerialDriver::~SerialDriver()
{
	   if(clockTimer) delete clockTimer;
	   clockTimer = nullptr;

	   while(outgoingBuffer.size() > 0)
	   {
		   outgoingBuffer.pop();
	   }
}
//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************


void SerialDriver::enqueueReadWrite(uint8_t numb, uint8_t* dataPacket, uint8_t r_w)
{
    write(numb, dataPacket);
    //outgoingBuffer.push(Message(numb, dataPacket, r_w));
}

void SerialDriver::handleTimeout()
{
	emit timerClocked();
    return;
    if(outgoingBuffer.size() > 0)
	{
		Message m = outgoingBuffer.front();
		readWrite(m.numBytes, m.dataPacket.data(), m.r_w);
		outgoingBuffer.pop();
	}
}

//Open port
void SerialDriver::open(QString name, int tries, int delay, bool *success)
{
	int cnt = 0;
	bool fd = false;
	int comProgress = 0;

	emit openProgress(comProgress);

	USBSerialPort.setPortName(name);
	USBSerialPort.setBaudRate(USBSerialPort.Baud115200);
	USBSerialPort.setDataBits(QSerialPort::Data8);
	USBSerialPort.setParity(QSerialPort::NoParity);
	USBSerialPort.setStopBits(QSerialPort::OneStop);
	USBSerialPort.setFlowControl(QSerialPort::NoFlowControl);

	connect(&USBSerialPort, &QSerialPort::readyRead, this, &SerialDriver::handleReadyRead);

	do
	{
		fd = USBSerialPort.open(QIODevice::ReadWrite);  //returns true if successful
		cnt++;
		if(cnt >= tries)
			break;

		//When false, print error code:
		if(fd == false)
		{
			qDebug() << "Try #" << cnt << " failed. Error: " << \
						USBSerialPort.errorString() << ".\n";
			emit openProgress(100*cnt/tries);
		}

		usleep(delay);
	}while(fd != true);


	if (fd == false)
	{
		qDebug() << "Tried " << cnt << " times, couldn't open " << name << ".\n";
		emit openProgress(0);
		comPortOpen = false;
		emit openStatus(comPortOpen);

		*success = false;
	}
	else
	{
		qDebug() << "Successfully opened " << name << ".\n";
		emit openProgress(100);
		comPortOpen = true;
		emit openStatus(comPortOpen);

		//Clear any data that was already in the buffers:
		//while(USBSerialPort.waitForReadyRead(100))
		{
			USBSerialPort.clear((QSerialPort::AllDirections));
		}

		*success = true;
		clockTimer->start();
	}
}

//Close port
void SerialDriver::close(void)
{
	//Turn comm. off
	comPortOpen = false;
	emit openStatus(comPortOpen);

	//Delay (for ongoing transmissions)
	usleep(100000);

	USBSerialPort.clear((QSerialPort::AllDirections));
	USBSerialPort.close();

	while(outgoingBuffer.size()) { outgoingBuffer.pop(); }

	clockTimer->stop();
}

//Read
int SerialDriver::read(unsigned char *buf)
{
	(void)buf;

	QByteArray baData;
	baData.resize(256);
//	bool dataReady = false;

//	dataReady = USBSerialPort.waitForReadyRead(USB_READ_TIMEOUT);
//	if(dataReady == true)
//	{
		baData = USBSerialPort.readAll();

		//We check to see if we are getting good packets, or a bunch of crap:
		int len = baData.length();
		if(len > 256)
		{
			qDebug() << "Data length over 256 bytes (" << len << "bytes)";
            USBSerialPort.clear((QSerialPort::AllDirections));
			emit dataStatus(0, DATAIN_STATUS_RED);
			return 0;
		}

		//qDebug() << "Read" << len << "bytes.";

		//Fill the rx buf with our new bytes:
		update_rx_buf_array_usb((uint8_t *)baData.data(), len);
		commPeriph[PORT_USB].rx.bytesReadyFlag = 1;
//	}
//	else
//	{
//		//qDebug("No USB bytes available.");
//		emit dataStatus(0, DATAIN_STATUS_RED);
//		return 0;
//	}

	//Notify user in GUI:
	emit dataStatus(0, DATAIN_STATUS_GREEN);   //***ToDo: support 4 channels
	emit newDataTimeout(true); //Reset counter
	return 1;
}

//Write
int SerialDriver::write(char bytes_to_send, unsigned char *serial_tx_data)
{
	qint64 write_ret = 0;

	//Check if COM was successfully opened:

	if(comPortOpen == true)
	{
        QByteArray myQBArray = QByteArray::fromRawData((const char*)serial_tx_data, bytes_to_send);
		write_ret = USBSerialPort.write(myQBArray);
	}
	else
	{
		QString msg = "COM port isn't open - can't send command";
		qDebug() << msg;
		emit setStatusBarMessage(msg);
	}

	return (int) write_ret;
}

void SerialDriver::readWrite(uint8_t numb, uint8_t *dataPacket, uint8_t r_w)
{
	/*	For Bench marking
	static QTime lastTime = QTime::currentTime();
	static double lp_msecs = 0;
	QTime currTime = QTime::currentTime();
	int msecs = lastTime.msecsTo(currTime);
	lastTime = currTime;
	lp_msecs = 0.1*msecs + 0.9*lp_msecs;
	//lp_msecs = msecs;
	static int debugCount = 0;
	debugCount++;
	debugCount%=100;
	if(debugCount == 0)
	{
		qDebug() << "Period in msecs: " << lp_msecs << ", frequency: " << 1000.0 / lp_msecs;
	}
	*/
	write(numb, dataPacket);
	//qDebug() << dataPacket;

	//Should we look for a reply?
	if(r_w == READ)
	{
		//Status to Yellow before we get the reply:
		emit dataStatus(0, DATAIN_STATUS_YELLOW);
/*
		//Did we receive data? Can we decode it?
		if(read(usb_rx))
		{
			decode_usb_rx(usb_rx);
			emit newDataReady();
		}
*/
	}
}

void SerialDriver::handleReadyRead()
{
	/*	Below code benchmarks the read stream proess
	 *  Measures the time between reads, low passes, periodically qDebug()'s it
	 * (use ctime lib instead of QTime because somehow QTime is so inefficient it changes the speed
	 *
	static float streamPeriod = 0;
	static clock_t tLast = clock();
	clock_t tCurr = clock();
	float periodInSecs = (float)(tCurr - tLast) / CLOCKS_PER_SEC;
	tLast = tCurr;

	streamPeriod = 0.9 * streamPeriod + 0.1 * periodInSecs;
	float  frequency = 1 / streamPeriod;

	static int count = 0;
	count++;
	if(frequency < 70)
		count%=100;
	else
		count%=200;

	if(!count)
		qDebug() << "Estimated period of reads: " << streamPeriod << ", frequency: " << frequency;
	*/

	QByteArray baData;
	baData.resize(256);
	bool dataReady = false;

	baData = USBSerialPort.readAll();

	//We check to see if we are getting good packets, or a bunch of crap:
	int len = baData.length();
	if(len > 256)
	{
		qDebug() << "Data length over 256 bytes (" << len << "bytes)";
        USBSerialPort.clear((QSerialPort::AllDirections));
		emit dataStatus(0, DATAIN_STATUS_RED);
		return;
	}

	//qDebug() << "Read" << len << "bytes.";

	//Fill the rx buf with our new bytes:
	update_rx_buf_array_usb((uint8_t *)baData.data(), len);
	commPeriph[PORT_USB].rx.bytesReadyFlag = 1;

	//Notify user in GUI:
	emit dataStatus(0, DATAIN_STATUS_GREEN);   //***ToDo: support 4 channels
	emit newDataTimeout(true); //Reset counter
	decode_usb_rx(usb_rx);
	emit newDataReady();
	return;
}
//****************************************************************************
// Private function(s):
//****************************************************************************

void SerialDriver::init(void)
{
	comPortOpen = false;
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

