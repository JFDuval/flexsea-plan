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
#include "WinViewExecute.h"
#include "WinControlControl.h"
#include "WinView2DPlot.h"
#include "WinConfig.h"
#include "WinSlaveComm.h"
#include "WinViewRicnu.h"
#include "WinConverter.h"
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

    setWindowTitle("FlexSEA Plan GUI v2.0 (Alpha Release - 09/2016)");
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

    //Create default objects:
    createConfig();
    createSlaveComm();

    //SerialDriver:
    mySerialDriver = new SerialDriver;

    //Datalogger:
    myDataLogger = new DataLogger;

    //Link Config and SerialDriver:

    connect(myConfig[0], SIGNAL(transmitOpenCom(QString,int,int)), \
            mySerialDriver, SLOT(open(QString,int,int)));
    connect(myConfig[0], SIGNAL(transmitCloseCom()), \
            mySerialDriver, SLOT(close()));
    connect(mySerialDriver, SIGNAL(openProgress(int,int)), \
            myConfig[0], SLOT(setComProgress(int,int)));

    //Link SerialDriver and SlaveComm:

    connect(mySerialDriver, SIGNAL(openStatus(bool)), \
            mySlaveComm[0], SLOT(receiveComOpenStatus(bool)));
    connect(mySlaveComm[0], SIGNAL(slaveReadWrite(uint, uint8_t*, uint8_t)), \
            mySerialDriver, SLOT(readWrite(uint, uint8_t*, uint8_t)));
    connect(mySerialDriver, SIGNAL(newDataReady()), \
            mySlaveComm[0], SLOT(receiveNewDataReady()));
    connect(mySerialDriver, SIGNAL(dataStatus(int, int)), \
            mySlaveComm[0], SLOT(receiveDataStatus(int, int)));
    connect(mySerialDriver, SIGNAL(newDataTimeout(bool)), \
            mySlaveComm[0], SLOT(receiveNewDataTimeout(bool)));

    //Log:
    connect(myConfig[0], SIGNAL(transmitOpenLogFile()), \
            myDataLogger, SLOT(openFile()));
    connect(mySlaveComm[0], SIGNAL(writeToLogFile(int)), \
            myDataLogger, SLOT(writeToFile(int)));
    connect(mySlaveComm[0], SIGNAL(closeLogFile()), \
            myDataLogger, SLOT(closeFile()));
    connect(myDataLogger, SIGNAL(setLogFileStatus(QString)), \
            myConfig[0], SLOT(setLogFileStatus(QString)));
    connect(myDataLogger, SIGNAL(setStatusBarMessage(QString)), \
            this, SLOT(setStatusBar(QString)));

    //SerialDriver message:
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

//Creates a new View Execute window
void MainWindow::createViewExecute(void)
{
    QString msg = "";

    //Limited number of windows:
    if(exViewObjectCount < (EX_VIEW_WINDOWS_MAX))
    {
        //WinViewExecute *myViewEx = new WinViewExecute(ui->mdiArea);
        myViewEx[exViewObjectCount] = new WinViewExecute(ui->mdiArea);
        myViewEx[exViewObjectCount]->setAttribute(Qt::WA_DeleteOnClose);
        myViewEx[exViewObjectCount]->show();

        msg = "Created 'Execute View' object index " + \
                QString::number(exViewObjectCount) + " (max index = " \
                + QString::number(EX_VIEW_WINDOWS_MAX-1) + ").";
        ui->statusBar->showMessage(msg);

        //Link SerialDriver and Execute:
        connect(mySerialDriver, SIGNAL(newDataReady()), \
                myViewEx[exViewObjectCount], SLOT(refreshDisplayExecute()));

        //Link to MainWindow for the close signal:
        connect(myViewEx[exViewObjectCount], SIGNAL(windowClosed()), \
                this, SLOT(closeViewExecute()));

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

//Creates a new Config window
void MainWindow::createConfig(void)
{
    QString msg = "";

    //Limited number of windows:
    if(configObjectCount < (CONFIG_WINDOWS_MAX))
    {
        myConfig[configObjectCount] = new WinConfig(ui->mdiArea);
        myConfig[configObjectCount]->setAttribute(Qt::WA_DeleteOnClose);
        myConfig[configObjectCount]->show();

        msg = "Created 'Config' object index " + QString::number(configObjectCount) \
                + " (max index = " + QString::number(CONFIG_WINDOWS_MAX-1) + ").";
        qDebug() << msg;
        ui->statusBar->showMessage(msg);

        //Link to MainWindow for the close signal:
        connect(myConfig[configObjectCount], SIGNAL(windowClosed()), \
                this, SLOT(closeConfig()));

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
        myControl[controlObjectCount] = new WinControlControl(ui->mdiArea);
        myControl[controlObjectCount]->setAttribute(Qt::WA_DeleteOnClose);
        myControl[controlObjectCount]->show();

        msg = "Created 'Control' object index " + QString::number(controlObjectCount) \
                + " (max index = " + QString::number(CONTROL_WINDOWS_MAX-1) + ").";
        qDebug() << msg;
        ui->statusBar->showMessage(msg);

        //Link to MainWindow for the close signal:
        connect(myControl[controlObjectCount], SIGNAL(windowClosed()), \
                this, SLOT(closeControlControl()));

        //Link to SlaveComm to send commands:
        connect(myControl[controlObjectCount], SIGNAL(writeCommand(char,unsigned char*)), \
                mySlaveComm[0], SLOT(receiveExternalSlaveWrite(char,unsigned char*)));

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
        my2DPlot[plot2DObjectCount] = new WinView2DPlot(ui->mdiArea);
        my2DPlot[plot2DObjectCount]->setAttribute(Qt::WA_DeleteOnClose);
        my2DPlot[plot2DObjectCount]->show();

        //Link SerialDriver and 2DPlot:
        connect(mySerialDriver, SIGNAL(newDataReady()), \
                my2DPlot[plot2DObjectCount], SLOT(refresh2DPlot()));

        msg = "Created '2DPlot' object index " + QString::number(plot2DObjectCount) \
                + " (max index = " + QString::number(PLOT2D_WINDOWS_MAX-1) + ").";
        qDebug() << msg;
        ui->statusBar->showMessage(msg);

        //Link to MainWindow for the close signal:
        connect(my2DPlot[plot2DObjectCount], SIGNAL(windowClosed()), \
                this, SLOT(closeView2DPlot()));

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
        mySlaveComm[slaveCommObjectCount] = new WinSlaveComm(ui->mdiArea);
        mySlaveComm[slaveCommObjectCount]->setAttribute(Qt::WA_DeleteOnClose);
        mySlaveComm[slaveCommObjectCount]->show();

        msg = "Created 'Slave Comm' object index " + QString::number(slaveCommObjectCount) \
                + " (max index = " + QString::number(SLAVECOMM_WINDOWS_MAX-1) + ").";
        ui->statusBar->showMessage(msg);

        //Link to MainWindow for the close signal:
        connect(mySlaveComm[slaveCommObjectCount], SIGNAL(windowClosed()), \
                this, SLOT(closeSlaveComm()));

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
        myAnyCommand[anyCommandObjectCount] = new WinAnyCommand(ui->mdiArea);
        myAnyCommand[anyCommandObjectCount]->setAttribute(Qt::WA_DeleteOnClose);
        myAnyCommand[anyCommandObjectCount]->show();

        msg = "Created 'AnyCommand' object index " + QString::number(anyCommandObjectCount) \
                + " (max index = " + QString::number(ANYCOMMAND_WINDOWS_MAX-1) + ").";
        ui->statusBar->showMessage(msg);

        //Link to MainWindow for the close signal:
        connect(myAnyCommand[anyCommandObjectCount], SIGNAL(windowClosed()), \
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
        myViewRicnu[ricnuViewObjectCount] = new WinViewRicnu(ui->mdiArea);
        myViewRicnu[ricnuViewObjectCount]->setAttribute(Qt::WA_DeleteOnClose);
        myViewRicnu[ricnuViewObjectCount]->show();

        msg = "Created 'RIC/NU View' object index " + \
                QString::number(ricnuViewObjectCount) + " (max index = " \
                + QString::number(RICNU_VIEW_WINDOWS_MAX-1) + ").";
        ui->statusBar->showMessage(msg);

        //Link SerialDriver and RIC/NU:
        connect(mySerialDriver, SIGNAL(newDataReady()), \
                myViewRicnu[ricnuViewObjectCount], SLOT(refreshDisplayExecute()));

        //Link to MainWindow for the close signal:
        connect(myViewRicnu[ricnuViewObjectCount], SIGNAL(windowClosed()), \
                this, SLOT(closeViewExecute()));

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
        myConverter[converterObjectCount] = new WinConverter(ui->mdiArea);
        myConverter[converterObjectCount]->setAttribute(Qt::WA_DeleteOnClose);
        myConverter[converterObjectCount]->show();

        msg = "Created 'Converter' object index " + QString::number(converterObjectCount) \
                + " (max index = " + QString::number(CONVERTER_WINDOWS_MAX-1) + ").";
        ui->statusBar->showMessage(msg);

        //Link to MainWindow for the close signal:
        connect(myConverter[converterObjectCount], SIGNAL(windowClosed()), \
                this, SLOT(closeConverter()));

        converterObjectCount++;
    }
    else
    {
        //qDebug() << "Maximum number of Converter objects reached.";
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
