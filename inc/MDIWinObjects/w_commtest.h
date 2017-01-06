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
	void on_pushButton_w0_clicked();
	void refreshDisplay(void);

	void on_comboBox_slave_currentIndexChanged(int index);

private:
	// Static Variable

	//Variables & Objects:
	Ui::W_CommTest *ui;
	int active_slave, active_slave_index;
	QTimer *refreshDelayTimer;

	//Function(s):
	void init(void);
	void readCommTest(void);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_COMMTEST_H
