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

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_SlaveComm::W_SlaveComm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_SlaveComm)
{
    ui->setupUi(this);

    setWindowTitle("Slave Communication");
    setWindowIcon(QIcon(":icons/d_logo_small.png"));

    initSlaveCom();
    initTimers();
}

W_SlaveComm::~W_SlaveComm()
{
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
            emit closeLogFile(0);   //ToDo support multiple files
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
    //***ToDo Fix***
    myFlexSEA_Generic.packetVisualizer(numb, tx_data);
    emit slaveReadWrite(numb, tx_data, WRITE);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_SlaveComm::initSlaveCom(void)
{
    QString slave_name;

    //Safeguard - protected from signals emited during setup
    allComboBoxesPopulated = false;

    //Status bar:
    //===========

    ui->statusbar->setTextFormat(Qt::RichText);
    updateStatusBar("Slave communication object created. Ready.");

    //State variables:
    //================
    sc_comPortOpen = false;
    pb_state[0] = 0;
    pb_state[1] = 0;
    pb_state[2] = 0;
    pb_state[3] = 0;
    logThisItem[0] = false;
    logThisItem[1] = false;
    logThisItem[2] = false;
    logThisItem[3] = false;

    //On/Off Button:
    //==============

    ui->pushButton1->setText(QChar(0x2718));
    ui->pushButton1->setAutoFillBackground(true);
    ui->pushButton1->setStyleSheet("background-color: rgb(127, 127, 127); \
                                    color: rgb(0, 0, 0)");
    ui->pushButton2->setText(QChar(0x2718));
    ui->pushButton2->setAutoFillBackground(true);
    ui->pushButton2->setStyleSheet("background-color: rgb(127, 127, 127); \
                                   color: rgb(0, 0, 0)");
    ui->pushButton3->setText(QChar(0x2718));
    ui->pushButton3->setAutoFillBackground(true);
    ui->pushButton3->setStyleSheet("background-color: rgb(127, 127, 127); \
                                  color: rgb(0, 0, 0)");
    ui->pushButton4->setText(QChar(0x2718));
    ui->pushButton4->setAutoFillBackground(true);
    ui->pushButton4->setStyleSheet("background-color: rgb(127, 127, 127); \
                                  color: rgb(0, 0, 0)");

    //Log checkboxes:
    //===============
    ui->checkBoxLog1->setChecked(false);
    ui->checkBoxLog2->setChecked(false);
    ui->checkBoxLog3->setChecked(false);
    ui->checkBoxLog4->setChecked(false);
    ui->checkBoxLog1->setEnabled(false);
    ui->checkBoxLog2->setEnabled(false);
    ui->checkBoxLog3->setEnabled(false);
    ui->checkBoxLog4->setEnabled(false);

    //All pushbutton start disabled:
    //==============================
    ui->pushButton1->setDisabled(true);
    ui->pushButton2->setDisabled(true);
    ui->pushButton3->setDisabled(true);
    ui->pushButton4->setDisabled(true);

    //Receive indicators:
    //===================
    initDisplayDataReceived();
    displayDataReceived(0,DATAIN_STATUS_GREY);
    displayDataReceived(1,DATAIN_STATUS_GREY);
    displayDataReceived(2,DATAIN_STATUS_GREY);
    displayDataReceived(3,DATAIN_STATUS_GREY);

    //Populates Slave list:
    //=====================
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBoxSlave1, SL_BASE_ALL, SL_LEN_ALL);
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBoxSlave2, SL_BASE_ALL, SL_LEN_ALL);
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBoxSlave3, SL_BASE_ALL, SL_LEN_ALL);
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBoxSlave4, SL_BASE_ALL, SL_LEN_ALL);

    //Variables:
    active_slave_index[0] = ui->comboBoxSlave1->currentIndex();
    active_slave_index[1] = ui->comboBoxSlave2->currentIndex();
    active_slave_index[2] = ui->comboBoxSlave3->currentIndex();
    active_slave_index[3] = ui->comboBoxSlave4->currentIndex();
    active_slave[0] = myFlexSEA_Generic.getSlaveID(SL_BASE_ALL, active_slave_index[0]);
    active_slave[1] = myFlexSEA_Generic.getSlaveID(SL_BASE_ALL, active_slave_index[1]);
    active_slave[2] = myFlexSEA_Generic.getSlaveID(SL_BASE_ALL, active_slave_index[2]);
    active_slave[3] = myFlexSEA_Generic.getSlaveID(SL_BASE_ALL, active_slave_index[3]);

    //Populates Experiment/Command list:
    //==================================

    myFlexSEA_Generic.populateExpComboBox(ui->comboBoxExp1);
    myFlexSEA_Generic.populateExpComboBox(ui->comboBoxExp2);
    myFlexSEA_Generic.populateExpComboBox(ui->comboBoxExp3);
    myFlexSEA_Generic.populateExpComboBox(ui->comboBoxExp4);

    //Refresh Rate:
    //==================================

    var_list_refresh << "100Hz" << "50Hz" << "33Hz" << "20Hz" \
                     << "10Hz" << "5Hz" << "1Hz";
    for(int index = 0; index < var_list_refresh.count(); index++)
    {
        ui->comboBoxRefresh1->addItem(var_list_refresh.at(index));
        ui->comboBoxRefresh2->addItem(var_list_refresh.at(index));
        ui->comboBoxRefresh3->addItem(var_list_refresh.at(index));
        ui->comboBoxRefresh4->addItem(var_list_refresh.at(index));
    }

    //Start at 33Hz:
    ui->comboBoxRefresh1->setCurrentIndex(2);
    ui->comboBoxRefresh2->setCurrentIndex(2);
    ui->comboBoxRefresh3->setCurrentIndex(2);
    ui->comboBoxRefresh4->setCurrentIndex(2);
    selected_refresh_index[0] = 2;
    selected_refresh_index[1] = 2;
    selected_refresh_index[2] = 2;
    selected_refresh_index[3] = 2;
    previous_refresh_index[0] = selected_refresh_index[0];
    previous_refresh_index[1] = selected_refresh_index[1];
    previous_refresh_index[2] = selected_refresh_index[2];
    previous_refresh_index[3] = selected_refresh_index[3];

    //ComboBoxes are all set:
    allComboBoxesPopulated = true;

    //Connect default slots:
    connectSCItem(0, 2, 0);
    connectSCItem(1, 2, 0);
    connectSCItem(2, 2, 0);
    connectSCItem(3, 2, 0);

    //For now, Experiments 2-4 are disabled:
    //======================================
    ui->comboBoxSlave2->setDisabled(true);
    ui->comboBoxExp2->setDisabled(true);
    ui->comboBoxRefresh2->setDisabled(true);
    ui->comboBoxSlave3->setDisabled(true);
    ui->comboBoxExp3->setDisabled(true);
    ui->comboBoxRefresh3->setDisabled(true);
    ui->comboBoxSlave4->setDisabled(true);
    ui->comboBoxExp4->setDisabled(true);
    ui->comboBoxRefresh4->setDisabled(true);
    ui->pushButton2->setDisabled(true);
    ui->pushButton3->setDisabled(true);
    ui->pushButton4->setDisabled(true);
}

void W_SlaveComm::initTimers(void)
{
    master_timer = new QTimer(this);
    connect(master_timer, SIGNAL(timeout()), this, SLOT(masterTimerEvent()));
    master_timer->start(TIM_FREQ_TO_P(MASTER_TIMER));
}

//Place pictograms on labels:
void W_SlaveComm::initDisplayDataReceived(void)
{
    QFont f( "Arial", 12, QFont::Bold);

    ui->stat1->setText(QChar(0x2B07));
    ui->stat1->setAlignment(Qt::AlignCenter);
    ui->stat1->setFont(f);

    ui->stat2->setText(QChar(0x2B07));
    ui->stat2->setAlignment(Qt::AlignCenter);
    ui->stat2->setFont(f);

    ui->stat3->setText(QChar(0x2B07));
    ui->stat3->setAlignment(Qt::AlignCenter);
    ui->stat3->setFont(f);

    ui->stat4->setText(QChar(0x2B07));
    ui->stat4->setAlignment(Qt::AlignCenter);
    ui->stat4->setFont(f);
}

//The 4 PB slots call this function:
void W_SlaveComm::managePushButton(int idx, bool forceOff)
{
    QPushButton **pb_ptr = &ui->pushButton1;
    QCheckBox **cb_ptr = &ui->checkBoxLog1;
    switch(idx)
    {
        case 0:
            pb_ptr = &ui->pushButton1;
            cb_ptr = &ui->checkBoxLog1;
            break;
        case 1:
            pb_ptr = &ui->pushButton2;
            cb_ptr = &ui->checkBoxLog2;
            break;
        case 2:
            pb_ptr = &ui->pushButton3;
            cb_ptr = &ui->checkBoxLog3;
            break;
        case 3:
            pb_ptr = &ui->pushButton4;
            cb_ptr = &ui->checkBoxLog4;
            break;
    }

    if(forceOff == true)
    {
        pb_state[idx] = 0;
        (*pb_ptr)->setText(QChar(0x2718));
        (*pb_ptr)->setStyleSheet("background-color: rgb(127, 127, 127); \
                                        color: rgb(0, 0, 0)");
        logThisItem[idx] = false;
    }
    else
    {
        //qDebug() << "PB[" << idx << "] clicked.";
        if(pb_state[idx] == 0)
        {
            //Turn ON:

            pb_state[idx] = 1;
            (*pb_ptr)->setText(QChar(0x2714));
            (*pb_ptr)->setStyleSheet("background-color: rgb(0, 255, 0); \
                                            color: rgb(0, 0, 0)");

            //Logging?
            if((*cb_ptr)->isChecked() == true)
            {
                logThisItem[idx] = true;
            }
            else
            {
                logThisItem[idx] = false;
            }
        }
        else
        {
            //Turn OFF:

            pb_state[idx] = 0;
            (*pb_ptr)->setText(QChar(0x2718));
            (*pb_ptr)->setStyleSheet("background-color: rgb(127, 127, 127); \
                                            color: rgb(0, 0, 0)");

             if(logThisItem[0] == true)
             {
                 logThisItem[0] = false;
                 emit closeLogFile(0);   //ToDo support multiple files
             }
        }
    }

    //All GUI events call configSlaveComm():
    configSlaveComm(idx);
}

void W_SlaveComm::updateStatusBar(QString txt)
{
    QString finalTxt = "<font color=#808080>[Status] " + txt + "</font>";
    ui->statusbar->setText(finalTxt);
}

//Connect a SlaveComm item with a timer
void W_SlaveComm::connectSCItem(int item, int sig_idx, int breakB4make)
{
    if(item == 0)
    {
        //Break old connection?
        if(breakB4make)
        {
            QObject::disconnect(sc_connections[item]);
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
        //ToDo Items 2-4
    }
}

//"Data Received" Arrows:
void W_SlaveComm::displayDataReceived(int idx, int status)
{
    QLabel **label_ptr = &ui->stat1;
    switch(idx)
    {
        case 0:
            label_ptr = &ui->stat1;
            break;
        case 1:
            label_ptr = &ui->stat2;
            break;
        case 2:
            label_ptr = &ui->stat3;
            break;
        case 3:
            label_ptr = &ui->stat4;
            break;
    }

    switch(status)
    {
        case DATAIN_STATUS_GREY:
            (*label_ptr)->setStyleSheet("QLabel { background-color: rgb(127,127,127); color: black;}");
            break;
        case DATAIN_STATUS_GREEN:
                (*label_ptr)->setStyleSheet("QLabel { background-color: rgb(0,255,0); color: black;}");
                break;
        case DATAIN_STATUS_YELLOW:
                (*label_ptr)->setStyleSheet("QLabel { background-color: rgb(255,255,0); color: black;}");
                break;
        case DATAIN_STATUS_RED:
                (*label_ptr)->setStyleSheet("QLabel { background-color: rgb(255,0,0); color: black;}");
                break;
        default:
            (*label_ptr)->setStyleSheet("QLabel { background-color: black; color: white;}");
            break;
    }
}

//This function will connect a Timer signal and a Slot. Updated when "something" changes.
void W_SlaveComm::configSlaveComm(int item)
{
    QString msg = "", msg_ref = "";

    if(allComboBoxesPopulated == true)
    {
        //qDebug() << "[In fct ""configSlaveComm"", item=" << item << "].";

        if(item == 0)
        {
            //Refresh all fields:
            active_slave_index[0] = ui->comboBoxSlave1->currentIndex();
            active_slave[0] = myFlexSEA_Generic.getSlaveID(SL_BASE_ALL, \
                                                           active_slave_index[0]);
            selected_exp_index[0] = ui->comboBoxExp1->currentIndex();
            selected_refresh_index[0] = ui->comboBoxRefresh1->currentIndex();

            //Now we connect a time slot to that stream command:
            if(previous_refresh_index[0] != selected_refresh_index[0])
            {
                //Refresh changed, we need to update connections.
                connectSCItem(0, selected_refresh_index[0], 1);

                msg_ref = "Changed connection.";
            }
            else
            {
                msg_ref = "";
            }

            //
        }
        else
        {
            //ToDo deal with Items 2-4 here
        }

        //Update status message:
        msg = "Updated #" + QString::number(item+1) + ": (" \
                + QString::number(active_slave_index[item]) + ", " \
                + QString::number(selected_exp_index[item]) + ", " \
                + QString::number(selected_refresh_index[item]) + "). ";
        if(pb_state[item] == 1)
        {
            msg += "Stream ON. ";
        }
        else
        {
            msg += "Stream OFF. ";
        }
        updateStatusBar(msg + msg_ref);

        previous_refresh_index[item] = selected_refresh_index[item];
    }
}

//Argument is the item line (0-3)
//Read All should be programmed for all boards - it returns all the onboard
//sensor values
void W_SlaveComm::sc_read_all(uint8_t item)
{
    int numb = 0;
    uint8_t slaveId = active_slave[item];
    uint8_t slaveIndex = active_slave_index[item];
    uint8_t expIndex = selected_exp_index[item];

    //1) Stream
    numb = tx_cmd_data_read_all(slaveId, CMD_READ, payload_str, PAYLOAD_BUF_LEN);
    numb = comm_gen_str(payload_str, comm_str_usb, PAYLOAD_BUF_LEN);
    numb = COMM_STR_BUF_LEN;
    emit slaveReadWrite(numb, comm_str_usb, READ);

    //2) Decode values
    myFlexSEA_Generic.decodeSlave(SL_BASE_ALL, slaveIndex);
    //(Uncertain about timings, probably delayed by 1 sample)

    //3) Log
    if(logThisItem[item] == true)
    {
        emit writeToLogFile(item, slaveIndex, expIndex);
    }
}

//Argument is the item line (0-3)
//Read All should be programmed for all boards - it returns all the onboard
//sensor values
void W_SlaveComm::sc_read_all_ricnu(uint8_t item)
{
    int numb = 0;
    uint8_t slaveId = active_slave[item];
    uint8_t slaveIndex = active_slave_index[item];
    uint8_t expIndex = selected_exp_index[item];

    //1) Stream
    numb = tx_cmd_data_read_all_ricnu(slaveId, CMD_READ, payload_str, PAYLOAD_BUF_LEN);
    numb = comm_gen_str(payload_str, comm_str_usb, PAYLOAD_BUF_LEN);
    numb = COMM_STR_BUF_LEN;
    emit slaveReadWrite(numb, comm_str_usb, READ);

    //2) Decode values
    //...

    //3) Log
    if(logThisItem[item] == true)
    {
        emit writeToLogFile(item, slaveIndex, expIndex);
    }
}

//
void W_SlaveComm::indicatorTimeout(bool rst)
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

//This is what gets connected to a timer slot.
void W_SlaveComm::sc_item1_slot(void)
{
    if(pb_state[0] == 1 && sc_comPortOpen == true) //ToDo: add slot, and private variable
    {
        switch(selected_exp_index[0])
        {
            case 0: //Read All (Barebone)
                sc_read_all(0);
                break;
            case 1: //In Control
                qDebug() << "Not programmed!";
                break;
            case 2: //Strain Amp
                qDebug() << "Not programmed!";
                break;
            case 3: //RIC/NU Knee
                sc_read_all_ricnu(0);
                break;
            case 4: //CSEA Knee
                qDebug() << "Not programmed!";
                break;
            case 5: //2DOF Ankle
                qDebug() << "Not programmed!";
                break;
            default:
                break;
        }
    }
}

//Master timebase is 100Hz. We divide is to get [100, 50, 33, 20, 10, 5, 1]Hz
void W_SlaveComm::masterTimerEvent(void)
{    
    static int tb50Hz = 0, tb33Hz = 0, tb20Hz = 0, tb10Hz = 0, tb5Hz = 0, tb1Hz = 0;

    //Increment all counters:
    tb50Hz++;
    tb33Hz++;
    tb20Hz++;
    tb10Hz++;
    tb5Hz++;
    tb1Hz++;

    //Emit signals:

    emit masterTimer100Hz();
    indicatorTimeout(false);

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
    configSlaveComm(0);
}

void W_SlaveComm::on_comboBoxSlave2_currentIndexChanged(int index)
{
    configSlaveComm(1);
}

void W_SlaveComm::on_comboBoxSlave3_currentIndexChanged(int index)
{
    configSlaveComm(2);
}

void W_SlaveComm::on_comboBoxSlave4_currentIndexChanged(int index)
{
    configSlaveComm(3);
}

void W_SlaveComm::on_comboBoxExp1_currentIndexChanged(int index)
{
    configSlaveComm(0);
}

void W_SlaveComm::on_comboBoxExp2_currentIndexChanged(int index)
{
    configSlaveComm(1);
}

void W_SlaveComm::on_comboBoxExp3_currentIndexChanged(int index)
{
    configSlaveComm(2);
}

void W_SlaveComm::on_comboBoxExp4_currentIndexChanged(int index)
{
    configSlaveComm(3);
}

void W_SlaveComm::on_comboBoxRefresh1_currentIndexChanged(int index)
{
    configSlaveComm(0);
}

void W_SlaveComm::on_comboBoxRefresh2_currentIndexChanged(int index)
{
    configSlaveComm(1);
}

void W_SlaveComm::on_comboBoxRefresh3_currentIndexChanged(int index)
{
    configSlaveComm(2);
}

void W_SlaveComm::on_comboBoxRefresh4_currentIndexChanged(int index)
{
    configSlaveComm(3);
}
