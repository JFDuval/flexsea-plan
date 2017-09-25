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
	[This file] w_strain.h: Strain View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_strain.h"
#include "ui_w_strain.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Strain::W_Strain(QWidget *parent,
				   StrainDevice *deviceLogPtr,
				   DisplayMode mode,
				   QList<StrainDevice> *deviceListPtr) :
	QWidget(parent),
	ui(new Ui::W_Strain)
{
	ui->setupUi(this);

	deviceLog  = deviceLogPtr;
	deviceList = deviceListPtr;

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	lastDisplayMode = (DisplayMode)1000; // To force the init
	updateDisplayMode(mode, nullptr);

	QString ttip = "<html><head/><body><p>Decoded: ±100% of full scale"
				   "</p></body></html>";

	ui->disp_strain_ch1_d->setToolTip(ttip);
	ui->disp_strain_ch2_d->setToolTip(ttip);
	ui->disp_strain_ch3_d->setToolTip(ttip);
	ui->disp_strain_ch4_d->setToolTip(ttip);
	ui->disp_strain_ch5_d->setToolTip(ttip);
	ui->disp_strain_ch6_d->setToolTip(ttip);
}

W_Strain::~W_Strain()
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
void W_Strain::refreshDisplay(void)
{
	int index = ui->comboBoxSlave->currentIndex();
	display(&((*deviceList)[index]), 0);
}

void W_Strain::refreshDisplayLog(int index, FlexseaDevice * devPtr)
{
	if(devPtr->slaveName == deviceLog->slaveName)
	{
		if(deviceLog->stList.isEmpty() == false)
		{
			 display(deviceLog, index);
		}
	}
}

void W_Strain::updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr)
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

void W_Strain::initLive(void)
{
	//Populates Slave list:
	ui->comboBoxSlave->clear();

	for(int i = 0; i < (*deviceList).length(); i++)
	{
		ui->comboBoxSlave->addItem((*deviceList)[i].slaveName);
	}
}

void W_Strain::initLog(void)
{
	//Populates Slave list:
	ui->comboBoxSlave->clear();
	ui->comboBoxSlave->addItem(deviceLog->slaveName);
}

void W_Strain::display(StrainDevice *devicePtr, int index)
{
	struct strain_s *st = devicePtr->stList[index];

	//Raw values:
	//===========

	ui->disp_strain_ch1->setText(QString::number(st->ch[0].strain_filtered));
	ui->disp_strain_ch2->setText(QString::number(st->ch[1].strain_filtered));
	ui->disp_strain_ch3->setText(QString::number(st->ch[2].strain_filtered));
	ui->disp_strain_ch4->setText(QString::number(st->ch[3].strain_filtered));
	ui->disp_strain_ch5->setText(QString::number(st->ch[4].strain_filtered));
	ui->disp_strain_ch6->setText(QString::number(st->ch[5].strain_filtered));

	//Decoded values:
	//===================

	//When reading the board indirectly (when it's attached to another board)
	//it doesn't get decoded. We do it here:
	if(st->preDecoded == 1)
	{
		StrainDevice::decode(st);
	}

	ui->disp_strain_ch1_d->setText(QString::number(st->decoded.strain[0],'i',0));
	ui->disp_strain_ch2_d->setText(QString::number(st->decoded.strain[1],'i',0));
	ui->disp_strain_ch3_d->setText(QString::number(st->decoded.strain[2],'i',0));
	ui->disp_strain_ch4_d->setText(QString::number(st->decoded.strain[3],'i',0));
	ui->disp_strain_ch5_d->setText(QString::number(st->decoded.strain[4],'i',0));
	ui->disp_strain_ch6_d->setText(QString::number(st->decoded.strain[5],'i',0));

	//==========
}


//****************************************************************************
// Private slot(s):
//****************************************************************************
