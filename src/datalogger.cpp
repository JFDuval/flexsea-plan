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
	[This file] 
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "datalogger.h"
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

DataLogger::DataLogger(QWidget *parent) : QWidget(parent)
{
    fileOpened[0] = false;
    fileOpened[1] = false;
    fileOpened[2] = false;
    fileOpened[3] = false;
	logDirectory();
	init();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void DataLogger::openFile(uint8_t item)
{
    QString msg = "";

    //File Dialog (returns the selected file name):
    QDir::setCurrent("../Plan-GUI-Logs");
    filename = QFileDialog::getSaveFileName( \
                this,
                tr("Open Log File"),
                QDir::currentPath(),
                tr("Log files (*.txt *.csv);;All files (*.*)"));

    //Extract filename to simplify UI:
    QString path = QDir::currentPath();
    int pathLen = path.length();
    //qDebug() << "Current path: " << path << ", len = " << pathLen;
    QString shortFileName = filename.mid(pathLen+1);
    //qDebug() << "File name: " << shortFileName;

    //Now we open it:
    logFile.setFileName(filename);
    if(logFile.open(QIODevice::ReadWrite))
    {
        msg = "Successfully opened: '" + shortFileName + "'.";
        emit setLogFileStatus(msg);
        qDebug() << msg;
    }
    else
    {
        msg = "Datalogging file error!";
        emit setLogFileStatus(msg);
        qDebug() << msg;
    }
    //Associate stream to file:
    logFileStream.setDevice(&logFile);

    msg = "Opened '" + filename + "'.";
    emit setStatusBarMessage(msg);
    fileOpened[0] = true;
}

void DataLogger::writeToFile(uint8_t item, uint8_t slaveIndex, uint8_t expIndex)
{
    qint64 t_ms = 0;
    static qint64 t_ms_initial[4] = {0,0,0,0};
    static bool isFirstTime[4] = {true,true,true,true};

    void (DataLogger::*headerFctPtr) (uint8_t item);
    void (DataLogger::*logFctPtr) (QTextStream *filePtr, uint8_t slaveIndex, \
                       char term, qint64 t_ms, QString t_text);

    QString t_text = "";

    getFctPtrs(slaveIndex, expIndex, &headerFctPtr, &logFctPtr);
    //headerFctPtr = &
    //logFctPtr = &DataLogger::logReadAllExec;

    //Writting for the first time?
    if(isFirstTime[item] == true)
    {
        //Init timestamp ms:
        isFirstTime[item] = false;
        logTimestamp(&t_ms, &t_text);
        t_ms_initial[item] = t_ms;

        //Header:
        writeIdentifier(item, slaveIndex, expIndex);
        (this->*headerFctPtr)(item);
    }

    //Timestamps:
    logTimestamp(&t_ms, &t_text);
    t_ms -= t_ms_initial[item];

    if(fileOpened[item] == true)
    {
        //And we add to the text file:
        (this->*logFctPtr)(&logFileStream, slaveIndex, '\n', t_ms, t_text);
    }
    else
    {
        emit setStatusBarMessage("Datalogger: no file selected.");
    }
}

//What Header and Log format should we use? Based on slaveIndex and expIndex
//this function picks the right functions and returns pointers
void DataLogger::getFctPtrs(uint8_t slaveIndex, uint8_t expIndex, \
                            void (DataLogger::**myHeaderFctPtr) (uint8_t item), \
                            void (DataLogger::**myLogFctPtr) (QTextStream *filePtr, uint8_t slaveIndex, \
                                               char term, qint64 t_ms, QString t_text))
{
    //Board type? Extract base via address&integer trick
    uint8_t bType = myFlexSEA_Generic.getSlaveBoardType(SL_BASE_ALL, slaveIndex);

    //And now, experiment per experiment:
    switch(expIndex)
    {
        case 0: //Read All (barebone)
            switch(bType)
            {
                case FLEXSEA_PLAN_BASE:
                    break;
                case FLEXSEA_MANAGE_BASE:
                    break;
                case FLEXSEA_EXECUTE_BASE:
                    (*myHeaderFctPtr) = &writeExecuteReadAllHeader;
                    (*myLogFctPtr) = &logReadAllExec;
                    break;
                case FLEXSEA_BATTERY_BASE:
                    break;
                case FLEXSEA_STRAIN_BASE:
                    break;
                case FLEXSEA_GOSSIP_BASE:
                    break;
            }
            break;
        case 1: //In Control
            qDebug() << "Not programmed!";
            break;
        case 2: //Strain Amp
            qDebug() << "Not programmed!";
            break;
        case 3: //RIC/NU Knee
            (*myHeaderFctPtr) = &writeReadAllRicnuHeader;
            (*myLogFctPtr) = &logReadAllRicnu;
            break;
        case 4: //CSEA Knee
            qDebug() << "Not programmed!";
            break;
        case 5: //2DOF Ankle
            qDebug() << "Not programmed!";
            break;
        default:
            qDebug() << "Invalid Experiment - can't write Log Header";
            break;
    }
}

void DataLogger::closeFile(uint8_t item)
{
    if(fileOpened[item] == true)
    {
        logFileStream << endl;
        logFile.close();
        fileOpened[item] = false;
    }
}

void DataLogger::logReadAllExec(QTextStream *filePtr, uint8_t slaveIndex, \
                                char term, qint64 t_ms, QString t_text)
{
    struct execute_s *exPtr;
    myFlexSEA_Generic.assignExecutePtr(&exPtr, SL_BASE_ALL, slaveIndex);

    (*filePtr) << t_text << ',' << \
                        t_ms << ',' << \
                        exPtr->accel.x << ',' << \
                        exPtr->accel.y << ',' << \
                        exPtr->accel.z << ',' << \
                        exPtr->gyro.x << ',' << \
                        exPtr->gyro.y << ',' << \
                        exPtr->gyro.z << ',' << \
                        exPtr->strain << ',' << \
                        exPtr->analog[0] << ',' << \
                        exPtr->analog[1] << ',' << \
                        exPtr->current << ',' << \
                        exPtr->enc_display << ',' << \
                        exPtr->volt_batt << ',' << \
                        exPtr->volt_int << ',' << \
                        exPtr->temp << ',' << \
                        exPtr->status1 << ',' << \
                        exPtr->status2 << \
                        term;
}

void DataLogger::logReadAllRicnu(QTextStream *filePtr, uint8_t slaveIndex, \
                                 char term, qint64 t_ms, QString t_text)
{
    struct ricnu_s *myPtr;
    myFlexSEA_Generic.assignRicnuPtr(&myPtr, SL_BASE_ALL, slaveIndex);

    logFileStream << t_text << ',' << \
                        t_ms << ',' << \
                        myPtr->ex.accel.x << ',' << \
                        myPtr->ex.accel.y << ',' << \
                        myPtr->ex.accel.z << ',' << \
                        myPtr->ex.gyro.x << ',' << \
                        myPtr->ex.gyro.y << ',' << \
                        myPtr->ex.gyro.z << ',' << \
                        myPtr->ex.current << ',' << \
                        myPtr->ex.enc_commut << ',' << \
                        myPtr->ex.enc_control << ',' << \
                        myPtr->ex.volt_batt << ',' << \
                        myPtr->ext_strain[0] << ',' << \
                        myPtr->ext_strain[1] << ',' << \
                        myPtr->ext_strain[2] << ',' << \
                        myPtr->ext_strain[3] << ',' << \
                        myPtr->ext_strain[4] << ',' << \
                        myPtr->ext_strain[5] << ',' << \
                        term;
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void DataLogger::init(void)
{
    myTime = new QDateTime;
    myFlexSEA_Generic.init();
}

void DataLogger::logDirectory(void)
{
    //Do we already have a "Plan-GUI-Logs" directory?
    if(!QDir("../Plan-GUI-Logs").exists())
    {
        //No, create it:
        QDir().mkdir("../Plan-GUI-Logs");
        qDebug() << "Created /Plan-GUI-Logs";
        emit setStatusBarMessage("Created the /Plan-GUI-Logs directory.");
        //ui->statusBar->showMessage("Created the /Plan-GUI-Logs directory.");
    }
    else
    {
        qDebug() << "Using existing ""Plan-GUI-Logs"" directory";
    }
}

void DataLogger::logTimestamp(qint64 *t_ms, QString *t_text)
{
    *t_ms = myTime->currentMSecsSinceEpoch();
    *t_text = myTime->currentDateTime().toString();
}

void DataLogger::writeIdentifier(uint8_t item, uint8_t slaveIndex, uint8_t expIndex)
{
    QString msg, slaveName, expName;
    myFlexSEA_Generic.getSlaveName(SL_BASE_ALL, slaveIndex, &slaveName);
    myFlexSEA_Generic.getExpName(expIndex, &expName);

    //Top of the file description:
    msg = "[Datalogging: Item = " + QString::number(item) + " | Slave index = " + \
                        QString::number(slaveIndex) + " (" + slaveName + ") | " + \
                        "Experiment index = " + QString::number(expIndex) + " (" + \
                        expName + ")]\n";
    qDebug() << msg;
    if(fileOpened[item] == true)
    {
        logFileStream << msg;
    }
}

void DataLogger::writeExecuteReadAllHeader(uint8_t item)
{
    if(fileOpened[item] == true)
    {
        //Print header:
        logFileStream << "Timestamp," << \
                        "Timestamp (ms)," << \
                        "accel.x," << \
                        "accel.y," << \
                        "accel.z," << \
                        "gyro.x," << \
                        "gyro.y," << \
                        "gyro.z," << \
                        "strain," << \
                        "analog_0," << \
                        "analog_1," << \
                        "current," << \
                        "encoder," << \
                        "VB," << \
                        "VG," << \
                        "Temp," << \
                        "Status1," << \
                        "Status2" << \
                        endl;
    }
}

void DataLogger::writeReadAllRicnuHeader(uint8_t item)
{
    if(fileOpened[item] == true)
    {
        //Print header:
        logFileStream << "Timestamp," << \
                        "Timestamp (ms)," << \
                        "accel.x," << \
                        "accel.y," << \
                        "accel.z," << \
                        "gyro.x," << \
                        "gyro.y," << \
                        "gyro.z," << \
                        "current," << \
                        "enc-mot," << \
                        "enc-joint," << \
                        "VB," << \
                        "strain1," << \
                        "strain2," << \
                        "strain3," << \
                        "strain4," << \
                        "strain5," << \
                        "strain6," << \
                        endl;
    }
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

