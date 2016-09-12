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

#include "main.h"
#include "serialdriver.h"
#include <QDebug>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

SerialDriver::SerialDriver(QWidget *parent) : QWidget(parent)
{
    comPortOpen = false;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

//Open port
int SerialDriver::open(QString name, int tries, int delay)
{
    int cnt = 0;
    bool fd = false;
    int comProgress = 0;

    emit openProgress(comProgress, 0);

    USBSerialPort.setPortName(name);
    USBSerialPort.setBaudRate(USBSerialPort.Baud115200);
    USBSerialPort.setDataBits(QSerialPort::Data8);
    USBSerialPort.setParity(QSerialPort::NoParity);
    USBSerialPort.setStopBits(QSerialPort::OneStop);
    USBSerialPort.setFlowControl(QSerialPort::NoFlowControl);

    do
    {
        fd = USBSerialPort.open(QIODevice::ReadWrite);  //returns true if successful
        cnt++;
        if(cnt >= tries)
            break;

        //When false, print error code:
        if(fd == false)
        {
            qDebug() << "Try #" << cnt << " failed. Error: " << USBSerialPort.errorString() << ".\n";
            emit openProgress(100*cnt/tries, 0);
        }

        usleep(delay);
    }while(fd != true);


    if (fd == false)
    {
        qDebug() << "Tried " << cnt << " times, couldn't open " << name << ".\n";
        emit openProgress(0, 1);
        comPortOpen = false;
        emit openStatus(comPortOpen);

        return 1;
    }
    else
    {
        qDebug() << "Successfully opened " << name << ".\n";
        emit openProgress(100, 0);
        comPortOpen = true;
        emit openStatus(comPortOpen);

        //Clear any data that was already in the buffers:
        //while(USBSerialPort.waitForReadyRead(100))
        {
            USBSerialPort.clear((QSerialPort::AllDirections));
        }

        return 0;
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
}

//Read
int SerialDriver::read(unsigned char *buf)
{
    QByteArray baData;
    baData.resize(256);
    bool dataReady = false;

    dataReady = USBSerialPort.waitForReadyRead(100);
    if(dataReady == true)
    {
        baData = USBSerialPort.readAll();

        //We check to see if we are getting reasonnable packets, or a bunch of crap:
        int len = baData.length();
        if(len > 256)
        {
            qDebug() << "Data length over 256 bytes (" << len << "bytes)";
            len = 256;
            USBSerialPort.clear((QSerialPort::AllDirections));
            emit dataStatus(0, DATAIN_STATUS_RED);
            return 0;
        }

        //qDebug() << "Read" << len << "bytes.";

        //Fill the rx buf with our new bytes:
        update_rx_buf_array_usb((uint8_t *)baData.data(), len);
    }
    else
    {
        //qDebug("No USB bytes available.");
        emit dataStatus(0, DATAIN_STATUS_RED);
        return 0;
    }

    //Notify user in GUI:
    emit dataStatus(0, DATAIN_STATUS_GREEN);   //***ToDo: support 4 channels
    emit newDataTimeout(true); //Reset counter
    return 1;
}
//***ToDo***: emit dataStatus(0,x) needs to support 4 channel, not to
//always send channel 0

//Write
int SerialDriver::write(char bytes_to_send, unsigned char *serial_tx_data)
{
    qint64 write_ret = 0;
    QByteArray myQBArray;
    myQBArray = QByteArray::fromRawData((const char*)serial_tx_data, bytes_to_send);

    //Check if COM was successfully opened:

    if(comPortOpen == true)
    {
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

void SerialDriver::readWrite(uint numb, uint8_t *dataPacket, uint8_t r_w)
{
    write(numb, dataPacket);
    //qDebug() << dataPacket;

    //Should we look for a reply?
    if(r_w == READ)
    {
        //Status to Yellow before we get the reply:
        emit dataStatus(0, DATAIN_STATUS_YELLOW);

        //Did we receive data? Can we decode it?
        if(read(usb_rx))
        {
            decode_usb_rx(usb_rx);
            emit newDataReady();
        }
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

