#ifndef STREAMMANAGER_H
#define STREAMMANAGER_H

#include <QObject>
#include <QTimer>
#include <serialdriver.h>
#include <QString>
#include <ctime>
#include <QDateTime>
#include <QSharedPointer>
#include <FlexSEADevice/flexseaDevice.h>
#include <queue>

class ComManager : public QObject
{
	Q_OBJECT
public:
	explicit ComManager(QObject *parent = 0);
	virtual ~ComManager() {
		if(clockTimer) delete clockTimer;
		clockTimer = nullptr;
	}

	SerialDriver* mySerialDriver;

	static const int NUM_TIMER_FREQS = 11;

signals:
	//ComManager
	void sentRead(int cmd, int slave);
	void openRecordingFile(FlexseaDevice* device);
	void closeRecordingFile(FlexseaDevice* device);
	void streamingFrequency(int f);

	//SerialDriver
	void openStatus(SerialPortStatus status,int nbTries);
	void newDataReady(void);
	void dataStatus(int idx, int status);
	void newDataTimeout(bool rst);
	void setStatusBarMessage(QString msg);
	void writeToLogFile(FlexseaDevice*);
	void aboutToClose(void);


public slots:
	void init();
	void startStreaming(bool shouldLog, FlexseaDevice* logToDevice);
	void startAutoStreaming(bool shouldLog,	FlexseaDevice* logToDevice);
	void startAutoStreaming(bool shouldLog, FlexseaDevice* logToDevice, \
							uint8_t firstIndex, uint8_t lastIndex);
	void stopStreaming(FlexseaDevice* device);
	void stopStreaming(int cmd, uint8_t slave, int freq);
	void setOffsetParameter(QList<int> ricnuOffsets, QList<int> rigidOffsets, int minOffs, int maxOffs);
	QList<int> getRefreshRates();

	void receiveClock();
	void enqueueCommand(uint8_t numb, uint8_t* dataPacket);
	void onComPortClosing();

	void open(QString name, int tries, int delay, bool* success);
	void close(void);
	void tryReadWrite(uint8_t bytes_to_send, uint8_t *serial_tx_data, int timeout);
	int write(uint8_t bytes_to_send, uint8_t *serial_tx_data);

	void flush(void);
	void clear(void);

	void addDevice(FlexseaDevice* device);

private:

	//Variables & Objects:
	class Message {
	public:
		static void do_delete(uint8_t buf[]) { delete[] buf; }
		Message(uint8_t nb, uint8_t* data) {
			numBytes = nb;
			dataPacket = QSharedPointer<uint8_t>(new uint8_t[nb], do_delete);
			uint8_t* temp = dataPacket.data();
			for(int i = 0; i < numBytes; i++)
				temp[i] = data[i];
		}

		uint8_t numBytes;
		QSharedPointer<uint8_t> dataPacket;
		uint8_t r_w;
	};
	std::queue<Message> outgoingBuffer;

	class CmdSlaveRecord
	{
	public:
		CmdSlaveRecord(int c, int s, bool l, FlexseaDevice* d):
			cmdType(c), slaveIndex(s), shouldLog(l), device(d) {}
		int cmdType;
		int slaveIndex;
		bool shouldLog;
		clock_t initialTime;
		QString date;
		FlexseaDevice* device;
	};
	std::vector<CmdSlaveRecord> autoStreamLists[NUM_TIMER_FREQS];
	std::vector<CmdSlaveRecord> streamLists[NUM_TIMER_FREQS];

	void tryPackAndSend(int cmd, uint8_t slaveId);
	void packAndSendStopStreaming(int cmd, uint8_t slaveId);
	int getIndexOfFrequency(int freq);
	QString getNameOfExperiment(int cmd);
	int timerFrequencies[NUM_TIMER_FREQS];
	float timerIntervals[NUM_TIMER_FREQS];

	QList<QString> experimentLabels;
	QList<int> experimentCodes;
	QTimer* clockTimer;
	float clockPeriod;

	QList<int> ricnuOffsets, rigidOffsets;
	int minOffs = 0, maxOffs = 0;

	void sendCommands(int index);
	void sendCommandReadAll(uint8_t slaveId);
	void sendCommandReadAllRicnu(uint8_t slaveId);
	void sendCommandAnkle2DOF(uint8_t slaveId);
	void sendCommandBattery(uint8_t slaveId);
	void sendCommandInControl(uint8_t slaveId);
	void sendCommandDynamic(uint8_t slaveId);
	void sendCommandAngleTorqueProfile(uint8_t slaveId);
	void sendCommandRigid(uint8_t slaveId);
};
#endif // STREAMMANAGER_H
