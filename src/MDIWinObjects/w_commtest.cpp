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

#include "flexsea.h"
#include "flexsea_comm.h"
#include "w_commtest.h"
#include "flexsea_generic.h"
#include "ui_w_commtest.h"
#include "main.h"
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
#include <QDateTime>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_CommTest::W_CommTest(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_CommTest)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
	initTimers();
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

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_CommTest::init(void)
{
	//Populates Slave list:
	FlexSEA_Generic::populateSlaveComboBox(ui->comboBox_slave, SL_BASE_ALL, \
											SL_LEN_ALL);
	ui->comboBox_slave->setCurrentIndex(0);	//Execute 1 by default

	//Variables:
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);

	//Displays:
	ui->labelSentPackets->setText("0");
	ui->labelReceivedPackets->setText("0");
	ui->labelGoodPackets->setText("0");
	ui->labelSuccess->setText("0");
	ui->labelLossRate->setText("0");
	ui->lineEdit->setText(QString::number(DEFAULT_EXPERIMENT_TIMER_FREQ));

	//Seed:
	QTime myTime;
	initRandomGenerator(myTime.msecsSinceStartOfDay());
}

void W_CommTest::initTimers(void)
{
	experimentTimerFreq = DEFAULT_EXPERIMENT_TIMER_FREQ;

	displayTimer = new QTimer(this);
	connect(displayTimer, SIGNAL(timeout()), this, SLOT(refreshDisplay()));
	displayTimer->start(TIM_FREQ_TO_P(DISPLAY_TIMER));

	experimentTimer = new QTimer(this);
	connect(experimentTimer, SIGNAL(timeout()), this, SLOT(readCommTest()));
	experimentTimer->stop();
}

//Send a Read command:
void W_CommTest::readCommTest(void)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

	//Prepare and send command:
	tx_cmd_tools_comm_test_r(TX_N_DEFAULT, 1, 20);
	pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, READ);

	//FlexSEA_Generic::packetVisualizer(numb, comm_str_usb);
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
		successRate = 0;
		lossRate = 0;
	}
	else
	{
		successRate = (float)goodPackets/receivedPackets;
		lossRate = (float)(sentPackets-receivedPackets) / sentPackets;
	}

	ui->labelSentPackets->setText(QString::number(sentPackets));
	ui->labelReceivedPackets->setText(QString::number(receivedPackets));
	ui->labelGoodPackets->setText(QString::number(goodPackets));

	ui->labelSuccess->setText(QString::number(successRate, 'f',2));
	ui->labelLossRate->setText(QString::number(lossRate, 'f',2));
}

void W_CommTest::on_comboBox_slave_currentIndexChanged(int index)
{
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);
}

void W_CommTest::on_pushButtonStartStop_clicked()
{
	static bool status = false;
	int32_t tmpFreq = 0;

	if(status == false)
	{
		//We were showing Start.
		ui->pushButtonStartStop->setText("Stop test");
		ui->lineEdit->setEnabled(false);

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
	}
	else
	{
		//We were showing Stop.
		ui->pushButtonStartStop->setText("Start test");
		ui->lineEdit->setEnabled(true);
		experimentTimer->stop();
		status = false;
	}

	readCommTest();
}

void W_CommTest::on_pushButtonReset_clicked()
{
	sentPackets = 0;
	goodPackets = 0;
	badPackets = 0;
	receivedPackets = 0;
	successRate = 0.0;
}
