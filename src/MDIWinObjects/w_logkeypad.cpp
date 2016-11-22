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
#include "main.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_LogKeyPad::W_LogKeyPad(QWidget *parent,  struct logContainer_s *logRef) :
	QWidget(parent),
	ui(new Ui::W_LogKeyPad)
{
	ui->setupUi(this);

	setWindowTitle("Read & Display Log File");
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	myLogRef = logRef;
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
	//All values at 0:
	ui->FileNameLabel->setText(myLogRef->shortFileName);
	ui->labelCursorMs->setText(
				QString::number(myLogRef->logList.at(0).timeStamp_ms));
	ui->labelCursorPct->setText(QString::number(0, 'f', 1));
	ui->TimeSlider->setRange(0, myLogRef->logList.length()-1);
	ui->labelDataPoints->setText(QString::number(myLogRef->logList.length()));
	ui->labelLength->setText(
				QString::number(myLogRef->logList.last().timeStamp_ms/1000.0));
	ui->labelExperiment->setText(myLogRef->experimentName);
	ui->TimeSlider->setFocus();

}


//****************************************************************************
// Private slot(s):
//****************************************************************************


void W_LogKeyPad::on_TimeSlider_valueChanged(int value)
{
	emit logTimeSliderValueChanged(value);
	ui->labelCursorMs->setText(
				QString::number(myLogRef->logList.at(value).timeStamp_ms));
	ui->labelCursorPct->setText(
					QString::number(
					(value * 100.0) / (myLogRef->logList.length()-1), 'f', 1));
}
