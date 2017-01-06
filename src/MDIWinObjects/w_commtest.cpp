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

	//Timer used to refresh the received data:
	refreshDelayTimer = new QTimer(this);
	connect(refreshDelayTimer, SIGNAL(timeout()), this, SLOT(refreshDisplay()));

	//Seed:
	QTime myTime;
	initRandomGenerator(myTime.msecsSinceStartOfDay());
}

//Send a Read command:
void W_CommTest::readCommTest(void)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

	//Prepare and send command:
	tx_cmd_tools_comm_test_r(TX_N_DEFAULT, 1, 35);
	pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, READ);

	FlexSEA_Generic::packetVisualizer(numb, comm_str_usb);

	//Display will be refreshed in 75ms:
	refreshDelayTimer->start(75);
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_CommTest::on_pushButton_w0_clicked()
{
	readCommTest();
}

//Refreshes the User R values (display only):
void W_CommTest::refreshDisplay(void)
{
	refreshDelayTimer->stop();

	qDebug() << "Packets sent: " << sentPackets << "Good packets: " \
			 << goodPackets << "Bad packets: " << badPackets;
}

void W_CommTest::on_comboBox_slave_currentIndexChanged(int index)
{
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);
}
