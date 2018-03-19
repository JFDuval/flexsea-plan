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
#include "flexsea_cmd_in_control.h"
#include "main.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <flexsea_comm.h>
#include <flexsea_sys_def.h>
#include "../flexsea-projects/inc/flexsea_cmd_user.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_SlaveComm::W_SlaveComm(QWidget *parent,
						 QList<FlexseaDevice*> *executeDevListInit,
						 QList<FlexseaDevice*> *manageDevListInit,
						 QList<FlexseaDevice*> *gossipDevListInit,
						 QList<FlexseaDevice*> *batteryDevListInit,
						 QList<FlexseaDevice*> *strainDevListInit,
						 QList<FlexseaDevice*> *ricnuDevListInit,
						 QList<FlexseaDevice*> *ankle2DofDevListInit,
						 QList<FlexseaDevice*> *dynamicUserDevListInit,
						 QList<FlexseaDevice*> *rigidDevListInit,
						 QList<FlexseaDevice*> *pocketDevListInit,
						 QList<ComManager*> *comManagerListInit) :
	QWidget(parent),
	ui(new Ui::W_SlaveComm)
{
	if(!comManagerListInit) qDebug("Null RefreshRates passed to SlaveComm Window.");
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	//Used to register the custom type for the signal/slot function using it.
	qRegisterMetaType<QList<int>>();

	executeDevList = executeDevListInit;
	manageDevList = manageDevListInit;
	gossipDevList = gossipDevListInit;
	batteryDevList = batteryDevListInit;
	strainDevList = strainDevListInit;

	ricnuDevList = ricnuDevListInit;
	ankle2DofDevList = ankle2DofDevListInit;
	rigidDevList = rigidDevListInit;
	pocketDevList = pocketDevListInit;
	dynamicUserDevList = dynamicUserDevListInit;
	ComManagerList = comManagerListInit;

	initializeMaps();
	mapSerializedPointers();
	initExperimentList();
	initSlaveCom();
	initTimers();
	initConnection();

	#ifdef DEMO_1DOF
	ui->layoutUserEdit->hide();
	ui->layoutStatus->hide();
	rowShow(1, false);
	setRowDisabled(0, true);
	auto_checkbox[0]->setChecked(true);
	#endif

	//Always hiding last 2 rows:
	rowShow(2, false);
	rowShow(3, false);
}

W_SlaveComm::~W_SlaveComm()
{
	emit windowClosed();
	delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

void W_SlaveComm::addExperiment(QList<FlexseaDevice *> *deviceList, int cmdCode)
{
	if(numExperiments >= MAX_EXPERIMENTS) return;

	targetListMap[numExperiments] = deviceList;
	cmdMap[numExperiments] = cmdCode;

	numExperiments++;
}

void W_SlaveComm::getCurrentDevice(FlexseaDevice** device)
{
	*device = nullptr;

	if(ui && ui->comboBoxExp1 && ui->comboBoxSlave1)
	{
		int experimentIndex = ui->comboBoxExp1->currentIndex();
		int slaveIndex = ui->comboBoxSlave1->currentIndex();

		if(experimentIndex >= 0 && slaveIndex >= 0)
		{
			*device = (targetListMap[experimentIndex])->at(slaveIndex);
		}
	}
}

void W_SlaveComm::getSlaveId(int* slaveId)
{
	*slaveId = -1;

	if(ui && ui->comboBoxExp1 && ui->comboBoxSlave1)
	{
		int experimentIndex = ui->comboBoxExp1->currentIndex();
		int slaveIndex = ui->comboBoxSlave1->currentIndex();

		if(experimentIndex >= 0 && slaveIndex >= 0)
		{
			*slaveId = (targetListMap[experimentIndex])->at(slaveIndex)->slaveID;
		}
	}
}

//****************************************************************************
// Public slot(s):
//****************************************************************************

//This slot gets called when the port status changes (turned On or Off)
void W_SlaveComm::receiveComPortStatus0(SerialPortStatus status,int nbTries)
{
	receiveComPortStatus(0, status, nbTries);
}

void W_SlaveComm::receiveComPortStatus1(SerialPortStatus status,int nbTries)
{
	receiveComPortStatus(1, status, nbTries);
}

void W_SlaveComm::receiveComPortStatus(int row, SerialPortStatus status,int nbTries)
{
	// Not use by this slot.
	(void)nbTries;

	if(status == PortClosed)
	{
		qDebug() << "COM port was closed";

		on_off_pb_ptr[row]->setDisabled(true);
		managePushButton(row,true);
		displayDataReceived(row, DATAIN_STATUS_GREY);
		log_cb_ptr[row]->setDisabled(true);
		auto_checkbox[row]->setDisabled(true);
	}
	else if(status == PortOpeningSucceed)
	{
		qDebug() << "COM port was opened";

		on_off_pb_ptr[row]->setDisabled(false);
		#ifndef DEMO_1DOF
		log_cb_ptr[row]->setDisabled(false);
		auto_checkbox[row]->setDisabled(false);
		#endif
	}
}

void W_SlaveComm::updateIndicatorTimeout0(bool rst)
{
	updateIndicatorTimeout(0, rst);
}

void W_SlaveComm::updateIndicatorTimeout1(bool rst)
{
	updateIndicatorTimeout(1, rst);
}

void W_SlaveComm::updateIndicatorTimeout(int row, bool rst)
{
	static uint32_t counter[2] = {0,0};

	counter[row]++;
	if(counter[row] > INDICATOR_TIMEOUT)
	{
		displayDataReceived(row, DATAIN_STATUS_GREY);
		if(isStreaming){qDebug() << "Timeout on communication";}
	}

	if(rst == true)
	{
		counter[row] = 0;
	}
}

void W_SlaveComm::displayDataReceived0(int row, int status)
{
	// Not use by this slot.
	(void)row;
	displayDataReceived(0, status);
}

void W_SlaveComm::displayDataReceived1(int row, int status)
{
	// Not use by this slot.
	(void)row;
	displayDataReceived(1, status);
}

//"Data Received" Arrows:
void W_SlaveComm::displayDataReceived(int row, int status)
{
	if(!on_off_pb_ptr[row]->isChecked())
		status = DATAIN_STATUS_GREY;

	switch(status)
	{
		case DATAIN_STATUS_GREY:
			(labelStatusPtr[row])->setStyleSheet("QLabel { background-color: \
										rgb(127,127,127); color: black;}");
			break;
		case DATAIN_STATUS_GREEN:
			(labelStatusPtr[row])->setStyleSheet("QLabel { background-color: \
										rgb(0,255,0); color: black;}");
			break;
		case DATAIN_STATUS_YELLOW:
			(labelStatusPtr[row])->setStyleSheet("QLabel { background-color: \
										rgb(255,255,0); color: black;}");
			break;
		case DATAIN_STATUS_RED:
			(labelStatusPtr[row])->setStyleSheet("QLabel { background-color: \
										rgb(255,0,0); color: black;}");
			break;
		default:
			(labelStatusPtr[row])->setStyleSheet("QLabel { background-color: \
										black; color: white;}");
			break;
	}
}

void W_SlaveComm::startExperiment(uint8_t i, int r, bool log, bool autoSample, QString offs, QString uNotes)
{
	if(i >= MAX_COMM_INTERFACES)
	{
		for(int j = 0; j < MAX_COMM_INTERFACES; j++)
		{
			startOneExperiment(j, r, log, autoSample, offs, uNotes);
		}
	}
	else
	{
		startOneExperiment(i, r, log, autoSample, offs, uNotes);
	}
}

void W_SlaveComm::startOneExperiment(uint8_t i, int r, bool log, bool autoSample, QString offs, QString uNotes)
{
	on_off_pb_ptr[i]->setChecked(true);

	//We set a few things based on what we received
	comboBoxRefreshPtr[i]->setCurrentIndex(r);
	log_cb_ptr[i]->setChecked(log);
	auto_checkbox[i]->setChecked(autoSample);
	ui->lineEdit->setText(offs);
	ui->lineEdit_userNotes->setText(uNotes);
	isStreaming = true;	//ToDo should be indexed

	managePushButton(i, false);
}

void W_SlaveComm::stopExperiment(uint8_t i)
{
	if(i >= MAX_COMM_INTERFACES)
	{
		for(int j = 0; j < MAX_COMM_INTERFACES; j++)
		{
			stopOneExperiment(j);
		}
	}
	else
	{
		stopOneExperiment(i);
	}
}

void W_SlaveComm::stopOneExperiment(uint8_t i)
{
	on_off_pb_ptr[i]->setChecked(false);
	isStreaming = false;	//ToDo should be indexed
	managePushButton(i, false);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_SlaveComm::initExperimentList(void)
{
	readAllTargetList.append(*executeDevList);
	readAllTargetList.append(*manageDevList);
	readAllTargetList.append(*gossipDevList);
	readAllTargetList.append(*strainDevList);

	inControlTargetList.append(*executeDevList);

	ricnuTargetList.append(*executeDevList);
	ricnuTargetList.append(*manageDevList);

	ankle2DofTargetList.append(*executeDevList);
	ankle2DofTargetList.append(*manageDevList);

	batteryTargetList.append(*executeDevList);
	batteryTargetList.append(*manageDevList);
	rigidTargetList.append(*manageDevList);
	pocketTargetList.append(*manageDevList);
	dynamicUserTargetList.append(*dynamicUserDevList);
}

void W_SlaveComm::mapSerializedPointers(void)
{
	comboBoxSlavePtr[0] = ui->comboBoxSlave1;
	comboBoxSlavePtr[1] = ui->comboBoxSlave2;
	comboBoxSlavePtr[2] = ui->comboBoxSlave3;
	comboBoxSlavePtr[3] = ui->comboBoxSlave4;
	comboBoxExpPtr[0] = ui->comboBoxExp1;
	comboBoxExpPtr[1] = ui->comboBoxExp2;
	comboBoxExpPtr[2] = ui->comboBoxExp3;
	comboBoxExpPtr[3] = ui->comboBoxExp4;
	comboBoxRefreshPtr[0] = ui->comboBoxRefresh1;
	comboBoxRefreshPtr[1] = ui->comboBoxRefresh2;
	comboBoxRefreshPtr[2] = ui->comboBoxRefresh3;
	comboBoxRefreshPtr[3] = ui->comboBoxRefresh4;
	log_cb_ptr[0] = ui->checkBoxLog1;
	log_cb_ptr[1] = ui->checkBoxLog2;
	log_cb_ptr[2] = ui->checkBoxLog3;
	log_cb_ptr[3] = ui->checkBoxLog4;
	auto_checkbox[0] = ui->autoCheckBox1;
	auto_checkbox[1] = ui->autoCheckBox2;
	auto_checkbox[2] = ui->autoCheckBox3;
	auto_checkbox[3] = ui->autoCheckBox4;
	on_off_pb_ptr[0] = ui->pushButton1;
	on_off_pb_ptr[1] = ui->pushButton2;
	on_off_pb_ptr[2] = ui->pushButton3;
	on_off_pb_ptr[3] = ui->pushButton4;
	labelStatusPtr[0] = ui->stat1;
	labelStatusPtr[1] = ui->stat2;
	labelStatusPtr[2] = ui->stat3;
	labelStatusPtr[3] = ui->stat4;
	labelRowPtr[0] = ui->labelRow1;
	labelRowPtr[1] = ui->labelRow2;
	labelRowPtr[2] = ui->labelRow3;
	labelRowPtr[3] = ui->labelRow4;
}

void W_SlaveComm::initializeMaps()
{
	for(int i = 0; i < MAX_EXPERIMENTS; i++)
	{
		targetListMap[i] = nullptr;
		cmdMap[i] = -1;
	}

	targetListMap[0] = &readAllTargetList;
	targetListMap[1] = &inControlTargetList;
	targetListMap[2] = &ricnuTargetList;
	targetListMap[3] = nullptr;
	targetListMap[4] = &ankle2DofTargetList;
	targetListMap[5] = &batteryTargetList;
	//targetListMap[6] = &;
	targetListMap[7] = &dynamicUserTargetList;
	//targetListMap[8] = &;
	targetListMap[9] = &rigidTargetList;
	targetListMap[10]= &pocketTargetList;

	cmdMap[0] = CMD_READ_ALL;
	cmdMap[1] = CMD_IN_CONTROL;
	cmdMap[2] = CMD_RICNU;
	cmdMap[3] = -1;
	cmdMap[4] = CMD_A2DOF;
	cmdMap[5] = CMD_BATT;
	cmdMap[6] = CMD_MOTORTB;
	cmdMap[7] = CMD_USER_DYNAMIC;
	//cmdMap[8] = ;
	cmdMap[9] = CMD_READ_ALL_RIGID;
	cmdMap[10] = CMD_READ_ALL_POCKET;

	numExperiments = 12;
}

void W_SlaveComm::initTimers(void)
{
	dataTimeoutList.append(new QTimer(this));
	connect(dataTimeoutList.last(),	&QTimer::timeout,
			this,			&W_SlaveComm::dataTimeoutEvent0);
	dataTimeoutList.last()->start(DATA_TIMEOUT);

	dataTimeoutList.append(new QTimer(this));
	connect(dataTimeoutList.last(),	&QTimer::timeout,
			this,			&W_SlaveComm::dataTimeoutEvent1);
	dataTimeoutList.last()->start(DATA_TIMEOUT);
}

void W_SlaveComm::initConnection(void)
{
		connect(this,			&W_SlaveComm::setOffsetParameter0, \
				ComManagerList->at(0),	&ComManager::setOffsetParameter);

		connect(this,			&W_SlaveComm::startStreaming0, \
				ComManagerList->at(0),	&ComManager::startStreaming);

		connect(this,			SIGNAL(startAutoStreaming0(bool,FlexseaDevice*)), \
				ComManagerList->at(0),	SLOT(startAutoStreaming(bool, FlexseaDevice*)));

		connect(this,			SIGNAL(stopStreaming0(FlexseaDevice*)), \
				ComManagerList->at(0),	SLOT(stopStreaming(FlexseaDevice*)));

		connect(this,			&W_SlaveComm::setOffsetParameter1, \
				ComManagerList->at(1),	&ComManager::setOffsetParameter);

		connect(this,			&W_SlaveComm::startStreaming1, \
				ComManagerList->at(1),	&ComManager::startStreaming);

		connect(this,			SIGNAL(startAutoStreaming1(bool,FlexseaDevice*)), \
				ComManagerList->at(1),	SLOT(startAutoStreaming(bool, FlexseaDevice*)));

		connect(this,			SIGNAL(stopStreaming1(FlexseaDevice*)), \
				ComManagerList->at(1),	SLOT(stopStreaming(FlexseaDevice*)));



		connect(ComManagerList->at(0),	&ComManager::openStatus, \
				this,			&W_SlaveComm::receiveComPortStatus0);

		connect(ComManagerList->at(0),	&ComManager::dataStatus, \
				this,			&W_SlaveComm::displayDataReceived0);

		connect(ComManagerList->at(0),	&ComManager::newDataTimeout, \
				this,			&W_SlaveComm::updateIndicatorTimeout0);

		connect(ComManagerList->at(1),	&ComManager::openStatus, \
				this,			&W_SlaveComm::receiveComPortStatus1);

		connect(ComManagerList->at(1),	&ComManager::dataStatus, \
				this,			&W_SlaveComm::displayDataReceived1);

		connect(ComManagerList->at(1),	&ComManager::newDataTimeout, \
				this,			&W_SlaveComm::updateIndicatorTimeout1);
}

void W_SlaveComm::populateSlaveComboBox(QComboBox* box, int indexOfExperimentSelected)
{
	QString currentSlaveString = box->currentText();

	while(box->count() > 0)
		box->removeItem(0);

	QList<FlexseaDevice*> *listOfTargetsAvailableForExperiment = targetListMap[indexOfExperimentSelected];

	if(!listOfTargetsAvailableForExperiment)
	{
		box->addItem("N/A");
		return;
	}

	int indexCorrespondingToPreviouslySelected = -1;
	for(int i = 0; i < listOfTargetsAvailableForExperiment->size(); i++)
	{
		QString slaveString = listOfTargetsAvailableForExperiment->at(i)->slaveName;
		if(QString::compare(currentSlaveString, slaveString) == 0)
			indexCorrespondingToPreviouslySelected = i;

		box->addItem(listOfTargetsAvailableForExperiment->at(i)->slaveName);
	}

	if(indexCorrespondingToPreviouslySelected >= 0)
		box->setCurrentIndex(indexCorrespondingToPreviouslySelected);
}

void W_SlaveComm::initSlaveCom(void)
{
	QString log_cb_ptr_ttip = "<html><head/><body><p>Check this box to log the stream. \
			</p><p>It will log under the folder &quot;Plan-GUI-Logs&quot; \
			when the stream is active.</p></body></html>";
	QString auto_cb_ptr_ttip = "<html><head/><body><p>In Auto mode, Plan sends one\
							  read request and the slave starts streaming data at \
							  the desired frequency (unidirectional communication).</p>\
							  <p>In Normal mode one Read request is needed per Reply\
							  (bidirectional communication).</p></body></html>";
	QString on_off_pb_ttip = "<html><head/><body><p>Turn streaming on/off</p></body></html>";
	QString labelStatusttip = "<html><head/><body><p>Stream Status.</p></body></html>";
	QString labelOnOffTitle = "<html><head/><body><p>Activate the data stream\
							  for analysing window.</p></body></html>";
	QString lineEdit_ttip = "<html><head/><body><p>In regular mode entering '0,3' will read 2 \
							values (0 and 3). In Auto mode, it will take the min and max. \
							With the same input, it will read 4 values.</p></body></html>";

	//Safeguard - protected from signals emited during setup
	allComboBoxesPopulated = false;

	//Status bar:
	//===========

	ui->statusbar->setTextFormat(Qt::RichText);
	updateStatusBar("Slave communication object created. Ready.");

	QStringList refreshRateStrings;

	QFont font( "Arial", 12, QFont::Bold);

	for(int row = 0; row < MAX_SC_ITEMS; row++)
	{

		if(row < ComManagerList->length())
		{
			refreshRates = ComManagerList->at(row)->getRefreshRates();
			for(int i = 0; i < refreshRates.size(); i++)
				refreshRateStrings << (QString::number(refreshRates.at(i)) + "Hz");
		}

		if(comboBoxExpPtr[row])
		{
			//Fill the experiment combo box
			comboBoxExpPtr[row]->addItems(FlexSEA_Generic::var_list_exp);
			//Start with Rigid, CL enabled:
			comboBoxExpPtr[row]->setCurrentIndex(FlexSEA_Generic::var_list_exp.count()-3);

			//Fill the slave combo box accordingly
			int selectedExperimentIndex = comboBoxExpPtr[row]->currentIndex();
			this->populateSlaveComboBox(comboBoxSlavePtr[row], selectedExperimentIndex);
			comboBoxRefreshPtr[row]->addItems(refreshRateStrings);
			comboBoxRefreshPtr[row]->setCurrentIndex(6);	//100Hz
		}

		//Start 2nd row as 2nd slave:
		comboBoxSlavePtr[1]->setCurrentIndex(1);

		//Log checkboxes:
		(log_cb_ptr[row])->setChecked(false);
		(log_cb_ptr[row])->setEnabled(false);
		(log_cb_ptr[row])->setToolTip(log_cb_ptr_ttip);
		ui->labelLog->setToolTip(log_cb_ptr_ttip);

		(auto_checkbox[row])->setChecked(false);
		(auto_checkbox[row])->setEnabled(false);
		(auto_checkbox[row])->setToolTip(auto_cb_ptr_ttip);
		ui->labelAuto->setToolTip(auto_cb_ptr_ttip);

		//On/Off Button init:
		(on_off_pb_ptr[row])->setText(QChar(0x2718));
		(on_off_pb_ptr[row])->setAutoFillBackground(true);
		(on_off_pb_ptr[row])->setStyleSheet("background-color: rgb(127, 127, 127); \
										color: rgb(0, 0, 0)");
		(on_off_pb_ptr[row])->setToolTip(on_off_pb_ttip);
		(on_off_pb_ptr[row])->setDisabled(true);
		ui->labelOnOffTitle->setToolTip(labelOnOffTitle);

		// Label int:
		// Whites space are to allow balanced scale-up between on-off and label.
		(labelStatusPtr[row])->setText("      " + QString(QChar(0x2B07)) + "      ");
		(labelStatusPtr[row])->setAlignment(Qt::AlignCenter);
		(labelStatusPtr[row])->setFont(font);
		(labelStatusPtr[row])->setToolTip(labelStatusttip);
		displayDataReceived(row,DATAIN_STATUS_GREY);
	}

	//By default, disable, all row:
	//======================================
	for(int row = 0; row < MAX_SC_ITEMS; row++)
	{
		setRowDisabled(row, true);
	}

	//Default command line settings, RIC/NU & Rigid:
	emit setOffsetParameter0(QList<int>({0, 1}), QList<int>({0, 1}), 0, 0);
	emit setOffsetParameter1(QList<int>({0, 1}), QList<int>({0, 1}), 0, 0);

	defaultCmdLineText = "o=0,1,2,3;";
	//We start with Rigid, so we enable the CL:
	ui->lineEdit->setEnabled(true);
	ui->lineEdit->setText(defaultCmdLineText);
	ui->lineEdit->setToolTip(lineEdit_ttip);

	allComboBoxesPopulated = true;

	//User notes:
	ui->lineEdit_userNotes->setEnabled(true);

	return;
}

void W_SlaveComm::manageSelectedExperimentChanged(int row)
{
	int expIndex = comboBoxExpPtr[row]->currentIndex();
	this->populateSlaveComboBox(comboBoxSlavePtr[row], expIndex);

	//RICNU, Rigid and Pocket have cmd line arguments:
	bool en = (expIndex == 2 || expIndex == 9 || expIndex == 10);
	ui->lineEdit->setEnabled(en);
	ui->lineEdit->setText(en ? defaultCmdLineText : " ");
}

void W_SlaveComm::setRowDisabled(int row, bool disabled)
{
	comboBoxSlavePtr[row]->setDisabled(disabled);
	comboBoxExpPtr[row]->setDisabled(disabled);
	comboBoxRefreshPtr[row]->setDisabled(disabled);
	log_cb_ptr[row]->setDisabled(disabled);
	labelStatusPtr[row]->setDisabled(disabled);
	auto_checkbox[row]->setDisabled(disabled);
}

// This is a hack, basically just doing what we did before, which is hard coding it
// To properly fix this we have to have a separate class for projects vs devices
FlexseaDevice* W_SlaveComm::getTargetDevice(int cmd, int experimentIndex, int slaveIndex)
{
	FlexseaDevice* target = nullptr;

	switch(cmd)
	{
		case CMD_BATT:
			target = batteryDevList->at(0);
			break;
		case CMD_RICNU:
			target = ricnuDevList->at(0);
			break;
		case CMD_A2DOF:
			target = ankle2DofDevList->at(0);
			break;
			break;
		case CMD_READ_ALL_RIGID:
			target = rigidDevList->at(slaveIndex);
			break;
		case CMD_READ_ALL_POCKET:
			target = pocketDevList->at(slaveIndex);
			break;
		default:
			target = (targetListMap[experimentIndex])->at(slaveIndex);
			break;
	}

	return target;
}

//The 4 PB slots call this function:
void W_SlaveComm::managePushButton(int row, bool forceOff)
{
	int experimentIndex = comboBoxExpPtr[row]->currentIndex();
	int slaveIndex = comboBoxSlavePtr[row]->currentIndex();
	int refreshRateIndex = comboBoxRefreshPtr[row]->currentIndex();
	int cmdCode = cmdMap[experimentIndex];
	static bool cmdLineState[4] = {false, false, false, false};

	if(cmdCode < 0) return;

	FlexseaDevice* target = getTargetDevice(cmdCode, experimentIndex, slaveIndex);
	target->slaveID = (targetListMap[experimentIndex])->at(slaveIndex)->slaveID;
	target->frequency = refreshRates.at(refreshRateIndex);
	target->experimentIndex = cmdCode;
	target->experimentName = comboBoxExpPtr[row]->currentText();

	if(on_off_pb_ptr[row]->isChecked() == true &&
		forceOff == false && cmdCode >= 0)
	{
		// set button appearance
		on_off_pb_ptr[row]->setChecked(true);
		on_off_pb_ptr[row]->setText(QChar(0x2714));
		on_off_pb_ptr[row]->setStyleSheet("background-color: \
								rgb(0, 255, 0); color: rgb(0, 0, 0)");

		//Disable command line:
		cmdLineState[row] = ui->lineEdit->isEnabled();
		if(cmdLineState[row] == true)
		{
			//If it was enabled we disable it and read it:
			ui->lineEdit->setDisabled(true);
			readCommandLine(row);
		}

		// start streaming

		//User notes, with any comma replaced by a space:
		QString uNotes = ui->lineEdit_userNotes->text();
		uNotes.replace(","," ");
		ui->lineEdit_userNotes->setText(uNotes);
		target->userNotes = uNotes;

		ui->lineEdit_userNotes->setEnabled(false);

		emit activeSlaveStreaming(target->slaveName);

		if(auto_checkbox[row]->isChecked())
		{
			if(row == 0)
			{
				emit startAutoStreaming0(log_cb_ptr[row]->isChecked(), target);
			}
			else
			{
				emit startAutoStreaming1(log_cb_ptr[row]->isChecked(), target);
			}
		}
		else
		{
			if(row == 0)
			{
				emit startStreaming0(log_cb_ptr[row]->isChecked(), target);
			}
			else
			{
				emit startStreaming1(log_cb_ptr[row]->isChecked(), target);
			}
		}

		setRowDisabled(row, true);
	}
	else
	{
		// set button appearance
		(on_off_pb_ptr[row])->setChecked(false);
		(on_off_pb_ptr[row])->setText(QChar(0x2718));
		(on_off_pb_ptr[row])->setStyleSheet("background-color: \
						rgb(127, 127, 127); color: rgb(0, 0, 0)");

		//Enable command line if needed:
		if(cmdLineState[row] == true)
		{
			ui->lineEdit->setEnabled(true);
		}

		ui->lineEdit_userNotes->setEnabled(true);

		// Used to open view window by default.
		emit activeSlaveStreaming("None");

		// start streaming
		if(cmdCode >= 0)
		{
			if(row == 0)
			{
				emit stopStreaming0(target);
			}
			else
			{
				emit stopStreaming1(target);
			}
		}

		#ifndef DEMO_1DOF
		setRowDisabled(row, false);
		#else
		setRowDisabled(row, true);
		#endif
		displayDataReceived(row,DATAIN_STATUS_GREY);
	}
}

void W_SlaveComm::updateStatusBar(QString txt)
{
	QString finalTxt = "<font color=#808080>[Status] " + txt + "</font>";
	ui->statusbar->setText(finalTxt);
}

//s = true makes a line visible
void W_SlaveComm::rowShow(int row, bool s)
{
	if(row < 0 || row >= MAX_SC_ITEMS){return;}

	comboBoxExpPtr[row]->setHidden(!s);
	comboBoxSlavePtr[row]->setHidden(!s);
	comboBoxRefreshPtr[row]->setHidden(!s);
	log_cb_ptr[row]->setHidden(!s);
	auto_checkbox[row]->setHidden(!s);
	on_off_pb_ptr[row]->setHidden(!s);
	labelStatusPtr[row]->setHidden(!s);
	labelRowPtr[row]->setHidden(!s);
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_SlaveComm::on_pushButton1_clicked() { managePushButton(0, false); }
void W_SlaveComm::on_pushButton2_clicked() { managePushButton(1, false); }
void W_SlaveComm::on_pushButton3_clicked() { managePushButton(2, false); }
void W_SlaveComm::on_pushButton4_clicked() { managePushButton(3, false); }

void W_SlaveComm::on_comboBoxExp1_currentIndexChanged(int index)
{
	(void)index;
	manageSelectedExperimentChanged(0);
}

void W_SlaveComm::on_comboBoxExp2_currentIndexChanged(int index) {(void)index; manageSelectedExperimentChanged(1); }
void W_SlaveComm::on_comboBoxExp3_currentIndexChanged(int index) {(void)index; manageSelectedExperimentChanged(2); }
void W_SlaveComm::on_comboBoxExp4_currentIndexChanged(int index) {(void)index; manageSelectedExperimentChanged(3); }

//Command line input: enter pressed
void W_SlaveComm::readCommandLine(int row)
{
	qDebug() << "Command line:";
	QString txt = ui->lineEdit->text();
	QChar cmd = txt.at(0);
	int cmdInt = cmd.toLatin1();
	int len = txt.length();
	int tmp = 0, min = 0, max = 0;

	if(txt.at(len-1) == ';')
	{
		qDebug() << "Properly terminated command.";
	}
	else
	{
		return;
	}

	len -= 3;	//We only care about the offsets, not the framing

	int numEntries = (len+1)/2;
	qDebug() << "Entries:" << numEntries;

	QList<int> offsets;
	QChar offsetString;
	switch(cmdInt)
	{
		case 'o':
			for(int i = 0; i < numEntries; i++)
			{
				offsetString = txt.at(2 + 2*i).toLatin1();
				if(offsetString.isDigit())
				{
					tmp = QString(offsetString).toInt();
					offsets.append(tmp);
					qDebug() << "[o]ffset[:" << i << "] =" << offsetString;

					//Update min & max offsets:
					if(i == 0)
					{
						//First time:
						min = tmp;
						max = tmp;
					}
					else
					{
						if(tmp < min){min = tmp;}
						if(tmp > max){max = tmp;}
					}
				}
				else
				{
					qDebug() << "Invalid [o]ffset";
				}
			}

			qDebug() << "Min offset:" << min << ", Max offset:" << max;

			if(row == 0)
			{
				emit setOffsetParameter0(offsets, offsets, min, max);
			}
			else
			{
				emit setOffsetParameter1(offsets, offsets, min, max);
			}

			break;

		default:
			qDebug() << "Unknown command";
			break;
	}

	//qDebug() << "Result: " << offsetArray;
}

void W_SlaveComm::dataTimeoutEvent0(void)
{
	updateIndicatorTimeout(0, false);
}

void W_SlaveComm::dataTimeoutEvent1(void)
{
	updateIndicatorTimeout(1, false);
}
