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
	[This file] LogFile: Log File data class
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-11-30 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef LOGFILE_H
#define LOGFILE_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "main.h"

struct log_ss
{
	QString timeStampDate;
	int32_t timeStamp_ms;

	struct execute_s execute;
	struct manage_s manage;
	struct ricnu_s ricnu;
	struct strain_s strain;
	struct gossip_s gossip;
	struct battery_s battery;
};

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui {
class LogFile;
}

class LogFile : QWidget
{
	Q_OBJECT

public:
	explicit LogFile(QWidget *parent = 0);
	void init(void);
	void clear(void);
	void newDataLine(void);
	void decodeLastLine(void);
	void decodeAllLine(void);

	QString shortFileName;
	QString fileName;
	int dataloggingItem;
	int SlaveIndex;
	QString SlaveName;
	int experimentIndex;
	QString experimentName;
	int frequency;
	QList<struct log_ss> data;

};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // LOGFILE_H
