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
	[This file] w_config.h: Configuration Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_config.h"
#include "ui_w_config.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Config::W_Config(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_Config)
{

	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	//Init code:
	dataSourceState = None;
	initCom();

	comPortRefreshTimer = new QTimer(this);
	connect(comPortRefreshTimer, SIGNAL(timeout()), this, SLOT(getComList()));
	comPortRefreshTimer->start(REFRESH_PERIOD); //ms
	getComList();	//Call now to avoid lag when a new window is opened.
}

W_Config::~W_Config()
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

void W_Config::setComProgress(int val)
{
	ui->comProgressBar->setValue(val);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Config::initCom(void)
{
	//Bluetooth disabled for now:
	ui->pushButtonBTCon->setEnabled(false);

	//No manual entry, 0% progress, etc.:
	ui->comProgressBar->setValue(0);
	ui->openComButton->setDisabled(false);
	ui->closeComButton->setDisabled(true);
	ui->pbLoadLogFile->setDisabled(false);
	ui->pbCloseLogFile->setDisabled(true);
}

//This gets called by a timer (currently every 750ms)
/*Note: the list is always ordered by port number. If you connect to COM2 and
 * then plug COM1, it will display COM1. That's confusing for the users.*/
void W_Config::getComList(void)
{
	static int lastComPortCounts = 0;
	int ComPortCounts = 0;

	//Available ports?
	QList<QSerialPortInfo> comPortInfo = QSerialPortInfo::availablePorts();
	ComPortCounts = comPortInfo.length();

	//Did it change?
	if(ComPortCounts != lastComPortCounts)
	{
		//Yes.
		qDebug() << "COM Port list changed.";

		ui->comPortComboBox->clear();

		//No port?
		if(ComPortCounts == 0)
		{
			//Empty, add the No Port option
			ui->comPortComboBox->addItem("No Port");
		}
		else
		{
			//Rewrite the list:
			for(const QSerialPortInfo &info : comPortInfo)
			{
				ui->comPortComboBox->addItem(info.portName());
			}
		}
	}
	lastComPortCounts = ComPortCounts;
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_Config::on_openComButton_clicked()
{
	bool success = false;

	//Stop port refresh
	comPortRefreshTimer->stop();
	//Emit signal:
	emit openCom(ui->comPortComboBox->currentText(), 25, 100000, &success);

	//Connection is successful.
	if(success)
	{
		dataSourceState = LiveCOM;
		emit updateDataSourceStatus(dataSourceState, nullptr);

		ui->openComButton->setDisabled(true);
		ui->closeComButton->setDisabled(false);
		ui->comPortComboBox->setDisabled(true);

		ui->pbLoadLogFile->setDisabled(true);
		//ui->pushButtonBTCon->setDisabled(true);
	}
	else
	{
		comPortRefreshTimer->start(REFRESH_PERIOD);
	}
}

void W_Config::on_closeComButton_clicked()
{
	//Emit signal:
	emit closeCom();

	//Enable Open COM button:
	ui->openComButton->setDisabled(false);
	ui->closeComButton->setDisabled(true);
	ui->comProgressBar->setValue(0);
	ui->comPortComboBox->setDisabled(false);

	ui->pbLoadLogFile->setDisabled(false);
	//ui->pushButtonBTCon->setDisabled(false);

	dataSourceState = None;
	emit updateDataSourceStatus(dataSourceState, nullptr);

	// Avoid refresh lag
	getComList();
	// Restart the auto-Refresh
	comPortRefreshTimer->start(REFRESH_PERIOD);
}

void W_Config::on_pbLoadLogFile_clicked()
{
	bool isOpen;
	FlexseaDevice *devPtr;
	emit openReadingFile(&isOpen, &devPtr);

	if(isOpen)
	{
		ui->pbLoadLogFile->setDisabled(true);
		ui->pbCloseLogFile->setDisabled(false);
		ui->openComButton->setDisabled(true);
		//ui->pushButtonBTCon->setDisabled(true);
		dataSourceState = FromLogFile;
		emit updateDataSourceStatus(dataSourceState, devPtr);
		emit createLogKeypad(dataSourceState, devPtr);
	}
}

void W_Config::on_pbCloseLogFile_clicked()
{
	emit closeReadingFile();
	ui->pbLoadLogFile->setDisabled(false);
	ui->pbCloseLogFile->setDisabled(true);
	ui->openComButton->setDisabled(false);
	//ui->pushButtonBTCon->setDisabled(false);
	dataSourceState = None;
	emit updateDataSourceStatus(dataSourceState, nullptr);
}
