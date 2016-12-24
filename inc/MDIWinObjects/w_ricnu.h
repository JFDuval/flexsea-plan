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
	[This file] WinViewRicnu: RIC/NU Kneww View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-12 | jfduval | New file
	* 2016-09-12 | jfduval | Enabling stream
****************************************************************************/

#ifndef W_RICNU_H
#define W_RICNU_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexsea_generic.h"
#include "ricnuDevice.h"
#include "define.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Ricnu;
}

class W_Ricnu : public QWidget, public Counter<W_Ricnu>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Ricnu(QWidget *parent = 0,
					 RicnuDevice *deviceLogPtr = nullptr,
					 DisplayMode mode = DisplayLiveData,
					 QList<RicnuDevice> *deviceListPtr = nullptr);
	~W_Ricnu();

public slots:
	void refreshDisplay(void);
	void refreshDisplayLog(int index, FlexseaDevice * devPtr);
	void updateDisplayMode(DisplayMode mode);

signals:
	void windowClosed(void);

private:
	//Variables & Objects:
	Ui::W_Ricnu *ui;
	int active_slave, active_slave_index;

	DisplayMode displayMode;

	QList<RicnuDevice> * deviceList;
	RicnuDevice *deviceLog;

	//Function(s):
	void initLive(void);
	void initLog(void);
	void display(RicnuDevice *devicePtr, int index);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_RICNU_H
