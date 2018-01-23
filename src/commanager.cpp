#include "commanager.h"
#include <flexsea.h>
#include <flexsea_system.h>
#include <flexsea_board.h>
#include <w_event.h>
#include <QDebug>
#include <cmd-MIT_2DoF_Ankle_v1.h>
#include <cmd-RICNU_Knee_v1.h>
#include <cmd-Rigid.h>
#include <dynamic_user_structs.h>
#include <flexsea_cmd_angle_torque_profile.h>

ComManager::ComManager(QObject *parent) :
	QObject(parent)
{
	// Because this class is used in a thread, init is called after the class
	// has been passed to the thread. This avoid allocating heap in the
	// "creator thread" instead of the "SerialDriver thread".
	// see https://wiki.qt.io/QThreads_general_usage
}

void ComManager::init()
{
	//this needs to be in order from smallest to largest
	int timerFreqsInHz[NUM_TIMER_FREQS] = {1, 5, 10, 20, 33, 50, 100, 200, 300, 500, 1000};
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
	{
		timerFrequencies[i] = timerFreqsInHz[i];
		timerIntervals[i] = 1000.0f / timerFreqsInHz[i];
		autoStreamLists[i] = std::vector<CmdSlaveRecord>();
		streamLists[i] = std::vector<CmdSlaveRecord>();
	}

	clockPeriod = 2;
	clockTimer = new QTimer(this);
	clockTimer->setTimerType(Qt::PreciseTimer);
	clockTimer->setSingleShot(false);
	clockTimer->setInterval(clockPeriod);
	connect(clockTimer, &QTimer::timeout, this, &ComManager::receiveClock);

	mySerialDriver = new SerialDriver(this);

	// Connect the signals "passthrough"

	connect(mySerialDriver,	&SerialDriver::openStatus, \
			this,			&ComManager::openStatus);
	connect(mySerialDriver,	&SerialDriver::newDataReady, \
			this,			&ComManager::newDataReady);
	connect(mySerialDriver,	&SerialDriver::dataStatus, \
			this,			&ComManager::dataStatus);
	connect(mySerialDriver,	&SerialDriver::newDataTimeout, \
			this,			&ComManager::newDataTimeout);
	connect(mySerialDriver,	&SerialDriver::setStatusBarMessage, \
			this,			&ComManager::setStatusBarMessage);
	connect(mySerialDriver,	&SerialDriver::writeToLogFile, \
			this,			&ComManager::writeToLogFile);
	connect(mySerialDriver,	&SerialDriver::aboutToClose, \
			this,			&ComManager::aboutToClose);
}

int ComManager::getIndexOfFrequency(int freq)
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

void ComManager::startStreaming(bool shouldLog, FlexseaDevice* device)
{
	if(!device) return;

	int freq = device->frequency;
	int cmd = device->experimentIndex;
	uint8_t slave = device->slaveID;

	int indexOfFreq = getIndexOfFrequency(freq);

	if(indexOfFreq >= 0 && indexOfFreq < NUM_TIMER_FREQS && device)
	{
		device->isCurrentlyLogging = shouldLog;
		if(shouldLog)
		{
			emit openRecordingFile(device);
			device->initialClock = clock();
		}

		mySerialDriver->addDevice(device);

		CmdSlaveRecord record(cmd, slave, shouldLog, device);
		streamLists[indexOfFreq].push_back(record);
		qDebug() << "Started streaming cmd: " << cmd << ", for slave id: " << slave << "at frequency: " << freq;
		clockTimer->start();
		emit streamingFrequency(freq);
	}
	else
	{
		qDebug("Invalid frequency");
	}
}
void ComManager::startAutoStreaming(bool shouldLog, FlexseaDevice* logToDevice)
{
	startAutoStreaming(shouldLog, logToDevice, minOffs, maxOffs);
}

void ComManager::startAutoStreaming(bool shouldLog, \
							FlexseaDevice* device, uint8_t firstIndex, uint8_t lastIndex)
{
	if(!device) return;

	int freq = device->frequency;
	int cmd = device->experimentIndex;
	uint8_t slave = device->slaveID;

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

		mySerialDriver->addDevice(device);

		tx_cmd_stream_w(TX_N_DEFAULT, cmd, 1000/freq, shouldStart, firstIndex, lastIndex);
		tryPackAndSend(CMD_STREAM, device->slaveID);

		CmdSlaveRecord record(cmd, slave, shouldLog, device);
		autoStreamLists[indexOfFreq].push_back(record);
		qDebug() << "Started streaming cmd: " << cmd << ", for slave id: " << slave << "at frequency: " << freq;
		qDebug() << "Covering indexes" << firstIndex << "to" << lastIndex << ".";
		emit streamingFrequency(freq);
	}
	else
	{
		qDebug("Invalid frequency");
	}
}

void ComManager::stopStreaming(FlexseaDevice *device)
{
	if(!device) return;
	stopStreaming(device->experimentIndex, device->slaveID, device->frequency);
}

void ComManager::stopStreaming(int cmd, uint8_t slave, int freq)
{
	int indexOfFreq = getIndexOfFrequency(freq);
	if(indexOfFreq < 0) return;

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

				packAndSendStopStreaming(cmd, record.device->slaveID);
				record.device->isCurrentlyLogging = false;

				qDebug() << "Stopped streaming cmd: " << cmd << ", for slave id: " << slave << "at frequency: " << freq;

				if(record.shouldLog)
					emit closeRecordingFile(record.device);
			}
		}
	}
}

void ComManager::setOffsetParameter(QList<int> ricnuOffsets, QList<int> rigidOffsets, int minOffs, int maxOffs)
{
	this->ricnuOffsets = ricnuOffsets;
	this->rigidOffsets = rigidOffsets;
	this->minOffs = minOffs;
	this->maxOffs = maxOffs;
}

void ComManager::onComPortClosing()
{
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
	{
		for(unsigned int j = 0; j < autoStreamLists[i].size(); j++)
		{
			CmdSlaveRecord record = autoStreamLists[i].at(j);
			packAndSendStopStreaming(record.cmdType, record.device->slaveID);
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


void ComManager::open(QString name, int tries, int delay, bool* success)
{
	mySerialDriver->open(name, tries, delay, success);
}

void ComManager::close(void)
{
	mySerialDriver->close();
}

void ComManager::tryReadWrite(uint8_t bytes_to_send, uint8_t *serial_tx_data, int timeout)
{
	mySerialDriver->tryReadWrite(bytes_to_send, serial_tx_data, timeout);
}

int ComManager::write(uint8_t bytes_to_send, uint8_t *serial_tx_data)
{
	return mySerialDriver->write(bytes_to_send, serial_tx_data);
}

void ComManager::flush(void)
{
	mySerialDriver->flush();
}

void ComManager::clear(void)
{
	mySerialDriver->clear();
}

void ComManager::addDevice(FlexseaDevice* device)
{
	mySerialDriver->addDevice(device);
}

QList<int> ComManager::getRefreshRates()
{
	QList<int> result;
	for(int i = 0; i < NUM_TIMER_FREQS; i++)
		result.append(timerFrequencies[i]);
	return result;
}

void ComManager::tryPackAndSend(int cmd, uint8_t slaveId)
{
	uint16_t numb = 0;
	uint8_t info[2] = {PORT_USB, PORT_USB};
	pack(P_AND_S_DEFAULT, slaveId, info, &numb, comm_str_usb);

	if(mySerialDriver->isOpen())
	{
		mySerialDriver->write(numb, comm_str_usb);
		emit sentRead(cmd, slaveId);
	}
}

void ComManager::receiveClock()
{
	static float msSinceLast[NUM_TIMER_FREQS] = {0};
	const float TOLERANCE = 0.0001;

	if(outgoingBuffer.size())
	{
		Message m = outgoingBuffer.front();
		outgoingBuffer.pop();
		mySerialDriver->write(m.numBytes, m.dataPacket.data());
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

void ComManager::enqueueCommand(uint8_t numb, uint8_t* dataPacket)
{
	//If we are over a max size, clear the queue
	const unsigned int MAX_Q_SIZE = 200;

	if(outgoingBuffer.size() > MAX_Q_SIZE)
	{
		qDebug() << "ComManager::enqueueCommand, queue is above max size (" << MAX_Q_SIZE  << "), clearing queue...";
		while(outgoingBuffer.size())
			outgoingBuffer.pop();
	}

	outgoingBuffer.push(Message(numb, dataPacket));
	if(!clockTimer->isActive())
		clockTimer->start();
}


void ComManager::packAndSendStopStreaming(int cmd, uint8_t slaveId)
{
	if(cmd < 0) return;

	uint8_t shouldStart = 0; //ie should stop
	tx_cmd_stream_w(TX_N_DEFAULT, cmd, 0, shouldStart, 0, 0);
	tryPackAndSend(CMD_STREAM, slaveId);
}

void ComManager::sendCommands(int index)
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
			case CMD_RICNU:
				sendCommandReadAllRicnu(record.slaveIndex);
				break;
			case CMD_A2DOF:
				sendCommandAnkle2DOF(record.slaveIndex);
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
			case CMD_ANGLE_TORQUE_PROFILE:
				sendCommandAngleTorqueProfile(record.slaveIndex);
				break;
			case CMD_READ_ALL_RIGID:
				sendCommandRigid(record.slaveIndex);
				break;
			default:
				qDebug() << "Unsupported command was given: " << record.cmdType;
				stopStreaming(record.cmdType, record.slaveIndex, timerFrequencies[index]);
				break;
		}
	}
}

void ComManager::sendCommandReadAll(uint8_t slaveId)
{
	//1) Stream
	tx_cmd_data_read_all_r(TX_N_DEFAULT);
	tryPackAndSend(CMD_READ_ALL, slaveId);
}

void ComManager::sendCommandReadAllRicnu(uint8_t slaveId)
{
	(void) slaveId;
	if(ricnuOffsets.size() < 1) return;
	static int index = 0;
	tx_cmd_ricnu_r(TX_N_DEFAULT, (uint8_t)(ricnuOffsets.at(index)));
	index++;
	index %= ricnuOffsets.size();
	tryPackAndSend(CMD_RICNU, slaveId);
}

void ComManager::sendCommandAnkle2DOF(uint8_t slaveId)
{
	static int index = 0;

	//1) Stream
	tx_cmd_ankle2dof_r(TX_N_DEFAULT, index, 0, 0, 0);
	index++;
	index %= 2;
	tryPackAndSend(CMD_A2DOF, slaveId);
}

void ComManager::sendCommandBattery(uint8_t slaveId)
{
	//1) Stream
	tx_cmd_exp_batt_r(TX_N_DEFAULT);
	tryPackAndSend(CMD_BATT, slaveId);
}

void ComManager::sendCommandInControl(uint8_t slaveId)
{
	tx_cmd_in_control_r(TX_N_DEFAULT);
	tryPackAndSend(CMD_IN_CONTROL, slaveId);
}

void ComManager::sendCommandDynamic(uint8_t slaveId)
{
	tx_cmd_user_dyn_r(TX_N_DEFAULT, SEND_DATA);
	tryPackAndSend(CMD_USER_DYNAMIC, slaveId);
}

void ComManager::sendCommandAngleTorqueProfile(uint8_t slaveId)
{
	//tx_cmd_ankleTorqueProfile_r(TX_N_DEFAULT, 0);
	//tryPackAndSend(CMD_ANGLE_TORQUE_PROFILE, slaveId);
}

void ComManager::sendCommandRigid(uint8_t slaveId)
{
	if(rigidOffsets.size() < 1) return;
	static int index = 0;

	tx_cmd_rigid_r(TX_N_DEFAULT, (uint8_t)(ricnuOffsets.at(index)));
	index++;
	index %= rigidOffsets.size();
	tryPackAndSend(CMD_READ_ALL_RIGID, slaveId);
}

