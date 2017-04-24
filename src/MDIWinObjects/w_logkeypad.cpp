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
	[This file] w_LogKeyPad.h: LogKeyPAd Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-11-19 | Sebastien Belanger | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_logkeypad.h"
#include "ui_w_logkeypad.h"
#include "datalogger.h"
#include <QDebug>
#include "main.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_LogKeyPad::W_LogKeyPad(QWidget *parent, FlexseaDevice *flexSeaDevicePtr) :
	QWidget(parent),
	ui(new Ui::W_LogKeyPad)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	devicePtr = flexSeaDevicePtr;
	init();
}

W_LogKeyPad::~W_LogKeyPad()
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

void W_LogKeyPad::init(void)
{
	QString cursor, refresh;
	int len_ms = 0, samples = 0;
	double rate = 0.0;

	//Filename, experiment and basic stats:
	ui->FileNameLabel->setText(devicePtr->shortFileName);	//Filename
	ui->labelExperiment->setText(devicePtr->experimentName);//Experiment

	samples = devicePtr->timeStamp.length();
	ui->labelDataPoints->setText(QString::number(samples));

	len_ms = devicePtr->timeStamp.last().ms;
	ui->labelLength->setText(QString::number(len_ms/1000.0));

	rate = (double)samples / ((double)len_ms / 1000);
	refresh = "Average of " + QString::number(rate, 'f', 2) + "Hz";
	ui->labelRefresh->setText(refresh);

	//Slider:
	ui->TimeSlider->setRange(0, devicePtr->timeStamp.length()-1);
	cursor = QString::number(devicePtr->timeStamp[0].ms) + \
			 " ms (" + QString::number(0, 'f', 1) + "%)";

	cursor = QString::number(devicePtr->timeStamp[0].ms) + " ms (" +\
			 QString::number(0, 'f', 1) + "%, sample " +\
			 QString::number(0+1) + " of " +
			 QString::number(devicePtr->timeStamp.length()) + ")";

	ui->labelCursor->setText(cursor);

	ui->TimeSlider->setFocus();
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_LogKeyPad::on_TimeSlider_valueChanged(int index)
{
	QString cursor;
	int pctVal = 0;

	emit logTimeSliderValueChanged(index, devicePtr);

	pctVal = (index * 100.0) / (devicePtr->timeStamp.length()-1);
	cursor = QString::number(devicePtr->timeStamp[index].ms) + " ms (" +\
			 QString::number(pctVal, 'f', 1) + "%, sample " +\
			 QString::number(index+1) + " of " +
			 QString::number(devicePtr->timeStamp.length()) + ")";
	ui->labelCursor->setText(cursor);
}
