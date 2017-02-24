#ifndef STREAMMANAGER_H
#define STREAMMANAGER_H

#include <QObject>
#include <QTimer>
#include <serialdriver.h>
#include <QString>
#include <QDateTime>
#include <FlexSEADevice/flexseaDevice.h>

class StreamManager : public QObject
{
	Q_OBJECT
public:
	explicit StreamManager(QObject *parent = 0, SerialDriver* driver = nullptr);
	void startStreaming(int cmd, int slave, int freq, bool shouldLog, FlexseaDevice* logToDevice);
	void stopStreaming(int cmd, int slave, int freq);

	QList<int> getRefreshRates();

	static const int NUM_TIMER_FREQS = 8;
	QList<int> ricnuOffsets;

signals:
	void sentRead(int cmd, int slave);
	void openRecordingFile(FlexseaDevice* device);
	void writeToLogFile(FlexseaDevice* device);
	void closeRecordingFile(FlexseaDevice* device);

public slots:
	void receiveClock();

	void sendCommandReadAll(uint8_t slaveId);
	void sendCommandReadAllRicnu(uint8_t slaveId);
	void sendCommandAnkle2DOF(uint8_t slaveId);
	void sendCommandBattery(uint8_t slaveId);
	void sendCommandTestBench(uint8_t slaveId);

private:
	class CmdSlaveRecord
	{
	public:
		CmdSlaveRecord(int c, int s, bool l, FlexseaDevice* d):
			cmdType(c), slaveIndex(s), shouldLog(l), device(d) {}
		int cmdType;
		int slaveIndex;
		bool shouldLog;
		QDateTime* initialTime;
		FlexseaDevice* device;
	};

	void sendCommands(QList<CmdSlaveRecord> streamList);
	void tryPackAndSend(int cmd, uint8_t slaveId);
	int getIndexOfFrequency(int freq);
	QString getNameOfExperiment(int cmd);
	SerialDriver* serialDriver;
	int timerFrequencies[NUM_TIMER_FREQS];

	QList<QString> experimentLabels;
	QList<int> experimentCodes;
	QList<CmdSlaveRecord> streamLists[NUM_TIMER_FREQS];

};

#endif // STREAMMANAGER_H
