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
#include "serialdriver.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QInputDialog>
#include <QDir>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Config::W_Config(QWidget *parent, QStringList *initFavoritePort) :
	QWidget(parent),
	ui(new Ui::W_Config)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	//Init code:
	dataSourceState = None;
	initCom();
	favoritePort = initFavoritePort;

	comPortRefreshTimer = new QTimer(this);
	connect(comPortRefreshTimer,	&QTimer::timeout,
			this,					&W_Config::refreshComTimeout);
	comPortRefreshTimer->start(REFRESH_PERIOD); //ms
	refreshComList(true);	//Call now to avoid lag when a new window is opened.

	//Timer for sequential configuration, BT module:
	btConfigTimer = new QTimer(this);
	btConfigField = 0;

	//By default we link the timer to the GUI config
	connect(btConfigTimer, &QTimer::timeout,
				this, &W_Config::btConfig);
	timerConnectedTo = TCT_GUI;

	openProgressTimer = new QTimer(this);
	connect(openProgressTimer,	&QTimer::timeout,
			this,				&W_Config::progressUpdate);
}

W_Config::~W_Config()
{
	if(dataSourceState != None)
	{
		emit closeCom();
	}
	emit windowClosed();
	delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void W_Config::on_openStatusUpdate(SerialPortStatus status, int nbTries)
{
	//Connection is successful.
	if(status == WhileOpening)
	{
		progressTries = nbTries;
		progressCnt = 0;
	}
	else if(status == PortOpeningSucceed)
	{
		openProgressTimer->stop();
		ui->comProgressBar->setValue(100);

		dataSourceState = LiveCOM;
		emit updateDataSourceStatus(dataSourceState, nullptr);

		ui->openComButton->setDisabled(true);
		ui->closeComButton->setDisabled(false);
		ui->comPortComboBox->setDisabled(true);

		ui->pbLoadLogFile->setDisabled(true);

		//Enable Bluetooth button:
		ui->pbBTmode->setEnabled(true);
	}
	else if(PortOpeningFailed||
			PortClosed)
	{
		openProgressTimer->stop();
		ui->comProgressBar->setValue(0);
		dataSourceState = None;
		ui->pbLoadLogFile->setDisabled(false);
	}
	else
	{
		qDebug() << "SerialDriver Open status return not handled : ""Idle""";
	}
}

// This slot is used because the refreshComList is overloaded.
void W_Config::refreshComTimeout(void)
{
	refreshComList();
}

void W_Config::refresh(void)
{
	refreshComList(true);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Config::initCom(void)
{
	//Bluetooth:
	ui->pbBTmode->setEnabled(false);
	btDataMode = true;
	ui->pbBTmode->setText("Set mode: Cmd");
	disableBluetoothCommandButtons();

	//No manual entry, 0% progress, etc.:
	ui->comProgressBar->setValue(0);
	ui->btProgressBar->setValue(0);
	ui->btProgressBar->setDisabled(true);
	ui->openComButton->setDisabled(false);
	ui->closeComButton->setDisabled(true);
	ui->pbLoadLogFile->setDisabled(false);
	ui->pbCloseLogFile->setDisabled(true);

	ui->checkBoxFavoritePort->setChecked(false);
}

//This gets called by a timer
void W_Config::refreshComList(bool forceRefresh, bool keepCurrentSelection)
{
	static int refreshDivide = 0;
	int ComPortCounts = 0;
	QString nn;

	refreshDivide++;
	refreshDivide %= 8;
	if(!refreshDivide || forceRefresh)
	{
		//Available ports?
		QList<QSerialPortInfo> comPortInfo = QSerialPortInfo::availablePorts();
		ComPortCounts = comPortInfo.length();

		//Did it change?
		if((ComPortCounts != lastComPortCounts && dataSourceState == None)  ||
			 forceRefresh)
		{
			//Save the currentPort
			QString currentPortAll = ui->comPortComboBox->currentText();
			QString currentPort = currentPortAll.section(" ", 0, 0, \
														 QString::SectionSkipEmpty);

			qDebug() << "COM Port list changed.";

			ui->comPortComboBox->clear();

			//No port?
			if(ComPortCounts == 0)
			{
				ui->comPortComboBox->addItem(noPortString);
			}
			else
			{
				//Rewrite the list:
				for(const QSerialPortInfo &info : comPortInfo)
				{
					nn = getCOMnickname(&info);
					ui->comPortComboBox->addItem(info.portName() + " " + nn);
				}
			}

			lastComPortCounts = ComPortCounts;

			if(keepCurrentSelection == true)
			{
				int portIndex = ui->comPortComboBox->findText(currentPort);
				if(portIndex >= 0)
				{
					ui->comPortComboBox->setCurrentIndex(portIndex);
				}
			}
		}
	}

	//Handles the Favorite Ports:
	favoritePortManagement(lastComPortCounts);
}

void W_Config::favoritePortManagement(int ComPortCounts)
{
	bool isFavorite = false, cIFav = false;
	QString name = "";
	QStringList fullTxt;

	for(int i = 0; i < ComPortCounts; i++)
	{
		name = ui->comPortComboBox->itemText(i).split(" ").first();
		isFavorite = favoritePort->contains(name, Qt::CaseInsensitive);
		fullTxt = ui->comPortComboBox->itemText(i).split(" ");

		if(isFavorite)
		{
			//We add a star to the text
			ui->comPortComboBox->setItemText(i, fullTxt.at(0) + " " + fullTxt.at(1) + QString::fromUtf8(" [\u2605]"));

			//Checkbox star:
			if(ui->comPortComboBox->currentIndex() == i){cIFav = true;}
		}
		else
		{
			//Old star ready to be deleted?
			if(fullTxt.length() == 3)
			{
				ui->comPortComboBox->setItemText(i, fullTxt.at(0) + " " + fullTxt.at(1));
			}
		}
	}

	if(cIFav){ui->checkBoxFavoritePort->setChecked(true);}
	else{ui->checkBoxFavoritePort->setChecked(false);}
}

QString W_Config::getCOMnickname(const QSerialPortInfo *c)
{
	QString tmpD = c->description(), o = "[?]";
	QString tmpM = c->manufacturer();

	if(tmpD.contains("Bluetooth")){o = "[BT]";}
	if(tmpD.contains("STM")){o = "[USB-STM]";}
	if(tmpM.contains("Cypress")){o = "[USB-PSoC]";}

	return o;
}

//Main BT config: GUI
void W_Config::btConfig()
{
	//Settings - GUI:
	//---------------
	//Name = FlexSEA-ADDR
	//Baudrate = 230k
	//TX Power = max
	//Inquiry window = 0012
	//Page Window = 0012
	//CfgTimer = 15s
	//Clear any remote address
	//Slave mode
	//Optimize for throughput

	static uint8_t config[BT_FIELDS1][20] = {{"S-,FlexSEA\r"}, \
											{"SU,230K\r"}, \
											{"SY,0010\r"}, \
											{"SI,0012\r"}, \
											{"SJ,0012\r"}, \
											{"ST,15\r"}, \
											{"SR,Z\r"}, \
											{"SM,0\r"}, \
											{"SQ,0\r"}};

	static uint8_t len[BT_FIELDS1] = {11,8,8,8,8,6,5,5,5};

	if(btConfigField >= BT_FIELDS1)
	{
		btConfigTimer->stop();
		qDebug() << "Done with the BT GUI config (" << btConfigField << "/" << \
					BT_FIELDS1 << ").";
		return;
	}

	//Send:
	emit write(len[btConfigField], &config[btConfigField][0]);
	qDebug() << "Sent BT config GUI #" << btConfigField;
	btConfigField++;
	emit flush();
	btConfigTimer->start(BT_CONF_DELAY);
	ui->btProgressBar->setValue(100*btConfigField/BT_FIELDS1);
}

//Other BT config: Bilateral
void W_Config::btConfig2()
{
	//Settings - Bilateral:
	//---------------------
	//Name = FlexSEA-BWC-ADDR
	//Baudrate = 460k
	//TX Power = max
	//Inquiry window = 0012
	//Page Window = 0012
	//CfgTimer = 15s
	//Pair mode
	//Authentification
	//Buddy's address
	//Latency optimization SQ,16

	bool ok;

	static uint8_t config[BT_FIELDS2][20] = {{"S-,FlexSEA-BWC\r"}, \
											{"SU,460K\r"}, \
											{"SY,0010\r"}, \
											{"SI,0012\r"}, \
											{"SJ,0012\r"}, \
											{"ST,15\r"}, \
											{"SM,6\r"}, \
											{"SA,4\r"}, \
											{"SR,000000000000\r"},
											{"SQ,16\r"}};

	static uint8_t len[BT_FIELDS2] = {15,8,8,8,8,6,5,5,16,6};

	if(btConfigField == 0)
	{
		QString text = QInputDialog::getText(this, tr("Buddy's address"),
											 tr("Hex Address (12 alphanumerical):"), QLineEdit::Normal,
											 "000666", &ok);
		if (ok && !text.isEmpty() && text.length()==12)
		{
			qDebug() << "Received: " << text;
			for(int i = 0; i < 12; i++)
			{
				config[BT_FIELDS2-1][i+3] = (uint8_t)text.toLocal8Bit()[i];
			}

			//for(int i = 0; i < 16; i++){qDebug() << config[8][i];}
		}
		else
		{
			qDebug() << "Invalid - abort.";
			btConfigTimer->stop();
			ui->btProgressBar->setValue(0);
			return;
		}
	}

	if(btConfigField >= BT_FIELDS2)
	{
		btConfigTimer->stop();
		qDebug() << "Done with the BT BWC config (" << btConfigField << "/" << \
					BT_FIELDS2 << ").";
		return;
	}

	//Send:
	emit write(len[btConfigField], &config[btConfigField][0]);
	qDebug() << "Sent BT config BWC #" << btConfigField;
	btConfigField++;
	emit flush();
	btConfigTimer->start(BT_CONF_DELAY);
	ui->btProgressBar->setValue(100*btConfigField/BT_FIELDS2);
}

void W_Config::toggleBtDataMode(bool forceDataMode)
{
	uint8_t config[4] = {0,0,0,0};

	if(btDataMode == false || forceDataMode)
	{
		btDataMode = true;
		ui->pbBTmode->setText("Set mode: Cmd");
		config[0] = '-';
		config[1] = '-';
		config[2] = '-';
		config[3] = '\n';
		//writeCommand(4, config, 0);
		emit write(4, config);
		//We are now in Data mode:
		disableBluetoothCommandButtons();
		ui->btProgressBar->setValue(0);
		ui->btProgressBar->setEnabled(false);
	}
	else
	{
		btDataMode = false;
		ui->pbBTmode->setText("Set mode: Data");
		config[0] = '$';
		config[1] = '$';
		config[2] = '$';
		//writeCommand(3,config, 0);
		emit write(3, config);
		//We are now in CMD mode:
		enableBluetoothCommandButtons();
		ui->btProgressBar->setEnabled(true);
	}
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_Config::on_openComButton_clicked()
{
	bool success = false;
	QString nAll, n1;

	nAll = ui->comPortComboBox->currentText();
	n1 = nAll.section(" ", 0, 0, QString::SectionSkipEmpty);

	// Check if there is a port to open
	if(nAll != noPortString)
	{
		dataSourceState = LiveCOM;
		//Stop port refresh
		comPortRefreshTimer->stop();
		progressTries = 0;
		progressCnt = 0;
		//Emit signal:
		emit openCom(n1, COM_OPEN_TRIES, COM_OPEN_DELAY_US, &success);

		// Disable the log button during the port opening
		ui->pbLoadLogFile->setDisabled(true);
		openProgressTimer->start((int)((BT_DELAY_MS/COM_BAR_RES)));
	}
}

void W_Config::on_closeComButton_clicked()
{
	toggleBtDataMode(true);

	//Emit signal:
	emit closeCom();

	closingPortRoutine();
}

void W_Config::serialAboutToClose()
{
	closingPortRoutine();
}

void W_Config::closingPortRoutine(void)
{
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
	refreshComList(true);

	// Restart the auto-Refresh
	comPortRefreshTimer->start(REFRESH_PERIOD);

	//Disable Bluetooth button:
	ui->pbBTmode->setEnabled(false);
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

void W_Config::on_pbBTmode_clicked()
{
	toggleBtDataMode();
}

//Config for GUI:
void W_Config::on_pbBTgui_clicked()
{
	qDebug() << "Config GUI.";

	btConfigField = 0;
	btConfigTimer->setSingleShot(true);

	if(timerConnectedTo != TCT_GUI)
	{
		btConfigTimer->disconnect();
		connect(btConfigTimer, &QTimer::timeout,
						this, &W_Config::btConfig);
		qDebug() << "Timer connected to btConfig (GUI).";
	}

	btConfigTimer->start(50);
}

//Config for Bilateral:
void W_Config::on_pbBTbwc_clicked()
{
	qDebug() << "Config BWC.";

	btConfigField = 0;
	btConfigTimer->setSingleShot(true);

	if(timerConnectedTo != TCT_BWC)
	{
		btConfigTimer->disconnect();
		connect(btConfigTimer, &QTimer::timeout,
						this, &W_Config::btConfig2);
		qDebug() << "Timer connected to btConfig2 (BWC).";
	}

	btConfigTimer->start(50);
}

void W_Config::on_pbBTfactory_clicked()
{
	uint8_t config[6] = "SF,1\n";
	emit write(5, config);
	emit flush();
}

void W_Config::on_pbBTreset_clicked()
{
	uint8_t config[5] = "R,1\n";
	emit write(4, config);
	emit flush();
	toggleBtDataMode(true);
	emit on_closeComButton_clicked();
}

void W_Config::enableBluetoothCommandButtons(void)
{
	ui->pbBTfactory->setEnabled(true);
	ui->pbBTgui->setEnabled(true);
	ui->pbBTbwc->setEnabled(true);
	ui->pbBTreset->setEnabled(true);
	ui->pbBTfast->setEnabled(true);
}

void W_Config::disableBluetoothCommandButtons(void)
{
	ui->pbBTfactory->setEnabled(false);
	ui->pbBTgui->setEnabled(false);
	ui->pbBTbwc->setEnabled(false);
	ui->pbBTreset->setEnabled(false);
	ui->pbBTfast->setEnabled(false);
}

void W_Config::on_pbBTfast_clicked()
{
	uint8_t config[5] = "F,1\n";
	emit write(4, config);
	emit flush();

	emit on_pbBTmode_clicked();
}

void W_Config::on_checkBoxFavoritePort_clicked()
{
	QString nAll, n1;

	nAll = ui->comPortComboBox->currentText();
	n1 = nAll.section(" ", 0, 0, QString::SectionSkipEmpty);

	if(n1 != noPortString)
	{
		if(ui->checkBoxFavoritePort->isChecked())
		{
			if(!favoritePort->contains(n1, Qt::CaseInsensitive))
			{
				favoritePort->append(n1);
			}
		}
		else
		{
			if(favoritePort->contains(n1, Qt::CaseInsensitive))
			{
				favoritePort->removeOne(n1);
			}
		}
	}
}

void W_Config::progressUpdate()
{
	static int prog;
	if(progressCnt <= COM_BAR_RES)
	{
		//qDebug() << "bar update" << progressCnt << "  " << progressTries;
		prog = (((float)progressCnt / COM_BAR_RES)*(1.0/(COM_OPEN_TRIES)) + ((float)progressTries/(COM_OPEN_TRIES)))*100;
		ui->comProgressBar->setValue(prog);
		ui->comProgressBar->update();
		progressCnt++;
	}
}
