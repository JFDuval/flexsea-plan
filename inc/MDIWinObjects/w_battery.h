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
	[This file] w_battery.h: Battery View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

#ifndef W_BATTERY_H
#define W_BATTERY_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "batteryDevice.h"
#include "testBenchProject.h"
#include "define.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Battery;
}

class W_Battery : public QWidget, public Counter<W_Battery>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Battery(QWidget *parent = 0,
					   FlexseaDevice *currentLog = nullptr,
					   BatteryDevice *batteryLogPtrInit = nullptr,
					   TestBenchProject *testBenchLogPtrInit = nullptr,
					   DisplayMode mode = DisplayLiveData,
					   QList<BatteryDevice> *deviceListPtr = nullptr);
	~W_Battery();

	//Function(s):


public slots:

	void refreshDisplay(void);
	void refreshDisplayLog(int index, FlexseaDevice * devPtr);
	void updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr);

signals:
	void windowClosed(void);

private:
	//Variables & Objects:
	Ui::W_Battery *ui;

	DisplayMode displayMode, lastDisplayMode;

	QList<BatteryDevice> *deviceList;
	BatteryDevice *batteryLog;
	TestBenchProject *testBenchLog;

	//Function(s):
	void initLive(void);
	void initLog(FlexseaDevice *devPtr);
	void display(BatteryDevice *devicePtr, int index);
	void display(struct battery_s *ba);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_BATTERY_H
