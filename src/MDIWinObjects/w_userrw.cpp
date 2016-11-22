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

#include "w_userrw.h"
#include "flexsea_generic.h"
#include "ui_w_userrw.h"
#include "main.h"
#include <QString>
#include <QTextStream>
#include <QDebug>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_UserRW::W_UserRW(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_UserRW)
{
	ui->setupUi(this);

	setWindowTitle("User R/W");
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

	//Variables:
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);

	//All W boxes to 0:
	ui->w0->setText("0");
	ui->w1->setText("0");
	ui->w2->setText("0");
	ui->w3->setText("0");
}

//Send a Write command:
void W_UserRW::writeUserData(uint8_t index)
{
	//Refresh variable:
	user_data_1.w[0] = (int16_t)ui->w0->text().toInt();
	user_data_1.w[1] = (int16_t)ui->w1->text().toInt();
	user_data_1.w[2] = (int16_t)ui->w2->text().toInt();
	user_data_1.w[3] = (int16_t)ui->w3->text().toInt();

	qDebug() << "Write user data" << index << ":" << user_data_1.w[index];

	qDebug() << "ToDo: send command!";
}

//Send a Read command:
void W_UserRW::readUserData(void)
{
	qDebug() << "Read user data.";

	qDebug() << "ToDo: send command!";
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
