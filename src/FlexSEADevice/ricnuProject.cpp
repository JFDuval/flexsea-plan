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

#include "ricnuProject.h"
#include <QDebug>
#include <QTextStream>
#include "executeDevice.h"
#include "strainDevice.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

RicnuProject::RicnuProject(void): FlexseaDevice()
{
	if(header.length() != headerDecoded.length())
	{
		qDebug() << "Mismatch between header lenght Ricnu!";
	}

	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveTypeName = "ricnu";
}

RicnuProject::RicnuProject(execute_s *exPtr, strain_s *stPtr): FlexseaDevice()
{
	if(header.length() != headerDecoded.length())
	{
		qDebug() << "Mismatch between header lenght Ricnu!";
	}

	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	riList.append(new ricnu_s_plan());
	riList.last()->ex = exPtr;
	riList.last()->st = stPtr;
	serializedLength = header.length();
	slaveTypeName = "ricnu";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString RicnuProject::getHeaderStr(void)
{
	return header.join(',');
}

QStringList RicnuProject::header = QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"

								<< "Accel X"
								<< "Accel Y"
								<< "Accel Z"
								<< "Gyro X"
								<< "Gyro Y"
								<< "Gyro Z"
								<< "Motor current"
								<< "Encoder Motor"
								<< "Encoder Joint"
								<< "Strain[0]"
								<< "Strain[1]"
								<< "Strain[2]"
								<< "Strain[3]"
								<< "Strain[4]"
								<< "Strain[5]"
								<< "PWM";

QStringList RicnuProject::headerDecoded = QStringList()
								<< "Raw Value Only"
								<< "Raw Value Only"

								<< "Decoded: mg"
								<< "Decoded: mg"
								<< "Decoded: mg"
								<< "Decoded: deg/s"
								<< "Decoded: deg/s"
								<< "Decoded: deg/s"
								<< "Decoded: mA"
								<< "Raw value only"
								<< "Raw value only"
								<< "Decoded: ±100%"
								<< "Decoded: ±100%"
								<< "Decoded: ±100%"
								<< "Decoded: ±100%"
								<< "Decoded: ±100%"
								<< "Decoded: ±100%"
								<< "PWM, -1024 to 1024";

QString RicnuProject::getLastSerializedStr(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date						<< ',' << \
							timeStamp.last().ms							<< ',' << \

							riList.last()->ex->accel.x					<< ',' << \
							riList.last()->ex->accel.y					<< ',' << \
							riList.last()->ex->accel.z					<< ',' << \
							riList.last()->ex->gyro.x					<< ',' << \
							riList.last()->ex->gyro.y					<< ',' << \
							riList.last()->ex->gyro.z					<< ',' << \
							riList.last()->ex->current					<< ',' << \
							riList.last()->ex->enc_motor				<< ',' << \
							riList.last()->ex->enc_joint				<< ',' << \
							riList.last()->st->ch[0].strain_filtered	<< ',' << \
							riList.last()->st->ch[1].strain_filtered	<< ',' << \
							riList.last()->st->ch[2].strain_filtered	<< ',' << \
							riList.last()->st->ch[3].strain_filtered	<< ',' << \
							riList.last()->st->ch[4].strain_filtered	<< ',' << \
							riList.last()->st->ch[5].strain_filtered	<< ',' << \
							riList.last()->ex->sine_commut_pwm;
	return str;
}

void RicnuProject::appendSerializedStr(QStringList *splitLine)
{
	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		// Because of the pointer architecture of ricnu_s_plan , we need to
		// also add execute and strain structure
		appendEmptyLineWithStruct();

		timeStamp.last().date						= (*splitLine)[0];
		timeStamp.last().ms							= (*splitLine)[1].toInt();

		riList.last()->ex->accel.x					= (*splitLine)[2].toInt();
		riList.last()->ex->accel.y					= (*splitLine)[3].toInt();
		riList.last()->ex->accel.z					= (*splitLine)[4].toInt();
		riList.last()->ex->gyro.x					= (*splitLine)[5].toInt();
		riList.last()->ex->gyro.y					= (*splitLine)[6].toInt();
		riList.last()->ex->gyro.z					= (*splitLine)[7].toInt();
		riList.last()->ex->current					= (*splitLine)[8].toInt();
		riList.last()->ex->enc_motor				= (*splitLine)[9].toInt();
		riList.last()->ex->enc_joint				= (*splitLine)[10].toInt();
		riList.last()->st->ch[0].strain_filtered	= (*splitLine)[11].toInt();
		riList.last()->st->ch[1].strain_filtered	= (*splitLine)[12].toInt();
		riList.last()->st->ch[2].strain_filtered	= (*splitLine)[13].toInt();
		riList.last()->st->ch[3].strain_filtered	= (*splitLine)[14].toInt();
		riList.last()->st->ch[4].strain_filtered	= (*splitLine)[15].toInt();
		riList.last()->st->ch[5].strain_filtered	= (*splitLine)[16].toInt();
		riList.last()->ex->sine_commut_pwm			= (*splitLine)[17].toInt();
	}
}

struct std_variable RicnuProject::getSerializedVar(int parameter)
{
	return getSerializedVar(parameter, 0);
}

struct std_variable RicnuProject::getSerializedVar(int parameter, int index)
{
	struct std_variable var;

	if(index >= riList.length())
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
			var.rawGenPtr = &riList[index]->ex->accel.x;
			var.decodedPtr = &riList[index]->ex->decoded.accel.x;
			break;
		case 3: //"Accel Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &riList[index]->ex->accel.y;
			var.decodedPtr = &riList[index]->ex->decoded.accel.y;
			break;
		case 4: //"Accel Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &riList[index]->ex->accel.z;
			var.decodedPtr = &riList[index]->ex->decoded.accel.z;
			break;
		case 5: //"Gyro X"
			var.format = FORMAT_16S;
			var.rawGenPtr = &riList[index]->ex->gyro.x;
			var.decodedPtr = &riList[index]->ex->decoded.gyro.x;
			break;
		case 6: //"Gyro Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &riList[index]->ex->gyro.y;
			var.decodedPtr = &riList[index]->ex->decoded.gyro.y;
			break;
		case 7: //"Gyro Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &riList[index]->ex->gyro.z;
			var.decodedPtr = &riList[index]->ex->decoded.gyro.z;
			break;
		case 8: //"Motor current"
			var.format = FORMAT_16S;
			var.rawGenPtr = &riList[index]->ex->current;
			var.decodedPtr = &riList[index]->ex->decoded.current;
			break;
		case 9: //"Encoder Motor"
			var.format = FORMAT_32S;
			var.rawGenPtr = &riList[index]->ex->enc_motor;
			var.decodedPtr = nullptr;
			break;
		case 10: //"Encoder Joint"
			var.format = FORMAT_32S;
			var.rawGenPtr = &riList[index]->ex->enc_joint;
			var.decodedPtr = nullptr;
			break;
		case 11: //"Strain[0]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &riList[index]->st->ch[0].strain_filtered;
			var.decodedPtr = &riList[index]->decoded.ext_strain[0];
			break;
		case 12: //"Strain[1]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &riList[index]->st->ch[1].strain_filtered;
			var.decodedPtr = &riList[index]->decoded.ext_strain[1];
			break;
		case 13: //"Strain[2]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &riList[index]->st->ch[2].strain_filtered;
			var.decodedPtr = &riList[index]->decoded.ext_strain[2];
			break;
		case 14: //"Strain[3]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &riList[index]->st->ch[3].strain_filtered;
			var.decodedPtr = &riList[index]->decoded.ext_strain[3];
			break;
		case 15: //"Strain[4]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &riList[index]->st->ch[4].strain_filtered;
			var.decodedPtr = &riList[index]->decoded.ext_strain[4];
			break;
		case 16: //"Strain[5]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &riList[index]->st->ch[5].strain_filtered;
			var.decodedPtr = &riList[index]->decoded.ext_strain[5];
			break;
		case 17: //"PWM"
			var.format = FORMAT_16S;
			var.rawGenPtr = &riList[index]->ex->sine_commut_pwm;
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

void RicnuProject::clear(void)
{
	FlexseaDevice::clear();
	riList.clear();
	timeStamp.clear();
}

void RicnuProject::appendEmptyLine(void)
{
	timeStamp.append(TimeStamp());
	riList.append(new ricnu_s_plan());
}

void RicnuProject::appendEmptyLineWithStruct(void)
{
	appendEmptyLine();
	riList.last()->ex = new execute_s();
	riList.last()->st = new strain_s();
}

void RicnuProject::decodeLastLine(void)
{
	if(dataSource == LiveDataFile){StrainDevice::decompressRawBytes(riList.last()->st);}
	decode(riList.last());
}

void RicnuProject::decodeAllLine(void)
{
	for(int i = 0; i < riList.size(); ++i)
	{
		if(dataSource == LiveDataFile){StrainDevice::decompressRawBytes(riList[i]->st);}
		decode(riList[i]);
	}
}

void RicnuProject::decode(struct ricnu_s *riPtr)
{
	ExecuteDevice::decode(&riPtr->ex);
	StrainDevice::decode(&riPtr->st);
}

void RicnuProject::decode(struct ricnu_s_plan *riPtr)
{
	ExecuteDevice::decode(riPtr->ex);
	StrainDevice::decode(riPtr->st);
}

QString RicnuProject::getStatusStr(int index)
{
	(void)index;	//Unused

	return QString("No decoding available for this board");
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

