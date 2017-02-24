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
#include <QStringList>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

FlexseaDevice::FlexseaDevice()
{
	logItem = 0;
	slaveIndex = 0;
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
	slaveIndex = 0;
	slaveID = 0;
	slaveName.clear();
	experimentIndex = 0;
	experimentName.clear();
	frequency = 0;
	timeStamp.clear();
}

QString FlexseaDevice::getIdentifier(void)
{
	QStringList identifier = QStringList()
							<< "Datalogging Item:"
							<< QString::number(logItem)
							<< "Slave Index:"
							<< QString::number(slaveIndex)
							<< "Slave Name:"
							<< slaveName
							<< "Experiment Index:"
							<< QString::number(experimentIndex)
							<< "Experiment Name:"
							<< experimentName
							<< "Acquisition Frequency:"
							<< QString::number(frequency)
							<< "Slave type:"
							<< slaveType;

	return identifier.join(',');
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

