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
	if(header.length() != headerUnitList.length())
	{
		qDebug() << "Mismatch between header length Gossip!";
	}

	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveTypeName = "gossip";
}

GossipDevice::GossipDevice(gossip_s *devicePtr): FlexseaDevice()
{
	if(header.length() != headerUnitList.length())
	{
		qDebug() << "Mismatch between header length Gossip!";
	}

	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	goList.append(devicePtr);
	eventFlags.append(0);
	serializedLength = header.length();
	slaveTypeName = "gossip";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QStringList GossipDevice::header = QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"
								<< "Event Flags"
								<< "Accel X"
								<< "Accel Y"
								<< "Accel Z"
								<< "Gyro X"
								<< "Gyro Y"
								<< "Gyro Z"
								<< "Magneto X"
								<< "Magneto X"
								<< "Magneto Z"
								<< "IO[0]"
								<< "IO[0]"
								<< "CapSense[1]"
								<< "CapSense[2]"
								<< "CapSense[3]"
								<< "CapSense[4]"
								<< "Status";

QStringList GossipDevice::headerUnitList = QStringList()
								<< "Raw Value Only"
								<< "Raw Value Only"
								<< "Raw Value Only"
								<< "Decoded: mg"
								<< "Decoded: mg"
								<< "Decoded: mg"
								<< "Decoded: deg/s"
								<< "Decoded: deg/s"
								<< "Decoded: deg/s"
								<< "Decoded: uT"
								<< "Decoded: uT"
								<< "Decoded: uT"
								<< "Raw value only"
								<< "Raw value only"
								<< "Raw value only"
								<< "Raw value only"
								<< "Raw value only"
								<< "Raw value only"
								<< "Raw value only";

QString GossipDevice::getLastDataEntry(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date		<< ',' << \
							timeStamp.last().ms			<< ',' << \
							eventFlags.last()			<< ',' << \
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
	uint8_t idx = 0;

	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		appendEmptyElement();
		timeStamp.last().date		= (*splitLine)[idx++];
		timeStamp.last().ms			= (*splitLine)[idx++].toInt();
		eventFlags.last()			= (*splitLine)[idx++].toInt();

		goList.last()->accel.x		= (*splitLine)[idx++].toInt();
		goList.last()->accel.y		= (*splitLine)[idx++].toInt();
		goList.last()->accel.z		= (*splitLine)[idx++].toInt();
		goList.last()->gyro.x		= (*splitLine)[idx++].toInt();
		goList.last()->gyro.y		= (*splitLine)[idx++].toInt();
		goList.last()->gyro.z		= (*splitLine)[idx++].toInt();
		goList.last()->magneto.x	= (*splitLine)[idx++].toInt();
		goList.last()->magneto.y	= (*splitLine)[idx++].toInt();
		goList.last()->magneto.z	= (*splitLine)[idx++].toInt();
		goList.last()->io[0]		= (*splitLine)[idx++].toInt();
		goList.last()->io[1]		= (*splitLine)[idx++].toInt();
		goList.last()->capsense[0]	= (*splitLine)[idx++].toInt();
		goList.last()->capsense[1]	= (*splitLine)[idx++].toInt();
		goList.last()->capsense[2]	= (*splitLine)[idx++].toInt();
		goList.last()->capsense[3]	= (*splitLine)[idx++].toInt();
		goList.last()->status		= (*splitLine)[idx++].toInt();
	}
}

struct std_variable GossipDevice::getSerializedVar(int headerIndex, int index)
{
	struct std_variable var;

	if(index >= goList.length())
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
			var.rawGenPtr = &goList[index]->accel.x;
			var.decodedPtr = &goList[index]->decoded.accel.x;
			break;
		case 4: //"Accel Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &goList[index]->accel.y;
			var.decodedPtr = &goList[index]->decoded.accel.y;
			break;
		case 5: //"Accel Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &goList[index]->accel.z;
			var.decodedPtr = &goList[index]->decoded.accel.z;
			break;
		case 6: //"Gyro X"
			var.format = FORMAT_16S;
			var.rawGenPtr = &goList[index]->gyro.x;
			var.decodedPtr = &goList[index]->decoded.gyro.x;
			break;
		case 7: //"Gyro Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &goList[index]->gyro.y;
			var.decodedPtr = &goList[index]->decoded.gyro.y;
			break;
		case 8: //"Gyro Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &goList[index]->gyro.z;
			var.decodedPtr = &goList[index]->decoded.gyro.z;
			break;
		case 9: //"Magneto X"
			var.format = FORMAT_16S;
			var.rawGenPtr = &goList[index]->magneto.x;
			var.decodedPtr = &goList[index]->decoded.magneto.x;
			break;
		case 10: //"Magneto Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &goList[index]->magneto.y;
			var.decodedPtr = &goList[index]->decoded.magneto.y;
			break;
		case 11: //"Magneto Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &goList[index]->magneto.z;
			var.decodedPtr = &goList[index]->decoded.magneto.z;
			break;
		case 12: //"IO 1"
			var.format = FORMAT_16U;
			var.rawGenPtr = &goList[index]->io[0];
			var.decodedPtr = nullptr;
			break;
		case 13: //"IO 2"
			var.format = FORMAT_16U;
			var.rawGenPtr = &goList[index]->io[1];
			var.decodedPtr = nullptr;
			break;
		case 14: //"Capsense 1"
			var.format = FORMAT_16U;
			var.rawGenPtr = &goList[index]->capsense[0];
			var.decodedPtr = nullptr;
			break;
		case 15: //"Capsense 2"
			var.format = FORMAT_16U;
			var.rawGenPtr = &goList[index]->capsense[1];
			var.decodedPtr = nullptr;
		case 16: //"Capsense 3"
			var.format = FORMAT_16U;
			var.rawGenPtr = &goList[index]->capsense[2];
			var.decodedPtr = nullptr;
		case 17: //"Capsense 4"
			var.format = FORMAT_16U;
			var.rawGenPtr = &goList[index]->capsense[3];
			var.decodedPtr = nullptr;
		case 18: //"Status"
			var.format = FORMAT_8U;
			var.rawGenPtr = &goList[index]->status;
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

void GossipDevice::clear(void)
{
	FlexseaDevice::clear();
	goList.clear();
	timeStamp.clear();
	eventFlags.clear();
}

void GossipDevice::appendEmptyElement(void)
{
	timeStamp.append(TimeStamp());
	goList.append(new gossip_s());
	eventFlags.append(0);
}

void GossipDevice::decodeLastElement(void)
{
	decode(goList.last());
}

void GossipDevice::decodeAllElement(void)
{
	for(int i = 0; i < goList.size(); ++i)
	{
		decode(goList[i]);
	}
}

QString GossipDevice::getStatusStr(int index)
{
	(void)index;	//Unused

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

