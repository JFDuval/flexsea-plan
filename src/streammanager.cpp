#include "streammanager.h"
#include <flexsea.h>
#include <flexsea_comm.h>
#include <flexsea_system.h>
#include <flexsea_cmd_data.h>
#include <QDebug>

StreamManager::StreamManager(QObject *parent, SerialDriver* driver) :
	QObject(parent),
	serialDriver(driver)
{
	int timerFreqsInHz[NUM_TIMER_FREQS] = {1, 5, 10, 20, 33, 50, 100, 200};
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
	{
		timerFrequencies[i] = timerFreqsInHz[i];
		streamLists[i] = QList<CmdSlaveRecord>();
	}

	masterTimer = new QTimer();
	masterTimer->setInterval(5); // 200 Hz
	masterTimer->setSingleShot(false);
	connect(masterTimer, &QTimer::timeout, this, &StreamManager::masterTimerOut);

	masterTimer->start();
}

int StreamManager::NUM_TIMER_FREQS = 8;

int StreamManager::getIndexOfFrequency(int freq)
{
	int indexOfFreq = -1;
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
	{
		if(freq == timerFrequencies[i])
		{
			indexOfFreq = i;
			i = NUM_TIMER_FREQS;
		}
	}
	return indexOfFreq;
}

void StreamManager::startStreaming(int cmd, int slave, int freq)
{

	CmdSlaveRecord record(cmd, slave);
	int indexOfFreq = getIndexOfFrequency(freq);

	if(indexOfFreq >= 0 && indexOfFreq < NUM_TIMER_FREQS)
	{
		streamLists[indexOfFreq].append(record);
		qDebug() << "Started streaming cmd: " << cmd << ", for slave id: " << slave << "at frequency: " << freq;
	}
	else
	{
		qDebug("Invalid frequency");
	}

}

void StreamManager::stopStreaming(int cmd, int slave, int freq)
{
	int indexOfFreq = getIndexOfFrequency(freq);
	for(int i = 0; i < streamLists[indexOfFreq].size(); i++)
	{
		CmdSlaveRecord record = streamLists[indexOfFreq].at(i);
		if(record.cmdType == cmd && record.slaveIndex == slave)
		{
			streamLists[indexOfFreq].removeAt(i);
			qDebug() << "Stopped streaming cmd: " << cmd << ", for slave id: " << slave << "at frequency: " << freq;
		}
	}
}

void StreamManager::sendCommands(QList<CmdSlaveRecord> streamList)
{
	for(int i = 0; i < streamList.size(); i++)
	{
		CmdSlaveRecord record = streamList.at(i);
		switch(record.cmdType)
		{
		case CMD_READ_ALL:
			sendCommandReadAll(record.slaveIndex);
			break;
		case CMD_READ_ALL_RICNU:
			sendCommandReadAllRicnu(record.slaveIndex);
			break;
		case CMD_A2DOF:
			sendCommandAnkle2DOF(record.slaveIndex);
			break;
		case CMD_MOTORTB:
			sendCommandTestBench(record.slaveIndex);
			break;
		case CMD_BATT:
			sendCommandBattery(record.slaveIndex);
			break;
		default:
			qDebug() << "Unsupported command was given: " << record.cmdType;
			break;
		}
	}
}

void StreamManager::tryPackAndSend(int cmd, uint8_t slaveId)
{
	uint16_t numb = 0;
	uint8_t info[2] = {PORT_USB, PORT_USB};
	pack(P_AND_S_DEFAULT, slaveId
		 , info, &numb, comm_str_usb);

	if(serialDriver && serialDriver->isOpen())
	{
		serialDriver->readWrite(numb, comm_str_usb, READ);
		emit sentRead(cmd, slaveId);
	}
}

void StreamManager::receiveClock()
{
	static double msSinceLast[NUM_TIMER_FREQS] = {0};
	const double TOLERANCE = 0.001;
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
	{
		msSinceLast[i]++;
		double timerInterval = (1000.0 / timerFrequencies[i]);
		if((msSinceLast[i] + TOLERANCE) > timerInterval)
		{
			sendCommands(streamLists[i]);

			while((msSinceLast[i] + TOLERANCE) > timerInterval)
				msSinceLast[i] -= timerInterval;
		}
	}
}

void StreamManager::sendCommandReadAll(uint8_t slaveId)
{
	//1) Stream
	tx_cmd_data_read_all_r(TX_N_DEFAULT);
	tryPackAndSend(CMD_READ_ALL, slaveId);
}

void StreamManager::sendCommandReadAllRicnu(uint8_t slaveId)
{
	(void) slaveId;
	if(ricnuOffsets.size() < 1) return;
	static int index = 0;
	tx_cmd_ricnu_r(TX_N_DEFAULT, ricnuOffsets.at(index));
	index++;
	index %= ricnuOffsets.size();
	tryPackAndSend(CMD_READ_ALL_RICNU, slaveId);
}

void StreamManager::sendCommandAnkle2DOF(uint8_t slaveId)
{
	static int index = 0;

	//1) Stream
	tx_cmd_ankle2dof_r(TX_N_DEFAULT, index, 0, 0, 0);
	index++;
	index %= 2;
	tryPackAndSend(CMD_A2DOF, slaveId);
}

void StreamManager::sendCommandBattery(uint8_t slaveId)
{
	//1) Stream
	tx_cmd_exp_batt_r(TX_N_DEFAULT);
	tryPackAndSend(CMD_BATT, slaveId);
}

void StreamManager::sendCommandTestBench(uint8_t slaveId)
{
	static int index = 0;

	//1) Stream
	tx_cmd_motortb_r(TX_N_DEFAULT, index, 0, 0, 0);
	index++;
	index %= 3;
	tryPackAndSend(CMD_MOTORTB, slaveId);
}
