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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "w_slavecomm.h"

#include "w_execute.h"
#include "w_logkeypad.h"

// TODO The inclusion of logFile.h in execute and logkeypad cause an issue if
// they are include here before w_config. The compilation error is
// "'Log file' does not name a type"
// It's maybe due to a circular inclusion, but I've not succedd to understand
// What was going on. Any Help is welcome!

#include "w_config.h"

#include "w_anycommand.h"
#include "w_2dplot.h"
#include "w_control.h"
#include "w_ricnu.h"
#include "w_manage.h"
#include "w_calibration.h"
#include "w_userrw.h"
#include "w_battery.h"
#include "w_strain.h"
#include "w_gossip.h"
#include "w_converter.h"
#include "flexsea_generic.h"
#include "serialdriver.h"
#include "datalogger.h"
#include "main.h"

namespace Ui {
class MainWindow;
}

//MDI Objects: set maximums # of child
#define EX_VIEW_WINDOWS_MAX			5
#define MN_VIEW_WINDOWS_MAX			2
#define CONFIG_WINDOWS_MAX			1
#define SLAVECOMM_WINDOWS_MAX		1
#define ANYCOMMAND_WINDOWS_MAX		1
#define CONVERTER_WINDOWS_MAX		1
#define CONTROL_WINDOWS_MAX			1
#define PLOT2D_WINDOWS_MAX			2
#define RICNU_VIEW_WINDOWS_MAX		1
#define CALIB_WINDOWS_MAX			1
#define BATT_WINDOWS_MAX			2
#define LOGKEYPAD_WINDOWS_MAX		1
#define GOSSIP_WINDOWS_MAX			2
#define STRAIN_WINDOWS_MAX			2
#define USERRW_WINDOWS_MAX			1

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;

	// Sub-Windows
	W_Execute *myViewExecute[EX_VIEW_WINDOWS_MAX];
	W_Manage *myViewManage[MN_VIEW_WINDOWS_MAX];
	W_Config *myViewConfig[CONFIG_WINDOWS_MAX];
	W_SlaveComm *myViewSlaveComm[SLAVECOMM_WINDOWS_MAX];
	W_AnyCommand *myViewAnyCommand[ANYCOMMAND_WINDOWS_MAX];
	W_Converter *my_w_converter[CONVERTER_WINDOWS_MAX];
	W_Calibration *myViewCalibration[CALIB_WINDOWS_MAX];
	W_Control *myViewControl[CONTROL_WINDOWS_MAX];
	W_2DPlot *myView2DPlot[PLOT2D_WINDOWS_MAX];
	W_Ricnu *myViewRicnu[RICNU_VIEW_WINDOWS_MAX];
	W_Battery *myViewBatt[BATT_WINDOWS_MAX];
	W_LogKeyPad *myViewLogKeyPad[LOGKEYPAD_WINDOWS_MAX];
	W_Gossip *myViewGossip[GOSSIP_WINDOWS_MAX];
	W_Strain *myViewStrain[STRAIN_WINDOWS_MAX];
	W_UserRW *myUserRW[USERRW_WINDOWS_MAX];

	// Objects
	SerialDriver *mySerialDriver;
	DataLogger *myDataLogger;


signals:
	//Allow window to be independly opened in any order by providing a backbone connector
	void connectorRefreshLogTimeSlider(int index);
	void connectorUpdateDisplayMode(DisplayMode mode);
	void connectorWriteCommand(char ch,unsigned char* chPtr);

public slots:

	void translatorUpdateDataSourceStatus(DataSource status);
	void manageLogKeyPad(DataSource status);

	//MDI Windows (create):
	void createViewExecute(void);
	void createViewManage(void);
	void createView2DPlot(void);
	void createControlControl(void);
	void createConfig(void);
	void createCalib(void);
	void createSlaveComm(void);
	void createAnyCommand(void);
	void createViewRicnu(void);
	void createConverter(void);
	void createViewStrain(void);
	void createViewGossip(void);
	void createViewBattery(void);
	void createLogKeyPad(void);
	void createUserRW(void);

	//MDI Windows (closed):
	void closeViewExecute(void);
	void closeViewManage(void);
	void closeView2DPlot(void);
	void closeControlControl(void);
	void closeConfig(void);
	void closeCalib(void);
	void closeSlaveComm(void);
	void closeAnyCommand(void);
	void closeViewRicnu(void);
	void closeConverter(void);
	void closeViewStrain(void);
	void closeViewGossip(void);
	void closeViewBattery(void);
	void closeLogKeyPad(void);
	void closeUserRW(void);

	//Messages Status Bar + debug
	void sendWindowCreatedMsg(QString windowName, int index, int maxIndex);
	void sendWindowCreatedFailedMsg(QString windowName, int maxWindow);
	void sendCloseWindowMsg(QString windowName);

	//Message boxes:
	void displayAbout();
	void displayDocumentation();
	void displayLicense();
	void setStatusBar(QString msg);
};

#endif // MAINWINDOW_H
