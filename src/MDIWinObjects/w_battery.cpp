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

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Battery::W_Battery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_Battery)
{
    ui->setupUi(this);

    setWindowTitle("Battery - Barebone");
    setWindowIcon(QIcon(":icons/d_logo_small.png"));

    init();
}

W_Battery::~W_Battery()
{
    delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//Call this function to refresh the display
void W_Battery::refresh(void)
{
    struct battery_s *baPtr;
    myFlexSEA_Generic.assignBatteryPtr(&baPtr, SL_BASE_ALL, \
                                       ui->comboBox_slave->currentIndex());
    displayBattery(baPtr);
}

//****************************************************************************
// Public slot(s):
//****************************************************************************

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Battery::init(void)
{
    //Populates Slave list - active slave:
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBox_slave, \
                                            SL_BASE_BATT, SL_LEN_BATT);

    //Populates Slave list - connected to slave:
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBox_slaveM, \
                                            SL_BASE_ALL, SL_LEN_ALL);

    //Start with manage 1:
    ui->comboBox_slaveM->setCurrentIndex(SL_BASE_MN);
}

void W_Battery::displayBattery(struct battery_s *ba)
{
    int combined_status = 0;

    //Raw values:
    //===========

    ui->dispV->setText(QString::number(ba->voltage));
    ui->dispI->setText(QString::number(ba->current));
    ui->dispPB->setText(QString::number(ba->pushbutton));
    ui->dispTemp->setText(QString::number(ba->temp));
    ui->dispStatus1->setText(QString::number(ba->status));

    //Decoded values:
    //===============

    ui->dispVd->setText(QString::number((float)ba->decoded.voltage/1000,'f',2));
    ui->dispId->setText(QString::number((float)ba->decoded.current/1000,'f',2));
    ui->dispVd->setText(QString::number((float)ba->decoded.temp/10,'f',1));
    ui->dispPd->setText(QString::number((float)ba->decoded.power/1000,'f',2));
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
