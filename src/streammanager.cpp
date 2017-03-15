#include "streammanager.h"
#include <flexsea.h>
#include <flexsea_comm.h>
#include <flexsea_system.h>
#include <flexsea_cmd_data.h>
#include <flexsea_cmd_in_control.h>
#include <flexsea_cmd_stream.h>
#include <w_event.h>
#include <QDebug>

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
		streamLists[i] = std::vector<CmdSlaveRecord>();
	}

	clockTimer = new QTimer();
	clockTimer->setTimerType(Qt::PreciseTimer);
	clockTimer->setSingleShot(false);
	clockTimer->setInterval(10);
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
		streamLists[indexOfFreq].push_back(record);
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
	for(unsigned int i = 0; i < streamLists[indexOfFreq].size(); i++)
	{
		CmdSlaveRecord record = streamLists[indexOfFreq].at(i);
		if(record.cmdType == cmd && record.slaveIndex == slave)
		{
			streamLists[indexOfFreq].erase(streamLists[indexOfFreq].begin() + i);

			packAndSendStopStreaming(record.device->slaveID);
			record.device->isCurrentlyLogging = false;

			qDebug() << "Stopped streaming cmd: " << cmd << ", for slave id: " << slave << "at frequency: " << freq;

			if(record.shouldLog)
				emit closeRecordingFile(record.device);
		}
	}
}

void StreamManager::onComPortClosing()
{
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
	{
		for(unsigned int j = 0; j < streamLists[i].size(); j++)
		{
			CmdSlaveRecord record = streamLists[i].at(j);
			packAndSendStopStreaming(record.device->slaveID);
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
	if(!outgoingBuffer.size()) return;

	Message m = outgoingBuffer.front();
	outgoingBuffer.pop();
	serialDriver->write(m.numBytes, m.dataPacket.data());

	if(!outgoingBuffer.size())
		clockTimer->stop();

	return;
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
	if(outgoingBuffer.size() == 1)
		clockTimer->start();
}


void StreamManager::packAndSendStopStreaming(uint8_t slaveId)
{
	uint8_t shouldStart = 0; //ie should stop
	tx_cmd_stream_w(TX_N_DEFAULT, -1, 0, shouldStart);
	tryPackAndSend(CMD_STREAM, slaveId);
}
