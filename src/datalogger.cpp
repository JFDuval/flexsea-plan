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
#include "w_ricnu.h"
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
	logDirectory();
	init();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void DataLogger::openRecordingFile(uint8_t item)
{
	if(logRecordingFile[item].isOpen())
	{
		qDebug() << "File already open. Close it before opening a new one";
	}
	else
	{
		//File Dialog (returns the selected file name):
		QDir::setCurrent("Plan-GUI-Logs");
		QString filename = QFileDialog::getSaveFileName( \
					this,
					tr("Open Log File"),
					QDir::currentPath() + "\\.csv" ,
					tr("Log files (*.txt *.csv);;All files (*.*)"));

		//Extract filename to simplify UI:
		QString path = QDir::currentPath();
		int pathLen = path.length();
		QString shortFileName = filename.mid(pathLen+1);

		//Now we open it:
		logRecordingFile[item].setFileName(filename);

		openfile(item, filename, shortFileName);
	}
}

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

		//File Dialog
		QDir::setCurrent("Plan-GUI-Logs");
		QString fileName = QDir::currentPath() + "/" + shortFileName;
		QString numberedFileName = fileName;

		//Now we open it:
		int fileNameLen = fileName.length();
		numberedFileName.insert(fileNameLen	- 4,"_0");
		logRecordingFile[item].setFileName(numberedFileName);

		// Search for the next unused numbered file.
		uint16_t i = 0;
		while(logRecordingFile[item].exists() ||
			  i >= 10000)
		{
			++i;
			numberedFileName = fileName;
			numberedFileName.insert(fileNameLen - 4,
									"_" + QString::number(i));
			logRecordingFile[item].setFileName(numberedFileName);
		}

		QString numberedShortFileName = shortFileName + "_" + QString::number(i);

		openfile(item, numberedFileName, numberedShortFileName);
	}
}

void DataLogger::openfile(uint8_t item, QString fileName, QString shortFileName)
{
	QString msg = "";

	// Try to open the file.
	if(logRecordingFile[item].open(QIODevice::ReadWrite))
	{
		// TODO Datalogger should not know that there's a logFile and bar
		// status. Abstraction principle is not respected here. Is there a way
		// to use some sort of return value instead of signal slot?

		//Associate stream to file:
		logFileStream.setDevice(&logRecordingFile[item]);
		msg = tr("Opened '") + fileName + "'.";
		emit setStatusBarMessage(msg);
	}

	//If no file selected
	else
	{
		msg = tr("No log file selected.");
		qDebug() << msg;

		msg = tr("No log file selected, or the file couldn't be opened.");
		emit setStatusBarMessage(msg);
	}
}

void DataLogger::openReadingFile(bool * isOpen)
{
	QString msg = "";
	*isOpen = false;

	//File Dialog (returns the selected file name):
	QDir::setCurrent("Plan-GUI-Logs");
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

	if(logReadingFile.open(QIODevice::ReadOnly))
	{
		// Read and save the logfile informations.
		QString line;
		QStringList splitLine;

		line = logReadingFile.readLine();
		splitLine = line.split(',', QString::KeepEmptyParts);

		myLog.dataloggingItem	= splitLine[1];
		myLog.SlaveIndex		= splitLine[3];
		myLog.SlaveName			= splitLine[5];
		myLog.experimentIndex	= splitLine[7];
		myLog.experimentName	= splitLine[9];
		myLog.frequency			= splitLine[11].toInt();
		myLog.shortFileName = shortFileName;
		myLog.fileName		= filename;

		//Clear the column's header.
		line = logReadingFile.readLine();

		// TODO: Remove this by supporting multiple board
		// Quick hack: detect what board we are reading:

		if((myLog.SlaveIndex.toInt() >= 0 && myLog.SlaveIndex.toInt() <= 3) && \
			(myLog.experimentIndex.toInt() == 0))
		{
			qDebug() << "Reading from Execute";
		}
		else
		{
			qDebug() << "To this day, we can only load an Execute's log.";
			return;
		}

		while (!logReadingFile.atEnd())
		{
			line = logReadingFile.readLine();
			splitLine = line.split(',', QString::KeepEmptyParts);

			// If data line contain expected data
			if(splitLine.length() >= 20)
			{
				struct log_s newitem;
				myLog.logList.append(newitem);
				myLog.logList.last().timeStampDate		= splitLine[0];
				myLog.logList.last().timeStamp_ms		= splitLine[1].toInt();
				myLog.logList.last().execute.accel.x	= splitLine[2].toInt();
				myLog.logList.last().execute.accel.y	= splitLine[3].toInt();
				myLog.logList.last().execute.accel.z	= splitLine[4].toInt();
				myLog.logList.last().execute.gyro.x		= splitLine[5].toInt();
				myLog.logList.last().execute.gyro.y		= splitLine[6].toInt();
				myLog.logList.last().execute.gyro.z		= splitLine[7].toInt();
				myLog.logList.last().execute.strain		= splitLine[8].toInt();
				myLog.logList.last().execute.analog[0]	= splitLine[9].toInt();
				myLog.logList.last().execute.analog[1]	= splitLine[10].toInt();
				myLog.logList.last().execute.current	= splitLine[11].toInt();
				myLog.logList.last().execute.enc_display= splitLine[12].toInt();
				myLog.logList.last().execute.enc_control= splitLine[13].toInt();
				myLog.logList.last().execute.enc_commut	= splitLine[14].toInt();
				myLog.logList.last().execute.volt_batt	= splitLine[15].toInt();
				myLog.logList.last().execute.volt_int	= splitLine[16].toInt();
				myLog.logList.last().execute.temp		= splitLine[17].toInt();
				myLog.logList.last().execute.status1	= splitLine[18].toInt();
				myLog.logList.last().execute.status2	= splitLine[19].toInt();
				FlexSEA_Generic::decodeExecute(&myLog.logList.last().execute);
			}

		}

		msg = tr("Opened '") + filename + "'.";
		emit setStatusBarMessage(msg);
		qDebug() << msg;	//ToDo: msg is never set, prints ""
		*isOpen = true;
	}

	//If no file selected
	else
	{


		msg = tr("No log file selected or the file couldn't be opened.");
		emit setStatusBarMessage(msg);
		qDebug() << msg;
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
					(*myLogFctPtr) = &logReadAllManage;
					break;
				case FLEXSEA_EXECUTE_BASE:
					(*myHeaderFctPtr) = &writeExecuteReadAllHeader;
					(*myLogFctPtr) = &logReadAllExec;
					break;
				case FLEXSEA_BATTERY_BASE:
					break;
				case FLEXSEA_STRAIN_BASE:
					(*myHeaderFctPtr) = &writeStrainReadAllHeader;
					(*myLogFctPtr) = &logReadAllStrain;
					break;
				case FLEXSEA_GOSSIP_BASE:
					(*myHeaderFctPtr) = &writeGossipReadAllHeader;
					(*myLogFctPtr) = &logReadAllGossip;
					break;
			}
			break;
		case 1: //In Control
			qDebug() << "Not programmed!";
			break;
		case 2: //RIC/NU Knee
			(*myHeaderFctPtr) = &writeReadAllRicnuHeader;
			(*myLogFctPtr) = &logReadAllRicnu;
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
	// TODO: Implement a new class to handle clear of log properly.
	myLog.logList.clear();
	myLog.dataloggingItem.clear();
	myLog.SlaveIndex.clear();
	myLog.SlaveName.clear();
	myLog.experimentIndex.clear();
	myLog.experimentName.clear();
	myLog.frequency = 0;
	myLog.shortFileName.clear();
	myLog.fileName.clear();
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

	W_Ricnu::unpackCompressed6ch(myPtr->st.compressedBytes, \
						&myPtr->st.ch[0].strain_filtered, \
						&myPtr->st.ch[1].strain_filtered, \
						&myPtr->st.ch[2].strain_filtered, \
						&myPtr->st.ch[3].strain_filtered, \
						&myPtr->st.ch[4].strain_filtered, \
						&myPtr->st.ch[5].strain_filtered);

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

void DataLogger::logDirectory(void)
{
	//Do we already have a "Plan-GUI-Logs" directory?
	if(!QDir("Plan-GUI-Logs").exists())
	{
		//No, create it:
		QDir().mkdir("Plan-GUI-Logs");
		qDebug() << "Created Plan-GUI-Logs";
		emit setStatusBarMessage("Created the Plan-GUI-Logs directory.");
		//ui->statusBar->showMessage("Created the Plan-GUI-Logs directory.");
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

