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
	[This file] WinViewManage: Manage View Window (Wrapper)
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

#ifndef WIN_VIEW_MANAGE_H
#define WIN_VIEW_MANAGE_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QtGui>
#include <QWidget>
#include <QMdiSubWindow>
#include "w_manage.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui
{
    class WinViewManage;
}

class WinViewManage : public QMdiSubWindow
{
    Q_OBJECT

public:
	//Constructor & Destructor:
    explicit WinViewManage(QWidget *parent = 0);
    ~WinViewManage();

public slots:
    void refreshDisplayManage(void);

signals:
    void windowClosed(void);

private:
	//Variables & Objects:
    W_Manage *my_w_manage;
};

#endif // WIN_VIEW_MANAGE_H
