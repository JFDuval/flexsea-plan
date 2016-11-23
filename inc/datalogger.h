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

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui {
class DataLogger;
}

struct log_s
{
	QString timeStampDate;
	int32_t timeStamp_ms;

	struct execute_s execute;
};

struct logContainer_s
{
	QString shortFileName;
	QString fileName;
	QString dataloggingItem;
	QString SlaveName;
	QString SlaveIndex;
	QString experimentIndex;
	QString experimentName;
	uint16_t frequency;
	QList<struct log_s> logList;
};

class DataLogger : public QWidget
{
	Q_OBJECT

public:
	explicit DataLogger(QWidget *parent = 0);
	struct logContainer_s * getLogPtr(void) {return &myLog;}

public slots:
	void openRecordingFile(uint8_t item);
	void openRecordingFile(uint8_t item, QString fileName);
	void closeRecordingFile(uint8_t item);
	void openReadingFile(bool * isOpen);
	void closeReadingFile(void);
	void writeToFile(uint8_t item, uint8_t slaveIndex,
					 uint8_t expIndex, uint16_t refreshRate);

private slots:

private:
	//Variables & Objects:
	QFile logRecordingFile[4];
	QFile logReadingFile;

	QTextStream logFileStream;
	QDateTime *myTime;

	bool fileOpened[4];

	struct logContainer_s myLog;

	//Function(s):
	void init(void);
	void logTimestamp(qint64 *t_ms, QString *t_text);
	void writeIdentifier(uint8_t item, uint8_t slaveIndex,
									 uint8_t expIndex, uint16_t refreshRate);
	void writeExecuteReadAllHeader(uint8_t item);
	void writeReadAllRicnuHeader(uint8_t item);
	void writeManageReadAllHeader(uint8_t item);
	void writeStrainReadAllHeader(uint8_t item);
	void writeGossipReadAllHeader(uint8_t item);
	void openfile(uint8_t item, QString fileName, QString shortFileName);
	void logDirectory(void);
	void logReadAllExec(QTextStream *filePtr, uint8_t slaveIndex, \
							char term, qint64 t_ms, QString t_text);
	void logReadAllRicnu(QTextStream *filePtr, uint8_t slaveIndex, \
							char term, qint64 t_ms, QString t_text);
	void logReadAllManage(QTextStream *filePtr, uint8_t slaveIndex, \
									char term, qint64 t_ms, QString t_text);
	void logReadAllGossip(QTextStream *filePtr, uint8_t slaveIndex, \
									char term, qint64 t_ms, QString t_text);
	void logReadAllStrain(QTextStream *filePtr, uint8_t slaveIndex, \
									char term, qint64 t_ms, QString t_text);
	void getFctPtrs(uint8_t slaveIndex, uint8_t expIndex, \
					void (DataLogger::**myHeaderFctPtr) (uint8_t item), \
					void (DataLogger::**myLogFctPtr) (QTextStream *filePtr, uint8_t slaveIndex, \
					char term, qint64 t_ms, QString t_text));

signals:
	void setStatusBarMessage(QString msg);
	void setNewLogFileLoaded(QList<struct execute_s> &data);
};

//****************************************************************************
// Definition(s)
//****************************************************************************


#endif // DATALOGGER_H
