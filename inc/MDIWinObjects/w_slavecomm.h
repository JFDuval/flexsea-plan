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
#include "flexsea_generic.h"
#include "flexseaDevice.h"
#include "executeDevice.h"

//****************************************************************************
// Definition(s)
//****************************************************************************

#define MAX_SC_ITEMS            4
#define MAX_SLAVES              10
#define MASTER_TIMER            100 //Hz
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
						 QList<ExecuteDevice> *exDevListPtr = nullptr);
	~W_SlaveComm();

public slots:
	void displayDataReceived(int idx, int status);
	void receiveNewDataReady(void);
	void receiveComPortStatus(bool status);
	void updateIndicatorTimeout(bool rst);
	void externalSlaveWrite(char numb, unsigned char *tx_data);

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

signals:

	//Timers:
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
	void writeToLogFile(uint8_t item, uint8_t slaveIndex,
						uint8_t expIndex, uint16_t refreshRate);
	void writeToLogFiledev(FlexseaDevice *devicePtr, uint8_t item);
	void openRecordingFile(uint8_t item, QString fileName);
	void closeRecordingFile(uint8_t item);
	void slaveReadWrite(uint numb, uint8_t *dataPacket, uint8_t r_w);

private:
	//Variables & Objects:
	Ui::W_SlaveComm *ui;
	bool allComboBoxesPopulated;
	//Store active connections:

	QList<ExecuteDevice> *exDevList;

	FlexseaDevice *FlexSEADeviceList[MAX_SC_ITEMS];

	QMetaObject::Connection sc_connections[MAX_SC_ITEMS];
	int active_slave[MAX_SC_ITEMS], active_slave_index[MAX_SC_ITEMS];
	int selected_exp_index[MAX_SC_ITEMS];
	int selected_refresh_index[MAX_SC_ITEMS], previous_refresh_index[MAX_SC_ITEMS];
	QStringList var_list_refresh;
	QList<int> refreshRate;
	bool logThisItem[MAX_SC_ITEMS];
	QPushButton **on_off_pb_ptr[MAX_SC_ITEMS];
	QCheckBox **log_cb_ptr[MAX_SC_ITEMS];
	QComboBox **comboBoxSlavePtr[MAX_SC_ITEMS];
	QComboBox **comboBoxExpPtr[MAX_SC_ITEMS];
	QComboBox **comboBoxRefreshPtr[MAX_SC_ITEMS];
	QTimer *master_timer;
	bool sc_comPortOpen;
	//Will change this, but for now the payloads will be stored in:
	uint8_t tmp_payload_xmit[48];

	//Function(s):
	void initSlaveCom(void);
	void initTimers(void);
	void initDisplayDataReceived(void);
	void managePushButton(int idx, bool forceOff);
	void manageLogStatus(uint8_t idx);

	void sc_read_all(uint8_t item);
	void sc_read_all_ricnu(uint8_t item);
	void sc_ankle2dof(uint8_t item);

	FlexseaDevice* getDataObjectPtr(uint8_t slaveId, uint8_t slaveIndex);
	void configSlaveComm(int item);
	void updateStatusBar(QString txt);
	//Function pointers to timer signals:
	void connectSCItem(int item, int sig_idx);
};

#endif // W_SLAVECOMM_H
