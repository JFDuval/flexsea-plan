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
	This interface described as a generic interface to a flexsea Device.
	This data structure can function in two main mode.

	Live Data File: In live data file, there is only one element in the list
		that is continuously update. Last element is used to access the data.

	Log Data File: In this mode, the data structure list is filled with the
		data from a log file. This data is then accessed based on the index.
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
	// Constructor & Destructor
	explicit FlexseaDevice();
	virtual ~FlexseaDevice(){}

	// Interface function to implement
	/*!
		Return the header of the raw data structure
		\return A QstringList containing the headers of the raw data structure
	*/
	virtual QStringList getHeader(void) = 0;

	/*!
		Return the decoded mesure unit of the data structure
		\return A QstringList containing the decoded mesure unit of the data
		structure
	*/
	virtual QStringList getHeaderUnit(void) = 0;

	/*!
		Return the most recent raw data entry element.
		\return A QString containing last raw data entry element.
	*/
	virtual QString getLastDataEntry(void) = 0;

	/*!
		Give a serial access to the data. The headerIndex match the header
		index order. Allow a programm to acces specific data in a generic way.
		\param headerIndex : Index indicating wich data source is desired.
							 Indicate the datasource by provinding the index of
							 the datasource based on the header. Must be within
							 the header lenght.
		\param index : index indicating the element in the list desired. Used
						when the device is a log of an experiment.(LogDataFile)
		\return A std_variable containing the data format, the payload and the
				decoded value.
	*/
	virtual struct std_variable getSerializedVar(int headerIndex, int index) = 0;

	/*!
		Same as getSerializedVar(int headerIndex, int index) but with the index
		parameter force to 0. Used in LiveDataFile context.
	*/
	virtual struct std_variable getSerializedVar(int headerIndex) { return getSerializedVar(headerIndex, 0); }

	/*!
		Used to load a log file element in the flexsea Device. Used in
		LogDataFile mode.
		\param splitLine : a stringlist with a single element of the data
						   structure
	*/
	virtual void appendSerializedStr(QStringList *splitLine) = 0;

	/*!
		Fill the decoded data section of the structure based on the raw data
		conversion. Only the last element of the list is decoded.
	*/
	virtual void decodeLastElement(void) = 0;

	/*!
		Fill the decoded data section of the structure based on the raw data
		conversion. All the element are decode.
	*/
	virtual void decodeAllElement(void) = 0;

	/*!
		Add an empty element properly to the data structure.
	*/
	virtual void appendEmptyElement(void) = 0;

	/*!
		Return the lenght of the data list.
	*/
	virtual int length() = 0;

	/*!
		Delete all the data element properly.
	*/
	virtual void clear(void);

	// Interface generic function
	static QString getSlaveType(QStringList *splitLine);

	QString getIdentifierStr(void);
	QStringList getIdentifierStrList(void);
	void saveIdentifierStr(QStringList *splitLine);

	// Interface generic variable
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
