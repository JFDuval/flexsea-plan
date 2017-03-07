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
	[This file] testBenchProject: TestBench Project Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-01-19 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "testBenchProject.h"
#include <QDebug>
#include <QTextStream>
#include "executeDevice.h"
#include "batteryDevice.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

TestBenchProject::TestBenchProject(void): FlexseaDevice()
{
	if(header.length() != headerDecoded.length())
	{
		qDebug() << "Mismatch between header lenght TestBench!";
	}

	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveTypeName = "testBench";
}

TestBenchProject::TestBenchProject(execute_s *ex1Ptr, execute_s *ex2Ptr,
								   motortb_s *motbPtr, battery_s *baPtr): FlexseaDevice()
{
	if(header.length() != headerDecoded.length())
	{
		qDebug() << "Mismatch between header lenght TestBench!";
	}

	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	tbList.append(new testBench_s_plan());
	tbList.last()->ex1 = ex1Ptr;
	tbList.last()->ex2 = ex2Ptr;
	tbList.last()->mb  = motbPtr;
	tbList.last()->ba  = baPtr;

	serializedLength = header.length();
	slaveTypeName = "testBench";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString TestBenchProject::getHeaderStr(void)
{
	return header.join(',');
}

QStringList TestBenchProject::header = QStringList()
								<< "Timestamp"
								<< "Timestamp (ms)"

								<< "ex1[0]"
								<< "ex1[1]"
								<< "ex1[2]"
								<< "ex1[3]"
								<< "ex1[4]"
								<< "ex1[5]"

								<< "ex1 strain"
								<< "ex1 analog_0"
								<< "ex1 analog_1"
								<< "ex1 current"
								<< "ex1 enc"
								<< "ex1 VB"
								<< "ex1 VG"
								<< "ex1 Temp"
								<< "ex1 Status1"
								<< "ex1 Status2"

								<< "ex2[0]"
								<< "ex2[1]"
								<< "ex2[2]"
								<< "ex2[3]"
								<< "ex2[4]"
								<< "ex2[5]"

								<< "ex2 strain"
								<< "ex2 analog_0"
								<< "ex2 analog_1"
								<< "ex2 current"
								<< "ex2 enc"
								<< "ex2 VB"
								<< "ex2 VG"
								<< "ex2 Temp"
								<< "ex2 Status1"
								<< "ex2 Status2"

								<< "mn1[0]"
								<< "mn1[1]"
								<< "mn1[2]"
								<< "mn1[3]"

								<< "Batt.volt"
								<< "Batt.current"
								<< "Batt.power"
								<< "Batt.temp";

QStringList TestBenchProject::headerDecoded = QStringList()
								<< "Raw Value Only"
								<< "Raw Value Only"

								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"

								<< "Decoded: ±100%"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: mA"
								<< "Raw Value Only"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: 10x C"
								<< "Raw value only"
								<< "Raw value only"

								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"

								<< "Decoded: ±100%"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: mA"
								<< "Raw Value Only"
								<< "Decoded: mV"
								<< "Decoded: mV"
								<< "Decoded: 10x C"
								<< "Raw value only"
								<< "Raw value only"

								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"
								<< "Not implemented"

								<< "Decoded: mV"
								<< "Decoded: mA"
								<< "Decoded: mW"
								<< "Decoded: 10x C";

QString TestBenchProject::getLastSerializedStr(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date			<< ',' << \
							timeStamp.last().ms				<< ',' << \

							tbList.last()->mb->ex1[0]		<< ',' << \
							tbList.last()->mb->ex1[1]		<< ',' << \
							tbList.last()->mb->ex1[2]		<< ',' << \
							tbList.last()->mb->ex1[3]		<< ',' << \
							tbList.last()->mb->ex1[4]		<< ',' << \
							tbList.last()->mb->ex1[5]		<< ',' << \

							tbList.last()->ex1->strain		<< ',' << \
							tbList.last()->ex1->analog[0]	<< ',' << \
							tbList.last()->ex1->analog[1]	<< ',' << \
							tbList.last()->ex1->current		<< ',' << \
							*(tbList.last()->ex1->enc_ang)  << ',' << \
							tbList.last()->ex1->volt_batt	<< ',' << \
							tbList.last()->ex1->volt_int	<< ',' << \
							tbList.last()->ex1->temp		<< ',' << \
							tbList.last()->ex1->status1		<< ',' << \
							tbList.last()->ex1->status2		<< ',' << \

							tbList.last()->mb->ex2[0]		<< ',' << \
							tbList.last()->mb->ex2[1]		<< ',' << \
							tbList.last()->mb->ex2[2]		<< ',' << \
							tbList.last()->mb->ex2[3]		<< ',' << \
							tbList.last()->mb->ex2[4]		<< ',' << \
							tbList.last()->mb->ex2[5]		<< ',' << \

							tbList.last()->ex2->strain		<< ',' << \
							tbList.last()->ex2->analog[0]	<< ',' << \
							tbList.last()->ex2->analog[1]	<< ',' << \
							tbList.last()->ex2->current		<< ',' << \
							*(tbList.last()->ex2->enc_ang)	<< ',' << \
							tbList.last()->ex2->volt_batt	<< ',' << \
							tbList.last()->ex2->volt_int	<< ',' << \
							tbList.last()->ex2->temp		<< ',' << \
							tbList.last()->ex2->status1		<< ',' << \
							tbList.last()->ex2->status2		<< ',' << \

							tbList.last()->mb->mn1[0]		<< ',' << \
							tbList.last()->mb->mn1[1]		<< ',' << \
							tbList.last()->mb->mn1[2]		<< ',' << \
							tbList.last()->mb->mn1[3]		<< ',' << \

							tbList.last()->ba->voltage		<< ',' << \
							tbList.last()->ba->current		<< ',' << \
							tbList.last()->ba->decoded.power<< ',' << \
							tbList.last()->ba->temp;
	return str;
}

void TestBenchProject::appendSerializedStr(QStringList *splitLine)
{
	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		// Because of the pointer architecture of ricnu_s_plan , we need to
		// also add execute and strain structure
		appendEmptyLineWithStruct();

		timeStamp.last().date				= (*splitLine)[0];
		timeStamp.last().ms					= (*splitLine)[1].toInt();

		tbList.last()->mb->ex1[0]			= (*splitLine)[2].toInt();
		tbList.last()->mb->ex1[1]			= (*splitLine)[3].toInt();
		tbList.last()->mb->ex1[2]			= (*splitLine)[4].toInt();
		tbList.last()->mb->ex1[3]			= (*splitLine)[5].toInt();
		tbList.last()->mb->ex1[4]			= (*splitLine)[6].toInt();
		tbList.last()->mb->ex1[5]			= (*splitLine)[7].toInt();

		tbList.last()->ex1->strain			= (*splitLine)[8].toInt();
		tbList.last()->ex1->analog[0]		= (*splitLine)[9].toInt();
		tbList.last()->ex1->analog[1]		= (*splitLine)[10].toInt();
		tbList.last()->ex1->current			= (*splitLine)[11].toInt();
		*(tbList.last()->ex1->enc_ang)		= (*splitLine)[12].toInt();
		tbList.last()->ex1->volt_batt		= (*splitLine)[13].toInt();
		tbList.last()->ex1->volt_int		= (*splitLine)[14].toInt();
		tbList.last()->ex1->temp			= (*splitLine)[15].toInt();
		tbList.last()->ex1->status1			= (*splitLine)[16].toInt();
		tbList.last()->ex1->status2			= (*splitLine)[17].toInt();

		tbList.last()->mb->ex2[0]			= (*splitLine)[18].toInt();
		tbList.last()->mb->ex2[1]			= (*splitLine)[19].toInt();
		tbList.last()->mb->ex2[2]			= (*splitLine)[20].toInt();
		tbList.last()->mb->ex2[3]			= (*splitLine)[21].toInt();
		tbList.last()->mb->ex2[4]			= (*splitLine)[22].toInt();
		tbList.last()->mb->ex2[5]			= (*splitLine)[23].toInt();

		tbList.last()->ex2->strain			= (*splitLine)[24].toInt();
		tbList.last()->ex2->analog[0]		= (*splitLine)[25].toInt();
		tbList.last()->ex2->analog[1]		= (*splitLine)[26].toInt();
		tbList.last()->ex2->current			= (*splitLine)[27].toInt();
		*(tbList.last()->ex2->enc_ang)		= (*splitLine)[28].toInt();
		tbList.last()->ex2->volt_batt		= (*splitLine)[29].toInt();
		tbList.last()->ex2->volt_int		= (*splitLine)[30].toInt();
		tbList.last()->ex2->temp			= (*splitLine)[31].toInt();
		tbList.last()->ex2->status1			= (*splitLine)[32].toInt();
		tbList.last()->ex2->status2			= (*splitLine)[33].toInt();

		tbList.last()->mb->mn1[0]			= (*splitLine)[34].toInt();
		tbList.last()->mb->mn1[1]			= (*splitLine)[35].toInt();
		tbList.last()->mb->mn1[2]			= (*splitLine)[36].toInt();
		tbList.last()->mb->mn1[3]			= (*splitLine)[37].toInt();

		tbList.last()->ba->voltage			= (*splitLine)[38].toInt();
		tbList.last()->ba->current			= (*splitLine)[39].toInt();
		tbList.last()->ba->decoded.power	= (*splitLine)[40].toInt();
		tbList.last()->ba->temp				= (*splitLine)[41].toInt();
	}
}

struct std_variable TestBenchProject::getSerializedVar(int parameter)
{
	return getSerializedVar(parameter, 0);
}

struct std_variable TestBenchProject::getSerializedVar(int parameter, int index)
{
	struct std_variable var;

	if(index >= tbList.length())
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

		case 2: //"Ex1[0]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex1[0];
			var.decodedPtr = nullptr;
			break;
		case 3: //"Ex1[1]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex1[1];
			var.decodedPtr = nullptr;
			break;
		case 4: //"Ex1[2]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex1[2];
			var.decodedPtr = nullptr;
			break;
		case 5: //"Ex1[3]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex1[3];
			var.decodedPtr = nullptr;
			break;
		case 6: //"Ex1[4]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex1[4];
			var.decodedPtr = nullptr;
			break;
		case 7: //"Ex1[5]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex1[5];
			var.decodedPtr = nullptr;
			break;

		case 8: //"Strain"
			var.format = FORMAT_16U;
			var.rawGenPtr = &tbList[index]->ex1->strain;
			var.decodedPtr = &tbList[index]->ex1->decoded.strain;
			break;
		case 9: //"Analog[0]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &tbList[index]->ex1->analog[0];
			var.decodedPtr = &tbList[index]->ex1->decoded.analog[0];
			break;
		case 10: //"Analog[1]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &tbList[index]->ex1->analog[1];
			var.decodedPtr = &tbList[index]->ex1->decoded.analog[1];
			break;
		case 11: //"Current"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->ex1->current;
			var.decodedPtr = &tbList[index]->ex1->decoded.current;
			break;
		case 12: //"Encoder"
			var.format = FORMAT_32S;
			var.rawGenPtr = &tbList[index]->ex1->enc_ang;
			var.decodedPtr = nullptr;
			break;
		case 13: //"Battery Voltage"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex1->volt_batt;
			var.decodedPtr = &tbList[index]->ex1->decoded.volt_batt;
			break;
		case 14: //"Internal Voltage"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex1->volt_int;
			var.decodedPtr = &tbList[index]->ex1->decoded.volt_int;
			break;
		case 15: //"Temperature"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex1->temp;
			var.decodedPtr = &tbList[index]->ex1->decoded.temp;
			break;
		case 16: //"Status1"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex1->status1;
			var.decodedPtr = nullptr;
			break;
		case 17: //"Status2"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex1->status2;
			var.decodedPtr = nullptr;
			break;

		case 18: //"Ex2[0]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex2[0];
			var.decodedPtr = nullptr;
			break;
		case 19: //"Ex2[1]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex2[1];
			var.decodedPtr = nullptr;
			break;
		case 20: //"Ex2[2]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex2[2];
			var.decodedPtr = nullptr;
			break;
		case 21: //"Ex2[3]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex2[3];
			var.decodedPtr = nullptr;
			break;
		case 22: //"Ex2[4]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex2[4];
			var.decodedPtr = nullptr;
			break;
		case 23: //"Ex2[5]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->ex2[5];
			var.decodedPtr = nullptr;
			break;

		case 24: //"Strain"
			var.format = FORMAT_16U;
			var.rawGenPtr = &tbList[index]->ex2->strain;
			var.decodedPtr = &tbList[index]->ex2->decoded.strain;
			break;
		case 25: //"Analog[0]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &tbList[index]->ex2->analog[0];
			var.decodedPtr = &tbList[index]->ex2->decoded.analog[0];
			break;
		case 26: //"Analog[1]"
			var.format = FORMAT_16U;
			var.rawGenPtr = &tbList[index]->ex2->analog[1];
			var.decodedPtr = &tbList[index]->ex2->decoded.analog[1];
			break;
		case 27: //"Current"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->ex2->current;
			var.decodedPtr = &tbList[index]->ex2->decoded.current;
			break;
		case 28: //"Encoder"
			var.format = FORMAT_32S;
			var.rawGenPtr = &tbList[index]->ex2->enc_ang;
			var.decodedPtr = nullptr;
			break;
		case 29: //"Battery Voltage"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex2->volt_batt;
			var.decodedPtr = &tbList[index]->ex2->decoded.volt_batt;
			break;
		case 30: //"Internal Voltage"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex2->volt_int;
			var.decodedPtr = &tbList[index]->ex2->decoded.volt_int;
			break;
		case 31: //"Temperature"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex2->temp;
			var.decodedPtr = &tbList[index]->ex2->decoded.temp;
			break;
		case 32: //"Status1"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex2->status1;
			var.decodedPtr = nullptr;
			break;
		case 33: //"Status2"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ex2->status2;
			var.decodedPtr = nullptr;
			break;

		case 34: //"Mn1[0]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->mn1[0];
			var.decodedPtr = nullptr;
			break;
		case 35: //"Mn1[1]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->mn1[1];
			var.decodedPtr = nullptr;
			break;
		case 36: //"Mn1[2]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->mn1[2];
			var.decodedPtr = nullptr;
			break;
		case 37: //"Mn1[3]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->mb->mn1[3];
			var.decodedPtr = nullptr;
			break;

		case 38: //"Battery Voltage"
			var.format = FORMAT_16U;
			var.rawGenPtr = &tbList[index]->ba->voltage;
			var.decodedPtr = &tbList[index]->ba->decoded.voltage;
			break;
		case 39: //"Battery Current"
			var.format = FORMAT_16S;
			var.rawGenPtr = &tbList[index]->ba->current;
			var.decodedPtr = &tbList[index]->ba->decoded.current;
			break;
		case 40: //"Battery Power"
			var.format = NULL_PTR;
			var.rawGenPtr = nullptr;
			var.decodedPtr = &tbList[index]->ba->decoded.power;
			break;
		case 41: //"Battery Temperature"
			var.format = FORMAT_8U;
			var.rawGenPtr = &tbList[index]->ba->temp;
			var.decodedPtr = &tbList[index]->ba->decoded.temp;
			break;

		default:
			var.format = NULL_PTR;
			var.rawGenPtr = nullptr;
			var.decodedPtr = nullptr;
			break;
	}

	return var;
}

void TestBenchProject::clear(void)
{
	FlexseaDevice::clear();
	tbList.clear();
	timeStamp.clear();
}

void TestBenchProject::appendEmptyLine(void)
{
	timeStamp.append(TimeStamp());
	tbList.append(new testBench_s_plan());
}

void TestBenchProject::appendEmptyLineWithStruct(void)
{
	appendEmptyLine();
	tbList.last()->ex1 = new execute_s();
	tbList.last()->ex2 = new execute_s();
	tbList.last()->mb  = new motortb_s();
	tbList.last()->ba  = new battery_s();
}

void TestBenchProject::decodeLastLine(void)
{
	if(dataSource == LiveDataFile)
		{BatteryDevice::decompressRawBytes(tbList.last()->ba);}
	decode(tbList.last());
}

void TestBenchProject::decodeAllLine(void)
{
	for(int i = 0; i < tbList.size(); ++i)
	{
		decode(tbList[i]);
	}
}

void TestBenchProject::decode(struct testBench_s_plan *tbPtr)
{
	ExecuteDevice::decode(tbPtr->ex1);
	ExecuteDevice::decode(tbPtr->ex2);
	BatteryDevice::decode(tbPtr->ba);
}

QString TestBenchProject::getStatusStr(int index)
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

