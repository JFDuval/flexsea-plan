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
	stList.append(StrainStamp());
	stList.last().data = devicePtr;
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
	QTextStream(&str) <<	timeStamp.last().date					 << ',' << \
							timeStamp.last().ms						 << ',' << \
							stList.last().data->ch[0].strain_filtered << ',' << \
							stList.last().data->ch[1].strain_filtered << ',' << \
							stList.last().data->ch[2].strain_filtered << ',' << \
							stList.last().data->ch[3].strain_filtered << ',' << \
							stList.last().data->ch[4].strain_filtered << ',' << \
							stList.last().data->ch[5].strain_filtered;
	return str;
}

void StrainDevice::appendSerializedStr(QStringList *splitLine)
{

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
	stList.append(StrainStamp());
}

void StrainDevice::decodeLastLine(void)
{
	decode(stList.last().data);
}

void StrainDevice::decodeAllLine(void)
{
	for(int i = 0; i < stList.size(); ++i)
	{
		decode(stList[i].data);
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

//****************************************************************************
// Public slot(s):
//****************************************************************************


//****************************************************************************
// Private function(s):
//****************************************************************************


//****************************************************************************
// Private slot(s):
//****************************************************************************

