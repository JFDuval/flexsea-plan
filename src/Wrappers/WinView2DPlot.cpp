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
	[This file] WinView2DPlot: 2D Plot Window (Wrapper)
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "WinView2DPlot.h"
#include <QMdiSubWindow>
#include "w_2dplot.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

WinView2DPlot::WinView2DPlot(QWidget *parent, W_2DPlot::DisplayMode mode) :
  QMdiSubWindow(parent)
{
    my_2dplot = new W_2DPlot(this, mode);
    this->setWidget(my_2dplot);
    connect(my_2dplot, SIGNAL(dataSliderValueChanged(int)),\
            this, SIGNAL(winDataSliderValueChanged(int)));
    //my_2dplot->resize(400,300);
}

WinView2DPlot::~WinView2DPlot()
{
    emit windowClosed();
    my_2dplot->~W_2DPlot();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void WinView2DPlot::refresh2DPlot(void)
{
    my_2dplot->refresh2DPlot();
}

//****************************************************************************
// Private function(s):
//****************************************************************************

//****************************************************************************
// Private slot(s):
//****************************************************************************
