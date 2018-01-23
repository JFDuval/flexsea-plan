#ifndef DYNAMICUSERDATAMANAGER_H
#define DYNAMICUSERDATAMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <flexseaDevice.h>
#include <QTextStream>

class DynamicDevice : public FlexseaDevice
{
public:
	explicit DynamicDevice();
	virtual ~DynamicDevice(){}

	QStringList getHeader(void);
	QStringList getHeaderUnit(void);
	QString getLastDataEntry(void);
	struct std_variable getSerializedVar(int parameter, int index);
	void appendSerializedStr(QStringList *splitLine);
	void decodeLastElement(void);
	void decodeAllElement(void);
	void appendEmptyElement(void);
	int length();
	void clear(void);

	static void readPointerAsFormat(QTextStream* stream, void* data, uint8_t format);
	static QString readPointerAsFormat(void* data, uint8_t format);
};

class DynamicUserDataManager : public QObject
{
	Q_OBJECT
public:
	explicit DynamicUserDataManager(QObject *parent = 0);
	virtual ~DynamicUserDataManager() {
		if(device) delete device;
		device = nullptr;
	}

	DynamicDevice* getDevice();
	int getCommandCode();
	void requestMetaData(int slaveId);
	bool parseDynamicUserMetadata(QList<QString> *labels, QList<QString> *types);
	bool parseDynamicUserData(QList<QString> *values);
	bool getPlanFieldFlags(QList<bool> *flags);
	bool getExecFieldFlags(QList<bool> *flags);
	bool setPlanFieldFlag(int index, bool setTrue);

public slots:
	void handleNewMessage();

signals:
	void writeCommand(uint8_t numb, uint8_t *tx_data, uint8_t r_w);
	void newData();

private:
	static DynamicDevice* device;

};

#endif // DYNAMICUSERDATAMANAGER_H
