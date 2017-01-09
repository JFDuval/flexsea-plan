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
	[This file] w_LogKeyPad.h: LogKeyPAd Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-11-19 | Sebastien Belanger | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef W_LOGKEYPAD_H
#define W_LOGKEYPAD_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"

#include "flexseaDevice.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_LogKeyPad;
}

class W_LogKeyPad : public QWidget, public Counter<W_LogKeyPad>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_LogKeyPad(QWidget *parent = 0,
						FlexseaDevice *flexSeaDevicePtr = nullptr);
	~W_LogKeyPad();

signals:
	void windowClosed(void);
	void logTimeSliderValueChanged(int value, FlexseaDevice *);

private slots:

	void on_TimeSlider_valueChanged(int value);

private:
	//Variables & Objects:
	Ui::W_LogKeyPad *ui;
	FlexseaDevice *devicePtr;

	//Function(s):
	void init(void);
};

//****************************************************************************
// Definition(s)
//****************************************************************************


#endif // W_LOGKEYPAD_H
