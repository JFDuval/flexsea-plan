/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2017 Dephy, Inc. <http://dephy.com/>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] w_commtest.h: Communication Testing Tool
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-01-05 | jfduval | New code, initial release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include <flexsea_system.h>
#include "flexsea.h"
#include "flexsea_comm.h"
#include "w_commtest.h"
#include "flexsea_generic.h"
#include "ui_w_commtest.h"
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <flexsea_board.h>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_CommTest::W_CommTest(QWidget *parent,
					   SerialPortStatus comStatusInit) :
	QWidget(parent),
	ui(new Ui::W_CommTest)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
	initTimers();

	receiveComPortStatus(comStatusInit, -1);
}

W_CommTest::~W_CommTest()
{
	emit windowClosed();
	delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

//This slot gets called when the port status changes (turned On or Off)
void W_CommTest::receiveComPortStatus(SerialPortStatus status,int nbTries)
{
	// Not use by this slot.
	(void)nbTries;

	if(status == PortClosed)
	{
		//PushButton:
		ui->pushButtonReset->setDisabled(true);
		ui->pushButtonStartStop->setDisabled(true);
		startStopComTest(true);
	}
	else if(status == PortOpeningSucceed)
	{
		//PushButton:
		ui->pushButtonReset->setDisabled(false);
		ui->pushButtonStartStop->setDisabled(false);
	}
}

//Received new data?
void W_CommTest::receivedData(void)
{
	measuredRefreshReceive = getRefreshRateReceive();
}

//****************************************************************************
// Private function(s):
//****************************************************************************

//Window initialization:
void W_CommTest::init(void)
{
	//Populates Slave list:
	FlexSEA_Generic::populateSlaveComboBox(ui->comboBox_slave, SL_BASE_ALL, \
											SL_LEN_ALL);
	ui->comboBox_slave->setCurrentIndex(0);	//Execute 1 by default

	//Variables:
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);

	//Initialize common displays and controls:
	initCommon();

	//Tab by tab initialization:
	initTab1();
	initTab2();
	initTab3();

	//Always start on the first tab:
	ui->tabWidget->setCurrentIndex(0);
	currentTab = 0;

	slaveListCount = 0;
}

//Common displays and controls:
void W_CommTest::initCommon(void)
{
	//Displays:
	ui->labelSentPackets->setText("0");
	ui->labelReceivedPackets->setText("0");
	ui->labelGoodPackets->setText("0");
	ui->labelThroughput->setText("0");
	ui->labelLossRate->setText("0");
	ui->label_rrSend->setText("N/A");
	ui->label_rrReceive->setText("N/A");
	ui->label_rrSend_Avg->setText("N/A");
	ui->label_rrReceive_Avg->setText("N/A");
	ui->lineEdit->setText(QString::number(DEFAULT_EXPERIMENT_TIMER_FREQ));

	measuredRefreshSend = 0;
	measuredRefreshReceive = 0;

	//Seed:
	QTime myTime;
	initRandomGenerator(myTime.msecsSinceStartOfDay());

	setTimeElapsedCounterToZero();
}

//First tab: Plan <> Device test
void W_CommTest::initTab1(void)
{
	ui->label_info_tab1->setText("Test the communication between \nPlan and a device connected \nvia USB, Bluetooth, SPI, ...\n\nWe recommend using Busy Wait\nwith Manage.");
}

//Second tab: Manage <> Execute test
void W_CommTest::initTab2(void)
{
	ui->label_info_tab2->setText("Test the communication between \nPlan and a slave connected \nto Manage (RS-485).");
}

//Third tab: Many Execute
void W_CommTest::initTab3(void)
{
	ui->label_info_tab3->setText("Test the communication between \nPlan and 1-4 Execute connected \nto Manage (RS-485). Plan is the \nMaster, Manage is a pass-through.");
}

void W_CommTest::initTimers(void)
{
	experimentTimerFreq = DEFAULT_EXPERIMENT_TIMER_FREQ;

	displayTimer = new QTimer(this);
	connect(displayTimer,	&QTimer::timeout,
			this,			&W_CommTest::refreshDisplay);
	displayTimer->start(TIM_FREQ_TO_P(DISPLAY_TIMER));

	experimentTimer = new QTimer(this);
	connect(experimentTimer,	&QTimer::timeout,
			this,				&W_CommTest::readCommTest);
	experimentTimer->stop();

	statsTimer = new QDateTime;
}

//Send a Read command:
void W_CommTest::readCommTest(void)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;
	static uint8_t packetIndex = 0;
	static uint8_t currentSlaveIndex = 0;
	packetIndex++;

	//Prepare and send command:
	tx_cmd_tools_comm_test_r(TX_N_DEFAULT, 1, 20, packetIndex);
	if(currentTab != TAB_MANY_EX)
	{
		//We use the Active slave value:
		pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	}
	else
	{
		//Many Ex test:
		currentSlaveIndex++;
		currentSlaveIndex %= slaveListCount;
		pack(P_AND_S_DEFAULT, slaveList[currentSlaveIndex], info, &numb, comm_str_usb);
	}


	if(shouldBusyWait)
	{
		emit tryReadWrite(numb, comm_str_usb, 100);
	}
	else
	{
		emit write(numb, comm_str_usb);
	}

	//FlexSEA_Generic::packetVisualizer(numb, comm_str_usb);
	measuredRefreshSend = getRefreshRateSend();
}

//Returns the rate at which it is called, in Hz
//Average of 8 values
float W_CommTest::getRefreshRateSend(void)
{
	static qint64 oldTime = 0;
	qint64 newTime = 0, diffTime = 0;
	float t_s = 0.0, f = 0.0;
	static float avg = 0.0;
	static int counter = 0;
	static float fArray[8] = {0,0,0,0,0,0,0,0};
	static int divider = 0;

	//We take 10 samples, otherwise we get < 1ms and can't count
	divider++;
	divider %= 10;
	if(!divider)
	{
		//Actual frequency:
		newTime = statsTimer->currentMSecsSinceEpoch();
		diffTime = newTime - oldTime;
		oldTime = newTime;
		t_s = diffTime/1000.0;
		t_s /= 10;
		f = 1/t_s;

		//Average:
		counter++;
		counter %=8;
		fArray[counter] = f;
		avg = 0;
		for(int i = 0; i < 8; i++)
		{
			avg += fArray[i];
		}
		avg = avg / 8;
	}

	return avg;
}

//Returns the rate at which it is called, in Hz
//Average of 8 values
float W_CommTest::getRefreshRateReceive(void)
{
	static qint64 oldTime = 0;
	qint64 newTime = 0, diffTime = 0;
	float t_s = 0.0, f = 0.0;
	static float avg = 0.0;
	static int counter = 0;
	static float fArray[8] = {0,0,0,0,0,0,0,0};
	static int divider = 0;

	//We take 10 samples, otherwise we get < 1ms and can't count
	divider++;
	divider %= 10;
	if(!divider)
	{
		//Actual frequency:
		newTime = statsTimer->currentMSecsSinceEpoch();
		diffTime = newTime - oldTime;
		oldTime = newTime;
		t_s = diffTime/1000.0;
		t_s /= 10;
		f = 1/t_s;

		//Average:
		counter++;
		counter %=8;
		fArray[counter] = f;
		avg = 0;
		for(int i = 0; i < 8; i++)
		{
			avg += fArray[i];
		}
		avg = avg / 8;
	}

	return avg;
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

//Refreshes the User R values (display only):
void W_CommTest::refreshDisplay(void)
{
	receivedPackets = goodPackets + badPackets;
	if(sentPackets == 0 || receivedPackets == 0)
	{
		//Avoid /0
		throughputRate = 0;
		lossRate = 0;
		qualityRate = 0;
	}
	else
	{
		throughputRate = (float)receivedPackets/sentPackets;
		lossRate = (float)(sentPackets-receivedPackets) / sentPackets;
		qualityRate = (float)goodPackets / receivedPackets;
	}

	ui->labelSentPackets->setText(QString::number(sentPackets));
	ui->labelReceivedPackets->setText(QString::number(receivedPackets));
	ui->labelGoodPackets->setText(QString::number(goodPackets));
	ui->labelLostPackets->setText(QString::number(sentPackets-receivedPackets));

	QString txt = QString::number(100*throughputRate, 'f',2) + "%";
	ui->labelThroughput->setText(txt);
	txt = QString::number(100*lossRate, 'f',2) + "%";
	ui->labelLossRate->setText(txt);
	txt = QString::number(100*qualityRate, 'f',2) + "%";
	ui->labelQuality->setText(txt);

	QString refreshTxt;
	refreshTxt = QString::number(measuredRefreshSend, 'f', 2) + " Hz";
	ui->label_rrSend->setText(refreshTxt);
	refreshTxt = QString::number(measuredRefreshReceive, 'f', 2) + " Hz";
	ui->label_rrReceive->setText(refreshTxt);

	ui->label_packetOffset->setText(QString::number(packetOffset));

	//Average rates:
	if(computeAverage == true)
	{
		qint64 tE = getTimeElapsed();
		float r = getAverageRate(tE, sentPackets);
		refreshTxt = QString::number(r, 'f', 2) + " Hz";
		ui->label_rrSend_Avg->setText(refreshTxt);
		r = getAverageRate(tE, receivedPackets);
		refreshTxt = QString::number(r, 'f', 2) + " Hz";
		ui->label_rrReceive_Avg->setText(refreshTxt);
	}
}

void W_CommTest::on_comboBox_slave_currentIndexChanged(int index)
{
	(void)index;
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);
}

void W_CommTest::startStopComTest(bool forceStop)
{
	static bool status = false;
	int32_t tmpFreq = 0;

	if(forceStop == true)
	{
		status = true;
	}

	if(status == false)
	{
		//We were showing Start.
		ui->pushButtonStartStop->setText("Stop test");
		ui->lineEdit->setEnabled(false);

		if(currentTab == TAB_MANY_EX) {latchManyExTab();}

		tmpFreq = ui->lineEdit->text().toInt();
		if(tmpFreq < 1)
		{
			tmpFreq = 1;
		}

		if(tmpFreq > 100000)
		{
			tmpFreq = 100000;
		}
		ui->lineEdit->setText(QString::number(tmpFreq));

		experimentTimer->start(TIM_FREQ_TO_P(tmpFreq));

		status = true;

		ui->busyWaitButton->setDisabled(true);

		setTimeElapsedCounterToZero();
		computeAverage = true;
	}
	else
	{
		//We were showing Stop.
		ui->pushButtonStartStop->setText("Start test");
		ui->lineEdit->setEnabled(true);

		if(currentTab == TAB_MANY_EX) {releaseManyExTab();}

		experimentTimer->stop();
		status = false;

		ui->busyWaitButton->setDisabled(false);
		computeAverage = false;
	}
}

void W_CommTest::on_pushButtonStartStop_clicked()
{
	startStopComTest(false);
}

void W_CommTest::on_pushButtonReset_clicked()
{
	sentPackets = 0;
	goodPackets = 0;
	badPackets = 0;
	receivedPackets = 0;
	throughputRate = 0.0;
	qualityRate = 0.0;
}

void W_CommTest::on_busyWaitButton_pressed(void)
{
	shouldBusyWait = !shouldBusyWait;

	QString buttonText = "Busy Wait (";
	buttonText.append(shouldBusyWait ? "On)" : "Off)");
	ui->busyWaitButton->setText(buttonText);
}

void W_CommTest::on_tabWidget_currentChanged(int index)
{
	currentTab = index;

	if(index == TAB_MANY_EX)
	{
		//qDebug() << "Many Ex tab";
		ui->comboBox_slave->setDisabled(true);
	}
	else
	{
		ui->comboBox_slave->setEnabled(true);
	}
}

void W_CommTest::latchManyExTab(void)
{
	uint8_t cnt = 0;

	//How many boxes checked?
	if(ui->cbSEX1->isChecked())
	{
		slaveList[cnt] = FLEXSEA_EXECUTE_1;
		cnt++;
	}
	if(ui->cbSEX2->isChecked())
	{
		slaveList[cnt] = FLEXSEA_EXECUTE_2;
		cnt++;
	}
	if(ui->cbSEX3->isChecked())
	{
		slaveList[cnt] = FLEXSEA_EXECUTE_3;
		cnt++;
	}
	if(ui->cbSEX4->isChecked())
	{
		slaveList[cnt] = FLEXSEA_EXECUTE_4;
		cnt++;
	}

	slaveListCount = cnt;
	//qDebug() << "slaveListCount: " << slaveListCount;

	ui->cbSEX1->setEnabled(false);
	ui->cbSEX2->setEnabled(false);
	ui->cbSEX3->setEnabled(false);
	ui->cbSEX4->setEnabled(false);

	//Can't run test with no slave (empty list), so we force Ex1:
	if(!cnt)
	{
		ui->cbSEX1->setChecked(true);
		slaveList[0] = FLEXSEA_EXECUTE_1;
		slaveListCount = 1;
	}
}

void W_CommTest::releaseManyExTab(void)
{
	slaveList[0] = 0;
	slaveList[1] = 0;
	slaveList[2] = 0;
	slaveList[3] = 0;
	slaveListCount = 0;

	ui->cbSEX1->setEnabled(true);
	ui->cbSEX2->setEnabled(true);
	ui->cbSEX3->setEnabled(true);
	ui->cbSEX4->setEnabled(true);
}

//Returns the number of ms since a test was started
qint64 W_CommTest::getTimeElapsed(void)
{
	return (statsTimer->currentMSecsSinceEpoch() - timeElapsedStart);
}

//Call this at start to latch the starting timestamp
void W_CommTest::setTimeElapsedCounterToZero(void)
{
	timeElapsedStart = statsTimer->currentMSecsSinceEpoch();
}

//Computes the average rate
float W_CommTest::getAverageRate(qint64 dt, int32_t packets)
{
	int64_t tmp1 = (int64_t)(1000 * packets) / dt;
	float tmp2 = (float) tmp1;
	//qDebug() << "Average rate:" << tmp2 << endl;
	return (tmp2);
}
