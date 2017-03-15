/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2016 Dephy, Inc. <http://dephy.com/>

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
	[This file] userrw.h: User Read/Write Tool
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-11-22 | jfduval | New code, initial release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include <flexsea_buffers.h>
#include <flexsea_comm.h>
#include "w_userrw.h"
#include "flexsea_generic.h"
#include "ui_w_userrw.h"
#include "main.h"
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
#include <flexsea-user/inc/dynamic_user_structs.h>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_UserRW::W_UserRW(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_UserRW)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
}

W_UserRW::~W_UserRW()
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

void W_UserRW::init(void)
{
	//Populates Slave list:
	FlexSEA_Generic::populateSlaveComboBox(ui->comboBox_slave, SL_BASE_ALL, \
											SL_LEN_ALL);
	ui->comboBox_slave->setCurrentIndex(4);	//Manage 1 by default

	//Variables:
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);

	//All W boxes to 0:
	ui->w0->setText("0");
	ui->w1->setText("0");
	ui->w2->setText("0");
	ui->w3->setText("0");

	//All R boxes to 0:
	ui->r0->setText("0");
	ui->r1->setText("0");
	ui->r2->setText("0");
	ui->r3->setText("0");

	//Timer used to refresh the received data:
	refreshDelayTimer = new QTimer(this);
	connect(refreshDelayTimer, SIGNAL(timeout()), this, SLOT(refreshDisplay()));

	requestMetaData();
}

//Send a Write command:
void W_UserRW::writeUserData(uint8_t index)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

	//Refresh variable:
	user_data_1.w[0] = (int16_t)ui->w0->text().toInt();
	user_data_1.w[1] = (int16_t)ui->w1->text().toInt();
	user_data_1.w[2] = (int16_t)ui->w2->text().toInt();
	user_data_1.w[3] = (int16_t)ui->w3->text().toInt();

	//qDebug() << "Write user data" << index << ":" << user_data_1.w[index];

	//Prepare and send command:
	tx_cmd_data_user_w(TX_N_DEFAULT, index);
	pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, WRITE);
}

//Send a Read command:
void W_UserRW::readUserData(void)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

	//Prepare and send command:
	tx_cmd_data_user_r(TX_N_DEFAULT);
	pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, READ);

	//Display will be refreshed in 75ms:
	refreshDelayTimer->start(75);
}

void W_UserRW::requestMetaData()
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

	//Prepare and send command:
	tx_cmd_user_dyn_r(TX_N_DEFAULT, SEND_METADATA);
	pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, READ);
}

void W_UserRW::parseDynamicUserMetadata()
{
	QListWidget* labelList = ui->userCustomStructLabelList;
	QListWidget* valueList = ui->userCustomStructLabelList;
	labelList->clear();
	valueList->clear();
	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		QString label = "Unknown";
		if(dynamicUser_labels && dynamicUser_fieldLengths)
		{
			char* str = dynamicUser_labels[i];
			uint8_t length = dynamicUser_labelLengths[i];
			QChar qcompat[length];
			for(int j = 0; j < length; j++)
				qcompat[j] = str[j];

			label = QString(qcompat, length);
		}

		labelList->addItem(label);
		valueList->addItem(QStringLiteral("-"));
	}
}

void W_UserRW::parseDynamicUserData()
{
	QListWidget* valueList = ui->userCustomStructLabelList;
	for(int i = 0; i < dynamicUser_numFields; i++)
	{
		int value = 0;
		if(dynamicUser_data)
		{
			value = dynamicUser_data[i];
		}
		QListWidgetItem* item = valueList->item(i);
		if(item)
		{
			item->setText(QString::number(value));
		}
	}
}

void W_UserRW::receiveNewData()
{
	if(newMetaDataAvailable)
		parseDynamicUserMetadata();
	if(newDataAvailable)
		parseDynamicUserData();
}

void W_UserRW::comStatusChanged(bool isOpen)
{
	if(isOpen)
		requestMetaData();
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_UserRW::on_pushButton_w0_clicked()
{
	writeUserData(0);
}

void W_UserRW::on_pushButton_w1_clicked()
{
	writeUserData(1);
}

void W_UserRW::on_pushButton_w2_clicked()
{
	writeUserData(2);
}

void W_UserRW::on_pushButton_w3_clicked()
{
	writeUserData(3);
}

void W_UserRW::on_pushButton_refresh_clicked()
{
	readUserData();
}

//Refreshes the User R values (display only):
void W_UserRW::refreshDisplay(void)
{
	refreshDelayTimer->stop();

	ui->r0->setText(QString::number(user_data_1.r[0]));
	ui->r1->setText(QString::number(user_data_1.r[1]));
	ui->r2->setText(QString::number(user_data_1.r[2]));
	ui->r3->setText(QString::number(user_data_1.r[3]));

	parseDynamicUserMetadata();
}

void W_UserRW::on_comboBox_slave_currentIndexChanged(int index)
{
	active_slave_index = index;
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);
}
