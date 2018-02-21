/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2018 Dephy, Inc. <http://dephy.com/>

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
	[This file] w_status.h: Status window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2018-02-10 | sbelanger | New code, initial release
	*
****************************************************************************/

#ifndef W_STATUS_H
#define W_STATUS_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexsea_generic.h"
#include "serialdriver.h"
#include <dynamicuserdatamanager.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <QListWidgetItem>


//****************************************************************************
// Definition(s):
//****************************************************************************

#define NB_STATUS 10

//Data in status indicator:
#define STATUS_GREY		0
#define STATUS_GREEN	1
#define STATUS_YELLOW	2
#define STATUS_RED		3


//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Status;
}

class W_Status : public QWidget, public Counter<W_Status>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Status(QWidget *parent = 0, DynamicUserDataManager* userDataManager = nullptr);
	~W_Status();

public slots:
	void receiveNewData();
	void comStatusChanged(SerialPortStatus status,int nbTries);
	void externalErrorFlag();

signals:
	void windowClosed(void);
	void writeCommand(uint8_t numb, uint8_t *tx_data, uint8_t r_w);

private slots:
	void on_pb_clear_0_clicked();
	void on_pb_clear_1_clicked();
	void on_pb_clear_2_clicked();
	void on_pb_clear_3_clicked();
	void on_pb_clear_4_clicked();
	void on_pb_clear_5_clicked();
	void on_pb_clear_6_clicked();
	void on_pb_clear_7_clicked();
	void on_pb_clear_8_clicked();
	void on_pb_clear_9_clicked();

private:
	//Variables & Objects:
	Ui::W_Status *ui;
	QLabel *lab_name_ptr[NB_STATUS];
	QLabel *lab_indicator_ptr[NB_STATUS];
	QPushButton *pb_clear_ptr[NB_STATUS];

	int active_slave, active_slave_index;
	QTimer *refreshDelayTimer;
	DynamicUserDataManager* userDataMan;

	//Function(s):
	void init(void);
	void writeUserData(uint8_t index);
	void readUserData(void);
	void setStatus(int row, int status);
	void statusReset(int row);
	void initLabelText(void);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_STATUS_H
