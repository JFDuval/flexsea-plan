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
#include "flexsea_generic.h"
#include "serialdriver.h"
#include "datalogger.h"
#include "w_slavecomm.h"
#include "w_execute.h"
#include "w_logkeypad.h"
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
#include "w_incontrol.h"
#include "w_event.h"

#include "w_status.h"
#include "w_rigid.h"
#include "w_pocket.h"
#include "flexseaDevice.h"

#include "w_commtest.h"
#include <commanager.h>
#include <dynamicuserdatamanager.h>

#include <dataprovider.h>
#include "main.h"

#include <QList>
#include <QThread>

namespace Ui {
class MainWindow;
}

//MDI Objects: ID
#define CONFIG_WINDOWS_ID			0
#define LOGKEYPAD_WINDOWS_ID		1
#define SLAVECOMM_WINDOWS_ID		2
#define PLOT2D_WINDOWS_ID			3
#define CONTROL_WINDOWS_ID			4
#define INCONTROL_WINDOWS_ID		5
#define USERRW_WINDOWS_ID			6
#define EVENT_WINDOWS_ID			7
#define ANYCOMMAND_WINDOWS_ID		8
#define CONVERTER_WINDOWS_ID		9
#define CALIB_WINDOWS_ID			10
#define COMMTEST_WINDOWS_ID			11
#define EX_VIEW_WINDOWS_ID			12
#define MN_VIEW_WINDOWS_ID			13
#define BATT_WINDOWS_ID				14
#define GOSSIP_WINDOWS_ID			15
#define STRAIN_WINDOWS_ID			16
#define RICNU_VIEW_WINDOWS_ID		17
#define RIGID_WINDOWS_ID			18
#define STATUS_WINDOWS_ID			19
#define POCKET_WINDOWS_ID			20
#define WINDOWS_TYPES				21 //(has to match the list above)
#define WINDOWS_MAX_INSTANCES		5

//MDI Objects: set maximums # of child
#define EX_VIEW_WINDOWS_MAX			5
#define MN_VIEW_WINDOWS_MAX			2
#define CONFIG_WINDOWS_MAX			2
#define SLAVECOMM_WINDOWS_MAX		1
#define ANYCOMMAND_WINDOWS_MAX		1
#define CONVERTER_WINDOWS_MAX		1
#define CONTROL_WINDOWS_MAX			2
#define PLOT2D_WINDOWS_MAX			2
#define RICNU_VIEW_WINDOWS_MAX		1
#define CALIB_WINDOWS_MAX			1
#define BATT_WINDOWS_MAX			2
#define LOGKEYPAD_WINDOWS_MAX		1
#define GOSSIP_WINDOWS_MAX			2
#define STRAIN_WINDOWS_MAX			2
#define USERRW_WINDOWS_MAX			1
#define COMMTEST_WINDOWS_MAX		1
#define INCONTROL_WINDOWS_MAX		1
#define EVENT_WINDOWS_MAX			1
#define RIGID_WINDOWS_MAX			2
#define STATUS_WINDOWS_MAX			2
#define POCKET_WINDOWS_MAX			1

//Window information:
typedef struct {
	uint8_t id;
	QString nickname;
	bool open;
	QMdiSubWindow *winPtr;

}mdiState_s;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	void initMenus(void);
	void initFlexSeaDeviceObject(void);
	void initFlexSeaDeviceLog(void);
	void initSerialComm(void);
	void initializeDataProviders(void);

	Ui::MainWindow *ui;

	QString appPath;

	// Device Object
	QList<ExecuteDevice>	executeDevList;
	QList<ManageDevice>		manageDevList;
	QList<RigidDevice>		rigidDevList;
	QList<PocketDevice>		pocketDevList;
	QList<GossipDevice>		gossipDevList;
	QList<BatteryDevice>	batteryDevList;
	QList<StrainDevice>		strainDevList;
	QList<RicnuProject>		ricnuDevList;
	QList<Ankle2DofProject> ankle2DofDevList;

	// Specific Flexsea list
	QList<FlexseaDevice*>	executeFlexList;
	QList<FlexseaDevice*>	manageFlexList;
	QList<FlexseaDevice*>	rigidFlexList;
	QList<FlexseaDevice*>	pocketFlexList;
	QList<FlexseaDevice*>	gossipFlexList;
	QList<FlexseaDevice*>	batteryFlexList;
	QList<FlexseaDevice*>	strainFlexList;
	QList<FlexseaDevice*>	ricnuFlexList;
	QList<FlexseaDevice*>	ankle2DofFlexList;
	QList<FlexseaDevice*>	dynamicDeviceList;

	QList<DataProvider*> dataProviders;

	// Global Flexsea List
	QList<FlexseaDevice*> flexseaPtrlist;

	// Log Object
	ExecuteDevice executeLog = ExecuteDevice();
	ManageDevice manageLog = ManageDevice();
	GossipDevice gossipLog = GossipDevice();
	RigidDevice rigidLog = RigidDevice();
	PocketDevice pocketLog = PocketDevice();
	BatteryDevice batteryLog = BatteryDevice();
	StrainDevice strainLog = StrainDevice();
	RicnuProject ricnuLog = RicnuProject();
	Ankle2DofProject ankle2DofLog = Ankle2DofProject();

	FlexseaDevice* currentFlexLog;

	SerialPortStatus comPortStatus;
	QString activeSlaveNameStreaming;

	// Sub-Windows
	W_Execute *myViewExecute[EX_VIEW_WINDOWS_MAX] = {NULL};
	W_Manage *myViewManage[MN_VIEW_WINDOWS_MAX] = {NULL};
	W_Config *myViewConfig[CONFIG_WINDOWS_MAX] = {NULL};
	W_SlaveComm *myViewSlaveComm[SLAVECOMM_WINDOWS_MAX] = {NULL};
	W_AnyCommand *myViewAnyCommand[ANYCOMMAND_WINDOWS_MAX] = {NULL};
	W_Converter *my_w_converter[CONVERTER_WINDOWS_MAX] = {NULL};
	W_Calibration *myViewCalibration[CALIB_WINDOWS_MAX] = {NULL};
	W_Control *myViewControl[CONTROL_WINDOWS_MAX] = {NULL};
	W_2DPlot *myView2DPlot[PLOT2D_WINDOWS_MAX] = {NULL};
	W_Ricnu *myViewRicnu[RICNU_VIEW_WINDOWS_MAX] = {NULL};
	W_Battery *myViewBatt[BATT_WINDOWS_MAX] = {NULL};
	W_LogKeyPad *myViewLogKeyPad[LOGKEYPAD_WINDOWS_MAX] = {NULL};
	W_Gossip *myViewGossip[GOSSIP_WINDOWS_MAX] = {NULL};
	W_Strain *myViewStrain[STRAIN_WINDOWS_MAX] = {NULL};
	W_UserRW *myUserRW[USERRW_WINDOWS_MAX] = {NULL};
	W_CommTest *myViewCommTest[COMMTEST_WINDOWS_MAX] = {NULL};
	W_InControl *myViewInControl[INCONTROL_WINDOWS_MAX] = {NULL};
	W_Event *myEvent[EVENT_WINDOWS_MAX] = {NULL};
	W_Rigid *myViewRigid[RIGID_WINDOWS_MAX] = {NULL};
	W_Pocket *myViewPocket[POCKET_WINDOWS_MAX] = {NULL};
	W_Status *myStatus[STATUS_WINDOWS_MAX] = {NULL};

	//MDI state:
	mdiState_s mdiState[WINDOWS_TYPES][WINDOWS_MAX_INSTANCES];
	void (MainWindow::*mdiCreateWinPtr[WINDOWS_TYPES])(void);

	// Favorite Port list
	QList<QStringList> loadedConfig;
	QStringList favoritePort;

	// Objects
	//ChartController *chartController;
	QList<QThread*> comManagerThread;

	QList<DataLogger*> myDataLogger;
	QList<ComManager*> comManager;
	DynamicUserDataManager* userDataManager;

	QTimer *tabsStateTimer;
	QToolButton *restorebutton;
	QToolButton *closebutton;

	void writeSettings();
	void readSettings();

signals:
	//Allow window to be independly opened in any order by providing a backbone connector
	void connectorRefreshLogTimeSlider(int index, FlexseaDevice*);
	void connectorUpdateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr);
	void connectorCurrentSlaveStreaming(QString slaveName);
	void connectorRefresh(void);


public slots:

	void translatorActiveSlaveStreaming(QString slaveName);
	void saveComPortStatus(SerialPortStatus status);
	void translatorUpdateDataSourceStatus(DataSource status, FlexseaDevice* devPtr);
	void manageLogKeyPad(DataSource status, FlexseaDevice *);

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
	void createLogKeyPad(FlexseaDevice * devPtr);
	void createUserRW(void);
	void createViewCommTest(void);
	void createInControl(void);
	void createToolEvent(void);
	void createViewRigid(void);
	void createViewPocket(void);
	void createStatus(void);

	//MDI Windows (closed):
	void closeViewExecute(void);
	void closeViewManage(void);
	void closeView2DPlot(void);
	void closeControlControl(void);
	void closeConfig(int instanceNum);
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
	void closeViewCommTest(void);
	void closeToolEvent(void);
	void closeInControl(void);
	void closeViewRigid(void);
	void closeViewPocket(void);
	void closeStatus(void);
	void saveConfig(void);
	void loadConfig(void);
	void defaultConfig(void);
	void initializeCreateWindowFctPtr(void);
	//void initializeCloseWindowFctPtr(void);
	void emptyWinFct(void);
	void setWinGeo(int id, int obj, int x, int y, int w, int h);
	void initMdiState(void);

	//Miscellaneous

	DisplayMode getDisplayMode(void);

	//Messages Status Bar + debug
	void sendWindowCreatedMsg(QString windowName, int index, int maxIndex);
	void sendWindowCreatedFailedMsg(QString windowName, int maxWindow);
	void sendCloseWindowMsg(QString windowName);

	//Message boxes:
	void displayAbout();
	void displayDocumentation();
	void displayLicense();
	void setStatusBar(QString msg);

	void closeEvent(QCloseEvent *event);
	void loadCSVconfigFile(void);
	void saveCSVconfigFile(void);
	void applyLoadedConfig(void);
	void tabsStateUpdate(void);
	void closeButtonClicked(void);
	void restoreButtonClicked(void);
};

#endif // MAINWINDOW_H
