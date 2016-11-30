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
#include "flexsea_generic.h"
#include <QDebug>

// TODO The inclusion of logFile.h in execute and logkeypad cause an issue if
// they are include here before w_config in mainwindow.h . See mainwindow.h for
// more details.

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

void LogFile::decodeLastLine(void)
{
	// Execute
	if(SlaveIndex >= SL_BASE_EX && SlaveIndex < SL_BASE_MN)
	{
		FlexSEA_Generic::decodeExecute(&data.last().execute);
	}

	// Manage
	else if(SlaveIndex >= SL_BASE_MN && SlaveIndex < SL_BASE_PLAN)
	{
		FlexSEA_Generic::decodeManage(&data.last().manage);
	}

	// Plan
	else if(SlaveIndex >= SL_BASE_PLAN && SlaveIndex < SL_BASE_GOSSIP)
	{
		//TODO What to do in this case?
		qDebug() << "Error in log file: Is plan a structure? how to we manage it";
	}

	// Gossip
	else if(SlaveIndex >= SL_BASE_GOSSIP && SlaveIndex < SL_BASE_BATT)
	{
		FlexSEA_Generic::decodeGossip(&data.last().gossip);
	}

	// Battery
	else if(SlaveIndex >= SL_BASE_BATT && SlaveIndex < SL_BASE_STRAIN)
	{
		FlexSEA_Generic::decodeBattery(&data.last().battery);
	}

	// Strain
	else if(SlaveIndex >= SL_BASE_STRAIN && SlaveIndex < SL_BASE_RICNU)
	{
		FlexSEA_Generic::decodeStrain(&data.last().strain);
	}

	// RIC/NU
	else if(SlaveIndex >= SL_BASE_RICNU && SlaveIndex < SL_LEN_ALL)
	{
		FlexSEA_Generic::decodeRicnu(&data.last().ricnu);
	}

	else
	{
		qDebug() << "Error in log file: Structure not defined";
	}
}

void LogFile::decodeAllLine(void)
{
	int i;

	// Execute
	if(SlaveIndex >= SL_BASE_EX && SlaveIndex < SL_BASE_MN)
	{
		for(i = 0; i < data.size(); ++i)
		{
			FlexSEA_Generic::decodeExecute(&data[i].execute);
		}
	}

	// Manage
	else if(SlaveIndex >= SL_BASE_MN && SlaveIndex < SL_BASE_PLAN)
	{
		for(i = 0; i < data.size(); ++i)
		{
			FlexSEA_Generic::decodeManage(&data[i].manage);
		}
	}

	// Plan
	else if(SlaveIndex >= SL_BASE_PLAN && SlaveIndex < SL_BASE_GOSSIP)
	{
		//TODO What to do in this case?
		qDebug() << "Error in log file: Is plan a structure? how to we manage it";
	}

	// Gossip
	else if(SlaveIndex >= SL_BASE_GOSSIP && SlaveIndex < SL_BASE_BATT)
	{
		for(i = 0; i < data.size(); ++i)
		{
			FlexSEA_Generic::decodeGossip(&data[i].gossip);
		}
	}

	// Battery
	else if(SlaveIndex >= SL_BASE_BATT && SlaveIndex < SL_BASE_STRAIN)
	{
		for(i = 0; i < data.size(); ++i)
		{
			FlexSEA_Generic::decodeBattery(&data[i].battery);
		}
	}

	// Strain
	else if(SlaveIndex >= SL_BASE_STRAIN && SlaveIndex < SL_BASE_RICNU)
	{
		for(i = 0; i < data.size(); ++i)
		{
			FlexSEA_Generic::decodeStrain(&data[i].strain);
		}
	}

	// RIC/NU
	else if(SlaveIndex >= SL_BASE_RICNU && SlaveIndex < SL_LEN_ALL)
	{
		for(i = 0; i < data.size(); ++i)
		{
			FlexSEA_Generic::decodeRicnu(&data[i].ricnu);
		}
	}

	else
	{
		qDebug() << "Error in log file: Structure not defined";
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

