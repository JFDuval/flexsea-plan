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
	[This file] WinViewRicnu: RIC/NU Kneww View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-12 | jfduval | New file
	* 2016-09-12 | jfduval | Enabling stream
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_ricnu.h"
#include "ui_w_ricnu.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Ricnu::W_Ricnu(QWidget *parent,
				 RicnuProject *deviceLogPtr,
				 DisplayMode mode,
				 QList<RicnuProject> *deviceListPtr) :
	QWidget(parent),
	ui(new Ui::W_Ricnu)
{
	ui->setupUi(this);

	deviceLog = deviceLogPtr;
	deviceList = deviceListPtr;

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	lastDisplayMode = (DisplayMode)1000; // To force the init
	updateDisplayMode(mode, nullptr);
}

W_Ricnu::~W_Ricnu()
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
void W_Ricnu::refreshDisplay(void)
{
	int index = ui->comboBox_slave->currentIndex();
	display(&((*deviceList)[index]), 0);
}

void W_Ricnu::refreshDisplayLog(int index, FlexseaDevice * devPtr)
{
	if(devPtr->slaveName == deviceLog->slaveName)
	{
	   if(deviceLog->riList.isEmpty() == false)
	   {
			display(deviceLog, index);
	   }
	}
}

void W_Ricnu::updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr)
{
	(void)devPtr;
	displayMode = mode;

	if(displayMode != lastDisplayMode)
	{
		if(displayMode == DisplayLogData)
		{
			initLog();
		}
		else
		{
			initLive();
		}
	}

	lastDisplayMode = displayMode;
}

//****************************************************************************
// Private function(s):
//****************************************************************************
void W_Ricnu::initLive(void)
{
	//Populates Slave list:
	ui->comboBox_slave->clear();

	for(int i = 0; i < (*deviceList).length(); i++)
	{
		ui->comboBox_slave->addItem((*deviceList)[i].slaveName);
	}
}

void W_Ricnu::initLog(void)
{
	//Populates Slave list:
	ui->comboBox_slave->clear();
	ui->comboBox_slave->addItem(deviceLog->slaveName);
}

void W_Ricnu::display(RicnuProject *devicePtr, int index)
{
	struct ricnu_s_plan *ricnu = devicePtr->riList[index];

	//Raw values:
	//===========

	ui->disp_accx->setText(QString::number(ricnu->ex->accel.x));
	ui->disp_accy->setText(QString::number(ricnu->ex->accel.y));
	ui->disp_accz->setText(QString::number(ricnu->ex->accel.z));
	ui->disp_gyrox->setText(QString::number(ricnu->ex->gyro.x));
	ui->disp_gyroy->setText(QString::number(ricnu->ex->gyro.y));
	ui->disp_gyroz->setText(QString::number(ricnu->ex->gyro.z));

	ui->enc_mot->setText(QString::number(ricnu->enc_motor));
	ui->enc_joint->setText(QString::number(ricnu->enc_joint));
	ui->pwm->setText(QString::number(ricnu->ex->sine_commut_pwm));

	ui->strain1->setText(QString::number(ricnu->st->ch[0].strain_filtered));
	ui->strain2->setText(QString::number(ricnu->st->ch[1].strain_filtered));
	ui->strain3->setText(QString::number(ricnu->st->ch[2].strain_filtered));
	ui->strain4->setText(QString::number(ricnu->st->ch[3].strain_filtered));
	ui->strain5->setText(QString::number(ricnu->st->ch[4].strain_filtered));
	ui->strain6->setText(QString::number(ricnu->st->ch[5].strain_filtered));
	ui->disp_current->setText(QString::number(ricnu->ex->current));

	//Decode some of them:
	//===================

	ui->disp_accx_d->setText(QString::number((float)ricnu->ex->decoded.accel.x/1000,'f',2));
	ui->disp_accy_d->setText(QString::number((float)ricnu->ex->decoded.accel.y/1000,'f',2));
	ui->disp_accz_d->setText(QString::number((float)ricnu->ex->decoded.accel.z/1000,'f',2));

	ui->disp_gyrox_d->setText(QString::number((double)ricnu->ex->decoded.gyro.x, 'i', 0));
	ui->disp_gyroy_d->setText(QString::number((double)ricnu->ex->decoded.gyro.y, 'i', 0));
	ui->disp_gyroz_d->setText(QString::number((double)ricnu->ex->decoded.gyro.z, 'i', 0));

	ui->disp_current_d->setText(QString::number(ricnu->ex->decoded.current, 'i',0));

	//TODO change
	ui->strain1d->setText(QString::number(ricnu->st->decoded.strain[0],'i',0));
	ui->strain2d->setText(QString::number(ricnu->st->decoded.strain[1],'i',0));
	ui->strain3d->setText(QString::number(ricnu->st->decoded.strain[2],'i',0));
	ui->strain4d->setText(QString::number(ricnu->st->decoded.strain[3],'i',0));
	ui->strain5d->setText(QString::number(ricnu->st->decoded.strain[4],'i',0));
	ui->strain6d->setText(QString::number(ricnu->st->decoded.strain[5],'i',0));
}



//****************************************************************************
// Private slot(s):
//****************************************************************************
