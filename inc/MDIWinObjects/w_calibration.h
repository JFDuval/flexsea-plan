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
	[This file] w_calibration.h: Calibration View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

#ifndef W_CALIBRATION_H
#define W_CALIBRATION_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexsea_generic.h"
#include "cmd-ActPack.h"
#include "qtimer.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

//Data in status indicator:
#define BELT_STATUS_GREY	0
#define BELT_STATUS_GREEN	1
#define BELT_STATUS_YELLOW	2
#define BELT_STATUS_RED		3

#define FP_DELAY_DEFAULT			400	//ms
#define FP_DELAY_MIN				150
#define FP_DELAY_MAX				1000
#define FP_POLES					126

namespace Ui {
class W_Calibration;
}

class W_Calibration : public QWidget, public Counter<W_Calibration>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Calibration(QWidget *parent = 0);
	~W_Calibration();

	//Function(s):

public slots:
	void on_pbFindPoles_clicked();

signals:
	void writeCommand(uint8_t numb, uint8_t *tx_data, uint8_t r_w);
	void windowClosed(void);

private slots:
	void on_comboBox_slave_currentIndexChanged(int index);

	void on_comboBoxActPackFSM2_currentIndexChanged(int index);

	void on_pbCancelPoles_clicked();

	void on_pbCalibrateBelt_clicked();

	void on_pbSavePoles_clicked();

private:
	//Variables & Objects:
	Ui::W_Calibration *ui;
	int active_slave, active_slave_index;
	int calibration = 1;
	struct ActPack_s ActPack;
	QTimer *findPoleTimer;
	int findPoleTimePassed, findPoleTimeout;

	//Function(s):
	void init(void);
	void setBeltStatus(int status);
	void findPoleTimerUpdate();
	void sendActPack();
	void initActPack();
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_CALIBRATION_H
