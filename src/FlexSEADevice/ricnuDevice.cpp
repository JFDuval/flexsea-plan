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
	[This file] RicnuDevice: Ricnu Device Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-08 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "ricnuDevice.h"
#include <QDebug>
#include <QTextStream>
#include "executeDevice.h"
#include "strainDevice.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

RicnuDevice::RicnuDevice(void): FlexseaDevice()
{
	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveType = "ricnu";
}

RicnuDevice::RicnuDevice(execute_s *exPtr, strain_s *stPtr): FlexseaDevice()
{
	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	riList.append(new ricnu_s_plan());
	riList.last()->ex = exPtr;
	riList.last()->st = stPtr;
	serializedLength = header.length();
	slaveType = "ricnu";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString RicnuDevice::getHeaderStr(void)
{
	return header.join(',');
}

QStringList RicnuDevice::header = QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"
								<< "accel.x"
								<< "accel.y"
								<< "accel.z"
								<< "gyro.x"
								<< "gyro.y"
								<< "gyro.z"
								<< "current"
								<< "enc-mot"
								<< "enc-joint"
								<< "strain1"
								<< "strain2"
								<< "strain3"
								<< "strain4"
								<< "strain5"
								<< "strain6";

QString RicnuDevice::getLastSerializedStr(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date						<< ',' << \
							timeStamp.last().ms							<< ',' << \
							riList.last()->ex->accel.x					<< ',' << \
							riList.last()->ex->accel.y					<< ',' << \
							riList.last()->ex->accel.z					<< ',' << \
							riList.last()->ex->gyro.x					<< ',' << \
							riList.last()->ex->gyro.y					<< ',' << \
							riList.last()->ex->gyro.z					<< ',' << \
							riList.last()->ex->current					<< ',' << \
							riList.last()->ex->enc_motor				<< ',' << \
							riList.last()->ex->enc_joint				<< ',' << \
							riList.last()->st->ch[0].strain_filtered	<< ',' << \
							riList.last()->st->ch[1].strain_filtered	<< ',' << \
							riList.last()->st->ch[2].strain_filtered	<< ',' << \
							riList.last()->st->ch[3].strain_filtered	<< ',' << \
							riList.last()->st->ch[4].strain_filtered	<< ',' << \
							riList.last()->st->ch[5].strain_filtered;
	return str;
}

void RicnuDevice::appendSerializedStr(QStringList *splitLine)
{
	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		// Because of the pointer architecture of ricnu_s_plan , we need to
		// also add execute and strain structure
		appendEmptyLineWithExAndStStruct();

		timeStamp.last().date						= (*splitLine)[0];
		timeStamp.last().ms							= (*splitLine)[1].toInt();
		riList.last()->ex->accel.x					= (*splitLine)[2].toInt();
		riList.last()->ex->accel.y					= (*splitLine)[3].toInt();
		riList.last()->ex->accel.z					= (*splitLine)[4].toInt();
		riList.last()->ex->gyro.x					= (*splitLine)[5].toInt();
		riList.last()->ex->gyro.y					= (*splitLine)[6].toInt();
		riList.last()->ex->gyro.z					= (*splitLine)[7].toInt();
		riList.last()->ex->current					= (*splitLine)[8].toInt();
		riList.last()->ex->enc_motor				= (*splitLine)[9].toInt();
		riList.last()->ex->enc_joint				= (*splitLine)[10].toInt();
		riList.last()->st->ch[0].strain_filtered	= (*splitLine)[11].toInt();
		riList.last()->st->ch[1].strain_filtered	= (*splitLine)[12].toInt();
		riList.last()->st->ch[2].strain_filtered	= (*splitLine)[13].toInt();
		riList.last()->st->ch[3].strain_filtered	= (*splitLine)[14].toInt();
		riList.last()->st->ch[4].strain_filtered	= (*splitLine)[15].toInt();
		riList.last()->st->ch[5].strain_filtered	= (*splitLine)[16].toInt();
	}
}

void RicnuDevice::clear(void)
{
	FlexseaDevice::clear();
	riList.clear();
	timeStamp.clear();
}

void RicnuDevice::appendEmptyLine(void)
{
	timeStamp.append(TimeStamp());
	riList.append(new ricnu_s_plan());
}

void RicnuDevice::appendEmptyLineWithExAndStStruct(void)
{
	timeStamp.append(TimeStamp());
	riList.append(new ricnu_s_plan());
	riList.last()->ex = new execute_s();
	riList.last()->st = new strain_s();
}

void RicnuDevice::decodeLastLine(void)
{
	if(dataSource == LiveDataFile){StrainDevice::decompressRawBytes(riList.last()->st);}
	decode(riList.last());
}

void RicnuDevice::decodeAllLine(void)
{
	for(int i = 0; i < riList.size(); ++i)
	{
		if(dataSource == LiveDataFile){StrainDevice::decompressRawBytes(riList[i]->st);}
		decode(riList[i]);
	}
}

void RicnuDevice::decode(struct ricnu_s *riPtr)
{
	ExecuteDevice::decode(&riPtr->ex);
	StrainDevice::decode(&riPtr->st);
}

void RicnuDevice::decode(struct ricnu_s_plan *riPtr)
{
	ExecuteDevice::decode(riPtr->ex);
	StrainDevice::decode(riPtr->st);
}

QString RicnuDevice::getStatusStr(int index)
{
	return QString("No decoding available for this board");
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

