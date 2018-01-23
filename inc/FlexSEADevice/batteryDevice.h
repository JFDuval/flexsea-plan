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
	[This file] batteryDevice: Battery Device Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-08 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef BATTERYDEVICE_H
#define BATTERYDEVICE_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QList>
#include <QString>
#include <flexsea_global_structs.h>
#include "flexseaDevice.h"

//****************************************************************************
// Definition(s)
//****************************************************************************

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui
{
	class BatteryDevice;
}

class BatteryDevice : public FlexseaDevice
{
public:
	// Constructor & Destructor
	explicit BatteryDevice(void);
	explicit BatteryDevice(battery_s *devicePtr);

	// Interface implementation
	QStringList getHeader(void) {return header;}
	QStringList getHeaderUnit(void) {return headerUnitList;}
	QString getLastDataEntry(void);
	struct std_variable getSerializedVar(int headerIndex, int index);
	void appendSerializedStr(QStringList *splitLine);
	void decodeLastElement(void);
	void decodeAllElement(void);
	int length(void) {return baList.length();}
	void clear(void);
	void appendEmptyElement(void);

	// Class Function
	QString getStatusStr(int index);
	static void decode(struct battery_s *baPtr);
	static void decompressRawBytes(struct battery_s *baPtr);

	// Class Variable
	QList<struct battery_s*> baList;

private:
	static QStringList header;
	static QStringList headerUnitList;
};


//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // BATTERYDEVICE_H
