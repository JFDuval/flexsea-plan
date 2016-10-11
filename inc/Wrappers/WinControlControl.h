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
	[This file] WinControlControl: Control Window (Wrapper)
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef WIN_CONTROL_CONTROL_H
#define WIN_CONTROL_CONTROL_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QtGui>
#include <QWidget>
#include <QMdiSubWindow>
#include "w_control.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui
{
    class WinControlControl;
}

class WinControlControl : public QMdiSubWindow
{
    Q_OBJECT

public:
	//Constructor & Destructor:
    explicit WinControlControl(QWidget *parent = 0);
    ~WinControlControl();

public slots:
    void receiveWriteCommand(char numb, unsigned char *tx_data);
    void refreshDisplay(void);

signals:
    void windowClosed(void);
    void writeCommand(char numb, unsigned char *tx_data);

private:
	//Variables & Objects:
    W_Control *my_control;
};

#endif // WIN_CONTROL_CONTROL_H
