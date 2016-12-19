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
	[This file] Datalogger
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef DATALOGGER_H
#define DATALOGGER_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QList>
#include "flexsea_generic.h"
#include "main.h"
#include "logFile.h"

#include "executeDevice.h"
#include "manageDevice.h"
#include "gossipDevice.h"
#include "batteryDevice.h"
#include "strainDevice.h"
#include "ricnuDevice.h"

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui {
class DataLogger;
}

class DataLogger : public QWidget
{
	Q_OBJECT

public:
	explicit DataLogger(QWidget *parent = 0,
						ExecuteDevice *executeInitPtr = nullptr,
						ManageDevice *manageInitPtr = nullptr,
						GossipDevice *gossipInitPtr = nullptr,
						BatteryDevice *batteryInitPtr = nullptr,
						StrainDevice *strainInitPtr = nullptr,
						RicnuDevice *ricnuInitPtr = nullptr);

	LogFile * getLogFilePtr(void) {return &myLogFile;}

public slots:
	void openRecordingFile(FlexseaDevice *devicePtr, uint8_t item);
	void closeRecordingFile(uint8_t item);
	void openReadingFile(bool * isOpen);
	void closeReadingFile(void);
	void writeToFile(FlexseaDevice *devicePtr, uint8_t item);

private slots:

private:
	//Variables & Objects:
	QFile logRecordingFile[4];
	QFile logReadingFile;

	ExecuteDevice *executeDevPtr;
	ManageDevice *manageDevPtr;
	GossipDevice *gossipDevPtr;
	BatteryDevice *batteryDevPtr;
	StrainDevice *strainDevPtr;
	RicnuDevice *ricnuDevPtr;



	QString planGUIRootPath;
	QString logFolder;
	QString sessionFolder;

	QTextStream logFileStream;
	QDateTime *myTime;

	bool fileOpened[4];

	LogFile myLogFile;

	//Function(s):
	void init(void);
	void logTimestamp(qint64 *t_ms, QString *t_text);
	void openfile(uint8_t item, QString shortFileName);
	void initLogDirectory(void);
	void setStatus(QString str);

signals:
	void setStatusBarMessage(QString msg);
	void setNewLogFileLoaded(QList<struct execute_s> &data);
};

//****************************************************************************
// Definition(s)
//****************************************************************************


#endif // DATALOGGER_H
