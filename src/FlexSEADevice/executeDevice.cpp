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
	if(header.length() != headerDecoded.length())
	{
		qDebug() << "Mismatch between header lenght Execute!";
	}

	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveType = "execute";
}

ExecuteDevice::ExecuteDevice(execute_s *devicePtr): FlexseaDevice()
{
	if(header.length() != headerDecoded.length())
	{
		qDebug() << "Mismatch between header lenght Execute!";
	}

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
								<< "Accel X"
								<< "Accel Y"
								<< "Accel Z"
								<< "Gyro X"
								<< "Gyro Y"
								<< "Gyro Z"
								<< "Strain"
								<< "Analog[0]"
								<< "Analog[1]"
								<< "Analog[2]"
								<< "Analog[3]"
								<< "Analog[4]"
								<< "Analog[5]"
								<< "Analog[6]"
								<< "Analog[7]"
								<< "Motor Current"
								<< "Encoder Display"
								<< "Encoder Control"
								<< "Encoder Commutation"
								<< "Battery Voltage"
								<< "Int. Voltage"
								<< "Temperature"
								<< "Status1"
								<< "Status2";

QStringList ExecuteDevice::headerDecoded = QStringList()
								<< "Raw Value Only"
								<< "Raw Value Only"
								<< "Decoded: mg"
								<< "Decoded: mg"
								<< "Decoded: mg"
								<< "Decoded: deg/s"
								<< "Decoded: deg/s"
								<< "Decoded: deg/s"
								<< "Decoded: ±100%"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Raw value only"
								<< "Raw value only"
								<< "Raw value only"
								<< "Raw value only"
								<< "Raw value only"
								<< "Raw value only"
								<< "Decoded: mA"
								<< "Raw Value Only"
								<< "Raw Value Only"
								<< "Raw Value Only"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: 10x C"
								<< "Raw value only"
								<< "Raw value only";

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
							exList.last()->analog[2]	<< ',' << \
							exList.last()->analog[3]	<< ',' << \
							exList.last()->analog[4]	<< ',' << \
							exList.last()->analog[5]	<< ',' << \
							exList.last()->analog[6]	<< ',' << \
							exList.last()->analog[7]	<< ',' << \
							exList.last()->current		<< ',' << \
							exList.last()->enc_display	<< ',' << \
							exList.last()->enc_control	<< ',' << \
							exList.last()->enc_commut	<< ',' << \
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
		exList.last()->analog[2]	= (*splitLine)[11].toInt();
		exList.last()->analog[3]	= (*splitLine)[12].toInt();
		exList.last()->analog[4]	= (*splitLine)[13].toInt();
		exList.last()->analog[5]	= (*splitLine)[14].toInt();
		exList.last()->analog[6]	= (*splitLine)[15].toInt();
		exList.last()->analog[7]	= (*splitLine)[16].toInt();
		exList.last()->current		= (*splitLine)[17].toInt();
		exList.last()->enc_display	= (*splitLine)[18].toInt();
		exList.last()->enc_control	= (*splitLine)[19].toInt();
		exList.last()->enc_commut	= (*splitLine)[20].toInt();
		exList.last()->volt_batt	= (*splitLine)[21].toInt();
		exList.last()->volt_int		= (*splitLine)[22].toInt();
		exList.last()->temp			= (*splitLine)[23].toInt();
		exList.last()->status1		= (*splitLine)[24].toInt();
		exList.last()->status2		= (*splitLine)[25].toInt();
	}
}

struct std_variable ExecuteDevice::getSerializedVar(int parameter)
{
	return getSerializedVar(parameter, 0);
}

struct std_variable ExecuteDevice::getSerializedVar(int parameter, int index)
{
	struct std_variable var;

	if(index >= exList.length())
	{
		parameter = INT_MAX;
	}

	//Assign pointer:
	switch(parameter)
	{
		/*Format: (every Case except Unused)
		 * Line 1: data format, raw variable
		 * Line 2: raw variable
		 * Line 3: decoded variable (always int32),
					null if not decoded  */
		case 0: //"TimeStamp"
			var.format = FORMAT_QSTR;
			var.rawGenPtr = &timeStamp[index].date;
			var.decodedPtr = nullptr;
			break;
		case 1: //"TimeStamp (ms)"
			var.format = FORMAT_32S;
			var.rawGenPtr = &timeStamp[index].ms;
			var.decodedPtr = nullptr;
			break;
		case 2: //"Accel X"
			var.format = FORMAT_16S;
			var.rawGenPtr = &exList[index]->accel.x;
			var.decodedPtr = &exList[index]->decoded.accel.x;
			break;
		case 3:	//"Accel Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &exList[index]->accel.y;
			var.decodedPtr = &exList[index]->decoded.accel.y;
			break;
		case 4: //"Accel Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &exList[index]->accel.z;
			var.decodedPtr = &exList[index]->decoded.accel.z;
			break;
		case 5: //"Gyro X"
			var.format = FORMAT_16S;
			var.rawGenPtr = &exList[index]->gyro.x;
			var.decodedPtr = &exList[index]->decoded.gyro.x;
			break;
		case 6: //"Gyro Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &exList[index]->gyro.y;
			var.decodedPtr = &exList[index]->decoded.gyro.y;
			break;
		case 7: //"Gyro Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &exList[index]->gyro.z;
			var.decodedPtr = &exList[index]->decoded.gyro.z;
			break;
		case 8: //"Encoder Display"
			var.format = FORMAT_32S;
			var.rawGenPtr = &exList[index]->enc_display;
			var.decodedPtr = nullptr;
			break;
		case 9: //"Encoder Control"
			var.format = FORMAT_32S;
			var.rawGenPtr = &exList[index]->enc_control;
			var.decodedPtr = nullptr;
			break;
		case 10: //"Encoder Commutation"
			var.format = FORMAT_32S;
			var.rawGenPtr = &exList[index]->enc_commut;
			var.decodedPtr = nullptr;
			break;
		case 11: //"Motor current"
			var.format = FORMAT_16S;
			var.rawGenPtr = &exList[index]->current;
			var.decodedPtr = &exList[index]->decoded.current;
			break;
		case 12: //"Analog[0]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &exList[index]->analog[0];
			var.decodedPtr = &exList[index]->decoded.analog[0];
			break;
		case 13: //Analog[1]
			var.format = FORMAT_16U;
			var.rawGenPtr = &exList[index]->analog[1];
			var.decodedPtr = &exList[index]->decoded.analog[1];
			break;
		case 14: //Analog[2]
			var.format = FORMAT_16U;
			var.rawGenPtr = &exList[index]->analog[2];
			var.decodedPtr = &exList[index]->decoded.analog[2];
			break;
		case 15: //Analog[3]
			var.format = FORMAT_16U;
			var.rawGenPtr = &exList[index]->analog[3];
			var.decodedPtr = &exList[index]->decoded.analog[3];
			break;
		case 16: //Analog[4]
			var.format = FORMAT_16U;
			var.rawGenPtr = &exList[index]->analog[4];
			var.decodedPtr = &exList[index]->decoded.analog[4];
			break;
		case 17: //Analog[5]
			var.format = FORMAT_16U;
			var.rawGenPtr = &exList[index]->analog[5];
			var.decodedPtr = &exList[index]->decoded.analog[5];
			break;
		case 18: //Analog[6]
			var.format = FORMAT_16U;
			var.rawGenPtr = &exList[index]->analog[6];
			var.decodedPtr = &exList[index]->decoded.analog[6];
			break;
		case 19: //Analog[7]
			var.format = FORMAT_16U;
			var.rawGenPtr = &exList[index]->analog[7];
			var.decodedPtr = &exList[index]->decoded.analog[7];
			break;
		case 20: //"Strain"
			var.format = FORMAT_16U;
			var.rawGenPtr = &exList[index]->strain;
			var.decodedPtr = &exList[index]->decoded.strain;
			break;
		case 21: //"+VB"
			var.format = FORMAT_8U;
			var.rawGenPtr = &exList[index]->volt_batt;
			var.decodedPtr = &exList[index]->decoded.volt_batt;
			break;
		case 22: //"+VG"
			var.format = FORMAT_8U;
			var.rawGenPtr = &exList[index]->volt_int;
			var.decodedPtr = &exList[index]->decoded.volt_int;
			break;
		case 23: //"Temp"
			var.format = FORMAT_8U;
			var.rawGenPtr = &exList[index]->temp;
			var.decodedPtr = &exList[index]->decoded.temp;
			break;
		case 24: //"Status 1"
			var.format = FORMAT_8U;
			var.rawGenPtr = &exList[index]->status1;
			var.decodedPtr = nullptr;
			break;
		case 25: //"Status 2"
			var.format = FORMAT_8U;
			var.rawGenPtr = &exList[index]->status2;
			var.decodedPtr = nullptr;
			break;
		default:
			var.format = NULL_PTR;
			var.rawGenPtr = nullptr;
			var.decodedPtr = nullptr;
			break;
	}

	return var;
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

