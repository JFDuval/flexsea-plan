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
#include "flexsea_generic.h"
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

ManageDevice::ManageDevice(void): FlexseaDevice()
{
	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveType = "manage";
}

ManageDevice::ManageDevice(manage_s *devicePtr): FlexseaDevice()
{
	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	mnList.append(devicePtr);
	serializedLength = header.length();
	slaveType = "manage";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString ManageDevice::getHeaderStr(void)
{
	return header.join(',');
}
QStringList ManageDevice::header = QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"
								<< "accel.x"
								<< "accel.y"
								<< "accel.z"
								<< "gyro.x"
								<< "gyro.y"
								<< "gyro.z"
								<< "digitalIn"
								<< "sw1"
								<< "analog0"
								<< "analog1"
								<< "analog2"
								<< "analog3"
								<< "analog4"
								<< "analog5"
								<< "analog6"
								<< "analog7"
								<< "Status1";

QString ManageDevice::getLastSerializedStr(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date		<< ',' << \
							timeStamp.last().ms			<< ',' << \
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
	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		appendEmptyLine();
		timeStamp.last().date		= (*splitLine)[0];
		timeStamp.last().ms			= (*splitLine)[1].toInt();
		mnList.last()->accel.x		= (*splitLine)[2].toInt();
		mnList.last()->accel.y		= (*splitLine)[3].toInt();
		mnList.last()->accel.z		= (*splitLine)[4].toInt();
		mnList.last()->gyro.x		= (*splitLine)[5].toInt();
		mnList.last()->gyro.y		= (*splitLine)[6].toInt();
		mnList.last()->gyro.z		= (*splitLine)[7].toInt();
		mnList.last()->digitalIn	= (*splitLine)[8].toInt();
		mnList.last()->sw1			= (*splitLine)[9].toInt();
		mnList.last()->analog[0]	= (*splitLine)[10].toInt();
		mnList.last()->analog[1]	= (*splitLine)[11].toInt();
		mnList.last()->analog[2]	= (*splitLine)[12].toInt();
		mnList.last()->analog[3]	= (*splitLine)[13].toInt();
		mnList.last()->analog[4]	= (*splitLine)[14].toInt();
		mnList.last()->analog[5]	= (*splitLine)[15].toInt();
		mnList.last()->analog[6]	= (*splitLine)[16].toInt();
		mnList.last()->analog[7]	= (*splitLine)[17].toInt();
		mnList.last()->status1		= (*splitLine)[18].toInt();
	}
}

void ManageDevice::clear(void)
{
	FlexseaDevice::clear();
	mnList.clear();
	timeStamp.clear();
}

void ManageDevice::appendEmptyLine(void)
{
	timeStamp.append(TimeStamp());
	mnList.append(new manage_s());
}

void ManageDevice::decodeLastLine(void)
{
	decode(mnList.last());
}

void ManageDevice::decodeAllLine(void)
{
	for(int i = 0; i < mnList.size(); ++i)
	{
		decode(mnList[i]);
	}
}

QString ManageDevice::getStatusStr(int index)
{
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

