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
	[This file] FlexseaDevice: Interface class flexSEA device
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-07 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "flexseaDevice.h"
#include <QString>
#include <QStringList>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

FlexseaDevice::FlexseaDevice()
{
	logItem = 0;
	SlaveIndex = 0;
	slaveID = 0;
	experimentIndex = 0;
	frequency = 0;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

void FlexseaDevice::clear(void)
{
	shortFileName.clear();
	fileName.clear();
	logItem = 0;
	SlaveIndex = 0;
	slaveID = 0;
	slaveName.clear();
	experimentIndex = 0;
	experimentName.clear();
	targetSlaveName.clear();
	frequency = 0;
	timeStamp.clear();
}

QString FlexseaDevice::getIdentifierStr(void)
{

	return getIdentifierStrList().join(',');
}

QStringList FlexseaDevice::getIdentifierStrList(void)
{
	QStringList identifier = QStringList()
							<< "Datalogging Item:"
							<< QString::number(logItem)
							<< "Slave Index:"
							<< QString::number(SlaveIndex)
							<< "Slave Name:"
							<< slaveName
							<< "Experiment Index:"
							<< QString::number(experimentIndex)
							<< "Experiment Name:"
							<< experimentName
							<< "Aquisition Frequency:"
							<< QString::number(frequency)
							<< "Slave type:"
							<< slaveTypeName
							<< "Target Slave Name"
							<< targetSlaveName;

	return identifier;
}

void FlexseaDevice::saveIdentifierStr(QStringList *splitLine)
{
	FlexseaDevice::clear();
	//Check if data line contain the number of data expected
	QStringList identifier = FlexseaDevice::getIdentifierStrList();

	if(splitLine->length() >= identifier.length())
	{
		logItem			= (*splitLine)[1].toInt();
		SlaveIndex		= (*splitLine)[3].toInt();
		slaveName		= (*splitLine)[5];
		experimentIndex	= (*splitLine)[7].toInt();
		experimentName	= (*splitLine)[9];
		frequency		= (*splitLine)[11].toInt();
		targetSlaveName = (*splitLine)[15];
	}
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

