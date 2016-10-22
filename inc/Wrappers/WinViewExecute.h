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

#ifndef WIN_VIEW_EXECUTE_H
#define WIN_VIEW_EXECUTE_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QtGui>
#include <QWidget>
#include <QMdiSubWindow>
#include "w_execute.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui
{
    class WinViewExecute;
}

class WinViewExecute : public QMdiSubWindow
{
    Q_OBJECT

public:
	//Constructor & Destructor:
    explicit WinViewExecute(QWidget *parent = 0);
    ~WinViewExecute();

public slots:
    void refreshDisplayExecute(void);
    void winLoadNewLogFile(QList<struct execute_s> &data);
    void winRefreshDataSlider(int index);

signals:
    void windowClosed(void);

private:
	//Variables & Objects:
    W_Execute *my_w_execute;
};

#endif // WIN_VIEW_EXECUTE_H
