#include "streammanager.h"
#include <flexsea.h>
#include <flexsea_system.h>
#include <flexsea_board.h>
#include <w_event.h>
#include <QDebug>
#include <cmd-MIT_2DoF_Ankle_v1.h>
#include <cmd-RICNU_Knee_v1.h>
#include <cmd-MotorTestBench.h>
#include <dynamic_user_structs.h>

StreamManager::StreamManager(QObject *parent, SerialDriver* driver) :
	QObject(parent),
	serialDriver(driver)
{

	//this needs to be in order from smallest to largest
	int timerFreqsInHz[NUM_TIMER_FREQS] = {1, 5, 10, 20, 33, 50, 100, 200};
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
	{
		timerFrequencies[i] = timerFreqsInHz[i];
		timerIntervals[i] = 1000.0f / timerFreqsInHz[i];
		autoStreamLists[i] = std::vector<CmdSlaveRecord>();
		streamLists[i] = std::vector<CmdSlaveRecord>();
	}

    clockPeriod = 2;
	clockTimer = new QTimer();
	clockTimer->setTimerType(Qt::PreciseTimer);
	clockTimer->setSingleShot(false);
	clockTimer->setInterval(clockPeriod);
	connect(clockTimer, &QTimer::timeout, this, &StreamManager::receiveClock);
}

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

void StreamManager::startStreaming(int cmd, int slave, int freq, bool shouldLog, FlexseaDevice* device)
{
	if(!device) return;

	int indexOfFreq = getIndexOfFrequency(freq);

	if(indexOfFreq >= 0 && indexOfFreq < NUM_TIMER_FREQS && device)
	{
		device->isCurrentlyLogging = shouldLog;
		if(shouldLog)
		{
			emit openRecordingFile(device);
			device->initialClock = clock();
		}

		serialDriver->addDevice(device);

		CmdSlaveRecord record(cmd, slave, shouldLog, device);
		streamLists[indexOfFreq].push_back(record);
		qDebug() << "Started streaming cmd: " << cmd << ", for slave id: " << slave << "at frequency: " << freq;
		clockTimer->start();
	}
	else
	{
		qDebug("Invalid frequency");
	}
}

void StreamManager::startAutoStreaming(int cmd, int slave, int freq, bool shouldLog, FlexseaDevice* device)
{
	if(!device) return;

	int indexOfFreq = getIndexOfFrequency(freq);

	if(indexOfFreq >= 0 && indexOfFreq < NUM_TIMER_FREQS && device)
	{
		uint8_t shouldStart = 1;

		device->isCurrentlyLogging = shouldLog;
		if(shouldLog)
		{
			emit openRecordingFile(device);
			device->initialClock = clock();
		}

		serialDriver->addDevice(device);

		tx_cmd_stream_w(TX_N_DEFAULT, cmd, 1000/freq, shouldStart);
		tryPackAndSend(CMD_STREAM, device->slaveID);

		CmdSlaveRecord record(cmd, slave, shouldLog, device);
		autoStreamLists[indexOfFreq].push_back(record);
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

	std::vector<CmdSlaveRecord>* listArray[2] = {autoStreamLists, streamLists};

	for(int listIndex = 0; listIndex < 2; listIndex++)
	{
		std::vector<CmdSlaveRecord> *l = listArray[listIndex];

		for(unsigned int i = 0; i < (l)[indexOfFreq].size(); i++)
		{
			CmdSlaveRecord record = (l)[indexOfFreq].at(i);
			if(record.cmdType == cmd && record.slaveIndex == slave)
			{
				(l)[indexOfFreq].erase((l)[indexOfFreq].begin() + i);

				packAndSendStopStreaming(record.device->slaveID);
				record.device->isCurrentlyLogging = false;

				qDebug() << "Stopped streaming cmd: " << cmd << ", for slave id: " << slave << "at frequency: " << freq;

				if(record.shouldLog)
					emit closeRecordingFile(record.device);
			}
		}
	}
}

void StreamManager::onComPortClosing()
{
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
	{
		for(unsigned int j = 0; j < autoStreamLists[i].size(); j++)
		{
			CmdSlaveRecord record = autoStreamLists[i].at(j);
			packAndSendStopStreaming(record.device->slaveID);
			record.device->isCurrentlyLogging = false;
			qDebug() << "Stopped streaming cmd: " << record.cmdType << ", for slave id: " << record.device->slaveID << "at frequency: " << timerFrequencies[i];
			if(record.shouldLog)
				emit closeRecordingFile(record.device);
		}

		autoStreamLists[i].clear();
	}

	for(int i = 0; i < NUM_TIMER_FREQS; i++)
	{
		for(unsigned int j = 0; j < streamLists[i].size(); j++)
		{
			CmdSlaveRecord record = streamLists[i].at(j);
			record.device->isCurrentlyLogging = false;
			qDebug() << "Stopped streaming cmd: " << record.cmdType << ", for slave id: " << record.device->slaveID << "at frequency: " << timerFrequencies[i];
			if(record.shouldLog)
				emit closeRecordingFile(record.device);
		}

		streamLists[i].clear();
	}
}

QList<int> StreamManager::getRefreshRates()
{
	QList<int> result;
	result.reserve(NUM_TIMER_FREQS);
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
		result.append(timerFrequencies[i]);
	return result;
}

void StreamManager::tryPackAndSend(int cmd, uint8_t slaveId)
{
	uint16_t numb = 0;
	uint8_t info[2] = {PORT_USB, PORT_USB};
	pack(P_AND_S_DEFAULT, slaveId
		 , info, &numb, comm_str_usb);

	if(serialDriver && serialDriver->isOpen())
	{
		serialDriver->write(numb, comm_str_usb);
		emit sentRead(cmd, slaveId);
	}
}

void StreamManager::receiveClock()
{
	static float msSinceLast[NUM_TIMER_FREQS] = {0};
	const float TOLERANCE = 0.0001;

	if(outgoingBuffer.size())
	{
		Message m = outgoingBuffer.front();
		outgoingBuffer.pop();
		serialDriver->write(m.numBytes, m.dataPacket.data());
	}
	else
	{
		for(int i = 0; i < NUM_TIMER_FREQS; i++)
		{
			if(!streamLists[i].size()) continue;

			//received clocks comes in at 5ms/clock
			msSinceLast[i]+=clockPeriod;

			float timerInterval = timerIntervals[i];
			if((msSinceLast[i] + TOLERANCE) > timerInterval)
			{
				sendCommands(i);

				while((msSinceLast[i] + TOLERANCE) > timerInterval)
					msSinceLast[i] -= timerInterval;
			}
		}
	}

	bool noStreamAndNoBuffer = outgoingBuffer.size() == 0;
	int i = 0;
	while(i < NUM_TIMER_FREQS && noStreamAndNoBuffer)
		noStreamAndNoBuffer = streamLists[i++].size() == 0;

	if(noStreamAndNoBuffer)
		clockTimer->stop();

}

void StreamManager::enqueueCommand(uint8_t numb, uint8_t* dataPacket)
{
	//If we are over a max size, clear the queue
	const unsigned int MAX_Q_SIZE = 200;

	if(outgoingBuffer.size() > MAX_Q_SIZE)
	{
		qDebug() << "StreamManager::enqueueCommand, queue is above max size (" << MAX_Q_SIZE  << "), clearing queue...";
		while(outgoingBuffer.size())
			outgoingBuffer.pop();
	}

	outgoingBuffer.push(Message(numb, dataPacket));
	if(!clockTimer->isActive())
		clockTimer->start();
}


void StreamManager::packAndSendStopStreaming(uint8_t slaveId)
{
	uint8_t shouldStart = 0; //ie should stop
	tx_cmd_stream_w(TX_N_DEFAULT, -1, 0, shouldStart);
	tryPackAndSend(CMD_STREAM, slaveId);
}

void StreamManager::sendCommands(int index)
{
	if(index < 0 || index >= NUM_TIMER_FREQS) return;

	for(unsigned int i = 0; i < streamLists[index].size(); i++)
	{
		CmdSlaveRecord record = streamLists[index].at(i);
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
		case CMD_IN_CONTROL:
			sendCommandInControl(record.slaveIndex);
			break;
		case CMD_USER_DYNAMIC:
			sendCommandDynamic(record.slaveIndex);
			break;
		default:
			qDebug() << "Unsupported command was given: " << record.cmdType;
			stopStreaming(record.cmdType, record.slaveIndex, timerFrequencies[index]);
			break;
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
	tx_cmd_ricnu_r(TX_N_DEFAULT, (uint8_t)(ricnuOffsets.at(index)));
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
	(void)slaveId;
	//	static int index = 0;

	//1) Stream
//	motor_dto dto;
//	tx_cmd_motortb_r(TX_N_DEFAULT, index, &dto);
//	index++;
//	index %= 3;
//	tryPackAndSend(CMD_MOTORTB, slaveId);
}
void StreamManager::sendCommandInControl(uint8_t slaveId)
{
	tx_cmd_in_control_r(TX_N_DEFAULT);
	tryPackAndSend(CMD_IN_CONTROL, slaveId);
}

void StreamManager::sendCommandDynamic(uint8_t slaveId)
{
	tx_cmd_user_dyn_r(TX_N_DEFAULT, SEND_DATA);
	tryPackAndSend(CMD_USER_DYNAMIC, slaveId);
}
