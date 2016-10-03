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
	[This file] w_planconfig.h: Configuration Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_planconfig.h"
#include "ui_w_planconfig.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_PlanConfig::W_PlanConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_PlanConfig)
{
    ui->setupUi(this);

    setWindowTitle("Configuration");
    setWindowIcon(QIcon(":icons/d_logo_small.png"));

    //Init code:
    flagManualEntry = 0;
    initCom();
    initLog();
}

W_PlanConfig::~W_PlanConfig()
{
    delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void W_PlanConfig::setComProgress(int val, int rst)
{
    ui->comProgressBar->setValue(val);
    if(rst)
    {
        defaultComOffUi();
    }
}

void W_PlanConfig::setLogFileStatus(QString status)
{
    ui->logFileStatus1->setText(status);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_PlanConfig::initCom(void)
{
    //Flags:
    flagComInitDone = 0;

    //No manual entry, 0% progress, etc.:
    ui->comPortTxt->setText("");
    ui->comPortTxt->setDisabled(true);
    ui->comProgressBar->setValue(0);
    ui->comProgressBar->setDisabled(true);
    ui->openComButton->setDisabled(false);
    ui->closeComButton->setDisabled(true);

    //COM port list and button:
    getComList();
    ui->pushButtonRefresh->setText(QChar(0x21BB));

    //Flag for other functions:
    flagComInitDone = 1;
}

void W_PlanConfig::initLog(void)
{
    ui->logFileStatus1->setText("No log file selected.");

    //Items 2-4 are disabled for now:
    ui->logFileStatus2->setText("No log file selected.");
    ui->logFileStatus3->setText("No log file selected.");
    ui->logFileStatus4->setText("No log file selected.");
    ui->logFileStatus2->setDisabled(true);
    ui->logFileStatus3->setDisabled(true);
    ui->logFileStatus4->setDisabled(true);
    ui->pbOpenLog2->setDisabled(true);
    ui->pbOpenLog3->setDisabled(true);
    ui->pbOpenLog4->setDisabled(true);
}

void W_PlanConfig::getComList(void)
{
    //First, clear lists:
    comPortList.clear();
    ui->comPortComboBox->clear();

    //Available ports?
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos)
    {
        //qDebug() << info.portName();
        comPortList << info.portName();
        ui->comPortComboBox->addItem(comPortList.last());
    }
    //Add an option for manual entry:
    comPortList << "Manual Entry";
    ui->comPortComboBox->addItem(comPortList.last());

    //Enable lideEdit when we only have Manual Entry
    if(comPortList.length() == 1)
    {
        ui->comPortTxt->setDisabled(false);
        ui->comPortTxt->setText("");
    }

    //LineEdit mimics combobox:
    ui->comPortTxt->setText(ui->comPortComboBox->currentText());
}

void W_PlanConfig::defaultComOffUi(void)
{
    ui->openComButton->setDisabled(false);
    ui->comProgressBar->setDisabled(false);
    ui->comProgressBar->setValue(0);
    ui->closeComButton->setDisabled(true);
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_PlanConfig::on_comPortComboBox_currentIndexChanged(int index)
{
    //LineEdit mimics combobox:
    ui->comPortTxt->setText(ui->comPortComboBox->currentText());

    //Monitor combobox after init is completed:
    if(flagComInitDone != 0)
    {
        if(index == (comPortList.length() - 1))
        {
            flagManualEntry = 1;
            //qDebug() << "Manual entry";
            ui->comPortTxt->setDisabled(false);
            ui->comPortTxt->setText("");
        }
        else
        {
            flagManualEntry = 0;
            ui->comPortTxt->setDisabled(true);
        }
    }
}

void W_PlanConfig::on_openComButton_clicked()
{
    //Deal with display elements:
    defaultComOffUi();
    ui->openComButton->setDisabled(true);

    //Some checks:
    if(ui->comPortComboBox->currentIndex() == (comPortList.length() - 1))
    {
        //Last item is Manual Entry. Do we have text?
        if(ui->comPortTxt->text().length() > 0)
        {
            //qDebug() << "More than 0 char, valid.";
        }
        else
        {
            qDebug() << "0 char, invalid.";
            defaultComOffUi();
            return;
        }
    }

    //Emit signal:
    emit openCom(ui->comPortTxt->text(), 25, 100000);
    ui->closeComButton->setDisabled(false);
}

void W_PlanConfig::on_closeComButton_clicked()
{
    //Emit signal:
    emit closeCom();

    //Enable Open COM button:
    ui->openComButton->setEnabled(true);
    ui->openComButton->repaint();

    //Disable Close COM button:
    ui->closeComButton->setDisabled(true);
    ui->closeComButton->repaint();

    //ui->comStatusTxt->setText("COM Port closed.");
    ui->comProgressBar->setValue(0);
    ui->comProgressBar->setDisabled(true);

}

void W_PlanConfig::on_pushButtonRefresh_clicked()
{
    getComList();
}

void W_PlanConfig::on_pbOpenLog1_clicked()
{
    emit openLogFile(0);
}

void W_PlanConfig::on_pbOpenLog2_clicked()
{
    emit openLogFile(1);
}

void W_PlanConfig::on_pbOpenLog3_clicked()
{
    emit openLogFile(2);
}

void W_PlanConfig::on_pbOpenLog4_clicked()
{
    emit openLogFile(3);
}
