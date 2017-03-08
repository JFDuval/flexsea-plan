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
	[This file] w_strain.h: Strain View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

#ifndef W_STRAIN_H
#define W_STRAIN_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "strainDevice.h"
#include "define.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Strain;
}

class W_Strain : public QWidget, public Counter<W_Strain>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Strain(QWidget *parent = 0,
					  StrainDevice *deviceLogPtr = nullptr,
					  DisplayMode mode = DisplayLiveData,
					  QList<StrainDevice> *deviceListPtr = nullptr);
	~W_Strain();

	//Function(s):

public slots:
	void refreshDisplay(void);
	void refreshDisplayLog(int index, FlexseaDevice * devPtr);
	void updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr);

signals:
	void windowClosed(void);

private:
	//Variables & Objects:
	Ui::W_Strain *ui;

	DisplayMode displayMode;

	QList<StrainDevice> *deviceList;
	StrainDevice *deviceLog;

	//Function(s):
	void initLive(void);
	void initLog(void);
	void display(StrainDevice *devicePtr, int index);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_STRAIN_H
