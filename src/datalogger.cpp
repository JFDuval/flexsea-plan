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
<<<<<<< HEAD
	fileOpened[0] = false;
	fileOpened[1] = false;
	fileOpened[2] = false;
	fileOpened[3] = false;
=======
>>>>>>> origin/Dev_Seb
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
<<<<<<< HEAD
	QString msg = "";

	//File Dialog (returns the selected file name):
	QDir::setCurrent("Plan-GUI-Logs");
	filename = QFileDialog::getSaveFileName( \
=======
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

		msg = tr("No log file selected or the file couldn't be opened.");
		emit setStatusBarMessage(msg);
	}
}

void DataLogger::openReadingFile(void)
{
	QString msg = "";


	//File Dialog (returns the selected file name):
	QDir::setCurrent("Plan-GUI-Logs");
	QString filename = QFileDialog::getOpenFileName( \
>>>>>>> origin/Dev_Seb
				this,
				tr("Open Log File"),
				QDir::currentPath() + "\\.csv" ,
				tr("Log files (*.txt *.csv);;All files (*.*)"));

	//Extract filename to simplify UI:
	QString path = QDir::currentPath();
	int pathLen = path.length();
	QString shortFileName = filename.mid(pathLen+1);


	//Now we open it:
<<<<<<< HEAD
	logFile.setFileName(filename);
	if(logFile.open(QIODevice::ReadWrite))
	{
		msg = tr("Successfully opened: '") + shortFileName + "'.";
		emit setLogFileStatus(msg);
		qDebug() << msg;

		//Associate stream to file:
		logFileStream.setDevice(&logFile);
		msg = tr("Opened '") + filename + "'.";
		emit setStatusBarMessage(msg);

		//TODO Will it be the best way to handle the file status since another function can close it?
		fileOpened[item] = true;

=======
	logReadingFile.setFileName(filename);
	logReadingFile.size();
	if(logReadingFile.open(QIODevice::ReadOnly))
	{
		qDebug() << msg;

		//Associate stream to file:
		QString line, slaveName, expName;
		QStringList splitLine;
		//uint8_t item, slaveIndex, experimentIndex;

		// Read the logfile informations.
		line = logReadingFile.readLine();

		//TODO : design a better way to parse the header.
		//item = line.section(' ', 3, 3).toInt();
		//slaveIndex = line.section(' ', 8, 8).toInt();
		//slaveName = line.section(' ', 9, 10).remove("(").remove(")");
		//experimentIndex = line.section(' ', 15, 15).toInt();

		//Clear the column's header.
		line = logReadingFile.readLine();

		while (!logReadingFile.atEnd()) {
			line = logReadingFile.readLine();
			splitLine = line.split(',', QString::KeepEmptyParts);
			struct execute_s newitem;
			myExecute_s.append(newitem);
			if(splitLine.length() >= 20)
			{
				myExecute_s.last().accel.x   = splitLine[2].toInt();
				myExecute_s.last().accel.y   = splitLine[3].toInt();
				myExecute_s.last().accel.z   = splitLine[4].toInt();
				myExecute_s.last().gyro.x    = splitLine[5].toInt();
				myExecute_s.last().gyro.y    = splitLine[6].toInt();
				myExecute_s.last().gyro.z    = splitLine[7].toInt();
				myExecute_s.last().strain    = splitLine[8].toInt();
				myExecute_s.last().analog[0] = splitLine[9].toInt();
				myExecute_s.last().analog[1] = splitLine[10].toInt();
				myExecute_s.last().current   = splitLine[11].toInt();
				myExecute_s.last().enc_display = splitLine[12].toInt();
				myExecute_s.last().enc_control = splitLine[13].toInt();
				myExecute_s.last().enc_commut  = splitLine[14].toInt();
				myExecute_s.last().volt_batt = splitLine[15].toInt();
				myExecute_s.last().volt_int  = splitLine[16].toInt();
				myExecute_s.last().temp      = splitLine[17].toInt();
				myExecute_s.last().status1   = splitLine[18].toInt();
				myExecute_s.last().status2   = splitLine[19].toInt();
				FlexSEA_Generic::decodeExecute(&myExecute_s.last());
			}
		}

		//emit setNewLogFileLoaded(myExecute_s);

		msg = tr("Opened '") + filename + "'.";
		emit setStatusBarMessage(msg);
>>>>>>> origin/Dev_Seb
	}

	//If no file selected
	else
	{
<<<<<<< HEAD
		msg = tr("No log file selected.");
		emit setLogFileStatus(msg);
=======
>>>>>>> origin/Dev_Seb
		qDebug() << msg;

		msg = tr("No log file selected or the file couldn't be opened.");
		emit setStatusBarMessage(msg);
<<<<<<< HEAD
		fileOpened[item] = false;
		//todo
=======
>>>>>>> origin/Dev_Seb
	}
}

void DataLogger::writeToFile(uint8_t item, uint8_t slaveIndex, uint8_t expIndex)
{
	qint64 t_ms = 0;
	static qint64 t_ms_initial[4] = {0,0,0,0};
<<<<<<< HEAD
	static bool isFirstTime[4] = {true,true,true,true};
=======
>>>>>>> origin/Dev_Seb

	void (DataLogger::*headerFctPtr) (uint8_t item);
	void (DataLogger::*logFctPtr) (QTextStream *filePtr, uint8_t slaveIndex, \
					   char term, qint64 t_ms, QString t_text);

	QString t_text = "";

	getFctPtrs(slaveIndex, expIndex, &headerFctPtr, &logFctPtr);
<<<<<<< HEAD
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
=======
>>>>>>> origin/Dev_Seb

	//Timestamps:
	logTimestamp(&t_ms, &t_text);
	t_ms -= t_ms_initial[item];

<<<<<<< HEAD
	if(fileOpened[item] == true)
	{
=======
	if(logRecordingFile[item].isOpen())
	{
		//Writting for the first time?
		if(logRecordingFile[item].pos() == 0)
		{
			//Init timestamp ms:
			logTimestamp(&t_ms, &t_text);
			t_ms_initial[item] = t_ms;

			//Header:
			writeIdentifier(item, slaveIndex, expIndex);
			(this->*headerFctPtr)(item);
		}

>>>>>>> origin/Dev_Seb
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
<<<<<<< HEAD
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
=======
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
>>>>>>> origin/Dev_Seb
	}
}

void DataLogger::closeReadingFile(void)
{
<<<<<<< HEAD
	if(fileOpened[item] == true)
	{
		logFileStream << endl;
		logFile.close();
		fileOpened[item] = false;
	}
=======
	if(logReadingFile.isOpen())
	{
		logReadingFile.close();
	}
	myExecute_s.clear();
>>>>>>> origin/Dev_Seb
}

void DataLogger::logReadAllExec(QTextStream *filePtr, uint8_t slaveIndex, \
								char term, qint64 t_ms, QString t_text)
{
	struct execute_s *exPtr;
<<<<<<< HEAD
	myFlexSEA_Generic.assignExecutePtr(&exPtr, SL_BASE_ALL, slaveIndex);
=======
	FlexSEA_Generic::assignExecutePtr(&exPtr, SL_BASE_ALL, slaveIndex);
>>>>>>> origin/Dev_Seb

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
<<<<<<< HEAD
	myFlexSEA_Generic.assignStrainPtr(&stPtr, SL_BASE_ALL, slaveIndex);
=======
	FlexSEA_Generic::assignStrainPtr(&stPtr, SL_BASE_ALL, slaveIndex);
>>>>>>> origin/Dev_Seb

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
<<<<<<< HEAD
	myFlexSEA_Generic.assignGossipPtr(&goPtr, SL_BASE_ALL, slaveIndex);
=======
	FlexSEA_Generic::assignGossipPtr(&goPtr, SL_BASE_ALL, slaveIndex);
>>>>>>> origin/Dev_Seb

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
<<<<<<< HEAD
	myFlexSEA_Generic.assignRicnuPtr(&myPtr, SL_BASE_ALL, slaveIndex);
=======
	FlexSEA_Generic::assignRicnuPtr(&myPtr, SL_BASE_ALL, slaveIndex);
>>>>>>> origin/Dev_Seb

	logFileStream << t_text << ',' << \
						t_ms << ',' << \
						myPtr->ex.accel.x << ',' << \
						myPtr->ex.accel.y << ',' << \
						myPtr->ex.accel.z << ',' << \
						myPtr->ex.gyro.x << ',' << \
						myPtr->ex.gyro.y << ',' << \
						myPtr->ex.gyro.z << ',' << \
						myPtr->ex.current << ',' << \
						myPtr->ex.enc_motor << ',' << \
<<<<<<< HEAD
						myPtr->ex.enc_joint << ',' << \
=======
						myPtr->ex.enc_control << ',' << \
>>>>>>> origin/Dev_Seb
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
<<<<<<< HEAD
	myFlexSEA_Generic.assignManagePtr(&mnPtr, SL_BASE_MN, slaveIndex);
=======
	FlexSEA_Generic::assignManagePtr(&mnPtr, SL_BASE_MN, slaveIndex);
>>>>>>> origin/Dev_Seb

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
<<<<<<< HEAD
	myFlexSEA_Generic.init();
=======
>>>>>>> origin/Dev_Seb
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

void DataLogger::writeIdentifier(uint8_t item, uint8_t slaveIndex, uint8_t expIndex)
{
	QString msg, slaveName, expName;
<<<<<<< HEAD
	myFlexSEA_Generic.getSlaveName(SL_BASE_ALL, slaveIndex, &slaveName);
	myFlexSEA_Generic.getExpName(expIndex, &expName);
=======
	FlexSEA_Generic::getSlaveName(SL_BASE_ALL, slaveIndex, &slaveName);
	FlexSEA_Generic::getExpName(expIndex, &expName);
>>>>>>> origin/Dev_Seb

	//Top of the file description:
	msg = "[Datalogging: Item = " + QString::number(item) + " | Slave index = " + \
						QString::number(slaveIndex) + " (" + slaveName + ") | " + \
						"Experiment index = " + QString::number(expIndex) + " (" + \
						expName + ")]\n";
	qDebug() << msg;
<<<<<<< HEAD
	if(fileOpened[item] == true)
=======
	if(logRecordingFile[item].isOpen())
>>>>>>> origin/Dev_Seb
	{
		logFileStream << msg;
	}
}

void DataLogger::writeExecuteReadAllHeader(uint8_t item)
{
<<<<<<< HEAD
	if(fileOpened[item] == true)
=======
	if(logRecordingFile[item].isOpen())
>>>>>>> origin/Dev_Seb
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
<<<<<<< HEAD
	if(fileOpened[item] == true)
=======
	if(logRecordingFile[item].isOpen())
>>>>>>> origin/Dev_Seb
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
<<<<<<< HEAD
	if(fileOpened[item] == true)
=======
	if(logRecordingFile[item].isOpen())
>>>>>>> origin/Dev_Seb
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
<<<<<<< HEAD
	if(fileOpened[item] == true)
=======
	if(logRecordingFile[item].isOpen())
>>>>>>> origin/Dev_Seb
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
<<<<<<< HEAD
	if(fileOpened[item] == true)
=======
	if(logRecordingFile[item].isOpen())
>>>>>>> origin/Dev_Seb
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

