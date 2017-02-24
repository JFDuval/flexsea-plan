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
#include <QStringList>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>

#include "batteryDevice.h"
#include "executeDevice.h"
#include "gossipDevice.h"
#include "manageDevice.h"
#include "ricnuProject.h"
#include "strainDevice.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

bool DataLogger::sessionDirectoryCreated = false;

DataLogger::DataLogger(QWidget *parent,
					   ExecuteDevice *executeInitPtr,
					   ManageDevice *manageInitPtr,
					   GossipDevice *gossipInitPtr,
					   BatteryDevice *batteryInitPtr,
					   StrainDevice *strainInitPtr,
					   RicnuProject *ricnuInitPtr) :
	QWidget(parent)
{
	executeDevPtr = executeInitPtr;
	manageDevPtr = manageInitPtr;
	gossipDevPtr = gossipInitPtr;
	batteryDevPtr = batteryInitPtr;
	strainDevPtr = strainInitPtr;
	ricnuDevPtr = ricnuInitPtr;

	init();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

// param: fileIndex		when we log, if we go past MAX_NUM_LINES we start a new file, ending in -x where x is one higher than previous file
//						default value is shown in header (1 as of now)
QString DataLogger::generateFileName(QString shortFileName, int fileIndex)
{
	QString result = shortFileName;

	// Replace whitespace by underscore
	result.replace(" ", "_");
	// Replace double underscores with single underscore



	// Remove invalid character for a filename(According to Windows)
	result.remove(QRegExp("[<>:\"/|?*]"));
	result.remove("\\");

	// Add date and time to the short file name
	result.prepend(QDate::currentDate().toString("yyyy-MM-dd_") +
						  QTime::currentTime().toString("HH'h'mm'm'ss's'_"));
	QString prev;
	do {
		prev = result;
		result.replace("__", "_");
	}
	while(prev.compare(result) != 0);

	if(result.mid(result.length()-4) == ".csv")
	result = result.mid(0, result.length()-4);
	result.append("-" + QString::number(fileIndex) + ".csv");
	return result;
}

void DataLogger::openRecordingFile(FlexseaDevice *devicePtr)
{
	const QString U_SCORE = "_";
	QString shortFileName = devicePtr->slaveName + U_SCORE +
								 devicePtr->experimentName + U_SCORE +
								 QString::number(devicePtr->frequency) + "Hz.csv";

	devicePtr->shortFileName = shortFileName;

	bool isDeviceOpenAlready = false;
	if(deviceFileMap.contains(devicePtr))
	{
		deviceRecordIterator i = deviceFileMap.find(devicePtr);
		if(i.value().file->isOpen())
			isDeviceOpenAlready = true;
	}

	if(isDeviceOpenAlready)
	{
		setStatus("File already open. Close it before opening a new one");
	}
	else
	{
		QFile* logFile = openfile(generateFileName(devicePtr->shortFileName));
		FileRecord record(logFile, new int(0), new int(1));
		deviceFileMap.insert(devicePtr, record);
	}
}

QFile* DataLogger::openfile(QString name)
{
	// Create session directory the first time you log
	if(sessionDirectoryCreated == false){initLogDirectory();}

	// Set the folder to current directory
	QDir::setCurrent(planGUIRootPath + "\\" + logFolder + "\\" + sessionFolder);

	// Set the filename from the current directory
	QString fullPathToFile = QDir::currentPath() + "/" + name;

	QFile* logFile = new QFile();
	if(!logFile)
	{
		qDebug("DataLogger::openfile: Failed to create file object");
		return nullptr;
	}

	logFile->setFileName(fullPathToFile);

	// Try to open the file.
	if(logFile->open(QIODevice::ReadWrite))
	{
		// TODO Datalogger should not know that there's a logFile and bar
		// status. Abstraction principle is not respected here. Is there a way
		// to use some sort of return value instead of signal slot?

		//Associate stream to file:
		//TODO this won't work if we ever want to stream multiple files at once
		//We could add a QTextStream* to the record in the hashmap though
		//in which case this line would move elsewhere
		logFileStream.setDevice(logFile);
		setStatus(tr("Opened '") + fullPathToFile + "'.");
	}

	//If no file selected
	else
	{
		delete logFile;
		logFile = nullptr;
		setStatus("No log file selected, or the file couldn't be opened.");
	}
	return logFile;
}

void DataLogger::openReadingFile(bool * isOpen, FlexseaDevice **devPtr)
{
	*isOpen = false;
	FlexseaDevice *flexSEAPtr;

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
		setStatus("Error : No log file selected or the file couldn't be opened.");
		return;
	}

	//Check if the file is empty
	if(logReadingFile.size() == 0)
	{
		setStatus("Error : Loaded file was empty.");
		return;
	}

	// Read and save the logfile informations.
	QString line;
	QStringList splitLine;

	line = logReadingFile.readLine();
	splitLine = line.split(',', QString::KeepEmptyParts);

	//Check if the file header contain the expected number of data
	if(splitLine.length() < 14)
	{
		setStatus(\
		"Error : Loaded file header was not compatible (Header's too short)");
		return;
	}

	QString slavetype = splitLine[13];
	slavetype = slavetype.simplified();

	// Choose the right device class based on the slave Type.
	if	   (slavetype == executeDevPtr->slaveType)	{flexSEAPtr = executeDevPtr;}
	else if(slavetype == manageDevPtr->slaveType)	{flexSEAPtr = manageDevPtr;}
	else if(slavetype == gossipDevPtr->slaveType)	{flexSEAPtr = gossipDevPtr;}
	else if(slavetype == batteryDevPtr->slaveType)	{flexSEAPtr = batteryDevPtr;}
	else if(slavetype == strainDevPtr->slaveType)	{flexSEAPtr = strainDevPtr;}
	else if(slavetype == ricnuDevPtr->slaveType)	{flexSEAPtr = ricnuDevPtr;}
	else
	{
		setStatus("Error : Loaded file Slave Type is not supported.");
		return;
	}

	flexSEAPtr->clear();
	flexSEAPtr->logItem			= splitLine[1].toInt();
	flexSEAPtr->slaveIndex		= splitLine[3].toInt();
	flexSEAPtr->slaveName		= splitLine[5];
	flexSEAPtr->experimentIndex	= splitLine[7].toInt();
	flexSEAPtr->experimentName	= splitLine[9];
	flexSEAPtr->frequency		= splitLine[11].toInt();
	flexSEAPtr->shortFileName	= shortFileName;
	flexSEAPtr->fileName		= filename;

	//Clear the column's header.
	line = logReadingFile.readLine();
	splitLine = line.split(',', QString::KeepEmptyParts);
	//Check if data header contain the number of expected field
	if(splitLine.length() < flexSEAPtr->serializedLength)
	{
		setStatus("Column header it too short. Not supported");
		flexSEAPtr->clear();
		return;
	}

	while (!logReadingFile.atEnd())
	{
		line = logReadingFile.readLine();
		splitLine = line.split(',', QString::KeepEmptyParts);

		flexSEAPtr->appendSerializedStr(&splitLine);
	}

	flexSEAPtr->decodeAllLine();

	setStatus(tr("Opened '") + filename + "'.");

	// Return variable
	*isOpen = true;
	*devPtr = flexSEAPtr;
}

void DataLogger::writeToFile(FlexseaDevice *devicePtr)
{
	// Verify that the log file is properly opened.
	FileRecord record = deviceFileMap.value(devicePtr);
	QFile* logFile = record.file;
	int* numLinesWritten = record.numLines;

	if(logFile && logFile->isOpen())
	{
		if(*numLinesWritten >= MAX_NUM_LINES)
		{
			int* fileIndex = record.fileIndex;
			(*fileIndex)++;
			QString nextFileName = generateFileName(devicePtr->shortFileName, *fileIndex);
			logFile->close();
			delete logFile;
			logFile = openfile(nextFileName);
			*numLinesWritten = 0;
			deviceFileMap[devicePtr].file = logFile;
		}

		//Writting for the first time?
		if(logFile->pos() == 0)
		{
			//Header:
			logFileStream << devicePtr->getIdentifier() << endl;
			logFileStream << devicePtr->getHeaderStr() << endl;
		}

		//And we add to the text file:
		logFileStream << devicePtr->getLastSerializedStr() << endl;
		(*numLinesWritten)++;

	}
	else
	{
		emit setStatusBarMessage("Datalogger: no file selected.");
	}
}

void DataLogger::closeRecordingFile(FlexseaDevice *devicePtr)
{
	QFile* logFile = deviceFileMap.value(devicePtr).file;
	if(logFile && logFile->isOpen())
	{
		logFileStream << endl;
		logFile->close();
		delete logFile;
		delete deviceFileMap.value(devicePtr).numLines;
		delete deviceFileMap.value(devicePtr).fileIndex;
		deviceFileMap.remove(devicePtr);
	}
}

void DataLogger::closeReadingFile(void)
{
	if(logReadingFile.isOpen())
	{
		logReadingFile.close();
	}

	// TODO = Clear the proper structure.
	executeDevPtr->clear();
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void DataLogger::init(void)
{
	myTime = new QDateTime;
	for(int item = 0; item < LOG_NUM; ++item)
	{
	}
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
	}
	else
	{
		qDebug() << "Using existing """ + logFolder + """ directory";
	}

	QDir::setCurrent(logFolder);

	// Create this session folder
	QDir().mkdir(sessionFolder);

	sessionDirectoryCreated = true;
}

void DataLogger::setStatus(QString str)
{
	emit setStatusBarMessage(str);
	qDebug() << str;
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

