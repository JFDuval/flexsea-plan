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
    fileOpened = false;
	logDirectory();
	init();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void DataLogger::openFile(void)
{
    QString msg = "";

    //File Dialog (returns the selected file name):
    QDir::setCurrent("../Plan-GUI-Logs");
    filename = QFileDialog::getSaveFileName( \
                this,
                tr("Open Log File"),
                QDir::currentPath(),
                tr("Log files (*.txt *.csv);;All files (*.*)"));

    //Now we open it:
    logFile.setFileName(filename);
    if(logFile.open(QIODevice::ReadWrite))
    {
        msg = "Successfully opened:\n" + filename;
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
    fileOpened = true;

    //***ToDo*** what if it's not Execute???
    writeExecuteReadAllHeader();
}

void DataLogger::writeToFile(int slaveIndex)
{
    struct execute_s *exPtr;
    myFlexSEA_Generic.assignExecutePtr(&exPtr, slaveIndex);
    qint64 t_ms = 0;
    static qint64 t_ms_initial = 0;
    static bool isFirstTime = true;
    QString t_text = "";

    //Timestamps:
    if(isFirstTime == true)
    {
        isFirstTime = false;
        logTimestamp(&t_ms, &t_text);
        t_ms_initial = t_ms;
    }
    logTimestamp(&t_ms, &t_text);
    t_ms -= t_ms_initial;

    if(fileOpened == true)
    {
        //And we add to he text file:
        logFileStream << t_text << ',' << \
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
                            endl;
        //***ToDo*** this is only for Execute!
    }
    else
    {
        emit setStatusBarMessage("Datalogger: no file selected.");
    }
}

void DataLogger::closeFile(void)
{
    if(fileOpened == true)
    {
        logFileStream << endl;
        logFile.close();
        fileOpened = false;
    }
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void DataLogger::init(void)
{
    myTime = new QDateTime;
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

void DataLogger::writeExecuteReadAllHeader(void)
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

//****************************************************************************
// Private slot(s):
//****************************************************************************

