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
#include "testBenchProject.h"

//****************************************************************************
// Definition(s)
//****************************************************************************

#define MAX_SC_ITEMS            4
#define MAX_SLAVES				10
#define MASTER_TIMER            200 //Hz
#define MAX_EXPERIMENTS         10

#define TIM_FREQ_TO_P(f)		(1000/f)	//f in Hz, return in ms

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
							QList<FlexseaDevice*> *ankle2DofDevListInit = nullptr,
							QList<FlexseaDevice*> *testBenchDevListInit = nullptr);
	~W_SlaveComm();

public slots:
	void displayDataReceived(int idx, int status);
	void receiveNewDataReady(void);
	void receiveComPortStatus(bool status);
	void updateIndicatorTimeout(bool rst);
	void externalSlaveReadWrite(uint8_t numb, uint8_t *tx_data, uint8_t r_w);

private slots:

	void masterTimerEvent(void);
	void sc_item1_slot(void);

	//GUI elements:
	void on_pushButton1_clicked();
	void on_pushButton2_clicked();
	void on_pushButton3_clicked();
	void on_pushButton4_clicked();
	void on_comboBoxSlave1_currentIndexChanged(int index);
	void on_comboBoxSlave2_currentIndexChanged(int index);
	void on_comboBoxSlave3_currentIndexChanged(int index);
	void on_comboBoxSlave4_currentIndexChanged(int index);
	void on_comboBoxExp1_currentIndexChanged(int index);
	void on_comboBoxExp2_currentIndexChanged(int index);
	void on_comboBoxExp3_currentIndexChanged(int index);
	void on_comboBoxExp4_currentIndexChanged(int index);
	void on_comboBoxRefresh1_currentIndexChanged(int index);
	void on_comboBoxRefresh2_currentIndexChanged(int index);
	void on_comboBoxRefresh3_currentIndexChanged(int index);
	void on_comboBoxRefresh4_currentIndexChanged(int index);
	void on_checkBoxLog1_stateChanged(int arg1);
	void on_checkBoxLog2_stateChanged(int arg1);
	void on_checkBoxLog3_stateChanged(int arg1);
	void on_checkBoxLog4_stateChanged(int arg1);

	void on_lineEdit_returnPressed();

signals:

	//Timers:
	void masterTimer200Hz(void);
	void masterTimer100Hz(void);
	void masterTimer50Hz(void);
	void masterTimer20Hz(void);
	void masterTimer33Hz(void);
	void masterTimer10Hz(void);
	void masterTimer5Hz(void);
	void masterTimer1Hz(void);

	//Other:
	void refresh2DPlot(void);
	void windowClosed(void);
	void writeToLogFile(FlexseaDevice *devicePtr, uint8_t item);
	void openRecordingFile(uint8_t item, QString fileName);
	void openRecordingFile(FlexseaDevice *devicePtr, uint8_t item);
	void closeRecordingFile(uint8_t item);
	void slaveReadWrite(uint8_t numb, uint8_t *dataPacket, uint8_t r_w);

private:
	//Variables & Objects:
	Ui::W_SlaveComm *ui;
	bool allComboBoxesPopulated;
	//Store active connections:

	QList<FlexseaDevice*> *executeDevList;
	QList<FlexseaDevice*> *manageDevList;
	QList<FlexseaDevice*> *gossipDevList;
	QList<FlexseaDevice*> *batteryDevList;
	QList<FlexseaDevice*> *strainDevList;
	QList<FlexseaDevice*> *ricnuDevList;
	QList<FlexseaDevice*> *ankle2DofDevList;
	QList<FlexseaDevice*> *testBenchDevList;

	QList<FlexseaDevice*> readAllTargetList;
	QList<FlexseaDevice*> ricnuTargetList;
	QList<FlexseaDevice*> ankle2DofTargetList;
	QList<FlexseaDevice*> testBenchTargetList;
	QList<FlexseaDevice*> batteryTargetList;

	QList<FlexseaDevice*> *currentTargetList[MAX_SC_ITEMS];

	FlexseaDevice *targetDevice[MAX_SC_ITEMS];
	FlexseaDevice *logDevice[MAX_SC_ITEMS];

	QDateTime *myTime;
	qint64 t_ms_initial[MAX_SC_ITEMS] = {0,0,0,0};

	QMetaObject::Connection sc_connections[MAX_SC_ITEMS];
	int selected_exp_index[MAX_SC_ITEMS], previous_exp_index[MAX_SC_ITEMS];
	int selected_refresh_index[MAX_SC_ITEMS], previous_refresh_index[MAX_SC_ITEMS];
	QStringList var_list_refresh;
	QList<int> refreshRate;
	bool logThisItem[MAX_SC_ITEMS], previousLogThisItem[MAX_SC_ITEMS];

	QPushButton **on_off_pb_ptr[MAX_SC_ITEMS];
	QCheckBox **log_cb_ptr[MAX_SC_ITEMS];
	QComboBox **comboBoxSlavePtr[MAX_SC_ITEMS];
	QComboBox **comboBoxExpPtr[MAX_SC_ITEMS];
	QComboBox **comboBoxRefreshPtr[MAX_SC_ITEMS];
	QLabel **labelStatusPtr[MAX_SC_ITEMS];

	QTimer *master_timer;
	bool sc_comPortOpen;
	//Will change this, but for now the payloads will be stored in:
	uint8_t tmp_payload_xmit[48];

	//Command line (only for RIC/NU as of today):
	uint8_t cmdLineOffsetEntries = 0;
	char cmdLineOffsetArray[10];
	QString defaultCmdLineText;

	//Function(s):
	void initExperimentList(void);
	void initSlaveCom(void);
	void initTimers(void);
	void managePushButton(int idx, bool forceOff);
	void manageLogStatus(uint8_t idx);
	void logTimestamp(qint64 *t_ms, QString *t_text);

	void sc_read_all(uint8_t item);
	void sc_read_all_ricnu(uint8_t item);
	void sc_ankle2dof(uint8_t item);
	void sc_battery(uint8_t item);
	void sc_testbench(uint8_t item);
	void decodeAndLog(uint8_t item);
	void configSlaveComm(int item);
	void updateStatusBar(QString txt);
	//Function pointers to timer signals:
	void connectSCItem(int item, int sig_idx);
};

#endif // W_SLAVECOMM_H
