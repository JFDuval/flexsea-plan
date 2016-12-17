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

#include "datalogger.h"
#include "ricnuDevice.h"
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>

#include "batteryDevice.h"
#include "executeDevice.h"
#include "gossipDevice.h"
#include "manageDevice.h"
#include "ricnuDevice.h"
#include "strainDevice.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

DataLogger::DataLogger(QWidget *parent) : QWidget(parent)
{
	initLogDirectory();
	init();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void DataLogger::openRecordingFile(uint8_t item, QString shortFileName)
{
	if(logRecordingFile[item].isOpen())
	{
		qDebug() << "File already open. Close it before opening a new one";
	}

	else
	{
		//Add .csv extension if not present
		if(shortFileName.mid(shortFileName.length() - 4) != ".csv")
		{
			shortFileName.append(".csv");
		}

		// Add date and time to the short file name
		shortFileName.prepend(QDate::currentDate().toString("yyyy-MM-dd_") +
							  QTime::currentTime().toString("HH'h'mm'm'ss's'_"));

		openfile(item, shortFileName);
	}
}
void DataLogger::openRecordingFile(FlexseaDevice *devicePtr, uint8_t item)
{
	QString shortFileName = devicePtr->shortFileName;

	if(logRecordingFile[item].isOpen())
	{
		qDebug() << "File already open. Close it before opening a new one";
	}

	else
	{
		//Add .csv extension if not present
		if(shortFileName.mid(shortFileName.length()-4) != ".csv")
		{
			shortFileName.append(".csv");
		}

		// Add date and time to the short file name
		shortFileName.prepend(QDate::currentDate().toString("yyyy-MM-dd_") +
							  QTime::currentTime().toString("HH'h'mm'm'ss's'_"));

		openfile(item, shortFileName);
	}
}

void DataLogger::openfile(uint8_t item, QString shortFileName)
{
	// Replace whitespace by underscore
	shortFileName.replace(" ", "_");

	// Remove invalid character for a filename(According to Windows)
	shortFileName.remove(QRegExp("[<>:\"/|?*]"));
	shortFileName.remove("\\");


	// Set the folder to current directory
	QDir::setCurrent(planGUIRootPath + "\\" + logFolder + "\\" + sessionFolder);

	// Set the filename from the current directory
	QString fileName = QDir::currentPath() + "/" + shortFileName;

	logRecordingFile[item].setFileName(fileName);
	// Try to open the file.
	if(logRecordingFile[item].open(QIODevice::ReadWrite))
	{
		// TODO Datalogger should not know that there's a logFile and bar
		// status. Abstraction principle is not respected here. Is there a way
		// to use some sort of return value instead of signal slot?

		//Associate stream to file:
		logFileStream.setDevice(&logRecordingFile[item]);

		emit setStatusBarMessage(tr("Opened '") + fileName + "'.");
		qDebug() << tr("Opened '") + fileName + "'.";
	}

	//If no file selected
	else
	{
		qDebug() << tr("No log file selected.");
		emit setStatusBarMessage(
					tr("No log file selected, or the file couldn't be opened."));
	}
}

void DataLogger::openReadingFile(bool * isOpen)
{
	QString msg = "";
	*isOpen = false;

	//File Dialog (returns the selected file name):
	QDir::setCurrent(planGUIRootPath + "\\" + logFolder);
	QString filename = QFileDialog::getOpenFileName( \
				this,
				tr("Open Log File"),
				QDir::currentPath() + "\\.csv" ,
				tr("Log files (*.txt *.csv);;All files (*.*)"));

	//Extract filename to simplify UI:
	QString path = QDir::currentPath();
	int pathLen = path.length();
	QString shortFileName = filename.mid(pathLen+1);

	//Now we open it:
	logReadingFile.setFileName(filename);

	//Check if the file was successfully opened
	if(logReadingFile.open(QIODevice::ReadOnly) == false)
	{
		msg = tr("Error : No log file selected or the file couldn't be opened.");
		emit setStatusBarMessage(msg);
		qDebug() << msg;
		return;
	}

	//Check if the file is empty
	if(logReadingFile.size() == 0)
	{
		msg = tr("Error : Loaded file was empty.");
		emit setStatusBarMessage(msg);
		qDebug() << msg;
		return;
	}

	// Read and save the logfile informations.
	QString line;
	QStringList splitLine;

	line = logReadingFile.readLine();
	splitLine = line.split(',', QString::KeepEmptyParts);

	//Check if the file header contain the expected number of data
	if(splitLine.length() < 12)
	{
		msg = tr("Error : Loaded file format was not compatible");
		emit setStatusBarMessage(msg);
		qDebug() << msg;
		return;
	}

	myLogFile.dataloggingItem	= splitLine[1].toInt();
	myLogFile.SlaveIndex		= splitLine[3].toInt();
	myLogFile.SlaveName			= splitLine[5];
	myLogFile.experimentIndex	= splitLine[7].toInt();
	myLogFile.experimentName	= splitLine[9];
	myLogFile.frequency			= splitLine[11].toInt();
	myLogFile.shortFileName		= shortFileName;
	myLogFile.fileName			= filename;

	//Clear the column's header.
	line = logReadingFile.readLine();
	splitLine = line.split(',', QString::KeepEmptyParts);
	int test = splitLine.length();
	//Check if data header contain the number of expected field
	if(splitLine.length() < 20)
	{
		msg = tr("File format is not compatible");
		emit setStatusBarMessage(msg);
		qDebug() << msg;
		myLogFile.clear();
		return;
	}

	while (!logReadingFile.atEnd())
	{
		line = logReadingFile.readLine();
		splitLine = line.split(',', QString::KeepEmptyParts);

		//Check if data line contain the number of data expected
		if(splitLine.length() >= 20)
		{
			myLogFile.newDataLine();
			myLogFile.data.last().timeStampDate		= splitLine[0];
			myLogFile.data.last().timeStamp_ms		= splitLine[1].toInt();
			myLogFile.data.last().execute.accel.x	= splitLine[2].toInt();
			myLogFile.data.last().execute.accel.y	= splitLine[3].toInt();
			myLogFile.data.last().execute.accel.z	= splitLine[4].toInt();
			myLogFile.data.last().execute.gyro.x	= splitLine[5].toInt();
			myLogFile.data.last().execute.gyro.y	= splitLine[6].toInt();
			myLogFile.data.last().execute.gyro.z	= splitLine[7].toInt();
			myLogFile.data.last().execute.strain	= splitLine[8].toInt();
			myLogFile.data.last().execute.analog[0]	= splitLine[9].toInt();
			myLogFile.data.last().execute.analog[1]	= splitLine[10].toInt();
			myLogFile.data.last().execute.current	= splitLine[11].toInt();
			myLogFile.data.last().execute.enc_display= splitLine[12].toInt();
			myLogFile.data.last().execute.enc_control= splitLine[13].toInt();
			myLogFile.data.last().execute.enc_commut= splitLine[14].toInt();
			myLogFile.data.last().execute.volt_batt	= splitLine[15].toInt();
			myLogFile.data.last().execute.volt_int	= splitLine[16].toInt();
			myLogFile.data.last().execute.temp		= splitLine[17].toInt();
			myLogFile.data.last().execute.status1	= splitLine[18].toInt();
			myLogFile.data.last().execute.status2	= splitLine[19].toInt();
		}
	}

	myLogFile.decodeAllLine();

	msg = tr("Opened '") + filename + "'.";
	emit setStatusBarMessage(msg);
	qDebug() << msg;

	*isOpen = true;
}

void DataLogger::writeToFiledev(FlexseaDevice *devicePtr, uint8_t item)
{
	// Verify that the log file is properly opened.
	if(logRecordingFile[item].isOpen())
	{
		//Writting for the first time?
		if(logRecordingFile[item].pos() == 0)
		{
			//Header:
			writeIdentifier(devicePtr, item);
			writeReadAllHeader(devicePtr, item);
		}

		//And we add to the text file:
		logReadAll(devicePtr, item);
	}
	else
	{
		emit setStatusBarMessage("Datalogger: no file selected.");
	}
}

void DataLogger::writeToFile(uint8_t item, uint8_t slaveIndex,
							 uint8_t expIndex, uint16_t refreshRate)
{
	qint64 t_ms = 0;
	static qint64 t_ms_initial[4] = {0,0,0,0};

	void (DataLogger::*headerFctPtr) (uint8_t item);
	void (DataLogger::*logFctPtr) (QTextStream *filePtr, uint8_t slaveIndex, \
					   char term, qint64 t_ms, QString t_text);

	QString t_text = "";

	/*ToDo: why are we constantly calling that? It should be done once,
	 * the first time we start logging. The way it is now, we can switch board
	 * in the middle of a log, corrupting the data.*/
	getFctPtrs(slaveIndex, expIndex, &headerFctPtr, &logFctPtr);

	// Verify that the log file is properly opened.
	if(logRecordingFile[item].isOpen())
	{
		//Writting for the first time?
		if(logRecordingFile[item].pos() == 0)
		{
			//Init timestamp ms:
			logTimestamp(&t_ms, &t_text);
			t_ms_initial[item] = t_ms;

			//Header:
			writeIdentifier(item, slaveIndex, expIndex, refreshRate);
			(this->*headerFctPtr)(item);
		}

		//Timestamps:
		logTimestamp(&t_ms, &t_text);
		t_ms -= t_ms_initial[item];

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
	uint8_t bType = FlexSEA_Generic::getSlaveBoardType(SL_BASE_ALL, slaveIndex);

	//And now, experiment per experiment:
	switch(expIndex)
	{
		case 0: //Read All (barebone)
			switch(bType)
			{
				case FLEXSEA_PLAN_BASE:
					break;
				case FLEXSEA_MANAGE_BASE:
					//ToDo: why is Manage different than the others?
					//(*myHeaderFctPtr) = &writeManageReadAllHeader;
					*myHeaderFctPtr = &DataLogger::writeManageReadAllHeader;
					*myLogFctPtr = &DataLogger::logReadAllManage;
					break;
				case FLEXSEA_EXECUTE_BASE:
					*myHeaderFctPtr = &DataLogger::writeExecuteReadAllHeader;
					*myLogFctPtr = &DataLogger::logReadAllExec;
					break;
				case FLEXSEA_BATTERY_BASE:
					break;
				case FLEXSEA_STRAIN_BASE:
					*myHeaderFctPtr = &DataLogger::writeStrainReadAllHeader;
					*myLogFctPtr = &DataLogger::logReadAllStrain;
					break;
				case FLEXSEA_GOSSIP_BASE:
					*myHeaderFctPtr = &DataLogger::writeGossipReadAllHeader;
					*myLogFctPtr = &DataLogger::logReadAllGossip;
					break;
			}
			break;
		case 1: //In Control
			qDebug() << "Not programmed!";
			break;
		case 2: //RIC/NU Knee
			*myHeaderFctPtr = &DataLogger::writeReadAllRicnuHeader;
			*myLogFctPtr = &DataLogger::logReadAllRicnu;
			break;
		case 3: //CSEA Knee
			qDebug() << "Not programmed!";
			break;
		case 4: //2DOF Ankle
			qDebug() << "Not programmed!";
			break;
		default:
			qDebug() << "Invalid Experiment - can't write Log Header";
			break;
	}
}

void DataLogger::closeRecordingFile(uint8_t item)
{
	if(logRecordingFile[item].isOpen())
	{
		logFileStream << endl;
		logRecordingFile[item].close();
	}
}

void DataLogger::closeReadingFile(void)
{
	if(logReadingFile.isOpen())
	{
		logReadingFile.close();
	}

	myLogFile.clear();
}


void DataLogger::logReadAll(FlexseaDevice *device, uint8_t item)
{
	if(logRecordingFile[item].isOpen())
	{
		logFileStream << device->getLastLineStr() << endl;
	}
}

//ToDo: move these functions to their respective files
void DataLogger::logReadAllExec(QTextStream *filePtr, uint8_t slaveIndex, \
								char term, qint64 t_ms, QString t_text)
{
	struct execute_s *exPtr;
	FlexSEA_Generic::assignExecutePtr(&exPtr, SL_BASE_ALL, slaveIndex);

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
						exPtr->enc_control << ',' << \
						exPtr->enc_commut << ',' << \
						exPtr->volt_batt << ',' << \
						exPtr->volt_int << ',' << \
						exPtr->temp << ',' << \
						exPtr->status1 << ',' << \
						exPtr->status2 << \
						term;
}

void DataLogger::logReadAllStrain(QTextStream *filePtr, uint8_t slaveIndex, \
								char term, qint64 t_ms, QString t_text)
{
	struct strain_s *stPtr;
	FlexSEA_Generic::assignStrainPtr(&stPtr, SL_BASE_ALL, slaveIndex);

	(*filePtr) << t_text << ',' << \
						t_ms << ',' << \
						stPtr->ch[0].strain_filtered << ',' << \
						stPtr->ch[1].strain_filtered << ',' << \
						stPtr->ch[2].strain_filtered << ',' << \
						stPtr->ch[3].strain_filtered << ',' << \
						stPtr->ch[4].strain_filtered << ',' << \
						stPtr->ch[5].strain_filtered << \
						term;
}

void DataLogger::logReadAllGossip(QTextStream *filePtr, uint8_t slaveIndex, \
								char term, qint64 t_ms, QString t_text)
{
	struct gossip_s *goPtr;
	FlexSEA_Generic::assignGossipPtr(&goPtr, SL_BASE_ALL, slaveIndex);

	(*filePtr) << t_text << ',' << \
						t_ms << ',' << \
				  goPtr->accel.x << ',' << \
				  goPtr->accel.y << ',' << \
				  goPtr->accel.z << ',' << \
				  goPtr->gyro.x << ',' << \
				  goPtr->gyro.y << ',' << \
				  goPtr->gyro.z << ',' << \
				  goPtr->magneto.x << ',' << \
				  goPtr->magneto.y << ',' << \
				  goPtr->magneto.z << ',' << \
				  goPtr->io[0] << ',' << \
				  goPtr->io[1] << ',' << \
				  goPtr->capsense[0] << ',' << \
				  goPtr->capsense[1] << ',' << \
				  goPtr->capsense[2] << ',' << \
				  goPtr->capsense[3] << ',' << \
				  goPtr->status << \
				  term;
}

void DataLogger::logReadAllRicnu(QTextStream *filePtr, uint8_t slaveIndex, \
								 char term, qint64 t_ms, QString t_text)
{
	struct ricnu_s *myPtr;
	FlexSEA_Generic::assignRicnuPtr(&myPtr, SL_BASE_ALL, slaveIndex);

	RicnuDevice::unpackCompressed6ch(&myPtr->st);

	(*filePtr) << t_text << ',' << \
				t_ms << ',' << \
				myPtr->ex.accel.x << ',' << \
				myPtr->ex.accel.y << ',' << \
				myPtr->ex.accel.z << ',' << \
				myPtr->ex.gyro.x << ',' << \
				myPtr->ex.gyro.y << ',' << \
				myPtr->ex.gyro.z << ',' << \
				myPtr->ex.current << ',' << \
				myPtr->ex.enc_motor << ',' << \
				myPtr->ex.enc_joint << ',' << \
				myPtr->st.ch[0].strain_filtered << ',' << \
				myPtr->st.ch[1].strain_filtered << ',' << \
				myPtr->st.ch[2].strain_filtered << ',' << \
				myPtr->st.ch[3].strain_filtered << ',' << \
				myPtr->st.ch[4].strain_filtered << ',' << \
				myPtr->st.ch[5].strain_filtered << ',' << \
				term;
}

void DataLogger::logReadAllManage(QTextStream *filePtr, uint8_t slaveIndex, \
								char term, qint64 t_ms, QString t_text)
{
	struct manage_s *mnPtr;
	FlexSEA_Generic::assignManagePtr(&mnPtr, SL_BASE_MN, slaveIndex);

	(*filePtr) << t_text << ',' << \
						t_ms << ',' << \
						mnPtr->accel.x << ',' << \
						mnPtr->accel.y << ',' << \
						mnPtr->accel.z << ',' << \
						mnPtr->gyro.x << ',' << \
						mnPtr->gyro.y << ',' << \
						mnPtr->gyro.z << ',' << \
						mnPtr->digitalIn << ',' << \
						mnPtr->sw1 << ',' << \
						mnPtr->analog[0] << ',' << \
						mnPtr->analog[1] << ',' << \
						mnPtr->analog[2] << ',' << \
						mnPtr->analog[3] << ',' << \
						mnPtr->analog[4] << ',' << \
						mnPtr->analog[5] << ',' << \
						mnPtr->analog[6] << ',' << \
						mnPtr->analog[7] << ',' << \
						mnPtr->status1 << ',' << \
						term;
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void DataLogger::init(void)
{
	myTime = new QDateTime;
}

void DataLogger::initLogDirectory()
{
	// Save the root path of the execution of the program
	planGUIRootPath = QDir::currentPath();

	// Set the default folder
	logFolder = "Plan-GUI-Logs";
	sessionFolder = QDate::currentDate().toString("yyyy-MM-dd_") + \
					QTime::currentTime().toString("HH'h'mm'm'ss's'");

	sessionFolder.replace(" ", "_");
	sessionFolder.replace(":", "-");

	//Do we already have a "Plan-GUI-Logs" directory?
	if(!QDir().exists(logFolder))
	{
		//No, create it:
		QDir().mkdir(logFolder);
		qDebug() << QString("Created ") + logFolder;
		emit setStatusBarMessage("Created the " + logFolder + " directory.");
		//ui->statusBar->showMessage("Created the Plan-GUI-Logs directory.");
	}
	else
	{
		qDebug() << "Using existing """ + logFolder + """ directory";
	}

	QDir::setCurrent(logFolder);

	// Create this session folder
	QDir().mkdir(sessionFolder);
}

void DataLogger::logTimestamp(qint64 *t_ms, QString *t_text)
{
	*t_ms = myTime->currentMSecsSinceEpoch();
	*t_text = myTime->currentDateTime().toString();
}

void DataLogger::writeIdentifier(FlexseaDevice *devicePtr, uint8_t item)
{
	QString msg;
	//Top of the file description:
	msg =	QString("Datalogging Item:")				+ QString(',') +
			QString::number(item)						+ QString(',') +

			QString("Slave Index:")						+ QString(',') +
			QString::number(devicePtr->slaveIndex)		+ QString(',') +

			QString("Slave Name:")						+ QString(',') +
			devicePtr->SlaveName						+ QString(',') +

			QString("Experiment Index:")				+ QString(',') +
			QString::number(devicePtr->experimentIndex)	+ QString(',') +

			QString("Experiment Name:")					+ QString(',') +
			devicePtr->experimentName					+ QString(',') +

			QString("Aquisition Frequency:")			+ QString(',') +
			QString::number(devicePtr->frequency)		+ QString("\n");

	if(logRecordingFile[item].isOpen())
	{
		logFileStream << msg;
	}
}

void DataLogger::writeIdentifier(uint8_t item, uint8_t slaveIndex,
								 uint8_t expIndex, uint16_t refreshRate)
{
	QString msg, slaveName, expName;
	FlexSEA_Generic::getSlaveName(SL_BASE_ALL, slaveIndex, &slaveName);
	FlexSEA_Generic::getExpName(expIndex, &expName);

	//Top of the file description:
	msg =	QString("Datalogging Item:")	+ QString(',') +
			QString::number(item)			+ QString(',') +

			QString("Slave Index:")			+ QString(',') +
			QString::number(slaveIndex)		+ QString(',') +

			QString("Slave Name:")			+ QString(',') +
			slaveName						+ QString(',') +

			QString("Experiment Index:")	+ QString(',') +
			QString::number(expIndex)		+ QString(',') +

			QString("Experiment Name:")		+ QString(',') +
			expName							+ QString(',') +

			QString("Aquisition Frequency:")+ QString(',') +
			QString::number(refreshRate)	+ QString("\n");

	qDebug() << msg;
	if(logRecordingFile[item].isOpen())
	{
		logFileStream << msg;
	}
}

void DataLogger::writeReadAllHeader(FlexseaDevice *device, uint8_t item)
{
	if(logRecordingFile[item].isOpen())
	{
		logFileStream << device->getHeaderStr() << endl;
	}
}

void DataLogger::writeExecuteReadAllHeader(uint8_t item)
{
	if(logRecordingFile[item].isOpen())
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
						"enc-disp," << \
						"enc-cont," << \
						"enc-comm," << \
						"VB," << \
						"VG," << \
						"Temp," << \
						"Status1," << \
						"Status2" << \
						endl;
	}
}

void DataLogger::writeManageReadAllHeader(uint8_t item)
{
	if(logRecordingFile[item].isOpen())
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
						"digitalIn," << \
						"sw1," << \
						"analog0," << \
						"analog1," << \
						"analog2," << \
						"analog3," << \
						"analog4," << \
						"analog5," << \
						"analog6," << \
						"analog7," << \
						"Status1" << \
						endl;
	}
}

void DataLogger::writeStrainReadAllHeader(uint8_t item)
{
	if(logRecordingFile[item].isOpen())
	{
		//Print header:
		logFileStream << "Timestamp," << \
						"Timestamp (ms)," << \
						"ch1,"  << \
						"ch2,"  << \
						"ch3,"  << \
						"ch4,"  << \
						"ch5,"  << \
						"ch6"  << \
						endl;
	}
}

void DataLogger::writeGossipReadAllHeader(uint8_t item)
{
	if(logRecordingFile[item].isOpen())
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
						"magneto.x," << \
						"magneto.y," << \
						"magneto.z," << \
						"io1," << \
						"io2," << \
						"capsense1," << \
						"capsense2," << \
						"capsense3," << \
						"capsense4," << \
						"Status1" << \
						endl;
	}
}

void DataLogger::writeReadAllRicnuHeader(uint8_t item)
{
	if(logRecordingFile[item].isOpen())
	{
		qDebug() << "writeReadAllRicnuHeader";

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
						"strain1," << \
						"strain2," << \
						"strain3," << \
						"strain4," << \
						"strain5," << \
						"strain6" << \
						endl;
	}
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

