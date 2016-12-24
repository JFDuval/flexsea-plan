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
	[This file] StrainDevice: Strain Device Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-10 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "strainDevice.h"
#include "flexsea_generic.h"
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

StrainDevice::StrainDevice(void): FlexseaDevice()
{
	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveType = "strain";
}

StrainDevice::StrainDevice(strain_s *devicePtr): FlexseaDevice()
{
	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	stList.append(devicePtr);
	serializedLength = header.length();
	slaveType = "strain";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString StrainDevice::getHeaderStr(void)
{
	return header.join(',');
}

QStringList StrainDevice::header = QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"
								<< "ch1"
								<< "ch2"
								<< "ch3"
								<< "ch4"
								<< "ch5"
								<< "ch6";

QString StrainDevice::getLastSerializedStr(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date					<< ',' << \
							timeStamp.last().ms						 << ',' << \
							stList.last()->ch[0].strain_filtered	<< ',' << \
							stList.last()->ch[1].strain_filtered	<< ',' << \
							stList.last()->ch[2].strain_filtered	<< ',' << \
							stList.last()->ch[3].strain_filtered	<< ',' << \
							stList.last()->ch[4].strain_filtered	<< ',' << \
							stList.last()->ch[5].strain_filtered;
	return str;
}

void StrainDevice::appendSerializedStr(QStringList *splitLine)
{
	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		appendEmptyLine();
		timeStamp.last().date					= (*splitLine)[0];
		timeStamp.last().ms						= (*splitLine)[1].toInt();
		stList.last()->ch[0].strain_filtered	= (*splitLine)[2].toInt();
		stList.last()->ch[1].strain_filtered	= (*splitLine)[3].toInt();
		stList.last()->ch[2].strain_filtered	= (*splitLine)[4].toInt();
		stList.last()->ch[3].strain_filtered	= (*splitLine)[5].toInt();
		stList.last()->ch[4].strain_filtered	= (*splitLine)[6].toInt();
		stList.last()->ch[5].strain_filtered	= (*splitLine)[7].toInt();
	}
}

void StrainDevice::clear(void)
{
	FlexseaDevice::clear();
	stList.clear();
	timeStamp.clear();
}

void StrainDevice::appendEmptyLine(void)
{
	timeStamp.append(TimeStamp());
	stList.append(new strain_s());
}

void StrainDevice::decodeLastLine(void)
{
	if(dataSource == LiveDataFile){decompressbytes6ch(stList.last());}
	decode(stList.last());
}

void StrainDevice::decodeAllLine(void)
{
	for(int i = 0; i < stList.size(); ++i)
	{
		if(dataSource == LiveDataFile){decompressbytes6ch(stList.last());}
		decode(stList[i]);
	}
}

QString StrainDevice::getStatusStr(int index)
{
	return QString("No decoding available for this board");
}

void StrainDevice::decode(struct strain_s *stPtr)
{
	stPtr->decoded.strain[0] = (100*(stPtr->ch[0].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[1] = (100*(stPtr->ch[1].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[2] = (100*(stPtr->ch[2].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[3] = (100*(stPtr->ch[3].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[4] = (100*(stPtr->ch[4].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[5] = (100*(stPtr->ch[5].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
}

void StrainDevice::decompressbytes6ch(struct strain_s *stPtr)
{
	uint8_t *buf = stPtr->compressedBytes;

	stPtr->ch[0].strain_filtered = ((*(buf+0) << 8 | *(buf+1)) >> 4);
	stPtr->ch[1].strain_filtered = (((*(buf+1) << 8 | *(buf+2))) & 0xFFF);
	stPtr->ch[2].strain_filtered = ((*(buf+3) << 8 | *(buf+4)) >> 4);
	stPtr->ch[3].strain_filtered = (((*(buf+4) << 8 | *(buf+5))) & 0xFFF);
	stPtr->ch[4].strain_filtered = ((*(buf+6) << 8 | *(buf+7)) >> 4);
	stPtr->ch[5].strain_filtered = (((*(buf+7) << 8 | *(buf+8))) & 0xFFF);
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

