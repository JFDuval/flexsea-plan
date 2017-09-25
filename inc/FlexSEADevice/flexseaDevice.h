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
	[This file] FlexseaDevice: Interface class flexSEA device
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-07 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef FLEXSEADEVICE_H
#define FLEXSEADEVICE_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QString>
#include <QList>
#include <time.h>

//****************************************************************************
// Definition(s)
//****************************************************************************

//Variable formats:
#include <flexsea_dataformats.h>

enum DataSourceFile
{
	LiveDataFile,
	LogDataFile
};

struct std_variable
{
	void *rawGenPtr;
	uint8_t format;
	int32_t *decodedPtr;
};

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui
{
	class FlexseaDevice;
}

struct TimeStamp
{
	QString date;
	int32_t ms;
};

class FlexseaDevice
{
public:
	explicit FlexseaDevice();
	virtual ~FlexseaDevice(){}

	virtual QString getHeaderStr(void) = 0;
	virtual QStringList getHeaderList(void) = 0;
	virtual QStringList getHeaderDecList(void) = 0;
	virtual QString getLastSerializedStr(void) = 0;
	virtual struct std_variable getSerializedVar(int parameter, int index) = 0;
	virtual struct std_variable getSerializedVar(int parameter) { return getSerializedVar(parameter, 0); }
	virtual void appendSerializedStr(QStringList *splitLine) = 0;
	virtual void decodeLastLine(void) = 0;
	virtual void decodeAllLine(void) = 0;
	virtual void appendEmptyLine(void) = 0;
	virtual int length() = 0;
	virtual void clear(void);

	static QString getSlaveType(QStringList *splitLine);

	QString getIdentifierStr(void);
	QStringList getIdentifierStrList(void);
	void saveIdentifierStr(QStringList *splitLine);

	enum DataSourceFile dataSource;

	QString shortFileName;
	QString fileName;

	uint8_t slaveID;
	QString slaveTypeName;
	QString slaveName;


	QString targetSlaveName;
	int		experimentIndex;
	QString experimentName;
	QString userNotes = "Default user notes.";

	int		frequency;
	bool isCurrentlyLogging;

	void applyTimestamp();
	clock_t initialClock;

	QList<struct TimeStamp> timeStamp;
	QList<int> eventFlags;
	int serializedLength;
};

#endif // FLEXSEADEVICE_H
