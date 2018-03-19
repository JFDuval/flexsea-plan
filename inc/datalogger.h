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

#include "executeDevice.h"
#include "manageDevice.h"
#include "gossipDevice.h"
#include "batteryDevice.h"
#include "strainDevice.h"
#include "ricnuProject.h"
#include "pocketDevice.h"
#include <QHash>
#include "ankle2DofProject.h"
#include "rigidDevice.h"

#define LOG_NUM 4

#define MAX_NUM_LINES 50000

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
						RicnuProject *ricnuInitPtr = nullptr,
						Ankle2DofProject *ankle2DofInitPtr = nullptr,
						RigidDevice *rigidInitPtr = nullptr,
						PocketDevice *pocketInitPtr = nullptr,
						QString appPath = "");

public slots:
	void openRecordingFile(FlexseaDevice *devicePtr);
	void closeRecordingFile(FlexseaDevice *devicePtr);
	void openReadingFile(bool * isOpen, FlexseaDevice **devPtr);
	void closeReadingFile(void);
	void writeToFile(FlexseaDevice *devicePtr);

private slots:

private:
	//Variables & Objects:
	class FileRecord {
		public:
			FileRecord(QFile* f = nullptr, int* i = nullptr, int* n = nullptr):
				file(f), numLines(i), fileIndex(n){}
			QFile* file;
			int* numLines;
			int* fileIndex;
	};
	QHash<FlexseaDevice*, FileRecord> deviceFileMap;
	typedef QHash<FlexseaDevice*, FileRecord>::iterator deviceRecordIterator;

	QFile logReadingFile;
	bool sessionDirectoryCreated = false;

	ExecuteDevice *executeDevPtr;
	ManageDevice *manageDevPtr;
	GossipDevice *gossipDevPtr;
	BatteryDevice *batteryDevPtr;
	StrainDevice *strainDevPtr;
	RicnuProject *ricnuDevPtr;
	Ankle2DofProject *ankle2DofDevPtr;
	RigidDevice *rigidDevPtr;
	PocketDevice *pocketDevPtr;

	QString planGUIRootPath;
	QString logFolder;
	QString sessionFolder;

	QTextStream logFileStream;
	QDateTime *myTime;

	//Function(s):
	void init(void);
	void logTimestamp(qint64 *t_ms, QString *t_text);
	void writeManageA2DOFHeader(uint8_t item);
	QFile* openfile(QString name);
	void initLogDirectory(void);
	void setStatus(QString str);
	QString generateFileName(QString shortFileName, int fileIndex = 1);

signals:
	void setStatusBarMessage(QString msg);
	void logFileName(QString fn, QString fnp);
};

//****************************************************************************
// Definition(s)
//****************************************************************************


#endif // DATALOGGER_H
