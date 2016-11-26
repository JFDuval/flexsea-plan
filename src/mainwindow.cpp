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
	[This file] mainwindow.h: Main GUI Window - connects all the modules
	together
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	* 2016-09-12 | jfduval | create() RIC/NU view
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include "main.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QMainWindow::showMaximized();

	setWindowTitle("FlexSEA Plan GUI v2.0 (Alpha Release - 11/2016)");
	ui->statusBar->showMessage("Program launched. COM: Not Connected. \
								Stream status: N/A", 0);
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	//Prepare FlexSEA Stack:
	init_flexsea_payload_ptr();

	W_UserRW::setMaxWindow(USERRW_WINDOWS_MAX);

	//SerialDriver:
	mySerialDriver = new SerialDriver;

	//Datalogger:
	myDataLogger = new DataLogger;

	//Create default objects:
	createConfig();
	createSlaveComm();

	//Disable options that are not implemented:
	ui->menuFile->actions().at(3)->setEnabled(false);		//Load configuration
	ui->menuFile->actions().at(4)->setEnabled(false);		//Save configuration
	ui->menuControl->actions().at(1)->setEnabled(false);	//In Control


	//Log and MainWindow
	connect(myDataLogger, SIGNAL(setStatusBarMessage(QString)), \
			this, SLOT(setStatusBar(QString)));

	//SerialDriver and MainWindow
	connect(mySerialDriver, SIGNAL(setStatusBarMessage(QString)), \
			this, SLOT(setStatusBar(QString)));
}

MainWindow::~MainWindow()
{
	delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

//Transfer the signal from config to the
void MainWindow::translatorUpdateDataSourceStatus(DataSource status)
{
	if(status == LogFile)
	{
		emit connectorUpdateDisplayMode(DisplayLogData);
	}
	else
	{
		emit connectorUpdateDisplayMode(DisplayLiveData);
	}

}

void MainWindow::manageLogKeyPad(DataSource status)
{

	if(status == LogFile)
	{
		createLogKeyPad();
	}
	else
	{
		if(W_LogKeyPad::howManyInstance() > 0)
		{
				myViewLogKeyPad[0]->parentWidget()->close();
		}
	}
}


//Creates a new View Execute window
void MainWindow::createViewExecute(void)
{
	QString msg = "";
	int objectCount = W_Execute::howManyInstance();

	//Limited number of windows:
	if(objectCount < EX_VIEW_WINDOWS_MAX)
	{
		DisplayMode status = DisplayLiveData;
		if(W_Config::howManyInstance() > 0)
		{
			if(myViewConfig[0]->getDataSourceStatus() == LogFile)
			{
				status = DisplayLogData;
			}
		}

		myViewExecute[objectCount] = \
				new W_Execute(this, myDataLogger->getLogPtr(), status);
		ui->mdiArea->addSubWindow(myViewExecute[objectCount]);
		myViewExecute[objectCount]->show();

		msg = "Created 'Execute View' object index " + \
				QString::number(objectCount) + " (max index = " \
				+ QString::number(EX_VIEW_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Execute:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewExecute[objectCount], SLOT(refresh()));

		//Link to MainWindow for the close signal:
		connect(myViewExecute[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewExecute()));

		// Link to the slider of 2DPlot. Intermediate signal (connector) to
		// allow opening of window asynchroniously
		connect(this, SIGNAL(connectorRefreshLogTimeSlider(int)), \
				myViewExecute[objectCount], SLOT(displayLogData(int)));
		connect(this, SIGNAL(connectorUpdateDisplayMode(DisplayMode)), \
				myViewExecute[objectCount], SLOT(updateDisplayMode(DisplayMode)));
	}

	else
	{
		msg = "Maximum number of Execute View objects reached (" \
				+ QString::number(EX_VIEW_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeViewExecute(void)
{
	QString msg = "View Execute window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View Manage window
void MainWindow::createViewManage(void)
{
	QString msg = "";
	int objectCount = W_Manage::howManyInstance();

	//Limited number of windows:
	if(objectCount < (MN_VIEW_WINDOWS_MAX))
	{
		//WinViewExecute *myViewEx = new WinViewExecute(ui->mdiArea);
		myViewManage[objectCount] = new W_Manage(this);
		ui->mdiArea->addSubWindow(myViewManage[objectCount]);
		myViewManage[objectCount]->show();

		msg = "Created 'Manage View' object index " + \
				QString::number(objectCount) + " (max index = " \
				+ QString::number(MN_VIEW_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Manage:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewManage[objectCount], SLOT(refreshDisplayManage()));

		//Link to MainWindow for the close signal:
		connect(myViewManage[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewManage()));
	}

	else
	{
		msg = "Maximum number of Manage View objects reached (" \
				+ QString::number(MN_VIEW_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeViewManage(void)
{
	QString msg = "View Manage window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Config window
void MainWindow::createConfig(void)
{
	QString msg = "";
	int objectCount = W_Config::howManyInstance();

	//Limited number of windows:
	if(objectCount < (CONFIG_WINDOWS_MAX))
	{
		myViewConfig[objectCount] = new W_Config(this);
		ui->mdiArea->addSubWindow(myViewConfig[objectCount]);
		myViewConfig[objectCount]->show();

		msg = "Created 'Config' object index " + QString::number(objectCount) \
				+ " (max index = " + QString::number(CONFIG_WINDOWS_MAX-1) + ").";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewConfig[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeConfig()));

		//Link to DataLogger
		connect(myViewConfig[0], SIGNAL(openReadingFile(bool *)), \
				myDataLogger, SLOT(openReadingFile(bool *)));
		connect(myViewConfig[0], SIGNAL(closeReadingFile()), \
				myDataLogger, SLOT(closeReadingFile()));

		// Link to SerialDriver
		connect(myViewConfig[0], SIGNAL(openCom(QString,int,int)), \
				mySerialDriver, SLOT(open(QString,int,int)));
		connect(myViewConfig[0], SIGNAL(closeCom()), \
				mySerialDriver, SLOT(close()));
		connect(mySerialDriver, SIGNAL(openProgress(int,int)), \
				myViewConfig[0], SLOT(setComProgress(int,int)));
		connect(myViewConfig[0], SIGNAL(updateDataSourceStatus(DataSource)),
				this, SLOT(translatorUpdateDataSourceStatus(DataSource)));
		connect(myViewConfig[0], SIGNAL(updateDataSourceStatus(DataSource)),
				this, SLOT(manageLogKeyPad(DataSource)));
	}

	else
	{
		msg = "Maximum number of Config objects reached (" \
				+ QString::number(CONFIG_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeConfig(void)
{
	QString msg = "Config window closed.";

	if(W_LogKeyPad::howManyInstance() > 0)
	{
		myViewLogKeyPad[0]->parentWidget()->close();
	}

	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Control Control window
void MainWindow::createControlControl(void)
{
	QString msg = "";
	int objectCount = W_Control::howManyInstance();

	//Limited number of windows:
	if(objectCount < (CONTROL_WINDOWS_MAX))
	{
		myViewControl[objectCount] = new W_Control(this);
		ui->mdiArea->addSubWindow(myViewControl[objectCount]);
		myViewControl[objectCount]->show();

		msg = "Created 'Control' object index " + QString::number(objectCount) \
				+ " (max index = " + QString::number(CONTROL_WINDOWS_MAX-1) + ").";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewControl[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeControlControl()));

		//Link to SlaveComm to send commands:
		connect(myViewControl[objectCount], SIGNAL(writeCommand(char,unsigned char*)), \
				this, SIGNAL(connectorWriteCommand(char,unsigned char*)));
	}

	else
	{
		msg = "Maximum number of Control objects reached (" \
				+ QString::number(CONTROL_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeControlControl(void)
{
	QString msg = "Control window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View 2DPlot window
void MainWindow::createView2DPlot(void)
{
	QString msg = "";
	int objectCount = W_2DPlot::howManyInstance();

	//Limited number of windows:
	if(objectCount < (PLOT2D_WINDOWS_MAX))
	{
		myView2DPlot[objectCount] = new W_2DPlot(this);
		ui->mdiArea->addSubWindow(myView2DPlot[objectCount]);
		myView2DPlot[objectCount]->show();

		msg = "Created '2DPlot' object index " + QString::number(objectCount) \
				+ " (max index = " + QString::number(PLOT2D_WINDOWS_MAX-1) + ").";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and 2DPlot:
		/*connect(mySerialDriver, SIGNAL(newDataReady()), \
				myView2DPlot[objectCount], SLOT(refresh2DPlot())); */

		//New version: updates at fixed rate, not based on serial reply:
		connect(myViewSlaveComm[0], SIGNAL(refresh2DPlot()), \
				myView2DPlot[objectCount], SLOT(refresh2DPlot()));

		//For the trapeze/control tool:
		connect(myViewSlaveComm[0], SIGNAL(masterTimer100Hz()), \
				myView2DPlot[objectCount], SLOT(refreshControl()));

		//Link to MainWindow for the close signal:
		connect(myView2DPlot[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeView2DPlot()));
	}

	else
	{
		msg = "Maximum number of 2DPlot objects reached (" \
				+ QString::number(PLOT2D_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeView2DPlot(void)
{
	QString msg = "2D Plot window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Slave Comm window
void MainWindow::createSlaveComm(void)
{
	QString msg = "";
	int objectCount = W_SlaveComm::howManyInstance();

	//Limited number of windows:
	if(objectCount < (SLAVECOMM_WINDOWS_MAX))
	{
		//WinViewExecute *myViewEx = new WinViewExecute(ui->mdiArea);
		myViewSlaveComm[objectCount] = new W_SlaveComm(this);
		ui->mdiArea->addSubWindow(myViewSlaveComm[objectCount]);
		myViewSlaveComm[objectCount]->show();

		msg = "Created 'Slave Comm' object index " + QString::number(objectCount) \
				+ " (max index = " + QString::number(SLAVECOMM_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewSlaveComm[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeSlaveComm()));

		//Link SlaveComm and SerialDriver:
		connect(mySerialDriver, SIGNAL(openStatus(bool)), \
				myViewSlaveComm[0], SLOT(receiveComPortStatus(bool)));
		connect(myViewSlaveComm[0], SIGNAL(slaveReadWrite(uint, uint8_t*, uint8_t)), \
				mySerialDriver, SLOT(readWrite(uint, uint8_t*, uint8_t)));
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewSlaveComm[0], SLOT(receiveNewDataReady()));
		connect(mySerialDriver, SIGNAL(dataStatus(int, int)), \
				myViewSlaveComm[0], SLOT(displayDataReceived(int, int)));
		connect(mySerialDriver, SIGNAL(newDataTimeout(bool)), \
				myViewSlaveComm[0], SLOT(updateIndicatorTimeout(bool)));

		//Link SlaveComm and DataLogger
		connect(myViewSlaveComm[0], SIGNAL(openRecordingFile(uint8_t,QString)), \
				myDataLogger, SLOT(openRecordingFile(uint8_t,QString)));
		connect(myViewSlaveComm[0], SIGNAL(writeToLogFile(uint8_t,uint8_t
														  ,uint8_t,uint16_t)), \
				myDataLogger, SLOT(writeToFile(uint8_t,uint8_t,uint8_t,uint16_t)));
		connect(myViewSlaveComm[0], SIGNAL(closeRecordingFile(uint8_t)), \
				myDataLogger, SLOT(closeRecordingFile(uint8_t)));

		//Link SlaveComm and Control Trought connector
		connect(this, SIGNAL(connectorWriteCommand(char,unsigned char*)), \
				myViewSlaveComm[0], SLOT(externalSlaveWrite(char,unsigned char*)));
	}

	else
	{
		msg = "Maximum number of Slave Comm objects reached (" \
				+ QString::number(SLAVECOMM_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeSlaveComm(void)
{
	QString msg = "Slave Comm window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Any Command window
void MainWindow::createAnyCommand(void)
{
	QString msg = "";
	int objectCount = W_AnyCommand::howManyInstance();

	//Limited number of windows:
	if(objectCount < (ANYCOMMAND_WINDOWS_MAX))
	{
		myViewAnyCommand[objectCount] = new W_AnyCommand(this);
		ui->mdiArea->addSubWindow(myViewAnyCommand[objectCount]);
		myViewAnyCommand[objectCount]->show();

		msg = "Created 'AnyCommand' object index " + QString::number(objectCount) \
				+ " (max index = " + QString::number(ANYCOMMAND_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewAnyCommand[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeAnyCommand()));
	}

	else
	{
		msg = "Maximum number of AnyCommand objects reached (" \
				+ QString::number(ANYCOMMAND_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeAnyCommand(void)
{
	QString msg = "Any Command window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View RIC/NU window
void MainWindow::createViewRicnu(void)
{
	QString msg = "";
	int objectCount = W_Ricnu::howManyInstance();

	//Limited number of windows:
	if(objectCount < (RICNU_VIEW_WINDOWS_MAX))
	{
		//WinViewExecute *myViewEx = new WinViewExecute(ui->mdiArea);
		myViewRicnu[objectCount] = new W_Ricnu(this);
		ui->mdiArea->addSubWindow(myViewRicnu[objectCount]);
		myViewRicnu[objectCount]->show();

		msg = "Created 'RIC/NU View' object index " + \
				QString::number(objectCount) + " (max index = " \
				+ QString::number(RICNU_VIEW_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and RIC/NU:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewRicnu[objectCount], SLOT(refreshDisplayRicnu()));

		//Link to MainWindow for the close signal:
		connect(myViewRicnu[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewRicnu()));
	}

	else
	{
		msg = "Maximum number of RIC/NU View objects reached (" \
				+ QString::number(RICNU_VIEW_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeViewRicnu(void)
{
	QString msg = "View RIC/NU window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Converter window
void MainWindow::createConverter(void)
{
	QString msg = "";
	int objectCount = W_Converter::howManyInstance();

	//Limited number of windows:
	if(objectCount < (CONVERTER_WINDOWS_MAX))
	{
		my_w_converter[objectCount] = new W_Converter(this);
		ui->mdiArea->addSubWindow(my_w_converter[objectCount]);
		my_w_converter[objectCount]->show();

		msg = "Created 'Converter' object index " + QString::number(objectCount) \
				+ " (max index = " + QString::number(CONVERTER_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(my_w_converter[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeConverter()));
	}

	else
	{
		msg = "Maximum number of Converter objects reached (" \
				+ QString::number(CONVERTER_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeConverter(void)
{
	QString msg = "Converter window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Calibration window
void MainWindow::createCalib(void)
{
	QString msg = "";
	int objectCount = W_Calibration::howManyInstance();

	//Limited number of windows:
	if(objectCount < (CALIB_WINDOWS_MAX))
	{
		myViewCalibration[objectCount] = new W_Calibration(this);
		ui->mdiArea->addSubWindow(myViewCalibration[objectCount]);
		myViewCalibration[objectCount]->show();

		msg = "Created 'Calibration' object index " + QString::number(objectCount) \
				+ " (max index = " + QString::number(CALIB_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewCalibration[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeCalib()));
	}
	else
	{
		msg = "Maximum number of Calibration objects reached (" \
				+ QString::number(CALIB_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeCalib(void)
{
	QString msg = "Calibration window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new User R/W window
void MainWindow::createUserRW(void)
{
	QString msg = "";
	int objectCount = W_UserRW::howManyInstance();

	//Limited number of windows:
	if(objectCount < W_UserRW::getMaxWindow())
	{
		userRW.myWindow[objectCount] = new W_UserRW(this);
		ui->mdiArea->addSubWindow(userRW.myWindow[objectCount]);
		userRW.myWindow[objectCount]->show();

		msg = "Created '" + W_UserRW::desc + "' object index " +
				QString::number(objectCount) + " (max index = " +
				QString::number(W_UserRW::getMaxWindow()-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(userRW.myWindow[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeUserRW()));
	}

	else
	{
		msg = "Maximum number of '" + W_UserRW::desc + "'objects reached (" \
				+ QString::number(W_UserRW::getMaxWindow()) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeUserRW(void)
{
	QString msg = W_UserRW::desc + " window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View Gossip window
void MainWindow::createViewGossip(void)
{
	QString msg = "";
	int objectCount = W_Gossip::howManyInstance();

	//Limited number of windows:
	if(objectCount < (GOSSIP_WINDOWS_MAX))
	{
		myViewGossip[objectCount] = new W_Gossip(this);
		ui->mdiArea->addSubWindow(myViewGossip[objectCount]);
		myViewGossip[objectCount]->show();

		msg = "Created 'Gossip View' object index " + \
				QString::number(objectCount) + " (max index = " \
				+ QString::number(GOSSIP_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Gossip:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewGossip[objectCount], SLOT(refreshDisplayGossip()));

		//Link to MainWindow for the close signal:
		connect(myViewGossip[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewGossip()));
	}

	else
	{
		msg = "Maximum number of Gossip View objects reached (" \
				+ QString::number(GOSSIP_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeViewGossip(void)
{
	QString msg = "View Gossip window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View Strain window
void MainWindow::createViewStrain(void)
{
	QString msg = "";
	int objectCount = W_Strain::howManyInstance();

	//Limited number of windows:
	if(objectCount < (STRAIN_WINDOWS_MAX))
	{
		myViewStrain[objectCount] = new W_Strain(this);
		ui->mdiArea->addSubWindow(myViewStrain[objectCount]);
		myViewStrain[objectCount]->show();

		msg = "Created 'Strain View' object index " + \
				QString::number(objectCount) + " (max index = " \
				+ QString::number(STRAIN_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Strain:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewStrain[objectCount], SLOT(refreshDisplayStrain()));

		//Link to MainWindow for the close signal:
		connect(myViewStrain[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewStrain()));
	}

	else
	{
		msg = "Maximum number of Strain View objects reached (" \
				+ QString::number(STRAIN_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeViewStrain(void)
{
	QString msg = "View Strain window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View Battery window
void MainWindow::createViewBattery(void)
{
	QString msg = "";
	int objectCount = W_Battery::howManyInstance();

	//Limited number of windows:
	if(objectCount < (BATT_WINDOWS_MAX))
	{
		myViewBatt[objectCount] = new W_Battery(this);
		ui->mdiArea->addSubWindow(myViewBatt[objectCount]);
		myViewBatt[objectCount]->show();

		msg = "Created 'Battery View' object index " + \
				QString::number(objectCount) + " (max index = " \
				+ QString::number(BATT_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Battery:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewBatt[objectCount], SLOT(refreshDisplayBattery()));

		//Link to MainWindow for the close signal:
		connect(myViewBatt[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewBattery()));
	}

	else
	{
		msg = "Maximum number of Battery View objects reached (" \
				+ QString::number(BATT_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeViewBattery(void)
{
	QString msg = "View Battery window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new LogKeyPad
void MainWindow::createLogKeyPad(void)
{
	QString msg = "";
	int objectCount = W_LogKeyPad::howManyInstance();

	//Limited number of windows:
	if(objectCount < (LOGKEYPAD_WINDOWS_MAX))
	{
		myViewLogKeyPad[objectCount] = new W_LogKeyPad(this, myDataLogger->getLogPtr());
		ui->mdiArea->addSubWindow(myViewLogKeyPad[objectCount]);
		myViewLogKeyPad[objectCount]->show();
		myViewLogKeyPad[objectCount]->parentWidget()->setWindowFlags(
					Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

		msg = "Created 'LogKeyPad View' object index " + \
				QString::number(objectCount) + " (max index = " \
				+ QString::number(LOGKEYPAD_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		// Link for the data slider
		connect(myViewLogKeyPad[objectCount], SIGNAL(logTimeSliderValueChanged(int)), \
				this, SIGNAL(connectorRefreshLogTimeSlider(int)));

		//Link to MainWindow for the close signal:
		connect(myViewLogKeyPad[objectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeLogKeyPad()));
	}

	else
	{
		msg = "Maximum number of LogKeyPad View objects reached (" \
				+ QString::number(LOGKEYPAD_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeLogKeyPad(void)
{
	QString msg = "View LogKeyPad window closed.";
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

void MainWindow::displayAbout()
{
	QMessageBox::about(this, tr("About FlexSEA"), \
	tr("<center><u>FlexSEA: <b>Flex</b>ible, <b>S</b>calable <b>E</b>lectronics\
	 <b>A</b>rchitecture.</u><br><br>Project originaly developped at the \
	<a href='http://biomech.media.mit.edu/'>MIT Media Lab Biomechatronics \
	group</a>, now supported by <a href='http://dephy.com/'>Dephy, Inc.</a>\
	<br><br><b>Copyright &copy; Dephy, Inc. 2016</b><br><br>Software released \
	under the GNU GPL-3.0 license</center>"));
}

void MainWindow::displayLicense()
{
	QMessageBox::information(this, tr("Software License Information"), \
	tr("<center><b>Copyright &copy; Dephy, Inc. 2016</b>\
		<br><br>This program is free software: you can redistribute it and/or modify \
		it under the terms of the GNU General Public License as published by \
		the Free Software Foundation, either version 3 of the License, or \
		(at your option) any later version. <br><br>\
		This program is distributed in the hope that it will be useful,\
		but WITHOUT ANY WARRANTY; without even the implied warranty of \
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
		GNU General Public License for more details. <br><br>\
		You should have received a copy of the GNU General Public License \
		along with this program.  If not, see \
		<a href='http://www.gnu.org/licenses/'>\
		http://www.gnu.org/licenses/</a>.</center>"));
}

void MainWindow::displayDocumentation()
{
	QMessageBox::information(this, tr("Documentation"), \
	tr("<center>Documentation available online: \
		<a href='http://flexsea.media.mit.edu/'>\
		FlexSEA Documentation</a></center>"));
}

void MainWindow::setStatusBar(QString msg)
{
	ui->statusBar->showMessage(msg);
}
