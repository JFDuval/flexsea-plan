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
	[This file] ManageDevice: Manage Device Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-08 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "manageDevice.h"
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

ManageDevice::ManageDevice(void): FlexseaDevice()
{
	if(header.length() != headerUnitList.length())
	{
		qDebug() << "Mismatch between header length Manage!";
	}

	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveTypeName = "manage";
}

ManageDevice::ManageDevice(manage_s *devicePtr): FlexseaDevice()
{
	if(header.length() != headerUnitList.length())
	{
		qDebug() << "Mismatch between header length Manage!";
	}

	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	mnList.append(devicePtr);
	eventFlags.append(0);
	serializedLength = header.length();
	slaveTypeName = "manage";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QStringList ManageDevice::header = QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"
								<< "Event Flags"
								<< "Accel X"
								<< "Accel Y"
								<< "Accel Z"
								<< "Gyro X"
								<< "Gyro Y"
								<< "Gyro Z"
								<< "Digital Inputs"
								<< "Pushbutton"
								<< "Analog[0]"
								<< "Analog[1]"
								<< "Analog[2]"
								<< "Analog[3]"
								<< "Analog[4]"
								<< "Analog[5]"
								<< "Analog[6]"
								<< "Analog[7]"
								<< "Status";

QStringList ManageDevice::headerUnitList = QStringList()
								<< "Raw Value Only"
								<< "Raw Value Only"
								<< "Raw Value Only"
								<< "Decoded: mg"
								<< "Decoded: mg"
								<< "Decoded: mg"
								<< "Decoded: deg/s"
								<< "Decoded: deg/s"
								<< "Decoded: deg/s"
								<< "Raw Value Only"
								<< "Raw Value Only"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Raw Value Only";

QString ManageDevice::getLastDataEntry(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date		<< ',' << \
							timeStamp.last().ms			<< ',' << \
							eventFlags.last()				<< ',' << \
							mnList.last()->accel.x		<< ',' << \
							mnList.last()->accel.y		<< ',' << \
							mnList.last()->accel.z		<< ',' << \
							mnList.last()->gyro.x		<< ',' << \
							mnList.last()->gyro.y		<< ',' << \
							mnList.last()->gyro.z		<< ',' << \
							mnList.last()->digitalIn	<< ',' << \
							mnList.last()->sw1			<< ',' << \
							mnList.last()->analog[0]	<< ',' << \
							mnList.last()->analog[1]	<< ',' << \
							mnList.last()->analog[2]	<< ',' << \
							mnList.last()->analog[3]	<< ',' << \
							mnList.last()->analog[4]	<< ',' << \
							mnList.last()->analog[5]	<< ',' << \
							mnList.last()->analog[6]	<< ',' << \
							mnList.last()->analog[7]	<< ',' << \
							mnList.last()->status1;
	return str;
}

void ManageDevice::appendSerializedStr(QStringList *splitLine)
{
	uint8_t idx = 0;

	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		appendEmptyElement();
		timeStamp.last().date		= (*splitLine)[idx++];
		timeStamp.last().ms			= (*splitLine)[idx++].toInt();
		eventFlags.last()			= (*splitLine)[idx++].toInt();

		mnList.last()->accel.x		= (*splitLine)[idx++].toInt();
		mnList.last()->accel.y		= (*splitLine)[idx++].toInt();
		mnList.last()->accel.z		= (*splitLine)[idx++].toInt();
		mnList.last()->gyro.x		= (*splitLine)[idx++].toInt();
		mnList.last()->gyro.y		= (*splitLine)[idx++].toInt();
		mnList.last()->gyro.z		= (*splitLine)[idx++].toInt();
		mnList.last()->digitalIn	= (*splitLine)[idx++].toInt();
		mnList.last()->sw1			= (*splitLine)[idx++].toInt();
		mnList.last()->analog[0]	= (*splitLine)[idx++].toInt();
		mnList.last()->analog[1]	= (*splitLine)[idx++].toInt();
		mnList.last()->analog[2]	= (*splitLine)[idx++].toInt();
		mnList.last()->analog[3]	= (*splitLine)[idx++].toInt();
		mnList.last()->analog[4]	= (*splitLine)[idx++].toInt();
		mnList.last()->analog[5]	= (*splitLine)[idx++].toInt();
		mnList.last()->analog[6]	= (*splitLine)[idx++].toInt();
		mnList.last()->analog[7]	= (*splitLine)[idx++].toInt();
		mnList.last()->status1		= (*splitLine)[idx++].toInt();
	}
}

struct std_variable ManageDevice::getSerializedVar(int headerIndex, int index)
{
	struct std_variable var;

	if(index >= mnList.length())
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
		case 2: //"Event Flag"
			var.format = FORMAT_32S;
			var.rawGenPtr = &eventFlags[index];
			var.decodedPtr = nullptr;
			break;
		case 3: //"Accel X"
			var.format = FORMAT_16S;
			var.rawGenPtr = &mnList[index]->accel.x;
			var.decodedPtr = &mnList[index]->decoded.accel.x;
			break;
		case 4: //"Accel Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &mnList[index]->accel.y;
			var.decodedPtr = &mnList[index]->decoded.accel.y;
			break;
		case 5: //"Accel Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &mnList[index]->accel.z;
			var.decodedPtr = &mnList[index]->decoded.accel.z;
			break;
		case 6: //"Gyro X"
			var.format = FORMAT_16S;
			var.rawGenPtr = &mnList[index]->gyro.x;
			var.decodedPtr = &mnList[index]->decoded.gyro.x;
			break;
		case 7: //"Gyro Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &mnList[index]->gyro.y;
			var.decodedPtr = &mnList[index]->decoded.gyro.y;
			break;
		case 8: //"Gyro Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &mnList[index]->gyro.z;
			var.decodedPtr = &mnList[index]->decoded.gyro.z;
			break;
		case 9: //"Digital inputs"
			var.format = FORMAT_16U;
			var.rawGenPtr = &mnList[index]->digitalIn;
			var.decodedPtr = nullptr;
			break;
		case 10: //"Pushbutton"
			var.format = FORMAT_8U;
			var.rawGenPtr = &mnList[index]->sw1;
			var.decodedPtr = nullptr;
			break;
		case 11: //"Analog[0]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &mnList[index]->analog[0];
			var.decodedPtr = &mnList[index]->decoded.analog[0];
			break;
		case 12: //Analog[1]
			var.format = FORMAT_16U;
			var.rawGenPtr = &mnList[index]->analog[1];
			var.decodedPtr = &mnList[index]->decoded.analog[1];
			break;
		case 13: //"Analog[2]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &mnList[index]->analog[2];
			var.decodedPtr = &mnList[index]->decoded.analog[2];
			break;
		case 14: //Analog[3]
			var.format = FORMAT_16U;
			var.rawGenPtr = &mnList[index]->analog[3];
			var.decodedPtr = &mnList[index]->decoded.analog[3];
			break;
		case 15: //"Analog[4]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &mnList[index]->analog[4];
			var.decodedPtr = &mnList[index]->decoded.analog[4];
			break;
		case 16: //Analog[5]
			var.format = FORMAT_16U;
			var.rawGenPtr = &mnList[index]->analog[5];
			var.decodedPtr = &mnList[index]->decoded.analog[5];
			break;
		case 17: //"Analog[6]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &mnList[index]->analog[6];
			var.decodedPtr = &mnList[index]->decoded.analog[6];
			break;
		case 18: //Analog[7]
			var.format = FORMAT_16U;
			var.rawGenPtr = &mnList[index]->analog[7];
			var.decodedPtr = &mnList[index]->decoded.analog[7];
			break;
		case 19: //"Status"
			var.format = FORMAT_8U;
			var.rawGenPtr = &mnList[index]->status1;
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

void ManageDevice::clear(void)
{
	FlexseaDevice::clear();
	mnList.clear();
	timeStamp.clear();
	eventFlags.clear();
}

void ManageDevice::appendEmptyElement(void)
{
	timeStamp.append(TimeStamp());
	mnList.append(new manage_s());
	eventFlags.append(0);
}

void ManageDevice::decodeLastElement(void)
{
	decode(mnList.last());
}

void ManageDevice::decodeAllElement(void)
{
	for(int i = 0; i < mnList.size(); ++i)
	{
		decode(mnList[i]);
	}
}

QString ManageDevice::getStatusStr(int index)
{
	(void)index;	//Unused

	return QString("No decoding available for this board");
}

void ManageDevice::decode(struct manage_s *mnPtr)
{
	//Accel in mG
	mnPtr->decoded.accel.x = (1000*mnPtr->accel.x)/8192;
	mnPtr->decoded.accel.y = (1000*mnPtr->accel.y)/8192;
	mnPtr->decoded.accel.z = (1000*mnPtr->accel.z)/8192;

	//Gyro in degrees/s
	mnPtr->decoded.gyro.x = (100*mnPtr->gyro.x)/164;
	mnPtr->decoded.gyro.y = (100*mnPtr->gyro.y)/164;
	mnPtr->decoded.gyro.z = (100*mnPtr->gyro.z)/164;

	mnPtr->decoded.analog[0] = (int32_t)1000*((float)mnPtr->analog[0]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[1] = (int32_t)1000*((float)mnPtr->analog[1]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[2] = (int32_t)1000*((float)mnPtr->analog[2]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[3] = (int32_t)1000*((float)mnPtr->analog[3]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[4] = (int32_t)1000*((float)mnPtr->analog[4]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[5] = (int32_t)1000*((float)mnPtr->analog[5]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[6] = (int32_t)1000*((float)mnPtr->analog[6]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[7] = (int32_t)1000*((float)mnPtr->analog[7]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
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

