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
	[This file] w_manage.h: Manage View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_manage.h"
#include "ui_w_manage.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Manage::W_Manage(QWidget *parent,
				   ManageDevice *deviceLogPtr,
				   DisplayMode mode,
				   QList<ManageDevice> *deviceListPtr) :
	QWidget(parent),
	ui(new Ui::W_Manage)
{
	ui->setupUi(this);

	deviceLog = deviceLogPtr;
	deviceList = deviceListPtr;

	displayMode = mode;

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	updateDisplayMode(displayMode, nullptr);
}

W_Manage::~W_Manage()
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
void W_Manage::refreshDisplay(void)
{
	int index = ui->comboBox_slave->currentIndex();
	display(&((*deviceList)[index]), 0);
}

void W_Manage::refreshDisplayLog(int index, FlexseaDevice * devPtr)
{
	if(devPtr->slaveName == deviceLog->slaveName)
	{
		if(deviceLog->mnList.isEmpty() == false)
		{
			 display(deviceLog, index);
		}
	}
}

void W_Manage::updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr)
{
	(void)devPtr;
	displayMode = mode;
	if(displayMode == DisplayLogData)
	{
		initLog();
	}
	else
	{
		initLive();
	}
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Manage::initLive(void)
{
	//Populates Slave list:
	ui->comboBox_slave->clear();

	for(int i = 0; i < (*deviceList).length(); i++)
	{
		ui->comboBox_slave->addItem((*deviceList)[i].slaveName);
	}
}

void W_Manage::initLog(void)
{
	//Populates Slave list:
	ui->comboBox_slave->clear();
	ui->comboBox_slave->addItem(deviceLog->slaveName);
}


void W_Manage::display(ManageDevice *devicePtr, int index)
{
	struct manage_s *mn = devicePtr->mnList[index];
	//Raw values:
	//===========

	ui->disp_accx->setText(QString::number(mn->accel.x));
	ui->disp_accy->setText(QString::number(mn->accel.y));
	ui->disp_accz->setText(QString::number(mn->accel.z));
	ui->disp_gyrox->setText(QString::number(mn->gyro.x));
	ui->disp_gyroy->setText(QString::number(mn->gyro.y));
	ui->disp_gyroz->setText(QString::number(mn->gyro.z));

	if(mn->sw1 == 0)
	{
		ui->dispUserButton->setText("Pressed");
	}
	else
	{
		ui->dispUserButton->setText("Released");
	}

	ui->dispDI->setText("ToDo");

	ui->dispAna0->setText(QString::number(mn->analog[0]));
	ui->dispAna1->setText(QString::number(mn->analog[1]));
	ui->dispAna2->setText(QString::number(mn->analog[2]));
	ui->dispAna3->setText(QString::number(mn->analog[3]));
	ui->dispAna4->setText(QString::number(mn->analog[4]));
	ui->dispAna5->setText(QString::number(mn->analog[5]));
	ui->dispAna6->setText(QString::number(mn->analog[6]));
	ui->dispAna7->setText(QString::number(mn->analog[7]));

	//Decoded values:
	//===============

	ui->disp_accx_d->setText(QString::number((float)mn->decoded.accel.x/1000,'f',2));
	ui->disp_accy_d->setText(QString::number((float)mn->decoded.accel.y/1000,'f',2));
	ui->disp_accz_d->setText(QString::number((float)mn->decoded.accel.z/1000,'f',2));

	ui->disp_gyrox_d->setText(QString::number(mn->decoded.gyro.x, 'i', 0));
	ui->disp_gyroy_d->setText(QString::number(mn->decoded.gyro.y, 'i', 0));
	ui->disp_gyroz_d->setText(QString::number(mn->decoded.gyro.z, 'i', 0));

	ui->dispAna0d->setText(QString::number((float)mn->decoded.analog[0]/1000,'f',2));
	ui->dispAna1d->setText(QString::number((float)mn->decoded.analog[1]/1000,'f',2));
	ui->dispAna2d->setText(QString::number((float)mn->decoded.analog[2]/1000,'f',2));
	ui->dispAna3d->setText(QString::number((float)mn->decoded.analog[3]/1000,'f',2));
	ui->dispAna4d->setText(QString::number((float)mn->decoded.analog[4]/1000,'f',2));
	ui->dispAna5d->setText(QString::number((float)mn->decoded.analog[5]/1000,'f',2));
	ui->dispAna6d->setText(QString::number((float)mn->decoded.analog[6]/1000,'f',2));
	ui->dispAna7d->setText(QString::number((float)mn->decoded.analog[7]/1000,'f',2));

	//==========
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
