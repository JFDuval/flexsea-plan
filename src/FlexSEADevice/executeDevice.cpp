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
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

ExecuteDevice::ExecuteDevice(void): FlexseaDevice()
{
	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveType = "execute";
}

ExecuteDevice::ExecuteDevice(execute_s *devicePtr): FlexseaDevice()
{
	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	exList.append(devicePtr);
	serializedLength = header.length();
	slaveType = "execute";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString ExecuteDevice::getHeaderStr(void)
{
	return header.join(',');
}

QStringList ExecuteDevice::header = QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"
								<< "accel.x"
								<< "accel.y"
								<< "accel.z"
								<< "gyro.x"
								<< "gyro.y"
								<< "gyro.z"
								<< "strain"
								<< "analog_0"
								<< "analog_1"
								<< "current"
								<< "enc-disp"
								<< "enc-cont"
								<< "enc-comm"
								<< "VB"
								<< "VG"
								<< "Temp"
								<< "Status1"
								<< "Status2";

QString ExecuteDevice::getLastSerializedStr(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date		<< ',' << \
							timeStamp.last().ms			<< ',' << \
							exList.last()->accel.x		<< ',' << \
							exList.last()->accel.y		<< ',' << \
							exList.last()->accel.z		<< ',' << \
							exList.last()->gyro.x		<< ',' << \
							exList.last()->gyro.y		<< ',' << \
							exList.last()->gyro.z		<< ',' << \
							exList.last()->strain		<< ',' << \
							exList.last()->analog[0]	<< ',' << \
							exList.last()->analog[1]	<< ',' << \
							exList.last()->current		<< ',' << \
                            *(exList.last()->enc_ang)   << ',' << \
							exList.last()->volt_batt	<< ',' << \
							exList.last()->volt_int		<< ',' << \
							exList.last()->temp			<< ',' << \
							exList.last()->status1		<< ',' << \
							exList.last()->status2;
	return str;
}

void ExecuteDevice::appendSerializedStr(QStringList *splitLine)
{
	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		appendEmptyLine();
		timeStamp.last().date		= (*splitLine)[0];
		timeStamp.last().ms			= (*splitLine)[1].toInt();
		exList.last()->accel.x		= (*splitLine)[2].toInt();
		exList.last()->accel.y		= (*splitLine)[3].toInt();
		exList.last()->accel.z		= (*splitLine)[4].toInt();
		exList.last()->gyro.x		= (*splitLine)[5].toInt();
		exList.last()->gyro.y		= (*splitLine)[6].toInt();
		exList.last()->gyro.z		= (*splitLine)[7].toInt();
		exList.last()->strain		= (*splitLine)[8].toInt();
		exList.last()->analog[0]	= (*splitLine)[9].toInt();
		exList.last()->analog[1]	= (*splitLine)[10].toInt();
		exList.last()->current		= (*splitLine)[11].toInt();
        *(exList.last()->enc_ang)   = (*splitLine)[12].toInt();
		exList.last()->volt_batt	= (*splitLine)[15].toInt();
		exList.last()->volt_int		= (*splitLine)[16].toInt();
		exList.last()->temp			= (*splitLine)[17].toInt();
		exList.last()->status1		= (*splitLine)[18].toInt();
		exList.last()->status2		= (*splitLine)[19].toInt();
	}
}

void ExecuteDevice::clear(void)
{
	FlexseaDevice::clear();
	exList.clear();
	timeStamp.clear();
}

void ExecuteDevice::appendEmptyLine(void)
{
	timeStamp.append(TimeStamp());
	exList.append(new execute_s());
}

void ExecuteDevice::decodeLastLine(void)
{
	decode(exList.last());
}

void ExecuteDevice::decodeAllLine(void)
{
	for(int i = 0; i < exList.size(); ++i)
	{
		decode(exList[i]);
	}
}

QString ExecuteDevice::getStatusStr(int index)
{
	QString str;
	uint8_t status1 = exList[index]->status1;

	//WDCLK:
	if(GET_WDCLK_FLAG(status1))
	{
		str.append("Co-Processor Error");
	}

	//Disconnected battery:
	if(GET_DISCON_FLAG(status1) == BATT_DISCONNECTED)
	{
		if(str.isEmpty() == false){str.append(" | ");}
		str.append("Disconnected battery");
	}

	//Temperature:
	if(GET_OVERTEMP_FLAG(status1) == T_WARNING)
	{
		if(str.isEmpty() == false){str.append(" | ");}
		str.append("Temp. Near Limit");
	}
	else if(GET_OVERTEMP_FLAG(status1) == T_ERROR)
	{
		if(str.isEmpty() == false){str.append(" | ");}
		str.append("Temp. Error");
	}

	//Voltage - VB:
	if(GET_VB_FLAG(status1) == V_LOW)
	{
		if(str.isEmpty() == false){str.append(" | ");}
		str.append("VB Low");
	}
	else if(GET_VB_FLAG(status1) == V_HIGH)
	{
		if(str.isEmpty() == false){str.append(" | ");}
		str.append("VB High");
	}

	//Voltage - VG:
	if(GET_VG_FLAG(status1) == V_LOW)
	{
		if(str.isEmpty() == false){str.append(" | ");}
		str.append("VG Low");
	}
	else if(GET_VG_FLAG(status1) == V_HIGH)
	{
		if(str.isEmpty() == false){str.append(" | ");}
		str.append("VG High");
	}

	//If nothing is wrong:
	if(str.isEmpty() == true)
	{
		str.append("Status: OK");
	}

	return str;
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

	exPtr->decoded.strain = 0;

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

	exPtr->decoded.analog[1] = 0;
	exPtr->decoded.analog[2] = 0;
	exPtr->decoded.analog[3] = 0;
	exPtr->decoded.analog[4] = 0;
	exPtr->decoded.analog[5] = 0;
	exPtr->decoded.analog[6] = 0;
	exPtr->decoded.analog[7] = 0;
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

