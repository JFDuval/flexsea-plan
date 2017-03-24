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
#include <flexsea_board.h>
#include <peripherals.h>
#include <QDebug>
#include <unistd.h>
#include <QTime>
#include <flexsea_comm.h>
#include <flexsea_payload.h>
#include <ctime>
#include <w_event.h>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

SerialDriver::SerialDriver(QObject *parent) : QObject(parent)
{
	comPortOpen = false;
}

SerialDriver::~SerialDriver() {}
//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

//Open port
void SerialDriver::open(QString name, int tries, int delay, bool *success)
{
//    name = "/dev/ttyACM9";
	int cnt = 0;
	bool isPortOpen = false;
	int comProgress = 0;

	emit openProgress(comProgress);

	USBSerialPort.setPortName(name);
	USBSerialPort.setBaudRate(400000);
	USBSerialPort.setDataBits(QSerialPort::Data8);
	USBSerialPort.setParity(QSerialPort::NoParity);
	USBSerialPort.setStopBits(QSerialPort::OneStop);
	USBSerialPort.setFlowControl(QSerialPort::HardwareControl);

	connect(&USBSerialPort, &QSerialPort::readyRead, this, &SerialDriver::handleReadyRead);

	do
	{
		isPortOpen = USBSerialPort.open(QIODevice::ReadWrite);  //returns true if successful
		cnt++;
		if(cnt >= tries)
			break;

		//When false, print error code:
		if(!isPortOpen)
		{
			qDebug() << "Try #" << cnt << " failed. Error: " << \
						USBSerialPort.errorString() << ".\n";
			emit openProgress(100*cnt/tries);
		}

		usleep(delay);
	} while(!isPortOpen);

	if (!isPortOpen)
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

		//Clear any data that was already in the buffers:
		USBSerialPort.clear((QSerialPort::AllDirections));

		comPortOpen = true;
		emit openStatus(comPortOpen);

		*success = true;
	}
}

//Close port
void SerialDriver::close(void)
{
	emit aboutToClose();

	//Turn comm. off
	comPortOpen = false;
	emit openStatus(comPortOpen);

	USBSerialPort.flush();
	//Delay (for ongoing transmissions)
	usleep(100000);

	USBSerialPort.clear((QSerialPort::AllDirections));
	USBSerialPort.close();
}

int SerialDriver::write(uint8_t bytes_to_send, uint8_t *serial_tx_data)
{
	qint64 write_ret = 0;

	//Check if COM was successfully opened:

	if(comPortOpen)
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

FlexseaDevice* SerialDriver::getDeviceById(uint8_t slaveId)
{
	for(unsigned int i = 0; i < devices.size(); i++)
	{
		if(devices.at(i)->slaveID == slaveId)
			return devices.at(i);
	}
	return nullptr;
}

void SerialDriver::signalSuccessfulParse()
{
	//Update appropriate FlexseaDevice
	uint8_t slaveId = packet[PORT_USB][INBOUND].unpaked[P_XID];
	FlexseaDevice* device = getDeviceById(slaveId);
	if(device)
	{
		device->decodeLastLine();
		if(device->isCurrentlyLogging)
		{
			device->applyTimestamp();
			device->eventFlags.last() = W_Event::getEventCode();
			emit writeToLogFile(device);
		}
	}
	emit newDataReady();
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

	if(USBSerialPort.bytesAvailable() < (CHUNK_SIZE))
		return;

    QByteArray baData;
	baData = USBSerialPort.readAll();

	//We check to see if we are getting good packets, or a bunch of crap:
	int len = baData.length();
	if(len < 1) return;
	if(len > MAX_SERIAL_RX_LEN)
	{
		//qDebug() << "Data length over " << MAX_SERIAL_RX_LEN << " bytes (" << len << "bytes)";
		len = MAX_SERIAL_RX_LEN;
		USBSerialPort.clear((QSerialPort::AllDirections));
		emit dataStatus(0, DATAIN_STATUS_RED);
		return;
	}

	uint8_t numBuffers = (len / CHUNK_SIZE) + (len % CHUNK_SIZE != 0);
	largeRxBuffer = (uint8_t*)baData.data();

	int16_t remainingBytes = len;

	int numMessagesReceived = 0;
	int numMessagesExpected = (len / COMM_STR_BUF_LEN);
	int maxMessagesExpected = (len / COMM_STR_BUF_LEN + (len % COMM_STR_BUF_LEN != 0));
	uint16_t bytesToWrite;
	int error;
    for(int i = 0; i < numBuffers; i++)
    {
		bytesToWrite = remainingBytes > CHUNK_SIZE ? CHUNK_SIZE : remainingBytes;
		error = update_rx_buf_usb(&largeRxBuffer[i*CHUNK_SIZE], bytesToWrite);
		if(error)
			qDebug() << "circ_buff_write error:" << error;

		remainingBytes -= bytesToWrite;

		int successfulParse = 0;
		do {
			successfulParse = tryParseRx(&commPeriph[PORT_USB], &packet[PORT_USB][INBOUND]);
			if(successfulParse)
			{
				signalSuccessfulParse();
				numMessagesReceived++;
			}
		} while(successfulParse && numMessagesReceived < maxMessagesExpected);
	}

    // Notify user in GUI: ... TODO: support 4 channels
    if(numMessagesReceived >= numMessagesExpected)
        emit dataStatus(0, DATAIN_STATUS_GREEN);
    else if(numMessagesReceived == 0)
        emit dataStatus(0, DATAIN_STATUS_RED);
    else
        emit dataStatus(0, DATAIN_STATUS_YELLOW);

	if(numMessagesReceived)
        emit newDataTimeout(true); //Reset counter

	return;
}

void SerialDriver::addDevice(FlexseaDevice* device)
{
	if(!device) return;

	bool alreadyContainDevice = false;
	for(unsigned int i = 0; i < devices.size(); i++)
	{
		if(devices.at(i) == device)
		{
			alreadyContainDevice = true;
			i = devices.size();
		}
	}
	if(!alreadyContainDevice)
	{
		devices.push_back(device);
	}
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

