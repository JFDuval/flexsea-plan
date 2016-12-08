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
	[This file] LogFile: Log File data class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-11-30 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "flexsea_data.h"
#include "flexsea_generic.h"
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

ExecuteClass::ExecuteClass(enum DataSourceFile dataSourceInit)
{
	this->dataSource = dataSourceInit;
	dataloggingItem = 0;
	SlaveIndex = 0;
	experimentIndex = 0;
	frequency = 0;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString ExecuteClass::getHeaderStr(void)
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

QString ExecuteClass::getLastLineStr(void)
{
	QString str;
	QTextStream(&str) <<	data.last().timeStampDate << ',' << \
							data.last().timeStamp_ms << ',' << \
							data.last().exec.accel.x << ',' << \
							data.last().exec.accel.y << ',' << \
							data.last().exec.accel.z << ',' << \
							data.last().exec.gyro.x << ',' << \
							data.last().exec.gyro.y << ',' << \
							data.last().exec.gyro.z << ',' << \
							data.last().exec.strain << ',' << \
							data.last().exec.analog[0] << ',' << \
							data.last().exec.analog[1] << ',' << \
							data.last().exec.current << ',' << \
							data.last().exec.enc_display << ',' << \
							data.last().exec.enc_control << ',' << \
							data.last().exec.enc_commut << ',' << \
							data.last().exec.volt_batt << ',' << \
							data.last().exec.volt_int << ',' << \
							data.last().exec.temp << ',' << \
							data.last().exec.status1 << ',' << \
							data.last().exec.status2;
	return str;
}

void ExecuteClass::clear(void)
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

void ExecuteClass::appendEmptyLine(void)
{
	data.append(ExecuteT());
}

void ExecuteClass::decodeLastLine(void)
{
	decode(&data.last().exec);
}

void ExecuteClass::decodeAllLine(void)
{
	for(int i = 0; i < data.size(); ++i)
	{
		decode(&data[i].exec);
	}
}


//****************************************************************************
// Public slot(s):
//****************************************************************************


//****************************************************************************
// Private function(s):
//****************************************************************************
void ExecuteClass::decode(struct execute_s *exPtr)
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
// Private slot(s):
//****************************************************************************

