#ifndef STREAMMANAGER_H
#define STREAMMANAGER_H

#include <QObject>
#include <QTimer>
#include <serialdriver.h>
#include <QHash>

class StreamManager : public QObject
{
	Q_OBJECT
public:
	explicit StreamManager(QObject *parent = 0, SerialDriver* driver = nullptr);
	void startStreaming(int cmd, int slave, int freq);
	void stopStreaming(int cmd, int slave, int freq);

	const int NUM_TIMER_FREQS;
	QList<uint8_t> ricnuOffsets;

signals:
	void sentRead(int cmd, int slave);

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
		CmdSlaveRecord(int c, int s):  cmdType(c), slaveIndex(s) {}
		int cmdType;
		int slaveIndex;
	};

	void sendCommands(QList<CmdSlaveRecord> streamList);
	void tryPackAndSend(int cmd, uint8_t slaveId);
	int getIndexOfFrequency(int freq);
	SerialDriver* serialDriver;
	int timerFrequencies[NUM_TIMER_FREQS];
	QTimer* masterTimer;
	QList<CmdSlaveRecord> streamLists[NUM_TIMER_FREQS];

};

#endif // STREAMMANAGER_H
