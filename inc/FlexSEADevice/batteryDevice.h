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
	explicit BatteryDevice(void);
	explicit BatteryDevice(battery_s *devicePtr);

	// Interface implementation
	QString getHeaderStr(void);
	QStringList getHeaderList(void) {return header;}
	QStringList getHeaderDecList(void) {return headerDecoded;}
	QString getLastSerializedStr(void);
	struct std_variable getSerializedVar(int parameter);
	struct std_variable getSerializedVar(int parameter, int index);
	void appendSerializedStr(QStringList *splitLine);
	void decodeLastLine(void);
	void decodeAllLine(void);
	int length(void) {return baList.length();}
	void clear(void);
	void appendEmptyLine(void);
	QString getStatusStr(int index);

	QList<struct battery_s*> baList;
	static void decode(struct battery_s *baPtr);
	static void decompressRawBytes(struct battery_s *baPtr);

private:
	static QStringList header;
	static QStringList headerDecoded;
};


//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // BATTERYDEVICE_H
