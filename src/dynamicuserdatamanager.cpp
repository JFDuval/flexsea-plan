#include "dynamicuserdatamanager.h"
#include <dynamic_user_structs.h>
#include <flexsea_system.h>
#include <QTimer>
#include <QDebug>
#include <flexsea_comm.h>
#include <flexsea_board.h>

DynamicDevice* DynamicUserDataManager::device = nullptr;

DynamicUserDataManager::DynamicUserDataManager(QObject *parent) : QObject(parent) {}

void DynamicUserDataManager::requestMetaData(int slaveId)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

	//Prepare and send command:
	tx_cmd_user_dyn_r(TX_N_DEFAULT, SEND_METADATA);
	pack(P_AND_S_DEFAULT, slaveId, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, READ);
}

const char* FORMAT_TYPE_NAME_MAP[] = {"UINT32", "INT32", "UINT16", "INT16", "UINT8", "INT8", "QString", "NullPtr"};

void DynamicUserDataManager::handleNewMessage()
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;
	if(packAndSendOffsetRequest)
	{
		packAndSendOffsetRequest=0;
		pack(P_AND_S_DEFAULT, dynamicUser_slaveId, info, &numb, comm_str_usb);
		writeCommand(numb, comm_str_usb, WRITE);
	}
	else if(waitingOnFieldFlags && dynamicUser_numFields)
	{
		tx_cmd_user_dyn_w(TX_N_DEFAULT);
		pack(P_AND_S_DEFAULT, dynamicUser_slaveId, info, &numb, comm_str_usb);
		emit writeCommand(numb, comm_str_usb, WRITE);
	}
	else if(newMetaDataAvailable || newDataAvailable)
		emit newData();
}

bool DynamicUserDataManager::parseDynamicUserMetadata(QList<QString> *labels, QList<QString> *types)
{
	if(dynamicUser_numFields < 1) return false;
	if(!dynamicUser_labels || !dynamicUser_labelLengths || !labels) return false;
	if(!newMetaDataAvailable) return false;

	newMetaDataAvailable = 0;

	getDevice()->slaveID = dynamicUser_slaveId;

	labels->clear();
	types->clear();
	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		QString label = "Unknown";
		QString type = "Unknown";
		if(dynamicUser_labels && dynamicUser_labelLengths)
		{
			QString s;
			char* str = dynamicUser_labels[i];
			uint8_t length = dynamicUser_labelLengths[i];
			for(int j = 0; j < length; j++)
				s.append(str[j]);

			label = s;
		}
		if(dynamicUser_fieldTypes && dynamicUser_fieldTypes[i] < NULL_PTR)
		{
			int fieldType = dynamicUser_fieldTypes[i];
			int l = strlen(FORMAT_TYPE_NAME_MAP[fieldType]);
			QString s = "";
			for(int j = 0; j < l; j++)
				s.append(FORMAT_TYPE_NAME_MAP[fieldType][j]);
			type = s;
		}

		labels->append(label);
		types->append(type);
	}
	return true;
}

bool DynamicUserDataManager::parseDynamicUserData(QList<QString> *data)
{
	if(dynamicUser_numFields < 1) return false;
	if(!dynamicUser_data || !data) return false;
	if(!newDataAvailable) return false;

	newDataAvailable = 0;

	data->clear();

	uint8_t* d = dynamicUser_data;
	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		QString s = DynamicDevice::readPointerAsFormat(d, dynamicUser_fieldTypes[i]);
		data->append(s);

		int dataSize = FORMAT_SIZE_MAP[dynamicUser_fieldTypes[i]];
		if(dataSize < 1)
		{
			qDebug() << "Invalid Data Size, trying to decode some none numeric format";
			d = d + 1;
		}
		else { d = d + dataSize; }
	}
	return true;
}

bool DynamicUserDataManager::getPlanFieldFlags(QList<bool> *flags)
{
	flags->clear();
	if(dynamicUser_numFields < 1) return false;
	if(!dynamicUser_fieldFlagsPlan || !flags) return false;

	//this turns on the first 8 flags by default
	static bool firstTime = true;
	if(firstTime)
	{
		firstTime = false;
		for(uint8_t i = 0; i < 8 && i < dynamicUser_numFields; i++)
			dynamicUser_fieldFlagsPlan[i] = 1;
	}

	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		flags->append(dynamicUser_fieldFlagsPlan[i]);
	}
	return true;
}

bool DynamicUserDataManager::getExecFieldFlags(QList<bool> *flags)
{
	flags->clear();
	if(dynamicUser_numFields < 1) return false;
	if(!dynamicUser_fieldFlagsExec || !flags) return false;
	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		flags->append(dynamicUser_fieldFlagsExec[i]);
	}
	return true;
}

bool DynamicUserDataManager::setPlanFieldFlag(int index, bool setTrue)
{
	if(index < 0 || index >= dynamicUser_numFields) return false;
	if(!dynamicUser_fieldFlagsPlan) return false;

	qDebug() << "Setting a field flag";

	dynamicUser_fieldFlagsPlan[index] = setTrue;
	return true;
}

DynamicDevice* DynamicUserDataManager::getDevice()
{
	if(!device)
		device = new DynamicDevice();

	return device;
}

int DynamicUserDataManager::getCommandCode() { return CMD_USER_DYNAMIC; }

DynamicDevice::DynamicDevice()
{
	slaveID = -1;
	QString dd = "DynamicDevice";
	slaveName = dd;
	slaveTypeName = dd;
	targetSlaveName = dd;
	experimentIndex = -1;
	experimentName ="Dynamic Read";
	frequency = 0;
	isCurrentlyLogging = 0;
	serializedLength = dynamicUser_numFields;
}

QStringList DynamicDevice::getHeader(void)
{
	QStringList result;
	if(dynamicUser_numFields < 1 || !dynamicUser_labels || !dynamicUser_labelLengths) return result;

	result.append("Timestamp (date)");
	result.append("Timestamp (ms)");
	result.append("Event Flags");
	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		int labelLength = dynamicUser_labelLengths[i];
		QString label = QStringLiteral("");
		for(int j = 0; j < labelLength; j++)
			label.append(dynamicUser_labels[i][j]);

		result.append(label);
	}
	return result;
}

// todo change this to return the measure unit properly
QStringList DynamicDevice::getHeaderUnit(void) { return this->getHeader(); }

void DynamicDevice::readPointerAsFormat(QTextStream* stream, void* data, uint8_t format)
{
	if(format == FORMAT_32U)		*stream << (*((uint32_t*)data));
	else if(format == FORMAT_32S)	*stream << (*((int32_t*)data));
	else if(format == FORMAT_16U)	*stream << (*((uint16_t*)data));
	else if(format == FORMAT_16S)	*stream << (*((int16_t*)data));
	else if(format == FORMAT_8U)	*stream << (*((uint8_t*)data));
	else if(format == FORMAT_8S)	*stream << (*((int8_t*)data));
	else *stream << "-";
}

QString DynamicDevice::readPointerAsFormat(void* data, uint8_t format)
{
	QString s = QStringLiteral("");
	QTextStream stream(&s);
	readPointerAsFormat(&stream, data, format);
	return s;
}

QString DynamicDevice::getLastDataEntry(void)
{
	QString result;
	QTextStream stream(&result);

	if(dynamicUser_numFields < 1 || !dynamicUser_data || !dynamicUser_fieldTypes)
		return result;

	stream	<<	timeStamp.last().date		<< ',' << \
				timeStamp.last().ms			<< ',' << \
				eventFlags.last()			<< ',';

	uchar* data = dynamicUser_data;
	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		readPointerAsFormat(&stream, data, dynamicUser_fieldTypes[i]);

		if(i != dynamicUser_numFields - 1)
			stream << ", ";

		int dataSize = FORMAT_SIZE_MAP[dynamicUser_fieldTypes[i]];
		if(dataSize < 1)
		{
			qDebug() << "Invalid Data Size, trying to decode some none numeric format";
			data = data + 1;
		}
		else { data = data + dataSize; }
	}

	return result;
}

struct std_variable DynamicDevice::getSerializedVar(int headerIndex, int index)
{
	(void)index;
	struct std_variable v;
	headerIndex -= 3;	//Tweak this if 2DPlot and UserR/W are offseted
	if(headerIndex < 0 || headerIndex >= dynamicUser_numFields) return v;
	if(dynamicUser_numFields < 1 || !dynamicUser_data || !dynamicUser_fieldTypes)
		return v;

	uint8_t* data = dynamicUser_data;
	int i;
	for(i = 0; i < headerIndex; i++)
	{
		int dataSize = FORMAT_SIZE_MAP[dynamicUser_fieldTypes[i]];
		if(dataSize < 1)
		{
			qDebug() << "Invalid Data Size, trying to decode some none numeric format";
			data = data + 1;
		}
		else { data = data + dataSize; }
	}

	v.format = dynamicUser_fieldTypes[i];
	v.rawGenPtr = data;
	v.decodedPtr = nullptr;

	return v;
}

void DynamicDevice::appendSerializedStr(QStringList *splitLine) {(void)splitLine;}
void DynamicDevice::decodeLastElement(void){}
void DynamicDevice::decodeAllElement(void){}
void DynamicDevice::appendEmptyElement(void) {
	timeStamp.append(TimeStamp());
	eventFlags.append(0);
}
int DynamicDevice::length(){ return 1; }
void DynamicDevice::clear(void){}
