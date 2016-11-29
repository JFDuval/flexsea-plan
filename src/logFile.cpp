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
	[This file] serialdriver: Serial Port Driver
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "logFile.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

LogFile::LogFile(QWidget *parent) : QWidget(parent)
{
	init();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

void LogFile::init(void)
{
	dataloggingItem = 0;
	SlaveIndex = 0;
	experimentIndex = 0;
	frequency = 0;
}

void LogFile::clear(void)
{
	dataloggingItem = 0;
	SlaveIndex = 0;
	SlaveName.clear();
	experimentIndex = 0;
	experimentName.clear();
	frequency = 0;
	shortFileName.clear();
	fileName.clear();
	data.clear();
}

void LogFile::newDataLine(void)
{
	struct log_ss newitem;
	data.append(newitem);
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

