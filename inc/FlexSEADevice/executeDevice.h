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
	[This file] ExecuteDevice: Execute Device Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-07 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef EXECUTEDEVICE_H
#define EXECUTEDEVICE_H

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

//Qualitative:
#define V_LOW						1
#define V_NORMAL					0
#define V_HIGH						2
#define T_NORMAL					0
#define T_WARNING					1
#define T_ERROR						2
#define BATT_CONNECTED				0
#define BATT_DISCONNECTED			1
//If everything is normal STATUS1 == 0

//Display and conversions:
//========================

#define GET_WDCLK_FLAG(status1)		((status1 >> 7) & 0x01)
#define GET_DISCON_FLAG(status1)	((status1 >> 6) & 0x01)
#define GET_OVERTEMP_FLAG(status1)	((status1 >> 4) & 0x03)
#define GET_VB_FLAG(status1)		((status1 >> 2) & 0x03)
#define GET_VG_FLAG(status1)		((status1 >> 0) & 0x03)
#define GET_3V3_FLAG(status2)		((status2 >> 0) & 0x03)
#define GET_FSM_FLAG(status2)		((status2 >> 7) & 0x01)

//PSoC 5 ADC conversions:
#define P5_ADC_SUPPLY				5.0
#define P5_ADC_MAX					4096

//PSoC 4 ADC conversions:
#define P4_ADC_SUPPLY				5.0
#define P4_ADC_MAX					2048
#define P4_T0						0.5
#define P4_TC						0.01

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui
{
	class ExecuteDevice;
}

class ExecuteDevice : public FlexseaDevice
{
public:
	explicit ExecuteDevice(void);
	explicit ExecuteDevice(execute_s *devicePtr);
	virtual ~ExecuteDevice();

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
	int length(void) {return exList.length();}
	void clear(void);
	void appendEmptyLine(void);
	static QString getStatusStr(struct execute_s *ex);

	QList<struct execute_s*> exList;
	QList<bool> ownershipList;
	//QList<int> eventFlags;

	static void decode(struct execute_s *exPtr);

private:
	static QStringList header;
	static QStringList headerDecoded;
};


//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // EXECUTEDEVICE_H
