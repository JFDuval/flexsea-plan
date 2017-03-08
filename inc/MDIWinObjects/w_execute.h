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
	[This file] w_execute.h: Execute View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef W_EXECUTE_H
#define W_EXECUTE_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexseaDevice.h"
#include "executeDevice.h"
#include "ankle2DofProject.h"
#include "testBenchProject.h"
#include "define.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Execute;
}

class W_Execute : public QWidget, public Counter<W_Execute>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Execute(	QWidget *parent = 0,
						FlexseaDevice *currentLog = nullptr,
						ExecuteDevice *executeLogPtrInit = nullptr,
						Ankle2DofProject * ankle2DofLogPtrInit = nullptr,
						TestBenchProject * testBenchLogPtrInit = nullptr,
						DisplayMode mode = DisplayLiveData,
						QList<ExecuteDevice> *deviceListPtr = nullptr);
	~W_Execute();


public slots:
	void refreshDisplay(void);
	void refreshDisplayLog(int index, FlexseaDevice * devPtr);
	void updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr);

signals:
	void windowClosed(void);

private:
	//Variables & Objects:
	Ui::W_Execute *ui;

	DisplayMode displayMode;

	QList<ExecuteDevice> *deviceList;
	ExecuteDevice *executeLog;
	Ankle2DofProject *ankle2DofLog;
	TestBenchProject *testbenchLog;

	//Function(s):
	void initLive(void);
	void initLog(FlexseaDevice *devPtr);
	void display(ExecuteDevice *devicePtr, int index);
	void display(struct execute_s *ex);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_EXECUTE_H
