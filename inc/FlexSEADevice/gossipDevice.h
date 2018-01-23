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
	[This file] GossipDevice: Gossip Device Data Class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-08 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef GOSSIPDEVICE_H
#define GOSSIPDEVICE_H

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
	class GossipDevice;
}

class GossipDevice : public FlexseaDevice
{
public:
	// Constructor & Destructor
	explicit GossipDevice(void);
	explicit GossipDevice(gossip_s *devicePtr);

	// Interface implementation
	QStringList getHeader(void) {return header;}
	QStringList getHeaderUnit(void) {return headerUnitList;}
	QString getLastDataEntry(void);
	struct std_variable getSerializedVar(int headerIndex, int index);
	void appendSerializedStr(QStringList *splitLine);
	void decodeLastElement(void);
	void decodeAllElement(void);
	int length(void) {return goList.length();}
	void clear(void);
	void appendEmptyElement(void);

	// Class Function
	QString getStatusStr(int index);
	static void decode(struct gossip_s *goPtr);

	// Class Variable
	QList<struct gossip_s *> goList;

private:
	static QStringList header;
	static QStringList headerUnitList;
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // GOSSIPDEVICE_H
