#include "dynamicuserdatamanager.h"
#include <dynamic_user_structs.h>
#include <QTimer>
#include <QDebug>
#include <flexsea_comm.h>

DynamicDevice* DynamicUserDataManager::device = nullptr;

DynamicUserDataManager::DynamicUserDataManager(QObject *parent) : QObject(parent) {}

void DynamicUserDataManager::requestMetaData(int slaveId)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

	//Prepare and send command:
	tx_cmd_user_dyn_r(TX_N_DEFAULT, SEND_METADATA);
	pack(P_AND_S_DEFAULT, slaveId, info, &numb, comm_str_usb);
	qDebug() << "Requesting Metadata";
	emit writeCommand(numb, comm_str_usb, READ);

//	static QTimer* timer = nullptr;
//	if(!timer)
//	{
//		timer = new QTimer();
//		timer->setSingleShot(false);
//		connect(timer, &QTimer::timeout, this, &DynamicUserDataManager::testSlot);
//		timer->start(100);
//	}
}

void DynamicUserDataManager::testSlot()
{
//	uint8_t info[2] = {PORT_USB, PORT_USB};
//	uint16_t numb = 0;

//	tx_cmd_user_dyn_r(TX_N_DEFAULT, SEND_DATA);
//	pack(P_AND_S_DEFAULT, slave, info, &numb, comm_str_usb);
//	qDebug() << "Requesting Data";
//	emit writeCommand(numb, comm_str_usb, READ);
}

bool DynamicUserDataManager::parseDynamicUserMetadata(QList<QString> *labels)
{	
	if(dynamicUser_numFields < 1) return false;
	if(!dynamicUser_labels || !dynamicUser_labelLengths || !labels) return false;
	if(!newMetaDataAvailable) return false;

	newMetaDataAvailable = 0;

	getDevice()->slaveID = dynamicUser_slaveId;

	labels->clear();
	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		QString label = "Unknown";
		if(dynamicUser_labels && dynamicUser_labelLengths)
		{
			QString s;
			char* str = dynamicUser_labels[i];
			uint8_t length = dynamicUser_labelLengths[i];
			for(int j = 0; j < length; j++)
				s.append(str[j]);

			label = s;
		}

		labels->append(label);
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

QString DynamicDevice::getHeaderStr(void)
{
	QString result = QStringLiteral("");
	if(dynamicUser_numFields < 1 || !dynamicUser_labels || !dynamicUser_labelLengths) return result;

	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		int labelLength = dynamicUser_labelLengths[i];
		QString label = QStringLiteral("");
		for(int j = 0; j < labelLength; j++)
			label.append(dynamicUser_labels[i][j]);

		result.append(label);
		if(i != dynamicUser_numFields-1)
			result.append(", ");
	}
	return result;
}

QStringList DynamicDevice::getHeaderList(void)
{
	QStringList result;
	if(dynamicUser_numFields < 1 || !dynamicUser_labels || !dynamicUser_labelLengths) return result;

	result.append("Timestamp (date)");
	result.append("Timestamp (ms)");
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

QStringList DynamicDevice::getHeaderDecList(void) { return this->getHeaderList(); }

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

QString DynamicDevice::getLastSerializedStr(void)
{
	QString result;
	QTextStream stream(&result);

	if(dynamicUser_numFields < 1 || !dynamicUser_data || !dynamicUser_fieldTypes || !dynamicUser_fieldLengths) return result;

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

struct std_variable DynamicDevice::getSerializedVar(int parameter, int index)
{
	(void)index;
	struct std_variable v;
	parameter-=2;
	if(parameter < 0 || parameter >= dynamicUser_numFields) return v;
	if(dynamicUser_numFields < 1 || !dynamicUser_data || !dynamicUser_fieldTypes || !dynamicUser_fieldLengths) return v;

	uint8_t* data = dynamicUser_data;
	int i;
	for(i = 0; i < parameter; i++)
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
void DynamicDevice::decodeLastLine(void){}
void DynamicDevice::decodeAllLine(void){}
void DynamicDevice::appendEmptyLine(void){}
int DynamicDevice::length(){ return 1; }
void DynamicDevice::clear(void){}
