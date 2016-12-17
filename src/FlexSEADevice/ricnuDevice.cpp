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
#include "flexsea_generic.h"
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
}

RicnuDevice::RicnuDevice(ricnu_s *devicePtr): FlexseaDevice()
{
	this->dataSource = LiveDataFile;
	riList.append(RicnuStamp());
	riList.last().data = devicePtr;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString RicnuDevice::getHeaderStr(void)
{
	return QString("Timestamp,")	+ \
				   "Timestamp (ms),"+ \
				   "accel.x,"		+ \
				   "accel.y,"		+ \
				   "accel.z,"		+ \
				   "gyro.x,"		+ \
				   "gyro.y,"		+ \
				   "gyro.z,"		+ \
				   "current,"		+ \
				   "enc-mot,"		+ \
				   "enc-joint,"		+ \
				   "strain1,"		+ \
				   "strain2,"		+ \
				   "strain3,"		+ \
				   "strain4,"		+ \
				   "strain5,"		+ \
				   "strain6";
}

QString RicnuDevice::getLastLineStr(void)
{
	unpackCompressed6ch(&riList.last().data->st);

	QString str;
	QTextStream(&str) <<	riList.last().timeStampDate					<< ',' << \
							riList.last().timeStamp_ms					<< ',' << \
							riList.last().data->ex.accel.x				<< ',' << \
							riList.last().data->ex.accel.y				<< ',' << \
							riList.last().data->ex.accel.z				<< ',' << \
							riList.last().data->ex.gyro.x				<< ',' << \
							riList.last().data->ex.gyro.y				<< ',' << \
							riList.last().data->ex.gyro.z				<< ',' << \
							riList.last().data->ex.current				<< ',' << \
							riList.last().data->ex.enc_motor				<< ',' << \
							riList.last().data->ex.enc_joint				<< ',' << \
							riList.last().data->st.ch[0].strain_filtered	<< ',' << \
							riList.last().data->st.ch[1].strain_filtered << ',' << \
							riList.last().data->st.ch[2].strain_filtered << ',' << \
							riList.last().data->st.ch[3].strain_filtered << ',' << \
							riList.last().data->st.ch[4].strain_filtered << ',' << \
							riList.last().data->st.ch[5].strain_filtered;
	return str;
}

void RicnuDevice::clear(void)
{
	FlexseaDevice::clear();
	riList.clear();
}

void RicnuDevice::appendEmptyLine(void)
{
	riList.append(RicnuStamp());
}

void RicnuDevice::decodeLastLine(void)
{
	decode(riList.last().data);
}

void RicnuDevice::decodeAllLine(void)
{
	for(int i = 0; i < riList.size(); ++i)
	{
		decode(riList[i].data);
	}
}

void RicnuDevice::decode(struct ricnu_s *riPtr)
{
	ExecuteDevice::decode(&riPtr->ex);
	StrainDevice::decode(&riPtr->st);
}

QString RicnuDevice::getStatusStr(int index)
{
	return QString("No decoding available for this board");
}

//Unpack from buffer
void RicnuDevice::unpackCompressed6ch(struct strain_s *stPtr)
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

