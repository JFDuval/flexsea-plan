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
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

GossipDevice::GossipDevice(void): FlexseaDevice()
{
	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveType = "gossip";
}

GossipDevice::GossipDevice(gossip_s *devicePtr): FlexseaDevice()
{
	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	goList.append(devicePtr);
	serializedLength = header.length();
	slaveType = "gossip";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString GossipDevice::getHeaderStr(void)
{
	return header.join(',');
}

QStringList GossipDevice::header = QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"
								<< "accel.x"
								<< "accel.y"
								<< "accel.z"
								<< "gyro.x"
								<< "gyro.y"
								<< "gyro.z"
								<< "magneto.x"
								<< "magneto.y"
								<< "magneto.z"
								<< "io1"
								<< "io2"
								<< "capsense1"
								<< "capsense2"
								<< "capsense3"
								<< "capsense4"
								<< "Status1";

QString GossipDevice::getLastSerializedStr(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date		<< ',' << \
							timeStamp.last().ms			<< ',' << \
							goList.last()->accel.x		<< ',' << \
							goList.last()->accel.y		<< ',' << \
							goList.last()->accel.z		<< ',' << \
							goList.last()->gyro.x		<< ',' << \
							goList.last()->gyro.y		<< ',' << \
							goList.last()->gyro.z		<< ',' << \
							goList.last()->magneto.x	<< ',' << \
							goList.last()->magneto.y	<< ',' << \
							goList.last()->magneto.z	<< ',' << \
							goList.last()->io[0]		<< ',' << \
							goList.last()->io[1]		<< ',' << \
							goList.last()->capsense[0]	<< ',' << \
							goList.last()->capsense[1]	<< ',' << \
							goList.last()->capsense[2]	<< ',' << \
							goList.last()->capsense[3]	<< ',' << \
							goList.last()->status;
	return str;
}

void GossipDevice::appendSerializedStr(QStringList *splitLine)
{
	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		appendEmptyLine();
		timeStamp.last().date		= (*splitLine)[0];
		timeStamp.last().ms			= (*splitLine)[1].toInt();
		goList.last()->accel.x		= (*splitLine)[2].toInt();
		goList.last()->accel.y		= (*splitLine)[3].toInt();
		goList.last()->accel.z		= (*splitLine)[4].toInt();
		goList.last()->gyro.x		= (*splitLine)[5].toInt();
		goList.last()->gyro.y		= (*splitLine)[6].toInt();
		goList.last()->gyro.z		= (*splitLine)[7].toInt();
		goList.last()->magneto.x	= (*splitLine)[8].toInt();
		goList.last()->magneto.y	= (*splitLine)[9].toInt();
		goList.last()->magneto.z	= (*splitLine)[10].toInt();
		goList.last()->io[0]		= (*splitLine)[11].toInt();
		goList.last()->io[1]		= (*splitLine)[12].toInt();
		goList.last()->capsense[0]	= (*splitLine)[13].toInt();
		goList.last()->capsense[1]	= (*splitLine)[14].toInt();
		goList.last()->capsense[2]	= (*splitLine)[15].toInt();
		goList.last()->capsense[3]	= (*splitLine)[16].toInt();
		goList.last()->status		= (*splitLine)[17].toInt();
	}
}

void GossipDevice::clear(void)
{
	FlexseaDevice::clear();
	goList.clear();
	timeStamp.clear();
}

void GossipDevice::appendEmptyLine(void)
{
	timeStamp.append(TimeStamp());
	goList.append(new gossip_s());
}

void GossipDevice::decodeLastLine(void)
{
	decode(goList.last());
}

void GossipDevice::decodeAllLine(void)
{
	for(int i = 0; i < goList.size(); ++i)
	{
		decode(goList[i]);
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

