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
	[This file] w_slavecomm.h: Slave Communication Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef W_SLAVECOMM_H
#define W_SLAVECOMM_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <qcombobox.h>
#include <qlabel.h>
#include "flexsea_generic.h"
#include "flexseaDevice.h"
#include "executeDevice.h"
#include "manageDevice.h"
#include "gossipDevice.h"
#include "batteryDevice.h"
#include "strainDevice.h"
#include "ricnuProject.h"
#include "ankle2DofProject.h"
#include "rigidDevice.h"
#include "pocketDevice.h"
#include <commanager.h>

//****************************************************************************
// Definition(s)
//****************************************************************************

#define MAX_SC_ITEMS			4
#define MAX_SLAVES				10
#define MAX_EXPERIMENTS			13
#define MAX_COMM_INTERFACES		2

#define DATA_TIMEOUT			100	//Timer will fire every 100ms

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_SlaveComm;
}

class W_SlaveComm : public QWidget, public Counter<W_SlaveComm>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_SlaveComm(QWidget *parent = 0,
							QList<FlexseaDevice*> *executeDevListInit = nullptr,
							QList<FlexseaDevice*> *manageDevListInit = nullptr,
							QList<FlexseaDevice*> *gossipDevListInit = nullptr,
							QList<FlexseaDevice*> *batteryDevListInit = nullptr,
							QList<FlexseaDevice*> *strainDevListInit = nullptr,
							QList<FlexseaDevice*> *ricnuDevListInit = nullptr,
							QList<FlexseaDevice*> *ankle2DofDevListIni = nullptr,
							QList<FlexseaDevice*> *dynamicUserDevListInit = nullptr,
							QList<FlexseaDevice*> *rigidDevListInit = nullptr,
							QList<FlexseaDevice*> *pocketDevListInit = nullptr,
							QList<ComManager*>	  *comManagerListInit = nullptr);

	QList<ComManager*> *ComManagerList;

	void addExperiment(QList<FlexseaDevice*> *deviceList, int cmdCode);
	void setRowDisabled(int row, bool disabled);

	~W_SlaveComm();

public slots:

	void displayDataReceived0(int row, int status);
	void receiveComPortStatus0(SerialPortStatus status,int nbTries);
	void updateIndicatorTimeout0(bool rst);

	void displayDataReceived1(int row, int status);
	void receiveComPortStatus1(SerialPortStatus status,int nbTries);
	void updateIndicatorTimeout1(bool rst);

	void displayDataReceived(int row, int status);
	void receiveComPortStatus(int row, SerialPortStatus status,int nbTries);
	void updateIndicatorTimeout(int row, bool rst);

	void getSlaveId(int* slaveId);
	void getCurrentDevice(FlexseaDevice** device);
	void startExperiment(uint8_t i, int r, bool log, bool autoSample, QString offs, QString uNotes);
	void stopExperiment(uint8_t i);

private slots:

	//GUI elements:
	void on_pushButton1_clicked();
	void on_pushButton2_clicked();
	void on_pushButton3_clicked();
	void on_pushButton4_clicked();

	void on_comboBoxExp1_currentIndexChanged(int index);
	void on_comboBoxExp2_currentIndexChanged(int index);
	void on_comboBoxExp3_currentIndexChanged(int index);
	void on_comboBoxExp4_currentIndexChanged(int index);
	void readCommandLine(int row);
	void dataTimeoutEvent0(void);
	void dataTimeoutEvent1(void);

signals:
	void windowClosed(void);
	void activeSlaveStreaming(QString slaveName);
	void setOffsetParameter0(QList<int> ricnuOffsets, QList<int> rigidOffsets, int minOffs, int maxOffs);
	void setOffsetParameter1(QList<int> ricnuOffsets, QList<int> rigidOffsets, int minOffs, int maxOffs);
	void startStreaming0(bool shouldLog, FlexseaDevice* logToDevice);
	void startStreaming1(bool shouldLog, FlexseaDevice* logToDevice);
	void startAutoStreaming0(bool shouldLog, FlexseaDevice* logToDevice);
	void startAutoStreaming1(bool shouldLog, FlexseaDevice* logToDevice);
	void stopStreaming0(FlexseaDevice *device);
	void stopStreaming1(FlexseaDevice *device);
private:
	//Helper Functions
	void populateSlaveComboBox(QComboBox* box, int indexOfExperimentSelected);
	void manageSelectedExperimentChanged(int row);
	void mapSerializedPointers(void);
	void initializeMaps(void);
	void initTimers(void);
	FlexseaDevice* getTargetDevice(int cmd, int experimentIndex, int slaveIndex);
	void rowShow(int row, bool s);

	//UI objects
	Ui::W_SlaveComm *ui;

	QPushButton *on_off_pb_ptr[MAX_SC_ITEMS];
	QCheckBox *log_cb_ptr[MAX_SC_ITEMS];
	QCheckBox *auto_checkbox[MAX_SC_ITEMS];
	QComboBox *comboBoxSlavePtr[MAX_SC_ITEMS];
	QComboBox *comboBoxExpPtr[MAX_SC_ITEMS];
	QComboBox *comboBoxRefreshPtr[MAX_SC_ITEMS];
	QLabel *labelStatusPtr[MAX_SC_ITEMS];
	QLabel *labelRowPtr[MAX_SC_ITEMS];

	bool allComboBoxesPopulated;

	QList<FlexseaDevice*>* targetListMap[MAX_EXPERIMENTS];
	int cmdMap[MAX_EXPERIMENTS];

	//Variables & Objects:
	QList<FlexseaDevice*> *executeDevList;
	QList<FlexseaDevice*> *manageDevList;
	QList<FlexseaDevice*> *gossipDevList;
	QList<FlexseaDevice*> *batteryDevList;
	QList<FlexseaDevice*> *strainDevList;
	QList<FlexseaDevice*> *rigidDevList;
	QList<FlexseaDevice*> *pocketDevList;

	QList<FlexseaDevice*> *ricnuDevList;
	QList<FlexseaDevice*> *ankle2DofDevList;
	QList<FlexseaDevice*> *dynamicUserDevList;

	QList<FlexseaDevice*> readAllTargetList;
	QList<FlexseaDevice*> inControlTargetList;
	QList<FlexseaDevice*> ricnuTargetList;
	QList<FlexseaDevice*> ankle2DofTargetList;
	QList<FlexseaDevice*> dynamicUserTargetList;
	QList<FlexseaDevice*> batteryTargetList;
	QList<FlexseaDevice*> rigidTargetList;
	QList<FlexseaDevice*> pocketTargetList;

	//Command line (only for RIC/NU as of today):
	QString defaultCmdLineText;

	//Function(s):
	void initExperimentList(void);
	void initSlaveCom(void);
	void initConnection(void);
	void managePushButton(int idx, bool forceOff);
	void updateStatusBar(QString txt);
	void stopOneExperiment(uint8_t i);
	void startOneExperiment(uint8_t i, int r, bool log, bool autoSample, QString offs, QString uNotes);

	uint8_t numExperiments;

	QList<int> refreshRates;

	QList<QTimer*> dataTimeoutList;
	bool isStreaming = false;

};

#endif // W_SLAVECOMM_H
