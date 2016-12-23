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

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_slavecomm.h"
#include "flexsea_generic.h"
#include "serialdriver.h"
#include "ui_w_slavecomm.h"
#include "main.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_SlaveComm::W_SlaveComm(QWidget *parent,
						 QList<FlexseaDevice*> *flexSEADevListPtr) :
	QWidget(parent),
	ui(new Ui::W_SlaveComm)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	devList = flexSEADevListPtr;
	initSlaveCom();
	initTimers();
}

W_SlaveComm::~W_SlaveComm()
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

//This slot gets called when the port status changes (turned On or Off)
void W_SlaveComm::receiveComPortStatus(bool status)
{
	if(status == false)
	{
		qDebug() << "COM port was closed";

		sc_comPortOpen = false;

		//PushButton:
		ui->pushButton1->setDisabled(true);
		managePushButton(0,true);
		//Data Received indicator:
		displayDataReceived(0, DATAIN_STATUS_GREY);
		//Log check box:
		ui->checkBoxLog1->setDisabled(true);
		if(logThisItem[0] == true)
		{
			logThisItem[0] = false;
			emit closeRecordingFile(0);   //ToDo support multiple files
		}
	}
	else
	{
		qDebug() << "COM port was opened";

		sc_comPortOpen = true;

		//PushButton:
		ui->pushButton1->setDisabled(false);

		//Log check box:
		ui->checkBoxLog1->setDisabled(false);
	}
}

//A 3rd party is using SlaveComm to write to a slave (ex.: Control, Any Command)
void W_SlaveComm::externalSlaveWrite(char numb, unsigned char *tx_data)
{
	//First test: send right away
	//***TODO Fix***
	FlexSEA_Generic::packetVisualizer(numb, tx_data);
	emit slaveReadWrite(numb, tx_data, WRITE);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_SlaveComm::initSlaveCom(void)
{
	QString on_off_pb_ttip, log_cb_ptr_ttip, ttip, labelStatusttip = "";

	//Safeguard - protected from signals emited during setup
	allComboBoxesPopulated = false;

	myTime = new QDateTime;

	//Status bar:
	//===========

	ui->statusbar->setTextFormat(Qt::RichText);
	updateStatusBar("Slave communication object created. Ready.");

	//State variables:
	//================
	sc_comPortOpen = false;

	//Item serialized accessor
	//================
	comboBoxSlavePtr[0] = &ui->comboBoxSlave1;
	comboBoxSlavePtr[1] = &ui->comboBoxSlave2;
	comboBoxSlavePtr[2] = &ui->comboBoxSlave3;
	comboBoxSlavePtr[3] = &ui->comboBoxSlave4;
	comboBoxExpPtr[0] = &ui->comboBoxExp1;
	comboBoxExpPtr[1] = &ui->comboBoxExp2;
	comboBoxExpPtr[2] = &ui->comboBoxExp3;
	comboBoxExpPtr[3] = &ui->comboBoxExp4;
	comboBoxRefreshPtr[0] = &ui->comboBoxRefresh1;
	comboBoxRefreshPtr[1] = &ui->comboBoxRefresh2;
	comboBoxRefreshPtr[2] = &ui->comboBoxRefresh3;
	comboBoxRefreshPtr[3] = &ui->comboBoxRefresh4;
	log_cb_ptr[0] = &ui->checkBoxLog1;
	log_cb_ptr[1] = &ui->checkBoxLog2;
	log_cb_ptr[2] = &ui->checkBoxLog3;
	log_cb_ptr[3] = &ui->checkBoxLog4;
	on_off_pb_ptr[0] = &ui->pushButton1;
	on_off_pb_ptr[1] = &ui->pushButton2;
	on_off_pb_ptr[2] = &ui->pushButton3;
	on_off_pb_ptr[3] = &ui->pushButton4;
	labelStatusPtr[0] = &ui->stat1;
	labelStatusPtr[1] = &ui->stat2;
	labelStatusPtr[2] = &ui->stat3;
	labelStatusPtr[3] = &ui->stat4;

	// Serialized item init:
	//==============
	on_off_pb_ttip = "<html><head/><body><p>Turn streaming on/off</p></body></html>";
	log_cb_ptr_ttip = "<html><head/><body><p>Check this box to log the stream. \
			</p><p>It will log under the folder &quot;Plan-GUI-Logs&quot; \
			when the stream is active.</p></body></html>";
	labelStatusttip = "<html><head/><body><p>Stream Status.</p></body></html>";
	QFont font( "Arial", 12, QFont::Bold);

	var_list_refresh << "100Hz" << "50Hz" << "33Hz" << "20Hz" \
					 << "10Hz" << "5Hz" << "1Hz";
	refreshRate << 100 << 50 << 33 << 20
				<< 10 << 5 << 1;

	for(int item = 0; item < MAX_SC_ITEMS; item++)
	{
		logThisItem[item] = false;
		previousLogThisItem[item] = false;

		//Log checkboxes:
		//===============
		(*log_cb_ptr[item])->setChecked(false);
		(*log_cb_ptr[item])->setEnabled(false);
		(*log_cb_ptr[item])->setToolTip(log_cb_ptr_ttip);

		//On/Off Button init:
		//==============
		(*on_off_pb_ptr[item])->setText(QChar(0x2718));
		(*on_off_pb_ptr[item])->setAutoFillBackground(true);
		(*on_off_pb_ptr[item])->setStyleSheet("background-color: rgb(127, 127, 127); \
										color: rgb(0, 0, 0)");
		(*on_off_pb_ptr[item])->setToolTip(on_off_pb_ttip);
		(*on_off_pb_ptr[item])->setDisabled(true);

		//Populates Slave list:
		//=====================
		for(int i = 0; i < (*devList).length(); i++)
		{
			(*comboBoxSlavePtr[item])->addItem((*devList)[i]->slaveName);
		}

		//Receive indicators:
		//===================
		(*labelStatusPtr[item])->setText(QChar(0x2B07));
		(*labelStatusPtr[item])->setAlignment(Qt::AlignCenter);
		(*labelStatusPtr[item])->setFont(font);
		(*labelStatusPtr[item])->setToolTip(ttip);
		displayDataReceived(item,DATAIN_STATUS_GREY);

		//Populates Experiment/Command list:
		//==================================
		FlexSEA_Generic::populateExpComboBox((*comboBoxExpPtr[item]));

		//Refresh Rate:
		//==================================
		for(int i = 0; i < var_list_refresh.count(); i++)
		{
			(*comboBoxRefreshPtr[item])->addItem(var_list_refresh.at(i));
		}

		//Start at 33Hz:
		(*comboBoxRefreshPtr[item])->setCurrentIndex(2);
		selected_refresh_index[item] = 2;
		previous_refresh_index[item] = selected_refresh_index[item];

		//Connect default slots:
		connectSCItem(item, 2);
	}

	//ComboBoxes are all set:
	allComboBoxesPopulated = true;

	//For now, Experiments 2-4 are disabled:
	//======================================
	for(int item = 1; item < MAX_SC_ITEMS; item++)
	{
		(*comboBoxSlavePtr[item])->setDisabled(true);
		(*comboBoxExpPtr[item])->setDisabled(true);
		(*comboBoxRefreshPtr[item])->setDisabled(true);
		(*log_cb_ptr[item])->setDisabled(true);
		(*on_off_pb_ptr[item])->setDisabled(true);
		(*labelStatusPtr[item])->setDisabled(true);
	}
}

void W_SlaveComm::initTimers(void)
{
	master_timer = new QTimer(this);
	connect(master_timer, SIGNAL(timeout()), this, SLOT(masterTimerEvent()));
	master_timer->start(TIM_FREQ_TO_P(MASTER_TIMER));
}

void W_SlaveComm::logTimestamp(qint64 *t_ms, QString *t_text)
{
	*t_ms = myTime->currentMSecsSinceEpoch();
	*t_text = myTime->currentDateTime().toString();
}

//The 4 PB slots call this function:
void W_SlaveComm::managePushButton(int idx, bool forceOff)
{
	if((*on_off_pb_ptr[idx])->isChecked() == true &&
		forceOff == false)
	{
		// set button appearance
		(*on_off_pb_ptr[idx])->setChecked(true);
		(*on_off_pb_ptr[idx])->setText(QChar(0x2714));
		(*on_off_pb_ptr[idx])->setStyleSheet("background-color: \
								rgb(0, 255, 0); color: rgb(0, 0, 0)");
	}
	else
	{
		// set button appearance
		(*on_off_pb_ptr[idx])->setChecked(false);
		(*on_off_pb_ptr[idx])->setText(QChar(0x2718));
		(*on_off_pb_ptr[idx])->setStyleSheet("background-color: \
								rgb(127, 127, 127); color: rgb(0, 0, 0)");
	}

	//All GUI events call configSlaveComm():
	configSlaveComm(idx);
}

// Need to be called after configSlaveComm
void W_SlaveComm::manageLogStatus(uint8_t item)
{
	//Logging?
	if((*log_cb_ptr[item])->isChecked() &&
		(*on_off_pb_ptr[item])->isChecked())
	{
		emit openRecordingFile(selectedDeviceList[item] ,item);

		logThisItem[item] = true;

		// Update GUI
		ui->comboBoxRefresh1->setDisabled(true);
		QString ttip = "<html><head/><body><p>You can't change refresh rate while "
					   "logging.</p></body></html>";
		ui->comboBoxRefresh1->setToolTip(ttip);
	}

	else
	{
		if(logThisItem[item] == true)
		{
			ui->comboBoxRefresh1->setDisabled(false);
			logThisItem[item] = false;
			emit closeRecordingFile(item);

			ui->comboBoxRefresh1->setToolTip("");
		}
	}
}

void W_SlaveComm::updateStatusBar(QString txt)
{
	QString finalTxt = "<font color=#808080>[Status] " + txt + "</font>";
	ui->statusbar->setText(finalTxt);
}

//Connect a SlaveComm item with a timer
void W_SlaveComm::connectSCItem(int item, int sig_idx)
{
	if(item == 0)
	{
		//Break connection if there's already one?
		if((bool)sc_connections[item] == true)
		{
			disconnect(sc_connections[item]);
		}

		//New connection:
		switch(sig_idx)
		{
			case 0:
				sc_connections[item] = connect(this, SIGNAL(masterTimer100Hz()), \
										this, SLOT(sc_item1_slot()));
				break;
			case 1:
				sc_connections[item] = connect(this, SIGNAL(masterTimer50Hz()), \
										this, SLOT(sc_item1_slot()));
				break;
			case 2:
				sc_connections[item] = connect(this, SIGNAL(masterTimer33Hz()), \
										this, SLOT(sc_item1_slot()));
				break;
			case 3:
				sc_connections[item] = connect(this, SIGNAL(masterTimer20Hz()), \
										this, SLOT(sc_item1_slot()));
				break;
			case 4:
				sc_connections[item] = connect(this, SIGNAL(masterTimer10Hz()), \
										this, SLOT(sc_item1_slot()));
				break;
			case 5:
				sc_connections[item] = connect(this, SIGNAL(masterTimer5Hz()), \
										this, SLOT(sc_item1_slot()));
				break;
			case 6:
				sc_connections[item] = connect(this, SIGNAL(masterTimer1Hz()), \
										this, SLOT(sc_item1_slot()));
				break;
		}
	}
	else
	{
		//...
		//TODO Items 2-4
	}
}

//"Data Received" Arrows:
void W_SlaveComm::displayDataReceived(int item, int status)
{
	switch(status)
	{
		case DATAIN_STATUS_GREY:
			(*labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										rgb(127,127,127); color: black;}");
			break;
		case DATAIN_STATUS_GREEN:
			(*labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										rgb(0,255,0); color: black;}");
			break;
		case DATAIN_STATUS_YELLOW:
			(*labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										rgb(255,255,0); color: black;}");
			break;
		case DATAIN_STATUS_RED:
			(*labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										rgb(255,0,0); color: black;}");
			break;
		default:
			(*labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										black; color: white;}");
			break;
	}
}

//This function will connect a Timer signal and a Slot. Updated when
//"something" changes.
void W_SlaveComm::configSlaveComm(int item)
{
	QString msg = "", msg_ref = "";

	if(allComboBoxesPopulated == true)
	{
		int slaveindex = (*comboBoxSlavePtr[item])->currentIndex();
		//Refresh all fields:

		selected_exp_index[item] = (*comboBoxExpPtr[item])->currentIndex();
		selected_refresh_index[item] = (*comboBoxRefreshPtr[item])->currentIndex();


		// Fill the flexSEADevice Object metadata properly
		selectedDeviceList[item] = (*devList)[slaveindex];

		QString name;

		selectedDeviceList[item]->experimentIndex = selected_exp_index[item];
		FlexSEA_Generic::getExpName(selected_exp_index[item], &name);
		selectedDeviceList[item]->experimentName = name;

		selectedDeviceList[item]->frequency =
				uint16_t(refreshRate[selected_refresh_index[item]]);

		selectedDeviceList[item]->shortFileName =
				selectedDeviceList[item]->slaveName + "_" +
				selectedDeviceList[item]->experimentName + "_" +
				var_list_refresh[selected_refresh_index[item]] +
				".csv";

		selectedDeviceList[item]->logItem = item;


		//Now we connect a time slot to that stream command:
		if(previous_refresh_index[item] != selected_refresh_index[item])
		{
			//Refresh changed, we need to update connections.
			connectSCItem(item, selected_refresh_index[item]);

			msg_ref += "Changed connection.";
		}

		//Update status message:
		msg = "Updated #" + QString::number(item+1) + ": (" \
				+ QString::number(slaveindex) + ", " \
				+ QString::number(selected_exp_index[item]) + ", " \
				+ QString::number(selected_refresh_index[item]) + "). ";

		if((*on_off_pb_ptr[0])->isChecked() == true)
		{
			msg += "Stream ON. ";
		}
		else
		{
			msg += "Stream OFF. ";
		}

		manageLogStatus(item);

		updateStatusBar(msg + msg_ref);
		previous_refresh_index[item] = selected_refresh_index[item];
	}
}

//Argument is the item line (0-3)
//Read All should be programmed for all boards - it returns all the onboard
//sensor values
void W_SlaveComm::sc_read_all(uint8_t item)
{
	uint16_t numb = 0;
	uint8_t info[2] = {PORT_USB, PORT_USB};

	//1) Stream
	tx_cmd_data_read_all_r(TX_N_DEFAULT);
	pack(P_AND_S_DEFAULT, selectedDeviceList[item]->slaveID
		 , info, &numb, comm_str_usb);
	emit slaveReadWrite(numb, comm_str_usb, READ);

	decodeAndLog(item);
}

//Argument is the item line (0-3)
//Read All should be programmed for all boards - it returns all the onboard
//sensor values
void W_SlaveComm::sc_read_all_ricnu(uint8_t item)
{
	uint16_t numb = 0;
	uint8_t info[2] = {PORT_USB, PORT_USB};
	static uint8_t offset = 0;

	//1) Stream
	(!offset) ? offset = 1 : offset = 0;
	tx_cmd_ricnu_r(TX_N_DEFAULT, offset);
	pack(P_AND_S_DEFAULT, selectedDeviceList[item]->slaveID
		 , info, &numb, comm_str_usb);
	emit slaveReadWrite(numb, comm_str_usb, READ);

	decodeAndLog(item);
}

//Argument is the item line (0-3)
//Read All should be programmed for all boards - it returns all the onboard
//sensor values
void W_SlaveComm::sc_ankle2dof(uint8_t item)
{
	uint16_t numb = 0;
	uint8_t info[2] = {PORT_USB, PORT_USB};
	qint64 t_ms = 0;
	QString t_text = "";
	static uint8_t sel_slave = 0;

	//1) Stream
	tx_cmd_ankle2dof_r(TX_N_DEFAULT, sel_slave, 0, 0, 0);
	pack(P_AND_S_DEFAULT, selectedDeviceList[item]->slaveID
		 , info, &numb, comm_str_usb);
	emit slaveReadWrite(numb, comm_str_usb, READ);

	//***ToDo: update for multiple slaves!***
	//TODO Not sure if I support taht properly through the new flexseaDevice
	if(sel_slave == 0)
	{
		sel_slave = 1;
	}
	else
	{
		sel_slave = 0;
	}

	//2) Decode values
	(*devList)[sel_slave]->decodeLastLine();
	//(Uncertain about timings, probably delayed by 1 sample)

	//3) Log
	if(logThisItem[item] == true)
	{
		if(previousLogThisItem[item] == false)
		{
			logTimestamp(&t_ms, &t_text);
			t_ms_initial[item] = t_ms;
		}

		//Timestamps:
		logTimestamp(&t_ms, &t_text);
		t_ms -= t_ms_initial[item];

		selectedDeviceList[item]->timeStamp.last().Date = t_text;
		selectedDeviceList[item]->timeStamp.last().ms = t_ms;
		emit writeToLogFile(selectedDeviceList[item], item);
	}

	previousLogThisItem[item] = logThisItem[item];
}

void W_SlaveComm::decodeAndLog(uint8_t item)
{
	qint64 t_ms = 0;
	QString t_text = "";

	//2) Decode values
	selectedDeviceList[item]->decodeLastLine();
	//(Uncertain about timings, probably delayed by 1 sample)


	//3) Log
	if(logThisItem[item] == true)
	{
		if(previousLogThisItem[item] == false)
		{
			logTimestamp(&t_ms, &t_text);
			t_ms_initial[item] = t_ms;
		}

		//Timestamps:
		logTimestamp(&t_ms, &t_text);
		t_ms -= t_ms_initial[item];

		selectedDeviceList[item]->timeStamp.last().Date = t_text;
		selectedDeviceList[item]->timeStamp.last().ms = t_ms;
		emit writeToLogFile(selectedDeviceList[item], item);
	}

	previousLogThisItem[item] = logThisItem[item];
}

//
void W_SlaveComm::updateIndicatorTimeout(bool rst)
{
	static uint32_t counter = 0;

	counter++;
	if(counter > INDICATOR_TIMEOUT)
	{
		displayDataReceived(0, DATAIN_STATUS_GREY);
	}

	if(rst == true)
	{
		counter = 0;
	}
}

void W_SlaveComm::receiveNewDataReady(void)
{
	//my_w_slavecomm->
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

//This is what gets connected to a timer slot.
void W_SlaveComm::sc_item1_slot(void)
{
	if((*on_off_pb_ptr[0])->isChecked() == true
		&& sc_comPortOpen == true) //TODO: add slot, and private variable
	{
		switch(selected_exp_index[0])
		{
			case 0: //Read All (Barebone)
				sc_read_all(0);
				break;
			case 1: //In Control
				qDebug() << "Not programmed!";
				break;
			case 2: //RIC/NU Knee
				sc_read_all_ricnu(0);
				break;
			case 3: //CSEA Knee
				qDebug() << "Not programmed!";
				break;
			case 4: //2DOF Ankle
				sc_ankle2dof(0);
				break;
			default:
				break;
		}
	}
}

//Master timebase is 100Hz. We divide is to get [100, 50, 33, 20, 10, 5, 1]Hz
void W_SlaveComm::masterTimerEvent(void)
{
	static int tb50Hz = 0, tb33Hz = 0, tb20Hz = 0;
	static int tb10Hz = 0, tb5Hz = 0, tb1Hz = 0;

	//Increment all counters:
	tb50Hz++;
	tb33Hz++;
	tb20Hz++;
	tb10Hz++;
	tb5Hz++;
	tb1Hz++;

	//Emit signals:

	emit masterTimer100Hz();
	updateIndicatorTimeout(false);

	if(tb50Hz > 1)
	{
		tb50Hz = 0;
		emit masterTimer50Hz();
	}

	if(tb33Hz > 2)
	{
		tb33Hz = 0;
		emit masterTimer33Hz();
		emit refresh2DPlot();   //Move to desired slot
	}

	if(tb20Hz > 4)
	{
		tb20Hz = 0;
		emit masterTimer20Hz();
	}

	if(tb10Hz > 9)
	{
		tb10Hz = 0;
		emit masterTimer10Hz();
	}

	if(tb5Hz > 19)
	{
		tb5Hz = 0;
		emit masterTimer5Hz();
	}

	if(tb1Hz > 99)
	{
		tb1Hz = 0;
		emit masterTimer1Hz();
	}
}

void W_SlaveComm::on_pushButton1_clicked()
{
	managePushButton(0, false);
}

void W_SlaveComm::on_pushButton2_clicked()
{
	managePushButton(1, false);
}

void W_SlaveComm::on_pushButton3_clicked()
{
	managePushButton(2, false);
}

void W_SlaveComm::on_pushButton4_clicked()
{
	managePushButton(3, false);
}

void W_SlaveComm::on_comboBoxSlave1_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(0);
}

void W_SlaveComm::on_comboBoxSlave2_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(1);
}

void W_SlaveComm::on_comboBoxSlave3_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(2);
}

void W_SlaveComm::on_comboBoxSlave4_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(3);
}

void W_SlaveComm::on_comboBoxExp1_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(0);
}

void W_SlaveComm::on_comboBoxExp2_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(1);
}

void W_SlaveComm::on_comboBoxExp3_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(2);
}

void W_SlaveComm::on_comboBoxExp4_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(3);
}

void W_SlaveComm::on_comboBoxRefresh1_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(0);
}

void W_SlaveComm::on_comboBoxRefresh2_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(1);
}

void W_SlaveComm::on_comboBoxRefresh3_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(2);
}

void W_SlaveComm::on_comboBoxRefresh4_currentIndexChanged(int index)
{
	(void)index;	//Unused for now
	configSlaveComm(3);
}

void W_SlaveComm::on_checkBoxLog1_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now
	configSlaveComm(0);
}

void W_SlaveComm::on_checkBoxLog2_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now
	configSlaveComm(1);
}

void W_SlaveComm::on_checkBoxLog3_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now
	configSlaveComm(2);
}

void W_SlaveComm::on_checkBoxLog4_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now
	configSlaveComm(3);
}
