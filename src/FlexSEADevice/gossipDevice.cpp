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
	[This file] GossipDevice: Gossip Device Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-08 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "gossipDevice.h"
#include "flexsea_generic.h"
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

GossipDevice::GossipDevice(void): FlexseaDevice()
{
	this->dataSource = LogDataFile;
}

GossipDevice::GossipDevice(gossip_s *devicePtr): FlexseaDevice()
{
	this->dataSource = LiveDataFile;
	goList.append(GossipStamp());
	goList.last().data = devicePtr;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString GossipDevice::getHeaderStr(void)
{
	return QString("Timestamp,")	+ \
				   "Timestamp (ms),"+ \
				   "accel.x,"		+ \
				   "accel.y,"		+ \
				   "accel.z,"		+ \
				   "gyro.x,"		+ \
				   "gyro.y,"		+ \
				   "gyro.z,"		+ \
				   "magneto.x,"		+ \
				   "magneto.y,"		+ \
				   "magneto.z,"		+ \
				   "io1,"			+ \
				   "io2,"			+ \
				   "capsense1,"		+ \
				   "capsense2,"		+ \
				   "capsense3,"		+ \
				   "capsense4,"		+ \
				   "Status1";
}

QString GossipDevice::getLastLineStr(void)
{
	QString str;
	QTextStream(&str) <<	lastTimeStampDate				<< ',' << \
							lastTimeStamp_ms				<< ',' << \
							goList.last().data->accel.x		<< ',' << \
							goList.last().data->accel.y		<< ',' << \
							goList.last().data->accel.z		<< ',' << \
							goList.last().data->gyro.x		<< ',' << \
							goList.last().data->gyro.y		<< ',' << \
							goList.last().data->gyro.z		<< ',' << \
							goList.last().data->magneto.x	<< ',' << \
							goList.last().data->magneto.y	<< ',' << \
							goList.last().data->magneto.z	<< ',' << \
							goList.last().data->io[0]		<< ',' << \
							goList.last().data->io[1]		<< ',' << \
							goList.last().data->capsense[0]	<< ',' << \
							goList.last().data->capsense[1]	<< ',' << \
							goList.last().data->capsense[2]	<< ',' << \
							goList.last().data->capsense[3]	<< ',' << \
							goList.last().data->status;
	return str;
}

void GossipDevice::clear(void)
{
	FlexseaDevice::clear();
	goList.clear();
}

void GossipDevice::appendEmptyLine(void)
{
	goList.append(GossipStamp());
}

void GossipDevice::decodeLastLine(void)
{
	decode(goList.last().data);
}

void GossipDevice::decodeAllLine(void)
{
	for(int i = 0; i < goList.size(); ++i)
	{
		decode(goList[i].data);
	}
}

QString GossipDevice::getStatusStr(int index)
{
	return QString("No decoding available for this board");
}

void GossipDevice::decode(struct gossip_s *goPtr)
{
	//Accel in mG
	goPtr->decoded.accel.x = (1000*goPtr->accel.x)/8192;
	goPtr->decoded.accel.y = (1000*goPtr->accel.y)/8192;
	goPtr->decoded.accel.z = (1000*goPtr->accel.z)/8192;

	//Gyro in degrees/s
	goPtr->decoded.gyro.x = (100*goPtr->gyro.x)/164;
	goPtr->decoded.gyro.y = (100*goPtr->gyro.y)/164;
	goPtr->decoded.gyro.z = (100*goPtr->gyro.z)/164;

	//Magneto in uT (0.15uT/LSB)
	goPtr->decoded.magneto.x = (15*goPtr->magneto.x)/100;
	goPtr->decoded.magneto.y = (15*goPtr->magneto.y)/100;
	goPtr->decoded.magneto.z = (15*goPtr->magneto.z)/100;
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

