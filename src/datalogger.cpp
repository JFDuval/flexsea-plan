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

DataLogger::DataLogger(QWidget *parent,
					   ExecuteDevice *executeInitPtr,
					   ManageDevice *manageInitPtr,
					   GossipDevice *gossipInitPtr,
					   BatteryDevice *batteryInitPtr,
					   StrainDevice *strainInitPtr,
					   RicnuDevice *ricnuInitPtr) :
	QWidget(parent)
{
	executeDevPtr = executeInitPtr;
	manageDevPtr = manageInitPtr;
	gossipDevPtr = gossipInitPtr;
	batteryDevPtr = batteryInitPtr;
	strainDevPtr = strainInitPtr;
	ricnuDevPtr = ricnuInitPtr;

	initLogDirectory();
	init();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void DataLogger::openRecordingFile(FlexseaDevice *devicePtr, uint8_t item)
{
	QString shortFileName = devicePtr->shortFileName;

	if(logRecordingFile[item].isOpen())
	{
		setStatus("File already open. Close it before opening a new one");
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

		setStatus(tr("Opened '") + fileName + "'.");
	}

	//If no file selected
	else
	{
		setStatus("No log file selected, or the file couldn't be opened.");
	}
}

void DataLogger::openReadingFile(bool * isOpen)
{
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
	if(splitLine.length() < 12)
	{
		setStatus("Error : Loaded file format was not compatible");
		return;
	}

	executeDevPtr->logItem			= splitLine[1].toInt();
	executeDevPtr->slaveIndex		= splitLine[3].toInt();
	executeDevPtr->slaveName		= splitLine[5];
	executeDevPtr->experimentIndex	= splitLine[7].toInt();
	executeDevPtr->experimentName	= splitLine[9];
	executeDevPtr->frequency		= splitLine[11].toInt();
	executeDevPtr->shortFileName	= shortFileName;
	executeDevPtr->fileName			= filename;

	//Clear the column's header.
	line = logReadingFile.readLine();
	splitLine = line.split(',', QString::KeepEmptyParts);
	//Check if data header contain the number of expected field
	if(splitLine.length() < executeDevPtr->serializedLength)
	{
		setStatus("File format is not compatible");
		executeDevPtr->clear();
		return;
	}

	while (!logReadingFile.atEnd())
	{
		line = logReadingFile.readLine();
		splitLine = line.split(',', QString::KeepEmptyParts);

		executeDevPtr->appendSerializedStr(&splitLine);
	}

	executeDevPtr->decodeAllLine();

	setStatus(tr("Opened '") + filename + "'.");

	*isOpen = true;
}

void DataLogger::writeToFile(FlexseaDevice *devicePtr, uint8_t item)
{
	// Verify that the log file is properly opened.
	if(logRecordingFile[item].isOpen())
	{
		//Writting for the first time?
		if(logRecordingFile[item].pos() == 0)
		{
			//Header:
			logFileStream << devicePtr->getIdentifier() << endl;
			logFileStream << devicePtr->getHeaderStr() << endl;
		}

		//And we add to the text file:
		logFileStream << devicePtr->getLastSerializedStr() << endl;
	}
	else
	{
		emit setStatusBarMessage("Datalogger: no file selected.");
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

	executeDevPtr->clear();
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

void DataLogger::setStatus(QString str)
{
	emit setStatusBarMessage(str);
	qDebug() << str;
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

