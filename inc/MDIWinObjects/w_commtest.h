/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2017 Dephy, Inc. <http://dephy.com/>

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
	[This file] w_commtest.h: Communication Testing Tool
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-01-05 | jfduval | New code, initial release
	*
****************************************************************************/

#ifndef W_COMMTEST_H
#define W_COMMTEST_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexsea_generic.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_CommTest;
}

class W_CommTest : public QWidget, public Counter<W_CommTest>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_CommTest(QWidget *parent = 0);
	~W_CommTest();

	//Function(s):

public slots:

signals:
	void windowClosed(void);
	void writeCommand(uint8_t numb, uint8_t *tx_data, uint8_t r_w);

private slots:
	void refreshDisplay(void);
	void readCommTest(void);
	void on_comboBox_slave_currentIndexChanged(int index);
	void on_pushButtonStartStop_clicked();
	void on_pushButtonReset_clicked();

private:
	// Static Variable

	//Variables & Objects:
	Ui::W_CommTest *ui;
	int active_slave, active_slave_index;
	QTimer *experimentTimer, *displayTimer;
	QDateTime *statsTimer;
	float successRate, lossRate;
	int32_t receivedPackets;
	int32_t experimentTimerFreq;
	float measuredRefreshSend;

	//Function(s):
	void init(void);
	void initTimers(void);
	float getRefreshRateSend();
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#define TIM_FREQ_TO_P(f)				(1000/f)	//f in Hz, return in ms
#define DISPLAY_TIMER					25	//Hz
#define DEFAULT_EXPERIMENT_TIMER_FREQ	250

#endif // W_COMMTEST_H
