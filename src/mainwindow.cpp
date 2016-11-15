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

	//Objects count & limits
	exViewObjectCount = 0;
	configObjectCount = 0;
	slaveCommObjectCount = 0;
	anyCommandObjectCount = 0;
	converterObjectCount = 0;
	controlObjectCount = 0;
	plot2DObjectCount = 0;
	ricnuViewObjectCount = 0;
	mnViewObjectCount = 0;
	calibObjectCount = 0;
	gossipObjectCount = 0;
	battObjectCount = 0;
	strainObjectCount = 0;

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


//Creates a new View Execute window
void MainWindow::createViewExecute(void)
{
	QString msg = "";

	//Limited number of windows:
	if(exViewObjectCount < (EX_VIEW_WINDOWS_MAX))
	{
		DisplayMode status = DisplayLiveData;
		if(configObjectCount > 0)
		{
			if(myViewConfig[0]->getDataSourceStatus() == LogFile)
			{
				status = DisplayLogData;
			}
		}

		myViewExecute[exViewObjectCount] = new W_Execute(this, myDataLogger->getExecuteLogPtr(), status);
		ui->mdiArea->addSubWindow(myViewExecute[exViewObjectCount]);
		myViewExecute[exViewObjectCount]->show();

		msg = "Created 'Execute View' object index " + \
				QString::number(exViewObjectCount) + " (max index = " \
				+ QString::number(EX_VIEW_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Execute:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewExecute[exViewObjectCount], SLOT(refresh()));

		//Link to MainWindow for the close signal:
		connect(myViewExecute[exViewObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewExecute()));

		// Link to the slider of 2dplot. Intermediate signal (connector) to
		// allow opening of window asynchroniously
		connect(this, SIGNAL(connectorRefreshDataSlider(int)), \
				myViewExecute[exViewObjectCount], SLOT(displayLogData(int)));
		connect(this, SIGNAL(connectorUpdateDisplayMode(DisplayMode)), \
				myViewExecute[exViewObjectCount], SLOT(updateDisplayMode(DisplayMode)));


		exViewObjectCount++;
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

	if(exViewObjectCount > 0)
	{
		exViewObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View Manage window
void MainWindow::createViewManage(void)
{
	QString msg = "";

	//Limited number of windows:
	if(mnViewObjectCount < (MN_VIEW_WINDOWS_MAX))
	{
		//WinViewExecute *myViewEx = new WinViewExecute(ui->mdiArea);
		myViewManage[mnViewObjectCount] = new W_Manage(this);
		ui->mdiArea->addSubWindow(myViewManage[mnViewObjectCount]);
		myViewManage[mnViewObjectCount]->show();

		msg = "Created 'Manage View' object index " + \
				QString::number(mnViewObjectCount) + " (max index = " \
				+ QString::number(MN_VIEW_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Manage:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewManage[mnViewObjectCount], SLOT(refreshDisplayManage()));

		//Link to MainWindow for the close signal:
		connect(myViewManage[mnViewObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewManage()));

		mnViewObjectCount++;
	}
	else
	{
		msg = "Maximum number of Execute View objects reached (" \
				+ QString::number(EX_VIEW_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeViewManage(void)
{
	QString msg = "View Manage window closed.";

	if(mnViewObjectCount > 0)
	{
		mnViewObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Config window
void MainWindow::createConfig(void)
{
	QString msg = "";

	//Limited number of windows:
	if(configObjectCount < (CONFIG_WINDOWS_MAX))
	{

		myViewConfig[configObjectCount] = new W_Config(this);
		ui->mdiArea->addSubWindow(myViewConfig[configObjectCount]);
		myViewConfig[configObjectCount]->show();

		msg = "Created 'Config' object index " + QString::number(configObjectCount) \
				+ " (max index = " + QString::number(CONFIG_WINDOWS_MAX-1) + ").";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewConfig[configObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeConfig()));

		//Link to DataLogger
		connect(myViewConfig[0], SIGNAL(openRecordingFile(uint8_t)), \
				myDataLogger, SLOT(openRecordingFile(uint8_t)));
		connect(myViewConfig[0], SIGNAL(openReadingFile()), \
				myDataLogger, SLOT(openReadingFile()));
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

		configObjectCount++;
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

	if(configObjectCount > 0)
	{
		configObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Control Control window
void MainWindow::createControlControl(void)
{
	QString msg = "";

	//Limited number of windows:
	if(controlObjectCount < (CONTROL_WINDOWS_MAX))
	{
		myViewControl[controlObjectCount] = new W_Control(this);
		ui->mdiArea->addSubWindow(myViewControl[controlObjectCount]);
		myViewControl[controlObjectCount]->show();

		msg = "Created 'Control' object index " + QString::number(controlObjectCount) \
				+ " (max index = " + QString::number(CONTROL_WINDOWS_MAX-1) + ").";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewControl[controlObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeControlControl()));

		//Link to SlaveComm to send commands:
		connect(myViewControl[controlObjectCount], SIGNAL(writeCommand(char,unsigned char*)), \
				this, SIGNAL(connectorWriteCommand(char,unsigned char*)));

		controlObjectCount++;
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

	if(controlObjectCount > 0)
	{
		controlObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View 2DPlot window
void MainWindow::createView2DPlot(void)
{
	QString msg = "";

	//Limited number of windows:
	if(plot2DObjectCount < (PLOT2D_WINDOWS_MAX))
	{
		myView2DPlot[plot2DObjectCount] = new W_2DPlot(this);
		ui->mdiArea->addSubWindow(myView2DPlot[plot2DObjectCount]);
		myView2DPlot[plot2DObjectCount]->show();

		msg = "Created '2DPlot' object index " + QString::number(plot2DObjectCount) \
				+ " (max index = " + QString::number(PLOT2D_WINDOWS_MAX-1) + ").";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and 2DPlot:
		/*connect(mySerialDriver, SIGNAL(newDataReady()), \
				myView2DPlot[plot2DObjectCount], SLOT(refresh2DPlot())); */

		//New version: updates a fixed rate, not based on serial reply:
		connect(myViewSlaveComm[0], SIGNAL(refresh2DPlot()), \
				myView2DPlot[plot2DObjectCount], SLOT(refresh2DPlot()));

		//Link to MainWindow for the close signal:
		connect(myView2DPlot[plot2DObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeView2DPlot()));
		// TODO ok for one 2dplot, but when a second 2d plot will open, it wont works or be meaningfull.
		connect(myView2DPlot[plot2DObjectCount], SIGNAL(dataSliderValueChanged(int)), \
				this, SIGNAL(connectorRefreshDataSlider(int)));

		plot2DObjectCount++;
	}
	else
	{
		msg = "Maximum number of 2DPlot objects reached (" \
				+ QString::number(CONTROL_WINDOWS_MAX) + ")";
		qDebug() << msg;
		ui->statusBar->showMessage(msg);
	}
}

void MainWindow::closeView2DPlot(void)
{
	QString msg = "2D Plot window closed.";

	if(plot2DObjectCount > 0)
	{
		plot2DObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Slave Comm window
void MainWindow::createSlaveComm(void)
{
	QString msg = "";

	//Limited number of windows:
	if(slaveCommObjectCount < (SLAVECOMM_WINDOWS_MAX))
	{
		//WinViewExecute *myViewEx = new WinViewExecute(ui->mdiArea);
		myViewSlaveComm[slaveCommObjectCount] = new W_SlaveComm(this);
		ui->mdiArea->addSubWindow(myViewSlaveComm[slaveCommObjectCount]);
		myViewSlaveComm[slaveCommObjectCount]->show();

		msg = "Created 'Slave Comm' object index " + QString::number(slaveCommObjectCount) \
				+ " (max index = " + QString::number(SLAVECOMM_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewSlaveComm[slaveCommObjectCount], SIGNAL(windowClosed()), \
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
		connect(myViewSlaveComm[0], SIGNAL(writeToLogFile(uint8_t,uint8_t,uint8_t)), \
				myDataLogger, SLOT(writeToFile(uint8_t,uint8_t,uint8_t)));
		connect(myViewSlaveComm[0], SIGNAL(closeLogFile(uint8_t)), \
				myDataLogger, SLOT(closeRecordingFile(uint8_t)));

		//Link SlaveComm and Control Trought connector
		connect(this, SIGNAL(connectorWriteCommand(char,unsigned char*)), \
				myViewSlaveComm[0], SLOT(externalSlaveWrite(char,unsigned char*)));

		slaveCommObjectCount++;
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

	if(slaveCommObjectCount > 0)
	{
		slaveCommObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Any Command window
void MainWindow::createAnyCommand(void)
{
	QString msg = "";

	//Limited number of windows:
	if(anyCommandObjectCount < (ANYCOMMAND_WINDOWS_MAX))
	{
		myViewAnyCommand[anyCommandObjectCount] = new W_AnyCommand(this);
		ui->mdiArea->addSubWindow(myViewAnyCommand[anyCommandObjectCount]);
		myViewAnyCommand[anyCommandObjectCount]->show();


		msg = "Created 'AnyCommand' object index " + QString::number(anyCommandObjectCount) \
				+ " (max index = " + QString::number(ANYCOMMAND_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewAnyCommand[anyCommandObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeAnyCommand()));

		anyCommandObjectCount++;
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

	if(anyCommandObjectCount > 0)
	{
		anyCommandObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View RIC/NU window
void MainWindow::createViewRicnu(void)
{
	QString msg = "";

	//Limited number of windows:
	if(ricnuViewObjectCount < (RICNU_VIEW_WINDOWS_MAX))
	{
		//WinViewExecute *myViewEx = new WinViewExecute(ui->mdiArea);
		myViewRicnu[ricnuViewObjectCount] = new W_Ricnu(this);
		ui->mdiArea->addSubWindow(myViewRicnu[ricnuViewObjectCount]);
		myViewRicnu[ricnuViewObjectCount]->show();

		msg = "Created 'RIC/NU View' object index " + \
				QString::number(ricnuViewObjectCount) + " (max index = " \
				+ QString::number(RICNU_VIEW_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and RIC/NU:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewRicnu[ricnuViewObjectCount], SLOT(refreshDisplayRicnu()));

		//Link to MainWindow for the close signal:
		connect(myViewRicnu[ricnuViewObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewRicnu()));

		ricnuViewObjectCount++;
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

	if(ricnuViewObjectCount > 0)
	{
		ricnuViewObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Converter window
void MainWindow::createConverter(void)
{
	QString msg = "";

	//Limited number of windows:
	if(converterObjectCount < (CONVERTER_WINDOWS_MAX))
	{
		my_w_converter[converterObjectCount] = new W_Converter(this);
		ui->mdiArea->addSubWindow(my_w_converter[converterObjectCount]);
		my_w_converter[converterObjectCount]->show();

		msg = "Created 'Converter' object index " + QString::number(converterObjectCount) \
				+ " (max index = " + QString::number(CONVERTER_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(my_w_converter[converterObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeConverter()));

		converterObjectCount++;
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

	if(converterObjectCount > 0)
	{
		converterObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new Calibration window
void MainWindow::createCalib(void)
{
	QString msg = "";

	//Limited number of windows:
	if(calibObjectCount < (CALIB_WINDOWS_MAX))
	{
		myViewCalibration[calibObjectCount] = new W_Calibration(this);
		ui->mdiArea->addSubWindow(myViewCalibration[calibObjectCount]);
		myViewCalibration[calibObjectCount]->show();

		msg = "Created 'Calibration' object index " + QString::number(calibObjectCount) \
				+ " (max index = " + QString::number(CALIB_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link to MainWindow for the close signal:
		connect(myViewCalibration[calibObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeCalib()));

		calibObjectCount++;
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

	if(calibObjectCount > 0)
	{
		calibObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View Gossip window
void MainWindow::createViewGossip(void)
{
	QString msg = "";

	//Limited number of windows:
	if(gossipObjectCount < (GOSSIP_WINDOWS_MAX))
	{
		myViewGossip[gossipObjectCount] = new W_Gossip(this);
		ui->mdiArea->addSubWindow(myViewGossip[gossipObjectCount]);
		myViewGossip[gossipObjectCount]->show();

		msg = "Created 'Gossip View' object index " + \
				QString::number(gossipObjectCount) + " (max index = " \
				+ QString::number(GOSSIP_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Gossip:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewGossip[gossipObjectCount], SLOT(refreshDisplayGossip()));

		//Link to MainWindow for the close signal:
		connect(myViewGossip[gossipObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewGossip()));

		gossipObjectCount++;
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

	if(gossipObjectCount > 0)
	{
		gossipObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View Strain window
void MainWindow::createViewStrain(void)
{
	QString msg = "";

	//Limited number of windows:
	if(strainObjectCount < (STRAIN_WINDOWS_MAX))
	{
		myViewStrain[strainObjectCount] = new W_Strain(this);
		ui->mdiArea->addSubWindow(myViewStrain[strainObjectCount]);
		myViewStrain[strainObjectCount]->show();

		msg = "Created 'Strain View' object index " + \
				QString::number(strainObjectCount) + " (max index = " \
				+ QString::number(STRAIN_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Strain:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewStrain[strainObjectCount], SLOT(refreshDisplayStrain()));

		//Link to MainWindow for the close signal:
		connect(myViewStrain[strainObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewStrain()));

		strainObjectCount++;
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

	if(strainObjectCount > 0)
	{
		strainObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

//Creates a new View Battery window
void MainWindow::createViewBattery(void)
{
	QString msg = "";

	//Limited number of windows:
	if(battObjectCount < (BATT_WINDOWS_MAX))
	{
		myViewBatt[battObjectCount] = new W_Battery(this);
		ui->mdiArea->addSubWindow(myViewBatt[battObjectCount]);
		myViewBatt[battObjectCount]->show();

		msg = "Created 'Battery View' object index " + \
				QString::number(battObjectCount) + " (max index = " \
				+ QString::number(BATT_WINDOWS_MAX-1) + ").";
		ui->statusBar->showMessage(msg);

		//Link SerialDriver and Battery:
		connect(mySerialDriver, SIGNAL(newDataReady()), \
				myViewBatt[battObjectCount], SLOT(refreshDisplayBattery()));

		//Link to MainWindow for the close signal:
		connect(myViewBatt[battObjectCount], SIGNAL(windowClosed()), \
				this, SLOT(closeViewBattery()));

		battObjectCount++;
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

	if(battObjectCount > 0)
	{
		battObjectCount--;
	}
	qDebug() << msg;
	ui->statusBar->showMessage(msg);
}

void MainWindow::displayAbout()
{
	QMessageBox::about(this, tr("About FlexSEA"), \
	tr("<center><u>FlexSEA: <b>Flex</b>ible, <b>S</b>calable <b>E</b>lectronics <b>A</b>rchitecture.</u><br><br> \
	Project originaly developped at the <a href='http://biomech.media.mit.edu/'>MIT Media Lab Biomechatronics group</a>, now \
	supported by <a href='http://dephy.com/'>Dephy, Inc.</a><br><br><b>Copyright &copy; Dephy, Inc. 2016</b> \
	   <br><br>Software released under the GNU GPL-3.0 license</center>"));
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
			<a href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/</a>.</center>"));
}

void MainWindow::displayDocumentation()
{
	QMessageBox::information(this, tr("Documentation"), \
	tr("<center>Documentation available online: <a href='http://flexsea.media.mit.edu/'>\
	FlexSEA Documentation</a></center>"));
}

void MainWindow::setStatusBar(QString msg)
{
	ui->statusBar->showMessage(msg);
}
