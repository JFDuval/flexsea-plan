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
	[This file] batteryDevice: Battery Device Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-08 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "batteryDevice.h"
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

BatteryDevice::BatteryDevice(void): FlexseaDevice()
{
	if(header.length() != headerUnitList.length())
	{
		qDebug() << "Mismatch between header length Battery!";
	}

	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveTypeName = "battery";
}

BatteryDevice::BatteryDevice(battery_s *devicePtr): FlexseaDevice()
{
	if(header.length() != headerUnitList.length())
	{
		qDebug() << "Mismatch between header length Battery!";
	}

	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	baList.append(devicePtr);
	serializedLength = header.length();
	slaveTypeName = "battery";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QStringList BatteryDevice::header =	QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"
								<< "Battery Voltage"
								<< "Battery Current"
								// Special case, we want to display power even
								// if it's a decoded value
								<< "Power"
								<< "Temperature"
								<< "Pushbutton"
								<< "Status";

QStringList BatteryDevice::headerUnitList = QStringList()
								<< "Raw Value Only"
								<< "Raw Value Only"
								<< "Decoded: mV"
								<< "Decoded: mA"

								<< "Decoded: mW"
								<< "Decoded: 10x C"
								<< "Raw Values Only"
								<< "Raw Values Only";

QString BatteryDevice::getLastDataEntry(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date		<< ',' << \
							timeStamp.last().ms			<< ',' << \
							baList.last()->voltage		<< ',' << \
							baList.last()->current		<< ',' << \
							// Special case, we want to display power even
							// if it's a decoded value
							baList.last()->decoded.power<< ',' << \
							baList.last()->temp			<< ',' << \
							baList.last()->pushbutton	<< ',' << \
							baList.last()->status;
	return str;
}

void BatteryDevice::appendSerializedStr(QStringList *splitLine)
{
	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		appendEmptyElement();
		timeStamp.last().date		= (*splitLine)[0];
		timeStamp.last().ms			= (*splitLine)[1].toInt();
		baList.last()->voltage		= (*splitLine)[2].toInt();
		baList.last()->current		= (*splitLine)[3].toInt();
		baList.last()->decoded.power= (*splitLine)[4].toInt();
		baList.last()->temp			= (*splitLine)[5].toInt();
		baList.last()->pushbutton	= (*splitLine)[6].toInt();
		baList.last()->status		= (*splitLine)[7].toInt();
	}
}

struct std_variable BatteryDevice::getSerializedVar(int headerIndex, int index)
{
	struct std_variable var;

	if(index >= baList.length())
	{
		headerIndex = INT_MAX;
	}

	//Assign pointer:
	switch(headerIndex)
	{
		/*Format: (every Case except Unused)
		 * Line 1: data format, raw variable
		 * Line 2: raw variable
		 * Line 3: decoded variable (always int32),
					null if not decoded  */
		case 0: //"TimeStamp"
			var.format = FORMAT_QSTR;
			var.rawGenPtr = &timeStamp[index].date;
			var.decodedPtr = nullptr;
			break;
		case 1: //"TimeStamp (ms)"
			var.format = FORMAT_32S;
			var.rawGenPtr = &timeStamp[index].ms;
			var.decodedPtr = nullptr;
			break;
		case 2: //"Voltage"
			var.format = FORMAT_16U;
			var.rawGenPtr = &baList[index]->voltage;
			var.decodedPtr = &baList[index]->decoded.voltage;
			break;
		case 3: //"Current"
			var.format = FORMAT_16S;
			var.rawGenPtr = &baList[index]->current;
			var.decodedPtr = &baList[index]->decoded.current;
			break;
		case 4: //"Power"
			var.format = NULL_PTR;
			var.rawGenPtr = nullptr;
			var.decodedPtr = &baList[index]->decoded.power;
			break;
		case 5: //"Temperature"
			var.format = FORMAT_8U;
			var.rawGenPtr = &baList[index]->temp;
			var.decodedPtr = &baList[index]->decoded.temp;
			break;
		case 6: //"Pushbutton"
			var.format = FORMAT_8U;
			var.rawGenPtr = &baList[index]->pushbutton;
			var.decodedPtr = nullptr;
			break;
		case 7: //"Status"
			var.format = FORMAT_8U;
			var.rawGenPtr = &baList[index]->status;
			var.decodedPtr = nullptr;
			break;
		default:
			var.format = NULL_PTR;
			var.rawGenPtr = nullptr;
			var.decodedPtr = nullptr;
			break;
	}

	return var;
}

void BatteryDevice::clear(void)
{
	FlexseaDevice::clear();
	baList.clear();
	timeStamp.clear();
}

void BatteryDevice::appendEmptyElement(void)
{
	timeStamp.append(TimeStamp());
	baList.append(new battery_s());
}

void BatteryDevice::decodeLastElement(void)
{
	if(dataSource == LiveDataFile){decompressRawBytes(baList.last());}
	decode(baList.last());
}

void BatteryDevice::decodeAllElement(void)
{
	for(int i = 0; i < baList.size(); ++i)
	{
		decode(baList[i]);
	}
}

QString BatteryDevice::getStatusStr(int index)
{
	(void)index;	//Unused

	return QString("No decoding available for this board");
}

void BatteryDevice::decode(struct battery_s *baPtr)
{
	baPtr->decoded.voltage = baPtr->voltage;    //TODO mV
	baPtr->decoded.current = baPtr->current;    //TODO mA
	baPtr->decoded.power = baPtr->voltage * baPtr->current; // TODO mW
	baPtr->decoded.temp = baPtr->temp;          //TODO C*10
}

void BatteryDevice::decompressRawBytes(struct battery_s *baPtr)
{
	//Raw bytes to raw values:
	//========================

	baPtr->status = baPtr->rawBytes[0];
	baPtr->voltage = (baPtr->rawBytes[2] << 8) + baPtr->rawBytes[3];
	baPtr->current = (baPtr->rawBytes[4] << 8) + baPtr->rawBytes[5];
	baPtr->temp = baPtr->rawBytes[6];
}

//****************************************************************************
// Public slot(s):
//****************************************************************************


//****************************************************************************
// Private function(s):
//****************************************************************************


//****************************************************************************
// Private slot(s):
//****************************************************************************

