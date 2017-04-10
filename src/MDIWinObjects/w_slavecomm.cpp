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
#include "../flexsea-user/inc/flexsea_cmd_user.h"

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
						 QList<FlexseaDevice*> *testBenchDevListInit,
						 StreamManager* sm) :
	QWidget(parent),
	streamManager(sm),
	ui(new Ui::W_SlaveComm)
{
	if(!sm) qDebug("Null StreamManager passed to SlaveComm Window.");

	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	executeDevList = executeDevListInit;
	manageDevList = manageDevListInit;
	gossipDevList = gossipDevListInit;
	batteryDevList = batteryDevListInit;
	strainDevList = strainDevListInit;

	ricnuDevList = ricnuDevListInit;
	ankle2DofDevList = ankle2DofDevListInit;
	testBenchDevList = testBenchDevListInit;

	initializeMaps();
	mapSerializedPointers();
	initExperimentList();
	initSlaveCom();
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
void W_SlaveComm::receiveComPortStatus(bool status)
{
	if(status == false)
	{
		qDebug() << "COM port was closed";

		on_off_pb_ptr[0]->setDisabled(true);
		managePushButton(0,true);
		displayDataReceived(0, DATAIN_STATUS_GREY);
		log_cb_ptr[0]->setDisabled(true);
		auto_checkbox[0]->setDisabled(true);
	}
	else
	{
		qDebug() << "COM port was opened";

		on_off_pb_ptr[0]->setDisabled(false);
		log_cb_ptr[0]->setDisabled(false);
		auto_checkbox[0]->setDisabled(false);
	}
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

	testBenchTargetList.append(*executeDevList);
	testBenchTargetList.append(*manageDevList);

	batteryTargetList.append(*batteryDevList);
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
	targetListMap[6] = &testBenchTargetList;

	cmdMap[0] = CMD_READ_ALL;
	cmdMap[1] = CMD_IN_CONTROL;
	cmdMap[2] = CMD_READ_ALL_RICNU;
	cmdMap[3] = -1;
	cmdMap[4] = CMD_A2DOF;
	cmdMap[5] = CMD_BATT;
	cmdMap[6] = CMD_MOTORTB;

	numExperiments = 7;
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
	QString on_off_pb_ttip = "<html><head/><body><p>Turn streaming on/off</p></body></html>";
	QString labelStatusttip = "<html><head/><body><p>Stream Status.</p></body></html>";

	//Safeguard - protected from signals emited during setup
	allComboBoxesPopulated = false;

	//Status bar:
	//===========

	ui->statusbar->setTextFormat(Qt::RichText);
	updateStatusBar("Slave communication object created. Ready.");

	QStringList refreshRateStrings;
	QList<int> refreshRates = streamManager->getRefreshRates();
	for(int i = 0; i < refreshRates.size(); i++)
		refreshRateStrings << (QString::number(refreshRates.at(i)) + "Hz");

	QFont font( "Arial", 12, QFont::Bold);

	for(int row = 0; row < MAX_SC_ITEMS; row++)
	{
		if(comboBoxExpPtr[row])
		{
			//Fill the experiment combo box
			comboBoxExpPtr[row]->addItems(FlexSEA_Generic::var_list_exp);
			//Fill the slave combo box accordingly
			int selectedExperimentIndex = comboBoxExpPtr[row]->currentIndex();
			this->populateSlaveComboBox(comboBoxSlavePtr[row], selectedExperimentIndex);
			comboBoxRefreshPtr[row]->addItems(refreshRateStrings);
			comboBoxRefreshPtr[row]->setCurrentIndex(6);	//100Hz
		}

		//Log checkboxes:
		(log_cb_ptr[row])->setChecked(false);
		(log_cb_ptr[row])->setEnabled(false);
		(log_cb_ptr[row])->setToolTip(log_cb_ptr_ttip);

		(auto_checkbox[row])->setChecked(false);
		(auto_checkbox[row])->setEnabled(false);

		//On/Off Button init:
		(on_off_pb_ptr[row])->setText(QChar(0x2718));
		(on_off_pb_ptr[row])->setAutoFillBackground(true);
		(on_off_pb_ptr[row])->setStyleSheet("background-color: rgb(127, 127, 127); \
										color: rgb(0, 0, 0)");
		(on_off_pb_ptr[row])->setToolTip(on_off_pb_ttip);
		(on_off_pb_ptr[row])->setDisabled(true);

		// Label int:
		// Whites space are to allow balanced scale-up between on-off and label.
		(labelStatusPtr[row])->setText("      " + QString(QChar(0x2B07)) + "      ");
		(labelStatusPtr[row])->setAlignment(Qt::AlignCenter);
		(labelStatusPtr[row])->setFont(font);
		(labelStatusPtr[row])->setToolTip(labelStatusttip);
		displayDataReceived(row,DATAIN_STATUS_GREY);
	}

	//For now, rows 2-4 are disabled:
	//======================================
	for(int row = 1; row < MAX_SC_ITEMS; row++)
	{
		setRowDisabled(row, true);
		(on_off_pb_ptr[row])->setDisabled(true);
	}

	//Default command line settings, RIC/NU:
	streamManager->ricnuOffsets = QList<int>({0, 1});
	defaultCmdLineText = "o=0,1;";
	ui->lineEdit->setEnabled(false);
	ui->lineEdit->setText(" ");

	allComboBoxesPopulated = true;
	return;
}

void W_SlaveComm::manageSelectedExperimentChanged(int row)
{
	int indexOfNewlySelectedExperiment = comboBoxExpPtr[row]->currentIndex();
	this->populateSlaveComboBox(comboBoxSlavePtr[row], indexOfNewlySelectedExperiment);

	//ricnu has cmd line arguments
	ui->lineEdit->setEnabled(indexOfNewlySelectedExperiment == 2);
	ui->lineEdit->setText(indexOfNewlySelectedExperiment == 2 ? defaultCmdLineText : " ");
}

void W_SlaveComm::setRowDisabled(int row, bool disabled)
{
	(comboBoxSlavePtr[row])->setDisabled(disabled);
	(comboBoxExpPtr[row])->setDisabled(disabled);
	(comboBoxRefreshPtr[row])->setDisabled(disabled);
	(log_cb_ptr[row])->setDisabled(disabled);
	(labelStatusPtr[row])->setDisabled(disabled);
	(auto_checkbox[row])->setDisabled(disabled);
}

// This is a hack, basically just doing what we did before, which is hard coding it
// To properly fix this we have to have a separate class for projects vs devices
FlexseaDevice* W_SlaveComm::getTargetDevice(int cmd, int experimentIndex, int slaveIndex)
{
	FlexseaDevice* target = nullptr;

	switch(cmd)
	{
	case CMD_READ_ALL_RICNU:
		target = ricnuDevList->at(0);
		break;
	case CMD_A2DOF:
		target = ankle2DofDevList->at(0);
		break;
	case CMD_MOTORTB:
		target = testBenchDevList->at(0);
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

	if(cmdCode < 0) return;

	int refreshRate = streamManager->getRefreshRates()[refreshRateIndex];
	FlexseaDevice* target = getTargetDevice(cmdCode, experimentIndex, slaveIndex);
	int slaveId = (targetListMap[experimentIndex])->at(slaveIndex)->slaveID;

	if(on_off_pb_ptr[row]->isChecked() == true &&
		forceOff == false && cmdCode >= 0)
	{
		// set button appearance
		on_off_pb_ptr[row]->setChecked(true);
		on_off_pb_ptr[row]->setText(QChar(0x2714));
		on_off_pb_ptr[row]->setStyleSheet("background-color: \
								rgb(0, 255, 0); color: rgb(0, 0, 0)");

		// start streaming
		target->frequency = refreshRate;
		target->experimentIndex = cmdCode;
		target->experimentName = comboBoxExpPtr[row]->currentText();

		if(auto_checkbox[row]->isChecked())
		{
			streamManager->startAutoStreaming(cmdCode, slaveId, refreshRate, log_cb_ptr[row]->isChecked(), target);
		}
		else
		{
			streamManager->startStreaming(cmdCode, slaveId, refreshRate, log_cb_ptr[row]->isChecked(), target);
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

		// start streaming
		if(cmdCode >= 0)
			streamManager->stopStreaming(cmdCode, slaveId, refreshRate);

		setRowDisabled(row, false);
		displayDataReceived(row,DATAIN_STATUS_GREY);
	}
}

void W_SlaveComm::updateStatusBar(QString txt)
{
	QString finalTxt = "<font color=#808080>[Status] " + txt + "</font>";
	ui->statusbar->setText(finalTxt);
}

//"Data Received" Arrows:
void W_SlaveComm::displayDataReceived(int item, int status)
{
	if(!on_off_pb_ptr[item]->isChecked())
		status = DATAIN_STATUS_GREY;

	switch(status)
	{
		case DATAIN_STATUS_GREY:
			(labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										rgb(127,127,127); color: black;}");
			break;
		case DATAIN_STATUS_GREEN:
			(labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										rgb(0,255,0); color: black;}");
			break;
		case DATAIN_STATUS_YELLOW:
			(labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										rgb(255,255,0); color: black;}");
			break;
		case DATAIN_STATUS_RED:
			(labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										rgb(255,0,0); color: black;}");
			break;
		default:
			(labelStatusPtr[item])->setStyleSheet("QLabel { background-color: \
										black; color: white;}");
			break;
	}
}

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

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_SlaveComm::on_pushButton1_clicked() { managePushButton(0, false); }
void W_SlaveComm::on_pushButton2_clicked() { managePushButton(1, false); }
void W_SlaveComm::on_pushButton3_clicked() { managePushButton(2, false); }
void W_SlaveComm::on_pushButton4_clicked() { managePushButton(3, false); }

void W_SlaveComm::on_comboBoxExp1_currentIndexChanged(int index) {
	(void)index;
	manageSelectedExperimentChanged(0);
}
void W_SlaveComm::on_comboBoxExp2_currentIndexChanged(int index) {	(void)index; manageSelectedExperimentChanged(1); }
void W_SlaveComm::on_comboBoxExp3_currentIndexChanged(int index) {	(void)index; manageSelectedExperimentChanged(2); }
void W_SlaveComm::on_comboBoxExp4_currentIndexChanged(int index) {	(void)index; manageSelectedExperimentChanged(3); }

//Command line input: enter pressed
void W_SlaveComm::on_lineEdit_returnPressed()
{
	qDebug() << "Command line:";
	QString txt = ui->lineEdit->text();
	QChar cmd = txt.at(0);
	int cmdInt = cmd.toLatin1();
	int len = txt.length();

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
					offsets.append(QString(offsetString).toInt());
					qDebug() << "[o]ffset[:" << i << "] =" << offsetString;

				}
				else
				{
					qDebug() << "Invalid [o]ffset";
				}
			}

			streamManager->ricnuOffsets = offsets;
			break;

		default:
			qDebug() << "Unknown command";
			break;
	}

	//qDebug() << "Result: " << offsetArray;
}
