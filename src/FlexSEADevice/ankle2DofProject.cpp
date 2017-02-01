/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2017 Dephy, Inc. <http://dephy.com/>

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
	[This file] ankle2DofProject: ankle2Dof Project Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-01-16 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "ankle2DofProject.h"
#include <QDebug>
#include <QTextStream>
#include "executeDevice.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

Ankle2DofProject::Ankle2DofProject(void): FlexseaDevice()
{
	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveType = "ankle2Dof";
}

Ankle2DofProject::Ankle2DofProject(execute_s *ex1Ptr, execute_s *ex2Ptr): FlexseaDevice()
{
	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());
	akList.append(new ankle2Dof_s_plan());
	akList.last()->ex1 = ex1Ptr;
	akList.last()->ex2 = ex2Ptr;
	serializedLength = header.length();
	slaveType = "ankle2Dof";
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString Ankle2DofProject::getHeaderStr(void)
{
	return header.join(',');
}

QStringList Ankle2DofProject::header = QStringList()
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
								<< "Status2"
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

QString Ankle2DofProject::getLastSerializedStr(void)
{
	QString str;
	QTextStream(&str) <<	timeStamp.last().date			<< ',' << \
							timeStamp.last().ms				<< ',' << \
							akList.last()->ex1->accel.x		<< ',' << \
							akList.last()->ex1->accel.y		<< ',' << \
							akList.last()->ex1->accel.z		<< ',' << \
							akList.last()->ex1->gyro.x		<< ',' << \
							akList.last()->ex1->gyro.y		<< ',' << \
							akList.last()->ex1->gyro.z		<< ',' << \
							akList.last()->ex1->strain		<< ',' << \
							akList.last()->ex1->analog[0]	<< ',' << \
							akList.last()->ex1->analog[1]	<< ',' << \
							akList.last()->ex1->current		<< ',' << \
							akList.last()->ex1->enc_display	<< ',' << \
							akList.last()->ex1->enc_control	<< ',' << \
							akList.last()->ex1->enc_commut	<< ',' << \
							akList.last()->ex1->volt_batt	<< ',' << \
							akList.last()->ex1->volt_int	<< ',' << \
							akList.last()->ex1->temp		<< ',' << \
							akList.last()->ex1->status1		<< ',' << \
							akList.last()->ex1->status2		<< ',' << \
							akList.last()->ex2->accel.x		<< ',' << \
							akList.last()->ex2->accel.y		<< ',' << \
							akList.last()->ex2->accel.z		<< ',' << \
							akList.last()->ex2->gyro.x		<< ',' << \
							akList.last()->ex2->gyro.y		<< ',' << \
							akList.last()->ex2->gyro.z		<< ',' << \
							akList.last()->ex2->strain		<< ',' << \
							akList.last()->ex2->analog[0]	<< ',' << \
							akList.last()->ex2->analog[1]	<< ',' << \
							akList.last()->ex2->current		<< ',' << \
							akList.last()->ex2->enc_display	<< ',' << \
							akList.last()->ex2->enc_control	<< ',' << \
							akList.last()->ex2->enc_commut	<< ',' << \
							akList.last()->ex2->volt_batt	<< ',' << \
							akList.last()->ex2->volt_int	<< ',' << \
							akList.last()->ex2->temp		<< ',' << \
							akList.last()->ex2->status1		<< ',' << \
							akList.last()->ex2->status2;;
	return str;
}

void Ankle2DofProject::appendSerializedStr(QStringList *splitLine)
{
	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		// Because of the pointer architecture of ankle2Dof_s_plan , we also
		// need to also add data structure
		appendEmptyLineWithStruct();

		timeStamp.last().date				= (*splitLine)[0];
		timeStamp.last().ms					= (*splitLine)[1].toInt();
		akList.last()->ex1->accel.x			= (*splitLine)[2].toInt();
		akList.last()->ex1->accel.y			= (*splitLine)[3].toInt();
		akList.last()->ex1->accel.z			= (*splitLine)[4].toInt();
		akList.last()->ex1->gyro.x			= (*splitLine)[5].toInt();
		akList.last()->ex1->gyro.y			= (*splitLine)[6].toInt();
		akList.last()->ex1->gyro.z			= (*splitLine)[7].toInt();
		akList.last()->ex1->strain			= (*splitLine)[8].toInt();
		akList.last()->ex1->analog[0]		= (*splitLine)[9].toInt();
		akList.last()->ex1->analog[1]		= (*splitLine)[10].toInt();
		akList.last()->ex1->current			= (*splitLine)[11].toInt();
		akList.last()->ex1->enc_display		= (*splitLine)[12].toInt();
		akList.last()->ex1->enc_control		= (*splitLine)[13].toInt();
		akList.last()->ex1->enc_commut		= (*splitLine)[14].toInt();
		akList.last()->ex1->volt_batt		= (*splitLine)[15].toInt();
		akList.last()->ex1->volt_int		= (*splitLine)[16].toInt();
		akList.last()->ex1->temp			= (*splitLine)[17].toInt();
		akList.last()->ex1->status1			= (*splitLine)[18].toInt();
		akList.last()->ex1->status2			= (*splitLine)[19].toInt();
		akList.last()->ex2->accel.x			= (*splitLine)[20].toInt();
		akList.last()->ex2->accel.y			= (*splitLine)[21].toInt();
		akList.last()->ex2->accel.z			= (*splitLine)[22].toInt();
		akList.last()->ex2->gyro.x			= (*splitLine)[23].toInt();
		akList.last()->ex2->gyro.y			= (*splitLine)[24].toInt();
		akList.last()->ex2->gyro.z			= (*splitLine)[25].toInt();
		akList.last()->ex2->strain			= (*splitLine)[26].toInt();
		akList.last()->ex2->analog[0]		= (*splitLine)[27].toInt();
		akList.last()->ex2->analog[1]		= (*splitLine)[28].toInt();
		akList.last()->ex2->current			= (*splitLine)[29].toInt();
		akList.last()->ex2->enc_display		= (*splitLine)[30].toInt();
		akList.last()->ex2->enc_control		= (*splitLine)[31].toInt();
		akList.last()->ex2->enc_commut		= (*splitLine)[32].toInt();
		akList.last()->ex2->volt_batt		= (*splitLine)[33].toInt();
		akList.last()->ex2->volt_int		= (*splitLine)[34].toInt();
		akList.last()->ex2->temp			= (*splitLine)[35].toInt();
		akList.last()->ex2->status1			= (*splitLine)[36].toInt();
		akList.last()->ex2->status2			= (*splitLine)[37].toInt();
	}
}

void Ankle2DofProject::clear(void)
{
	FlexseaDevice::clear();
	akList.clear();
	timeStamp.clear();
}

void Ankle2DofProject::appendEmptyLine(void)
{
	timeStamp.append(TimeStamp());
	akList.append(new ankle2Dof_s_plan());
}

void Ankle2DofProject::appendEmptyLineWithStruct(void)
{
	appendEmptyLine();
	akList.last()->ex1 = new execute_s();
	akList.last()->ex2 = new execute_s();
}

void Ankle2DofProject::decodeLastLine(void)
{
	decode(akList.last());
}

void Ankle2DofProject::decodeAllLine(void)
{
	for(int i = 0; i < akList.size(); ++i)
	{
		decode(akList[i]);
	}
}

void Ankle2DofProject::decode(struct ankle2Dof_s_plan *akPtr)
{
	ExecuteDevice::decode(akPtr->ex1);
	ExecuteDevice::decode(akPtr->ex2);
}

QString Ankle2DofProject::getStatusStr(int index)
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

