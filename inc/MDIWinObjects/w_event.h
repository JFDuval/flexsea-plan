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
	[This file] w_event.h: Event Flagger Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-03-13 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef W_EVENT_H
#define W_EVENT_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include "counter.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

#define NUM_OF_BUTTONS		4

namespace Ui {
class W_Event;
}

class W_Event : public QWidget, public Counter<W_Event>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Event(QWidget *parent = 0);
	~W_Event();

	static QString flagText;
	static int flagCode;
	static QString getEventFlags(void);
	static int getEventCode(void);

signals:
	void windowClosed(void);
	void buttonClick(int index);

public slots:
	void externalButtonClick(int index);

private slots:

	void on_horizontalSlider_valueChanged(int value);
	void on_pushButtonA_clicked();
	void on_pushButtonB_clicked();
	void on_pushButtonC_clicked();
	void on_pushButtonD_clicked();

	void pushButtonEvent(int pb);

	void timeoutEvent(int pb);
	void timerPb0(void);
	void timerPb1(void);
	void timerPb2(void);
	void timerPb3(void);

private:
	//Variables & Objects:
	Ui::W_Event *ui;
	int delayValue;
	QTimer *timerPb[NUM_OF_BUTTONS];
	int flag[NUM_OF_BUTTONS];

	//Function(s):
	void init(void);
	QString buildList(void);
	int buildCode(void);
	QPushButton *buttons[NUM_OF_BUTTONS];
};

//****************************************************************************
// Definition(s)
//****************************************************************************


#endif // W_EVENT_H
