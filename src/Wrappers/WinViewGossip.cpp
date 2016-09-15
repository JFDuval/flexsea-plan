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
	[This file] WinViewGossip: Gossip View Window (Wrapper)
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "WinViewGossip.h"
#include <QMdiSubWindow>
#include "w_gossip.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

WinViewGossip::WinViewGossip(QWidget *parent) :
  QMdiSubWindow(parent)
{
    my_w_gossip = new W_Gossip(this);
    this->setWidget(my_w_gossip);
}

WinViewGossip::~WinViewGossip()
{
    emit windowClosed();
    my_w_gossip->~W_Gossip();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void WinViewGossip::refreshDisplayGossip(void)
{
    my_w_gossip->refresh();
}

//****************************************************************************
// Private function(s):
//****************************************************************************

//****************************************************************************
// Private slot(s):
//****************************************************************************

