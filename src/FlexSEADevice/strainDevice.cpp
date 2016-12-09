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
	[This file] ExecuteDevice: Execute Device Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-07 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "executeDevice.h"
#include "flexsea_generic.h"
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

ExecuteDevice::ExecuteDevice(enum DataSourceFile dataSourceInit): FlexseaDevice()
{
	this->dataSource = dataSourceInit;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString ExecuteDevice::getHeaderStr(void)
{
	return QString("Timestamp,") + \
				   "Timestamp (ms)," + \
				   "accel.x," + \
				   "accel.y," + \
				   "accel.z," + \
				   "gyro.x," + \
				   "gyro.y," + \
				   "gyro.z," + \
				   "strain," + \
				   "analog_0," + \
				   "analog_1," + \
				   "current," + \
				   "enc-disp," + \
				   "enc-cont," + \
				   "enc-comm," + \
				   "VB," + \
				   "VG," + \
				   "Temp," + \
				   "Status1," + \
				   "Status2";
}

QString ExecuteDevice::getLastLineStr(void)
{
	QString str;
	QTextStream(&str) <<	exList.last().timeStampDate << ',' << \
							exList.last().timeStamp_ms << ',' << \
							exList.last().data.accel.x << ',' << \
							exList.last().data.accel.y << ',' << \
							exList.last().data.accel.z << ',' << \
							exList.last().data.gyro.x << ',' << \
							exList.last().data.gyro.y << ',' << \
							exList.last().data.gyro.z << ',' << \
							exList.last().data.strain << ',' << \
							exList.last().data.analog[0] << ',' << \
							exList.last().data.analog[1] << ',' << \
							exList.last().data.current << ',' << \
							exList.last().data.enc_display << ',' << \
							exList.last().data.enc_control << ',' << \
							exList.last().data.enc_commut << ',' << \
							exList.last().data.volt_batt << ',' << \
							exList.last().data.volt_int << ',' << \
							exList.last().data.temp << ',' << \
							exList.last().data.status1 << ',' << \
							exList.last().data.status2;
	return str;
}

void ExecuteDevice::clear(void)
{
	FlexseaDevice::clear();
	exList.clear();
}

void ExecuteDevice::appendEmptyLine(void)
{
	exList.append(ExecuteStamp());
}

void ExecuteDevice::decodeLastLine(void)
{
	decode(&exList.last().data);
}

void ExecuteDevice::decodeAllLine(void)
{
	for(int i = 0; i < exList.size(); ++i)
	{
		decode(&exList[i].data);
	}
}


void ExecuteDevice::decode(struct execute_s *exPtr)
{
	//Accel in mG
	exPtr->decoded.accel.x = (1000*exPtr->accel.x)/8192;
	exPtr->decoded.accel.y = (1000*exPtr->accel.y)/8192;
	exPtr->decoded.accel.z = (1000*exPtr->accel.z)/8192;

	//Gyro in degrees/s
	exPtr->decoded.gyro.x = (100*exPtr->gyro.x)/164;
	exPtr->decoded.gyro.y = (100*exPtr->gyro.y)/164;
	exPtr->decoded.gyro.z = (100*exPtr->gyro.z)/164;

	//exPtr->decoded.current = (185*exPtr->current)/10;   //mA
	exPtr->decoded.current = exPtr->current;   //1mA/bit for sine comm.

	exPtr->decoded.volt_batt = (int32_t)1000*P4_ADC_SUPPLY*((16*\
						(float)exPtr->volt_batt/3 + 302 ) \
						/P4_ADC_MAX) / 0.0738;          //mV

	exPtr->decoded.volt_int = (int32_t)1000*P4_ADC_SUPPLY*((26*\
						(float)exPtr->volt_int/3 + 440 ) \
						/P4_ADC_MAX) / 0.43;            //mV

	exPtr->decoded.temp = (int32_t)10*((((2.625*(float)exPtr->temp + 41) \
					  /P4_ADC_MAX)*P4_ADC_SUPPLY) - P4_T0) / P4_TC; //C*10

	exPtr->decoded.analog[0] = (int32_t)1000*((float)exPtr->analog[0]/ \
						P5_ADC_MAX)*P5_ADC_SUPPLY;
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

