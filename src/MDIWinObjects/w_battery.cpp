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
#include "ui_w_battery.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Battery::W_Battery(QWidget *parent,
					 FlexseaDevice *currentLog,
					 BatteryDevice *batteryLogPtrInit,
					 TestBenchProject *testBenchLogPtrInit,
					 DisplayMode mode,
					 QList<BatteryDevice> *deviceListPtr) :
	QWidget(parent),
	ui(new Ui::W_Battery)
{
	ui->setupUi(this);

	batteryLog = batteryLogPtrInit;
	testBenchLog = testBenchLogPtrInit;
	deviceList = deviceListPtr;

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	ui->comboBox_slaveM->setDisabled(true);
	ui->comboBox_slaveM->addItem("Not implemented");

	updateDisplayMode(mode, currentLog);

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
void W_Battery::refreshDisplay(void)
{
	int index = ui->comboBox_slave->currentIndex();
	display(&((*deviceList)[index]), 0);
}

void W_Battery::refreshDisplayLog(int index, FlexseaDevice * devPtr)
{
	QString slaveName = devPtr->slaveName;

	if(devPtr->slaveName == batteryLog->slaveName)
	{
		if(batteryLog->baList.isEmpty() == false)
		{
			 display(batteryLog, index);
		}
	}
	else if (slaveName == testBenchLog->slaveName)
	{
		if(testBenchLog->tbList.isEmpty() == false)
		{
			display(testBenchLog->tbList[index]->ba);
		}
	}
}

void W_Battery::updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr)
{
	displayMode = mode;
	if(displayMode == DisplayLogData)
	{
		initLog(devPtr);
	}
	else
	{
		initLive();
	}
}

//****************************************************************************
// Private function(s):
//****************************************************************************


void W_Battery::initLive(void)
{
	//Populates Slave list:
	ui->comboBox_slave->clear();

	for(int i = 0; i < (*deviceList).length(); i++)
	{
		ui->comboBox_slave->addItem((*deviceList)[i].slaveName);
	}

// TODO: S.B. what will be the purpose of this box?
//	FlexSEA_Generic::populateSlaveComboBox(ui->comboBox_slaveM,
//											SL_BASE_ALL, SL_LEN_ALL);
//	//Start with manage 1:
//	ui->comboBox_slaveM->setCurrentIndex(SL_BASE_MN);

}

void W_Battery::initLog(FlexseaDevice *devPtr)
{
	QString slaveName = devPtr->slaveName;

	//Populates Slave list:
	ui->comboBox_slave->clear();

	if(slaveName == batteryLog->slaveName)
	{
		ui->comboBox_slave->addItem(batteryLog->slaveName);
	}
	else if (slaveName == testBenchLog->slaveName)
	{
		ui->comboBox_slave->addItem("battery 1");
	}
}

void W_Battery::display(BatteryDevice *devicePtr, int index)
{
	display(devicePtr->baList[index]);
}

void W_Battery::display(struct battery_s *ba)
{

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
	ui->dispTempD->setText(QString::number(ba->decoded.temp));
	ui->dispPd->setText(QString::number((float)ba->decoded.power/1000000, 'f', 1));
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
