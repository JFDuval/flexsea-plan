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
	[This file] w_status.cpp: Status window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2018-02-10 | sbelanger | New code, initial release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include <flexsea_system.h>
#include <flexsea_buffers.h>
#include <flexsea_comm.h>
#include "w_status.h"
#include "ui_w_status.h"
#include "flexsea_generic.h"
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
#include <flexsea_board.h>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Status::W_Status(QWidget *parent, DynamicUserDataManager* userDataManager) :
	QWidget(parent),
	ui(new Ui::W_Status),
	userDataMan(userDataManager)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
}

W_Status::~W_Status()
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

void W_Status::receiveNewData()
{

}

void W_Status::comStatusChanged(SerialPortStatus status,int nbTries)
{
	(void)nbTries;	// Not use by this slot.

	if(status == PortOpeningSucceed)
		userDataMan->requestMetaData(active_slave);
}

void W_Status::externalErrorFlag()
{
	qDebug() << "W_Status: Received a signal";
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Status::init(void)
{
	lab_name_ptr[0] = ui->lab_name_0;
	lab_name_ptr[1] = ui->lab_name_1;
	lab_name_ptr[2] = ui->lab_name_2;
	lab_name_ptr[3] = ui->lab_name_3;
	lab_name_ptr[4] = ui->lab_name_4;
	lab_name_ptr[5] = ui->lab_name_5;
	lab_name_ptr[6] = ui->lab_name_6;
	lab_name_ptr[7] = ui->lab_name_7;
	lab_name_ptr[8] = ui->lab_name_8;
	lab_name_ptr[9] = ui->lab_name_9;
	lab_indicator_ptr[0] = ui->lab_indicator_0;
	lab_indicator_ptr[1] = ui->lab_indicator_1;
	lab_indicator_ptr[2] = ui->lab_indicator_2;
	lab_indicator_ptr[3] = ui->lab_indicator_3;
	lab_indicator_ptr[4] = ui->lab_indicator_4;
	lab_indicator_ptr[5] = ui->lab_indicator_5;
	lab_indicator_ptr[6] = ui->lab_indicator_6;
	lab_indicator_ptr[7] = ui->lab_indicator_7;
	lab_indicator_ptr[8] = ui->lab_indicator_8;
	lab_indicator_ptr[9] = ui->lab_indicator_9;
	pb_clear_ptr[0] = ui->pb_clear_0;
	pb_clear_ptr[1] = ui->pb_clear_1;
	pb_clear_ptr[2] = ui->pb_clear_2;
	pb_clear_ptr[3] = ui->pb_clear_3;
	pb_clear_ptr[4] = ui->pb_clear_4;
	pb_clear_ptr[5] = ui->pb_clear_5;
	pb_clear_ptr[6] = ui->pb_clear_6;
	pb_clear_ptr[7] = ui->pb_clear_7;
	pb_clear_ptr[8] = ui->pb_clear_8;
	pb_clear_ptr[9] = ui->pb_clear_9;

	QFont font( "Arial", 12, QFont::Bold);

	for(int i = 0; i < NB_STATUS; ++i)
	{
		(lab_indicator_ptr[i])->setText("      " + QString(QChar(0x29BF)) + "      ");
		(lab_indicator_ptr[i])->setAlignment(Qt::AlignCenter);
		(lab_indicator_ptr[i])->setFont(font);
		setStatus(i, STATUS_GREY);

		lab_name_ptr[i]->setText("");
		pb_clear_ptr[i]->setEnabled(false);
	}

	initLabelText();

	//Populates Slave list:
//	FlexSEA_Generic::populateSlaveComboBox(ui->comboBox_slave, SL_BASE_ALL, \
//											SL_LEN_ALL);
//	ui->comboBox_slave->setCurrentIndex(0);	//Execute 1 by default

//	//Variables:
//	active_slave_index = ui->comboBox_slave->currentIndex();
//	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);



//	//Timer used to refresh the received data:
//	refreshDelayTimer = new QTimer(this);
//	connect(refreshDelayTimer,	&QTimer::timeout,
//			this,				&W_UserRW::refreshDisplay);

}

void W_Status::initLabelText(void)
{
	uint8_t idx = 0;
	lab_name_ptr[idx++]->setText("Supply Voltage");
	lab_name_ptr[idx++]->setText("Temperature");
	lab_name_ptr[idx++]->setText("I2t Battery");
	lab_name_ptr[idx++]->setText("I2t Motor");
	lab_name_ptr[idx++]->setText("BWC Link");
	lab_name_ptr[idx++]->setText("Ex Comm");
	lab_name_ptr[idx++]->setText("Re Comm");
	lab_name_ptr[idx++]->setText("Button");

	//Enable associated buttons:
	for(int i = 0; i < idx; i++){pb_clear_ptr[i]->setEnabled(true);}
}

//Send a Write command:
void W_Status::writeUserData(uint8_t index)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

//	//Refresh variable:
//	user_data_1.w[0] = (int16_t)ui->w0->text().toInt();
//	user_data_1.w[1] = (int16_t)ui->w1->text().toInt();
//	user_data_1.w[2] = (int16_t)ui->w2->text().toInt();
//	user_data_1.w[3] = (int16_t)ui->w3->text().toInt();

	//qDebug() << "Write user data" << index << ":" << user_data_1.w[index];

	//Prepare and send command:
	tx_cmd_data_user_w(TX_N_DEFAULT, index);
	pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, WRITE);
}

//Send a Read command:
void W_Status::readUserData(void)
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

void W_Status::setStatus(int row, int status)
{
	switch(status)
	{
		case STATUS_GREY:
			(lab_indicator_ptr[row])->setStyleSheet("QLabel { background-color: \
										rgb(127,127,127); color: black;}");
			break;
		case STATUS_GREEN:
			(lab_indicator_ptr[row])->setStyleSheet("QLabel { background-color: \
										rgb(0,255,0); color: black;}");
			break;
		case STATUS_YELLOW:
			(lab_indicator_ptr[row])->setStyleSheet("QLabel { background-color: \
										rgb(255,255,0); color: black;}");
			break;
		case STATUS_RED:
			(lab_indicator_ptr[row])->setStyleSheet("QLabel { background-color: \
										rgb(255,0,0); color: black;}");
			break;
		default:
			(lab_indicator_ptr[row])->setStyleSheet("QLabel { background-color: \
										black; color: white;}");
			break;
	}
}

void W_Status::statusReset(int row)
{

}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_Status::on_pb_clear_0_clicked(){statusReset(0);}
void W_Status::on_pb_clear_1_clicked(){statusReset(1);}
void W_Status::on_pb_clear_2_clicked(){statusReset(2);}
void W_Status::on_pb_clear_3_clicked(){statusReset(3);}
void W_Status::on_pb_clear_4_clicked(){statusReset(4);}
void W_Status::on_pb_clear_5_clicked(){statusReset(5);}
void W_Status::on_pb_clear_6_clicked(){statusReset(6);}
void W_Status::on_pb_clear_7_clicked(){statusReset(7);}
void W_Status::on_pb_clear_8_clicked(){statusReset(8);}
void W_Status::on_pb_clear_9_clicked(){statusReset(9);}
