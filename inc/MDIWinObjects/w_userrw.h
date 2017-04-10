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
	[This file] userrw.h: User Read/Write Tool
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-11-22 | jfduval | New code, initial release
	*
****************************************************************************/

#ifndef W_USERRW_H
#define W_USERRW_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexsea_generic.h"
#include <dynamicuserdatamanager.h>
#include <QListWidgetItem>
//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_UserRW;
}

class W_UserRW : public QWidget, public Counter<W_UserRW>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_UserRW(QWidget *parent = 0, DynamicUserDataManager* userDataManager = nullptr);
	~W_UserRW();

	void setUserCustomRowHidden(int row, bool shouldHide);

public slots:
	void receiveNewData();
	void comStatusChanged(bool isOpen);
	void handlePlanFlagListChange(QListWidgetItem* item);

signals:
	void windowClosed(void);
	void writeCommand(uint8_t numb, uint8_t *tx_data, uint8_t r_w);

private slots:
	void on_pushButton_w0_clicked();
	void on_pushButton_w1_clicked();
	void on_pushButton_w2_clicked();
	void on_pushButton_w3_clicked();
	void on_pushButton_refresh_clicked();
	void on_pushButton_hide_clicked();
	void refreshDisplay(void);


	void on_comboBox_slave_currentIndexChanged(int index);

private:
	//Variables & Objects:
	Ui::W_UserRW *ui;
	int active_slave, active_slave_index;
	QTimer *refreshDelayTimer;
	DynamicUserDataManager* userDataMan;

	//Function(s):
	void init(void);
	void writeUserData(uint8_t index);
	void readUserData(void);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_USERRW_H
