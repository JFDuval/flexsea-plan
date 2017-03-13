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
	[This file] w_event.h: Event Flagger Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-03-13 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_event.h"
#include "ui_w_event.h"
#include "main.h"
#include <QString>
#include <QTimer>
#include <QDebug>

//Static variables:
QString W_Event::flagText = "";
int W_Event::flagCode = 0;

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Event::W_Event(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_Event)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
}

W_Event::~W_Event()
{
	emit windowClosed();
	delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

QString W_Event::getEventFlags(void)
{
	return flagText;
}

int W_Event::getEventCode(void)
{
	return flagCode;
}

//****************************************************************************
// Public slot(s):
//****************************************************************************

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Event::init(void)
{
	//Slider & display label:
	ui->horizontalSlider->setMinimum(10);
	ui->horizontalSlider->setMaximum(1000);
	ui->horizontalSlider->setValue(250);	//Default

	//Timers:
	timerPb[0] = new QTimer(this);
	connect(timerPb[0], SIGNAL(timeout()), this, SLOT(timerPb0()));
	timerPb[1] = new QTimer(this);
	connect(timerPb[1], SIGNAL(timeout()), this, SLOT(timerPb1()));
	timerPb[2] = new QTimer(this);
	connect(timerPb[2], SIGNAL(timeout()), this, SLOT(timerPb2()));
	timerPb[3] = new QTimer(this);
	connect(timerPb[3], SIGNAL(timeout()), this, SLOT(timerPb3()));

	//Flags:
	flag[0] = 0;
	flag[1] = 0;
	flag[2] = 0;
	flag[3] = 0;

	//Pushbuttons:
	buttons[0] = ui->pushButtonA;
	buttons[1] = ui->pushButtonB;
	buttons[2] = ui->pushButtonC;
	buttons[3] = ui->pushButtonD;

	for(int i = 0; i < NUM_OF_BUTTONS; i++)
	{
		buttons[i]->setStyleSheet("background-color: \
						rgb(127, 127, 127); color: rgb(0, 0, 0)");
	}
}

QString W_Event::buildList(void)
{
	QString tmp = "";

	if(flag[0]) {tmp += 'A';};
	if(flag[1]) {tmp += 'B';};
	if(flag[2]) {tmp += 'C';};
	if(flag[3]) {tmp += 'D';};

	return tmp;
}

int W_Event::buildCode(void)
{
	int tmp = 0;

	if(flag[0]) {tmp += 1;};
	if(flag[1]) {tmp += 2;};
	if(flag[2]) {tmp += 4;};
	if(flag[3]) {tmp += 8;};

	return tmp;
}

//****************************************************************************
// Private slot(s):
//****************************************************************************


void W_Event::on_horizontalSlider_valueChanged(int value)
{
	ui->labelDelay->setText(QString::number(value));
	delayValue = value;
}

void W_Event::on_pushButtonA_clicked()
{
	pushButtonEvent(0);
}

void W_Event::on_pushButtonB_clicked()
{
	pushButtonEvent(1);
}

void W_Event::on_pushButtonC_clicked()
{
	pushButtonEvent(2);
}

void W_Event::on_pushButtonD_clicked()
{
	pushButtonEvent(3);
}

void W_Event::pushButtonEvent(int pb)
{
	timerPb[pb]->start(delayValue);
	flag[pb] = 1;

	buttons[pb]->setStyleSheet("background-color: \
					rgb(0, 255, 0); color: rgb(0, 0, 0)");

	flagText = buildList();
	flagCode = buildCode();
	qDebug() << "pbEvent:" << flagText << "'" << flagCode;
}

void W_Event::timeoutEvent(int pb)
{
	timerPb[pb]->stop();
	flag[pb] = 0;

	buttons[pb]->setStyleSheet("background-color: \
					rgb(127, 127, 127); color: rgb(0, 0, 0)");

	flagText = buildList();
	flagCode = buildCode();
	//qDebug() << "timeoutEvent:" << flagText;
}

void W_Event::timerPb0(void)
{
	timeoutEvent(0);
}

void W_Event::timerPb1(void)
{
	timeoutEvent(1);
}

void W_Event::timerPb2(void)
{
	timeoutEvent(2);
}

void W_Event::timerPb3(void)
{
	timeoutEvent(3);
}
