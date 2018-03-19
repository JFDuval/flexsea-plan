/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2017 Dephy, Inc. <http://dephy.com/>
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] pocketDevice: Pocket Device Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2018-02-22 | sbelanger | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "pocketDevice.h"
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

PocketDevice::PocketDevice(void): FlexseaDevice()
{
	if(header.length() != headerUnitList.length())
	{
		qDebug() << "Mismatch between header length Pocket!";
	}

	this->dataSource = LogDataFile;
	serializedLength = header.length();
	slaveTypeName = "pocket";
}

PocketDevice::PocketDevice(pocket_s *devicePtr): FlexseaDevice()
{
	if(header.length() != headerUnitList.length())
	{
		qDebug() << "Mismatch between header length Rigid!";
	}

	this->dataSource = LiveDataFile;
	timeStamp.append(TimeStamp());

	poList.append(devicePtr);
	poList.last()->ex[0].enc_ang = &enc_ang;
	poList.last()->ex[0].enc_ang_vel = &enc_vel;
	poList.last()->ex[0].joint_ang = &joint_ang;
	poList.last()->ex[0].joint_ang_vel = &joint_ang_vel;
	poList.last()->ex[0].joint_ang_from_mot = &joint_ang_from_mot;

	poList.last()->ex[1].enc_ang = &enc_ang2;
	poList.last()->ex[1].enc_ang_vel = &enc_vel2;
	poList.last()->ex[1].joint_ang = &joint_ang2;
	poList.last()->ex[1].joint_ang_vel = &joint_ang_vel2;
	poList.last()->ex[1].joint_ang_from_mot = &joint_ang_from_mot2;

	poList.last()->ctrl.ank_ang_deg = &ank_ang_deg;
	poList.last()->ctrl.ank_ang_from_mot = &ank_ang_from_mot;

	ownershipList.append(false); //we assume we don't own this device ptr, and whoever passed it to us is responsible for clean up
	eventFlags.append(0);

	serializedLength = header.length();
	slaveTypeName = "pocket";
}

PocketDevice::~PocketDevice()
{
	if(ownershipList.size() != poList.size())
	{
		qDebug() << "Pocket Device class cleaning up: execute list size doesn't match list of ownership info size.";
		qDebug() << "Not sure whether it is safe to delete these device records.";
		return;
	}

	while(ownershipList.size())
	{
		bool shouldDelete = ownershipList.takeLast();
		pocket_s* readyToDelete = poList.takeLast();
		if(shouldDelete)
		{
			delete readyToDelete->ex[0].enc_ang;
			readyToDelete->ex[0].enc_ang = nullptr;

			delete readyToDelete->ex[0].enc_ang_vel;
			readyToDelete->ex[0].enc_ang_vel = nullptr;

			delete readyToDelete->ex[0].joint_ang;
			readyToDelete->ex[0].joint_ang = nullptr;

			delete readyToDelete->ex[0].joint_ang_vel;
			readyToDelete->ex[0].joint_ang_vel = nullptr;

			delete readyToDelete->ex[0].joint_ang_from_mot;
			readyToDelete->ex[0].joint_ang_from_mot = nullptr;


			delete readyToDelete->ex[1].enc_ang;
			readyToDelete->ex[1].enc_ang = nullptr;

			delete readyToDelete->ex[1].enc_ang_vel;
			readyToDelete->ex[1].enc_ang_vel = nullptr;

			delete readyToDelete->ex[1].joint_ang;
			readyToDelete->ex[1].joint_ang = nullptr;

			delete readyToDelete->ex[1].joint_ang_vel;
			readyToDelete->ex[1].joint_ang_vel = nullptr;

			delete readyToDelete->ex[1].joint_ang_from_mot;
			readyToDelete->ex[1].joint_ang_from_mot = nullptr;

			delete readyToDelete->ctrl.ank_ang_deg;
			readyToDelete->ctrl.ank_ang_deg = nullptr;

			delete readyToDelete->ctrl.ank_ang_from_mot;
			readyToDelete->ctrl.ank_ang_from_mot = nullptr;

			delete readyToDelete;
		}
	}
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QStringList PocketDevice::header = QStringList()
	<< "Timestamp"
	<< "Timestamp (ms)"
	<< "Event Flags"
	<< "State time"
	<< "Last Offset"
	<< "Accel X"
	<< "Accel Y"
	<< "Accel Z"
	<< "Gyro X"
	<< "Gyro Y"
	<< "Gyro Z"
	<< "Analog[0]"
	<< "Analog[1]"
	<< "VB"
	<< "5V"
	<< "Battery current"
	<< "Temperature"
	<< "M1 Joint Angle"
	<< "M1 Joint Velocity"
	<< "M1 Motor Angle"
	<< "M1 Motor Velocity"
	<< "M1 Motor Accel"
	<< "M1 Motor Current"
	<< "M1 Motor Voltage"
	<< "M1 Strain"
	<< "M2 Joint Angle"
	<< "M2 Joint Velocity"
	<< "M2 Motor Angle"
	<< "M2 Motor Velocity"
	<< "M2 Motor Accel"
	<< "M2 Motor Current"
	<< "M2 Motor Voltage"
	<< "M2 Strain"
	<< "genVar[0]"
	<< "genVar[1]"
	<< "genVar[2]"
	<< "genVar[3]"
	<< "genVar[4]"
	<< "genVar[5]"
	<< "genVar[6]"
	<< "genVar[7]"
	<< "genVar[8]"
	<< "genVar[9]";

QStringList PocketDevice::headerUnitList = QStringList()
	<< "Raw Value Only"
	<< "Raw Value Only"
	<< "Raw Value Only"
	<< "Raw Value Only"
	<< "Raw Value Only"
	<< "Decoded: mg"
	<< "Decoded: mg"
	<< "Decoded: mg"
	<< "Decoded: deg/s"
	<< "Decoded: deg/s"
	<< "Decoded: deg/s"
	<< "Raw value only"
	<< "Raw value only"
	<< "mV"
	<< "mV"
	<< "mA"
	<< "Celsius"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "mA"
	<< "mV"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "mA"
	<< "mV"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only"
	<< "Raw value only";

QString PocketDevice::getLastDataEntry(void)
{
	QString str;
	QTextStream(&str) << \
		timeStamp.last().date						<< ',' << \
		timeStamp.last().ms							<< ',' << \
		eventFlags.last()							<< ',' << \
		poList.last()->ctrl.timestamp				<< ',' << \
		poList.last()->lastOffsetDecoded			<< ',' << \

		poList.last()->mn.accel.x					<< ',' << \
		poList.last()->mn.accel.y					<< ',' << \
		poList.last()->mn.accel.z					<< ',' << \
		poList.last()->mn.gyro.x					<< ',' << \
		poList.last()->mn.gyro.y					<< ',' << \
		poList.last()->mn.gyro.z					<< ',' << \

		poList.last()->mn.analog[0]					<< ',' << \
		poList.last()->mn.analog[1]					<< ',' << \
		poList.last()->re.vb						<< ',' << \
		poList.last()->re.v5						<< ',' << \
		poList.last()->re.current					<< ',' << \
		poList.last()->re.temp						<< ',' << \

		*(poList.last()->ex[0].joint_ang)			<< ',' << \
		*(poList.last()->ex[0].joint_ang_vel)		<< ',' << \
		*(poList.last()->ex[0].enc_ang)				<< ',' << \
		*(poList.last()->ex[0].enc_ang_vel)			<< ',' << \
		poList.last()->ex[0].mot_acc				<< ',' << \
		poList.last()->ex[0].mot_current			<< ',' << \
		poList.last()->ex[0].mot_volt				<< ',' << \
		poList.last()->ex[0].strain					<< ',' << \

		*(poList.last()->ex[1].joint_ang)			<< ',' << \
		*(poList.last()->ex[1].joint_ang_vel)		<< ',' << \
		*(poList.last()->ex[1].enc_ang)				<< ',' << \
		*(poList.last()->ex[1].enc_ang_vel)			<< ',' << \
		poList.last()->ex[1].mot_acc				<< ',' << \
		poList.last()->ex[1].mot_current			<< ',' << \
		poList.last()->ex[1].mot_volt				<< ',' << \
		poList.last()->ex[1].strain					<< ',' << \

		poList.last()->mn.genVar[0]					<< ',' << \
		poList.last()->mn.genVar[1]					<< ',' << \
		poList.last()->mn.genVar[2]					<< ',' << \
		poList.last()->mn.genVar[3]					<< ',' << \
		poList.last()->mn.genVar[4]					<< ',' << \
		poList.last()->mn.genVar[5]					<< ',' << \
		poList.last()->mn.genVar[6]					<< ',' << \
		poList.last()->mn.genVar[7]					<< ',' << \
		poList.last()->mn.genVar[8]					<< ',' << \
		poList.last()->mn.genVar[9];

	return str;
}

void PocketDevice::appendSerializedStr(QStringList *splitLine)
{
	uint8_t idx = 0;

	//Check if data line contain the number of data expected
	if(splitLine->length() >= serializedLength)
	{
		appendEmptyElement();
		timeStamp.last().date						= (*splitLine)[idx++];
		timeStamp.last().ms							= (*splitLine)[idx++].toInt();
		eventFlags.last()							= (*splitLine)[idx++].toInt();
		poList.last()->ctrl.timestamp				= (*splitLine)[idx++].toInt();
		poList.last()->lastOffsetDecoded			= (*splitLine)[idx++].toInt();

		poList.last()->mn.accel.x					= (*splitLine)[idx++].toInt();
		poList.last()->mn.accel.y					= (*splitLine)[idx++].toInt();
		poList.last()->mn.accel.z					= (*splitLine)[idx++].toInt();
		poList.last()->mn.gyro.x					= (*splitLine)[idx++].toInt();
		poList.last()->mn.gyro.y					= (*splitLine)[idx++].toInt();
		poList.last()->mn.gyro.z					= (*splitLine)[idx++].toInt();
		poList.last()->mn.analog[0]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.analog[1]					= (*splitLine)[idx++].toInt();

		poList.last()->re.vb						= (*splitLine)[idx++].toInt();
		poList.last()->re.v5						= (*splitLine)[idx++].toInt();
		poList.last()->re.current					= (*splitLine)[idx++].toInt();
		poList.last()->re.temp						= (*splitLine)[idx++].toInt();

		*(poList.last()->ex[0].joint_ang)			= (*splitLine)[idx++].toInt();
		*(poList.last()->ex[0].joint_ang_vel)		= (*splitLine)[idx++].toInt();
		*(poList.last()->ex[0].enc_ang)				= (*splitLine)[idx++].toInt();
		*(poList.last()->ex[0].enc_ang_vel)			= (*splitLine)[idx++].toInt();
		poList.last()->ex[0].mot_acc				= (*splitLine)[idx++].toInt();
		poList.last()->ex[0].mot_current			= (*splitLine)[idx++].toInt();
		poList.last()->ex[0].mot_volt				= (*splitLine)[idx++].toInt();
		poList.last()->ex[0].strain					= (*splitLine)[idx++].toInt();

		*(poList.last()->ex[1].joint_ang)			= (*splitLine)[idx++].toInt();
		*(poList.last()->ex[1].joint_ang_vel)		= (*splitLine)[idx++].toInt();
		*(poList.last()->ex[1].enc_ang)				= (*splitLine)[idx++].toInt();
		*(poList.last()->ex[1].enc_ang_vel)			= (*splitLine)[idx++].toInt();
		poList.last()->ex[1].mot_acc				= (*splitLine)[idx++].toInt();
		poList.last()->ex[1].mot_current			= (*splitLine)[idx++].toInt();
		poList.last()->ex[1].mot_volt				= (*splitLine)[idx++].toInt();
		poList.last()->ex[1].strain					= (*splitLine)[idx++].toInt();

		poList.last()->mn.genVar[0]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.genVar[1]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.genVar[2]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.genVar[3]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.genVar[4]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.genVar[5]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.genVar[6]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.genVar[7]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.genVar[8]					= (*splitLine)[idx++].toInt();
		poList.last()->mn.genVar[9]					= (*splitLine)[idx++].toInt();
	}
}

struct std_variable PocketDevice::getSerializedVar(int headerIndex, int index)
{
	struct std_variable var;

	if(index >= poList.length())
	{
		headerIndex = INT_MAX;
	}

	//Assign pointer:
	switch(headerIndex)
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
		case 2: //"Event Flag"
			var.format = FORMAT_32S;
			var.rawGenPtr = &eventFlags[index];
			var.decodedPtr = nullptr;
			break;
		case 3: //"State time"
			var.format = FORMAT_32U;
			var.rawGenPtr = &poList[index]->ctrl.timestamp;
			var.decodedPtr = nullptr;
			break;
		case 4: //"Last offset decoded"
			var.format = FORMAT_8U;
			var.rawGenPtr = &poList[index]->lastOffsetDecoded;
			var.decodedPtr = nullptr;
			break;
		case 5: //"Accel X"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.accel.x;
			var.decodedPtr = &poList[index]->mn.decoded.accel.x;
			break;
		case 6: //"Accel Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.accel.y;
			var.decodedPtr = &poList[index]->mn.decoded.accel.y;
			break;
		case 7: //"Accel Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.accel.z;
			var.decodedPtr = &poList[index]->mn.decoded.accel.z;
			break;
		case 8: //"Gyro X"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.gyro.x;
			var.decodedPtr = &poList[index]->mn.decoded.gyro.x;
			break;
		case 9: //"Gyro Y"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.gyro.y;
			var.decodedPtr = &poList[index]->mn.decoded.gyro.y;
			break;
		case 10: //"Gyro Z"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.gyro.z;
			var.decodedPtr = &poList[index]->mn.decoded.gyro.z;
			break;
		case 11: //"Analog 0"
			var.format = FORMAT_16U;
			var.rawGenPtr = &poList[index]->mn.analog[0];
			var.decodedPtr = nullptr;
			break;
		case 12: //"Analog 1"
			var.format = FORMAT_16U;
			var.rawGenPtr = &poList[index]->mn.analog[1];
			var.decodedPtr = nullptr;
			break;
		case 13: //"+VB"
			var.format = FORMAT_16U;
			var.rawGenPtr = &poList[index]->re.vb;
			var.decodedPtr = nullptr;
			break;
		case 14: //"+5V"
			var.format = FORMAT_16U;
			var.rawGenPtr = &poList[index]->re.v5;
			var.decodedPtr = nullptr;
			break;
		case 15: //"Battery Current"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->re.current;
			var.decodedPtr = nullptr;
			break;
		case 16: //"Temperature"
			var.format = FORMAT_8S;
			var.rawGenPtr = &poList[index]->re.temp;
			var.decodedPtr = nullptr;
			break;
		case 17: //"Joint Angle 0"
			var.format = FORMAT_16S;
			var.rawGenPtr = poList[index]->ex[0].joint_ang;
			var.decodedPtr = nullptr;
			break;
		case 18: //"Joint Velocity 0"
			var.format = FORMAT_16S;
			var.rawGenPtr = poList[index]->ex[0].joint_ang_vel;
			var.decodedPtr = nullptr;
			break;
		case 19: //"Motor Angle 0"
			var.format = FORMAT_32S;
			var.rawGenPtr = poList[index]->ex[0].enc_ang;
			var.decodedPtr = nullptr;
			break;
		case 20: //"Motor Velocity 0"
			var.format = FORMAT_32S;
			var.rawGenPtr = poList[index]->ex[0].enc_ang_vel;
			var.decodedPtr = nullptr;
			break;
		case 21: //"Motor Acceleration 0"
			var.format = FORMAT_32S;
			var.rawGenPtr = &poList[index]->ex[0].mot_acc;
			var.decodedPtr = nullptr;
			break;
		case 22: //"Motor current 0"
			var.format = FORMAT_32S;
			var.rawGenPtr = &poList[index]->ex[0].mot_current;
			var.decodedPtr = nullptr;
			break;
		case 23: //"Motor Voltage 0"
			var.format = FORMAT_32S;
			var.rawGenPtr = &poList[index]->ex[0].mot_volt;
			var.decodedPtr = nullptr;
			break;
		case 24: //"Strain"
			var.format = FORMAT_16U;
			var.rawGenPtr = &poList[index]->ex[0].strain;
			var.decodedPtr = nullptr;
			break;
		case 25: //"Joint Angle 1"
			var.format = FORMAT_16S;
			var.rawGenPtr = poList[index]->ex[1].joint_ang;
			var.decodedPtr = nullptr;
			break;
		case 26: //"Joint Velocity 1"
			var.format = FORMAT_16S;
			var.rawGenPtr = poList[index]->ex[1].joint_ang_vel;
			var.decodedPtr = nullptr;
			break;
		case 27: //"Motor Angle 1"
			var.format = FORMAT_32S;
			var.rawGenPtr = poList[index]->ex[1].enc_ang;
			var.decodedPtr = nullptr;
			break;
		case 28: //"Motor Velocity 1"
			var.format = FORMAT_32S;
			var.rawGenPtr = poList[index]->ex[1].enc_ang_vel;
			var.decodedPtr = nullptr;
			break;
		case 29: //"Motor Acceleration 1"
			var.format = FORMAT_32S;
			var.rawGenPtr = &poList[index]->ex[1].mot_acc;
			var.decodedPtr = nullptr;
			break;
		case 30: //"Motor current 1"
			var.format = FORMAT_32S;
			var.rawGenPtr = &poList[index]->ex[1].mot_current;
			var.decodedPtr = nullptr;
			break;
		case 31: //"Motor Voltage 1"
			var.format = FORMAT_32S;
			var.rawGenPtr = &poList[index]->ex[1].mot_volt;
			var.decodedPtr = nullptr;
			break;
		case 32: //"Strain 1"
			var.format = FORMAT_16U;
			var.rawGenPtr = &poList[index]->ex[1].strain;
			var.decodedPtr = nullptr;
			break;
		case 33: //"genVar[0]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[0];
			var.decodedPtr = nullptr;
			break;
		case 34: //"genVar[1]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[1];
			var.decodedPtr = nullptr;
			break;
		case 35: //"genVar[2]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[2];
			var.decodedPtr = nullptr;
			break;
		case 36: //"genVar[3]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[3];
			var.decodedPtr = nullptr;
			break;
		case 37: //"genVar[4]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[4];
			var.decodedPtr = nullptr;
			break;
		case 38: //"genVar[5]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[5];
			var.decodedPtr = nullptr;
			break;
		case 39: //"genVar[6]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[6];
			var.decodedPtr = nullptr;
			break;
		case 40: //"genVar[7]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[7];
			var.decodedPtr = nullptr;
			break;
		case 41: //"genVar[8]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[8];
			var.decodedPtr = nullptr;
			break;
		case 42: //"genVar[9]"
			var.format = FORMAT_16S;
			var.rawGenPtr = &poList[index]->mn.genVar[9];
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

void PocketDevice::clear(void)
{
	FlexseaDevice::clear();
	poList.clear();
	ownershipList.clear();
	timeStamp.clear();
	eventFlags.clear();
}

void PocketDevice::appendEmptyElement(void)
{
	timeStamp.append(TimeStamp());

	pocket_s *emptyStruct = new pocket_s();
	emptyStruct->ex[0].enc_ang = new int32_t();
	emptyStruct->ex[0].enc_ang_vel = new int32_t();
	emptyStruct->ex[0].joint_ang = new int16_t();
	emptyStruct->ex[0].joint_ang_vel = new int16_t();
	emptyStruct->ex[0].joint_ang_from_mot = new int16_t();

	emptyStruct->ex[1].enc_ang = new int32_t();
	emptyStruct->ex[1].enc_ang_vel = new int32_t();
	emptyStruct->ex[1].joint_ang = new int16_t();
	emptyStruct->ex[1].joint_ang_vel = new int16_t();
	emptyStruct->ex[1].joint_ang_from_mot = new int16_t();

	emptyStruct->ctrl.ank_ang_deg = new int16_t();
	emptyStruct->ctrl.ank_ang_from_mot = new int16_t();

	poList.append(emptyStruct);
	ownershipList.append(true); // we own this struct, so we must delete it in destructor
	eventFlags.append(0);
}

void PocketDevice::decodeLastElement(void)
{
	decode(poList.last());
}

void PocketDevice::decodeAllElement(void)
{
	for(int i = 0; i < poList.size(); ++i)
	{
		decode(poList[i]);
	}
}

QString PocketDevice::getStatusStr(int index)
{
	(void)index;	//Unused

	return QString("No decoding available for this board");
}

void PocketDevice::decode(struct pocket_s *riPtr)
{
	//Accel in mG
	riPtr->mn.decoded.accel.x = (1000*riPtr->mn.accel.x)/8192;
	riPtr->mn.decoded.accel.y = (1000*riPtr->mn.accel.y)/8192;
	riPtr->mn.decoded.accel.z = (1000*riPtr->mn.accel.z)/8192;

	//Gyro in degrees/s
	riPtr->mn.decoded.gyro.x = (10*riPtr->mn.gyro.x)/328;
	riPtr->mn.decoded.gyro.y = (10*riPtr->mn.gyro.y)/328;
	riPtr->mn.decoded.gyro.z = (10*riPtr->mn.gyro.z)/328;

	//Magneto in uT (0.15uT/LSB)
	riPtr->mn.decoded.magneto.x = (15*riPtr->mn.magneto.x)/100;
	riPtr->mn.decoded.magneto.y = (15*riPtr->mn.magneto.y)/100;
	riPtr->mn.decoded.magneto.z = (15*riPtr->mn.magneto.z)/100;
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

