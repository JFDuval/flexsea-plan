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
	[This file] flexsea_generic: Generic functions used by many classes
*****************************************************************************/

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

//Slaves & Slave lists:
//=====================

//Slave list: base index:
#define SL_BASE_ALL					0
#define SL_BASE_EX					0
#define SL_BASE_MN					4
#define SL_BASE_PLAN				6
#define SL_BASE_GOSSIP				7
#define SL_BASE_BATT				9
#define SL_BASE_STRAIN				10
#define SL_BASE_RICNU				11
//Slave list: length/number of fields
#define SL_LEN_ALL					12
#define SL_LEN_EX					4
#define SL_LEN_MN					2
#define SL_LEN_PLAN					1
#define SL_LEN_GOSSIP				2
#define SL_LEN_BATT					1
#define SL_LEN_STRAIN				1
#define SL_LEN_RICNU				1

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

	//Pointer assignements - raw structures:
	static void assignExecutePtr(struct execute_s **myPtr, uint8_t base, uint8_t slave);
	static void assignManagePtr(struct manage_s **myPtr, uint8_t base, uint8_t slave);
	static void assignRicnuPtr(struct ricnu_s **myPtr, uint8_t base, uint8_t slave);
	static void assignStrainPtr(struct strain_s **myPtr, uint8_t base, uint8_t slave);
	static void assignGossipPtr(struct gossip_s **myPtr, uint8_t base, uint8_t slave);
	static void assignBatteryPtr(struct battery_s **myPtr, uint8_t base, uint8_t slave);

	static void populateSlaveComboBox(QComboBox *cbox, uint8_t base, uint8_t len);
	static void populateExpComboBox(QComboBox *cbox);
	static uint8_t getSlaveBoardType(uint8_t base, uint8_t index);
	static void getExpName(uint8_t index, QString *expName);
	static uint8_t getSlaveID(uint8_t base, uint8_t index);
	static void packetVisualizer(uint numb, uint8_t *packet);
	static void decodeRicnu(uint8_t base, uint8_t index);

	static uint8_t list_to_slave[SL_LEN_ALL];
	static QStringList var_list_slave, var_list_exp;

};

#endif // FLEXSEA_GENERIC_H
