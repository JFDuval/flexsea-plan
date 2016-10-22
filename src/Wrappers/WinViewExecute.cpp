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
	[This file] WinViewExecute: Execute View Window (Wrapper)
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "WinViewExecute.h"
#include "flexsea_system.h"
#include <QMdiSubWindow>
#include "w_execute.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

WinViewExecute::WinViewExecute(QWidget *parent) :
  QMdiSubWindow(parent)
{
    my_w_execute = new W_Execute(this);
    this->setWidget(my_w_execute);
}

WinViewExecute::~WinViewExecute()
{
    emit windowClosed();
    my_w_execute->~W_Execute();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void WinViewExecute::refreshDisplayExecute(void)
{
    my_w_execute->refresh();
}

void WinViewExecute::winLoadNewLogFile(QList<struct execute_s> &data)
{
    my_w_execute->loadLogFile(data);
}

void WinViewExecute::winRefreshDataSlider(int index)
{
    my_w_execute->refreshDataSlider(index);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

//****************************************************************************
// Private slot(s):
//****************************************************************************

