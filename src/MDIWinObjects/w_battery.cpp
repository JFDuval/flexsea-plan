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
	[This file] w_battery.h: Battery View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_battery.h"
#include "flexsea_generic.h"
#include "ui_w_battery.h"
#include "main.h"
#include <QString>
#include <QTextStream>
#include <QDebug>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Battery::W_Battery(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_Battery)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
}

W_Battery::~W_Battery()
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

//Call this function to refresh the display
void W_Battery::refreshDisplayBattery(void)
{
	struct battery_s *baPtr = &batt1;

	displayBattery(baPtr);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Battery::init(void)
{
	//Disable modules that aren't programmed yet:
	ui->dispVmin->setDisabled(true);
	ui->dispVminD->setDisabled(true);
	ui->dispVmax->setDisabled(true);
	ui->dispVmaxD->setDisabled(true);
	ui->dispICont->setDisabled(true);
	ui->dispIContD->setDisabled(true);
	ui->dispIP->setDisabled(true);
	ui->dispIPD->setDisabled(true);
	ui->dispStatus1->setDisabled(true);
	ui->labelStatus->setText("");
}

void W_Battery::displayBattery(struct battery_s *ba)
{
	//Raw bytes to raw values:
	//========================

	ba->status = ba->rawBytes[0];
	ba->voltage = (ba->rawBytes[2] << 8) + ba->rawBytes[3];
	ba->current = (ba->rawBytes[4] << 8) + ba->rawBytes[5];
	ba->temp = ba->rawBytes[6];

	//Raw values:
	//===========

	ui->dispV->setText(QString::number(ba->voltage));
	ui->dispI->setText(QString::number(ba->current));
	ui->dispPB->setText(QString::number(ba->pushbutton));
	ui->dispTemp->setText(QString::number(ba->temp));
	ui->dispStatus1->setText(QString::number(ba->status));

	//Decoded values:
	//===============

	FlexSEA_Generic::decodeBattery(ba);
	ui->dispVd->setText(QString::number((float)ba->decoded.voltage/1000,'f',2));
	ui->dispId->setText(QString::number((float)ba->decoded.current/1000,'f',2));
	ui->dispTempD->setText(QString::number(ba->decoded.temp));
	ui->dispPd->setText(QString::number((float)ba->decoded.power/1000000, 'f', 1));
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
