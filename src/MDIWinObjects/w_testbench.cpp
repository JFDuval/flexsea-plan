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
	[This file] w_testbench.h: Test Bench View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-08 | jfduval | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_testbench.h"
#include "flexsea_generic.h"
#include "ui_w_testbench.h"
#include "main.h"
#include <QString>
#include <QTextStream>
#include <QDebug>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_TestBench::W_TestBench(QWidget *parent,
						 TestBenchProject *deviceLogPtr,
						 DisplayMode mode,
						 QList<TestBenchProject> *deviceListPtr) :
	QWidget(parent),
	ui(new Ui::W_TestBench)
{
	ui->setupUi(this);

	deviceLog = deviceLogPtr;
	deviceList = deviceListPtr;

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	lastDisplayMode = (DisplayMode)1000; // To force the init
	updateDisplayMode(mode, nullptr);
}

W_TestBench::~W_TestBench()
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
void W_TestBench::refreshDisplay(void)
{
	display(&((*deviceList)[0]), 0);
}

void W_TestBench::refreshDisplayLog(int index, FlexseaDevice * devPtr)
{
	if(devPtr->slaveName == deviceLog->slaveName)
	{
		if(deviceLog->tbList.isEmpty() == false)
		{
			 display(deviceLog, index);
		}
	}
}

void W_TestBench::updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr)
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

void W_TestBench::initLive(void)
{

}

void W_TestBench::initLog(void)
{

}

void W_TestBench::display(TestBenchProject *devicePtr, int index)
{
	display(devicePtr->tbList[index]->mb);
}

//Call this function to refresh the display
void W_TestBench::display(struct motortb_s *mb)
{
	//Execute 1:
	ui->dispEx1_0->setText(QString::number(mb->ex1[0]));
	ui->dispEx1_1->setText(QString::number(mb->ex1[1]));
	ui->dispEx1_2->setText(QString::number(mb->ex1[2]));
	ui->dispEx1_3->setText(QString::number(mb->ex1[3]));
	ui->dispEx1_4->setText(QString::number(mb->ex1[4]));
	ui->dispEx1_5->setText(QString::number(mb->ex1[5]));

	//Execute 2:
	ui->dispEx2_0->setText(QString::number(mb->ex2[0]));
	ui->dispEx2_1->setText(QString::number(mb->ex2[1]));
	ui->dispEx2_2->setText(QString::number(mb->ex2[2]));
	ui->dispEx2_3->setText(QString::number(mb->ex2[3]));
	ui->dispEx2_4->setText(QString::number(mb->ex2[4]));
	ui->dispEx2_5->setText(QString::number(mb->ex2[5]));

	//Manage 1:
	ui->dispMn1_0->setText(QString::number(mb->mn1[0]));
	ui->dispMn1_1->setText(QString::number(mb->mn1[1]));
	ui->dispMn1_2->setText(QString::number(mb->mn1[2]));
	ui->dispMn1_3->setText(QString::number(mb->mn1[3]));
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
