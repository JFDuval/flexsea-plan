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
#include "flexsea_generic.h"
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

BatteryDevice::BatteryDevice(void): FlexseaDevice()
{
	this->dataSource = LogDataFile;
}

BatteryDevice::BatteryDevice(battery_s *devicePtr): FlexseaDevice()
{
	this->dataSource = LiveDataFile;
	baList.append(BatteryStamp());
	baList.last().data = devicePtr;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString BatteryDevice::getHeaderStr(void)
{
	return QString("Timestamp,")	+ \
				   "Timestamp (ms),"+ \
				   "voltage,"		+ \
				   "current,"		+ \
				   "temp,"			+ \
				   "pushbutton,"	+ \
				   "status";
}

QString BatteryDevice::getLastLineStr(void)
{
	QString str;
	QTextStream(&str) <<	lastTimeStampDate				<< ',' << \
							lastTimeStamp_ms				<< ',' << \
							baList.last().data->voltage		<< ',' << \
							baList.last().data->current		<< ',' << \
							baList.last().data->temp		<< ',' << \
							baList.last().data->pushbutton	<< ',' << \
							baList.last().data->status;
	return str;
}

void BatteryDevice::clear(void)
{
	FlexseaDevice::clear();
	baList.clear();
}

void BatteryDevice::appendEmptyLine(void)
{
	baList.append(BatteryStamp());
}

void BatteryDevice::decodeLastLine(void)
{
	decode(baList.last().data);
}

void BatteryDevice::decodeAllLine(void)
{
	for(int i = 0; i < baList.size(); ++i)
	{
		decode(baList[i].data);
	}
}

QString BatteryDevice::getStatusStr(int index)
{
	return QString("No decoding available for this board");
}

void BatteryDevice::decode(struct battery_s *baPtr)
{
	baPtr->decoded.voltage = baPtr->voltage;    //TODO mV
	baPtr->decoded.current = baPtr->current;    //TODO mA
	baPtr->decoded.power = baPtr->voltage * baPtr->current; // mW
	baPtr->decoded.temp = baPtr->temp;          //TODO C*10
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

