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

class StreamManager : public QObject
{
	Q_OBJECT
public:
	explicit StreamManager(QObject *parent = 0, SerialDriver* driver = nullptr);
	virtual ~StreamManager() {
		if(clockTimer) delete clockTimer;
		clockTimer = nullptr;
	}

	void startStreaming(int cmd, int slave, int freq, bool shouldLog, FlexseaDevice* logToDevice);
	void startAutoStreaming(int cmd, int slave, int freq, bool shouldLog, FlexseaDevice* logToDevice);
	void stopStreaming(int cmd, int slave, int freq);

	QList<int> getRefreshRates();

	static const int NUM_TIMER_FREQS = 8;
	QList<int> ricnuOffsets;

signals:
	void sentRead(int cmd, int slave);
	void openRecordingFile(FlexseaDevice* device);
	void closeRecordingFile(FlexseaDevice* device);

public slots:
	void receiveClock();
	void enqueueCommand(uint8_t numb, uint8_t* dataPacket);
	void onComPortClosing();

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
	void packAndSendStopStreaming(uint8_t slaveId);
	int getIndexOfFrequency(int freq);
	QString getNameOfExperiment(int cmd);
	SerialDriver* serialDriver;
	int timerFrequencies[NUM_TIMER_FREQS];
	float timerIntervals[NUM_TIMER_FREQS];

	QList<QString> experimentLabels;
	QList<int> experimentCodes;
	QTimer* clockTimer;
	float clockPeriod;

	void sendCommands(int index);
	void sendCommandReadAll(uint8_t slaveId);
	void sendCommandReadAllRicnu(uint8_t slaveId);
	void sendCommandAnkle2DOF(uint8_t slaveId);
	void sendCommandBattery(uint8_t slaveId);
	void sendCommandTestBench(uint8_t slaveId);
	void sendCommandInControl(uint8_t slaveId);
	void sendCommandDynamic(uint8_t slaveId);
};
#endif // STREAMMANAGER_H
