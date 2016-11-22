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

	setWindowTitle("Configuration");
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	//Init code:
	flagManualEntry = 0;
	dataSourceState = None;
	initCom();

	QTimer *comPortRefreshTimer = new QTimer(this);
	connect(comPortRefreshTimer, SIGNAL(timeout()), this, SLOT(getComList()));
	comPortRefreshTimer->start(750); //ms
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

void W_Config::setComProgress(int val, int rst)
{
	ui->comProgressBar->setValue(val);
	if(rst)
	{
		defaultComOffUi();
	}
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Config::initCom(void)
{
	//Flags:
	flagComInitDone = 0;

	//Bluetooth disabled for now:
	ui->pushButtonBTCon->setEnabled(false);

	//No manual entry, 0% progress, etc.:
	ui->comProgressBar->setValue(0);
	ui->comProgressBar->setDisabled(true);
	ui->openComButton->setDisabled(false);
	ui->closeComButton->setDisabled(true);
	ui->pbLoadLogFile->setDisabled(false);
	ui->pbCloseLogFile->setDisabled(true);

	//Flag for other functions:
	flagComInitDone = 1;
}

//This gets called by a timer (currently every 750ms)
/*Note: the list is always ordered by port number. If you connect to COM2 and
 * then plug COM1, it will display COM1. That's confusing for the users.*/
void W_Config::getComList(void)
{
	static int lastComPortAvailable = 0;
	int comPortAvailable = 0;

	//Available ports?
	QList<QSerialPortInfo> comPortInfo = QSerialPortInfo::availablePorts();
	comPortAvailable = comPortInfo.length();
	//qDebug() << "Now: " << comPortAvailable << "Last: " << lastComPortAvailable;

	//No port?
	if(comPortAvailable == 0)
	{
		//Is the list empty?
		if(comPortList.length() == 0)
		{
			//Empty, add the No Port option
			comPortList << "No Port";
			ui->comPortComboBox->addItem(comPortList.last());
		}
	}

	//Did it change?
	if(comPortAvailable != lastComPortAvailable)
	{
		//Yes.
		qDebug() << "COM Port list changed.";

		//Clear list:
		comPortList.clear();
		ui->comPortComboBox->clear();

		//Write new one:
		for(const QSerialPortInfo &info : comPortInfo)
		{
			//qDebug() << info.portName();
			comPortList << info.portName();
			ui->comPortComboBox->addItem(comPortList.last());
		}
	}

	lastComPortAvailable = comPortAvailable;
}

void W_Config::defaultComOffUi(void)
{
	ui->openComButton->setDisabled(false);
	ui->comProgressBar->setDisabled(false);
	ui->comProgressBar->setValue(0);
	ui->closeComButton->setDisabled(true);
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_Config::on_openComButton_clicked()
{
	//Deal with display elements:
	defaultComOffUi();
	ui->openComButton->setDisabled(true);

	//Emit signal:
	emit openCom(ui->comPortComboBox->currentText(), 25, 100000);

	// TODO We Should have a way to know if the connection was successful
	if(1)//Connection is successful.
	{
		dataSourceState = LiveCOM;
		emit updateDataSourceStatus(dataSourceState);
		ui->pbLoadLogFile->setDisabled(true);
		//ui->pushButtonBTCon->setDisabled(true);
		ui->closeComButton->setDisabled(false);
		ui->comPortComboBox->setDisabled(true);
	}
}

void W_Config::on_closeComButton_clicked()
{
	//Emit signal:
	emit closeCom();

	//Enable Open COM button:
	ui->openComButton->setEnabled(true);
	ui->openComButton->repaint();

	//Disable Close COM button:
	ui->closeComButton->setDisabled(true);
	ui->closeComButton->repaint();

	//ui->comStatusTxt->setText("COM Port closed.");
	ui->comProgressBar->setValue(0);
	ui->comProgressBar->setDisabled(true);

	ui->pbLoadLogFile->setDisabled(false);
	//ui->pushButtonBTCon->setDisabled(false);

	ui->comPortComboBox->setDisabled(false);

	dataSourceState = None;
	emit updateDataSourceStatus(dataSourceState);

}

void W_Config::on_pbLoadLogFile_clicked()
{
	emit openReadingFile();
	ui->pbLoadLogFile->setDisabled(true);
	ui->pbCloseLogFile->setDisabled(false);
	ui->openComButton->setDisabled(true);
	//ui->pushButtonBTCon->setDisabled(true);
	dataSourceState = LogFile;
	emit updateDataSourceStatus(dataSourceState);
}

void W_Config::on_pbCloseLogFile_clicked()
{
	emit closeReadingFile();
	ui->pbLoadLogFile->setDisabled(false);
	ui->pbCloseLogFile->setDisabled(true);
	ui->openComButton->setDisabled(false);
	//ui->pushButtonBTCon->setDisabled(false);
	dataSourceState = None;
	emit updateDataSourceStatus(dataSourceState);
}
