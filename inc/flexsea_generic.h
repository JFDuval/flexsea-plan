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
	[This file] flexsea_generic.h: Generic functions used by many classes
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef FLEXSEA_GENERIC_H
#define FLEXSEA_GENERIC_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include <QString>
#include <QComboBox>

//****************************************************************************
// Definition(s)
//****************************************************************************

#define MAX_SLAVES                  10

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui {
class FlexSEA_Generic;
}

class FlexSEA_Generic : public QWidget
{
	Q_OBJECT
	
public:
    explicit FlexSEA_Generic(QWidget *parent = 0);

	void init(void);
    void assignExecutePtr(struct execute_s **ex_ptr, uint8_t slave);

	uint8_t getSlaveCodeEx(uint8_t index);
	void getSlaveNameEx(uint8_t index, QString *name);
	uint8_t getSlaveLenEx(void);
	
	uint8_t getSlaveCodeAll(uint8_t index);
	void getSlaveNameAll(uint8_t index, QString *name);
	uint8_t getSlaveLenAll(void);
	
    void populateComboBoxEx(QComboBox *cbox);
    void populateComboBoxAll(QComboBox *cbox);

    void packetVisualizer(uint numb, uint8_t *packet);
	
public slots:

private slots:	

private:
    //Lookup from list to actual slave number (FlexSEA convention):
    uint8_t list_to_slave_ex[MAX_SLAVES], list_to_slave_all[MAX_SLAVES];
    QStringList var_list_slave_ex, var_list_slave_all;
};

#endif // FLEXSEA_GENERIC_H
