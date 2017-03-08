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
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	* 2016-09-12 | jfduval | Moved the status display here, from w_execute
	* 2016-09-16 | jfduval | Major code rework to support multiple boards
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "flexsea_generic.h"
#include "main.h"
#include <QComboBox>
#include <QDebug>

#include "batteryDevice.h"
#include "executeDevice.h"
#include "gossipDevice.h"
#include "manageDevice.h"
#include "ricnuProject.h"
#include "strainDevice.h"


//****************************************************************************
// Static Variable initialization
//****************************************************************************

//Lookup from list to actual slave number (FlexSEA convention):
uint8_t FlexSEA_Generic::list_to_slave[SL_LEN_ALL] = {FLEXSEA_EXECUTE_1,
														FLEXSEA_EXECUTE_2,
														FLEXSEA_EXECUTE_3,
														FLEXSEA_EXECUTE_4,
														FLEXSEA_MANAGE_1,
														FLEXSEA_MANAGE_2,
														FLEXSEA_PLAN_1,
														FLEXSEA_GOSSIP_1,
														FLEXSEA_GOSSIP_2,
														FLEXSEA_BATTERY_1,
														FLEXSEA_STRAIN_1,
														FLEXSEA_VIRTUAL_PROJECT};
//Slaves:
QStringList FlexSEA_Generic::var_list_slave =    QStringList()
												 << "Execute 1"
												 << "Execute 2"
												 << "Execute 3"
												 << "Execute 4"
												 << "Manage 1"
												 << "Manage 2"
												 << "Plan 1"
												 << "Gossip 1"
												 << "Gossip 2"
												 << "Battery 1"
												 << "Strain 1"
												 << "RIC/NU 1";

//Experiments:
QStringList FlexSEA_Generic::var_list_exp = QStringList()
											<< "Read All (Barebone)"
											<< "In Control"
											<< "RICNU Knee"
											<< "CSEA Knee"
											<< "2DOF Ankle"
											<< "Battery Board"
											<< "Test Bench"
											<< "[Your project]";

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

FlexSEA_Generic::FlexSEA_Generic(QWidget *parent) : QWidget(parent)
{

}

//****************************************************************************
// Public function(s):
//****************************************************************************

//Populates a Slave List ComboBox
void FlexSEA_Generic::populateSlaveComboBox(QComboBox *cbox, uint8_t base, \
											uint8_t len)
{
	QString slave_name;

	for(int index = base; index < (base+len); index++)
	{
		slave_name = var_list_slave.at(index);
		cbox->addItem(slave_name);
	}
}

//Populates an Experiment List Combo Box - no parameters
void FlexSEA_Generic::populateExpComboBox(QComboBox *cbox)
{
	QString exp_name;

	for(int index = 0; index < var_list_exp.length(); index++)
	{
		exp_name = var_list_exp.at(index);
		cbox->addItem(exp_name);
	}
}

//Returns the experiment name, as a QString
void FlexSEA_Generic::getExpName(uint8_t index, QString *expName)
{
	*expName = var_list_exp.at(index);
}

//Returns the FlexSEA Slave ID as a uint8
uint8_t FlexSEA_Generic::getSlaveID(uint8_t base, uint8_t index)
{
	return list_to_slave[base + index];
}

//Prints a FlexSEA packet on the debug terminal
void FlexSEA_Generic::packetVisualizer(uint numb, uint8_t *packet)
{
	QString msg1 = "";
	uint8_t cmd7bits = 0, rw = 0;

	//From CMD1 to R/W and 7-bit code:
	cmd7bits = CMD_7BITS(packet[5]);
	rw = IS_CMD_RW(packet[5]);
	if(rw == READ)
		msg1 = "(Read ";
	else
		msg1 = "(Write ";
	msg1 += QString::number(cmd7bits);
	msg1 += ")";

	qDebug() << "-------------------------";
	qDebug() << "FlexSEA Packet Visualizer";
	qDebug() << "-------------------------";

	QString msg2 = "Raw: ";
	for(uint i = 0; i < numb; i++)
	{
		msg2 += (QString::number(packet[i]) + ',');
	}
	qDebug() << msg2;

	qDebug() << "numb: " << numb;
	qDebug() << "HEADER: " << packet[0];
	qDebug() << "BYTES:: " << packet[1];
	qDebug() << "P_XID: " << packet[2];
	qDebug() << "P_RID: " << packet[3];
	qDebug() << "P_CMDS: " << packet[4];
	qDebug() << "P_CMD1: " << packet[5] << msg1;
	qDebug() << "P_DATA1:" << packet[6];
	qDebug() << "P_DATA2:" << packet[7];
	qDebug() << "...";
	qDebug() << "CHECKSUM:" << packet[packet[1]+2];
	qDebug() << "FOOTER: " << packet[packet[1]+3];

	qDebug() << "-------------------------";
}

//Assign pointer
//TODO: should we use flexsea_system's executePtrXid instead?
void FlexSEA_Generic::assignExecutePtr(struct execute_s **myPtr, uint8_t base, \
									   uint8_t slave)
{
	//Based on selected slave, what structure do we use?
	switch(list_to_slave[base+slave])
	{
		case FLEXSEA_EXECUTE_1:
			*myPtr = &exec1;
			break;
		case FLEXSEA_EXECUTE_2:
			*myPtr = &exec2;
			break;
		case FLEXSEA_EXECUTE_3:
			*myPtr = &exec3;
			break;
		case FLEXSEA_EXECUTE_4:
			*myPtr = &exec4;
			break;
		default:
			*myPtr = &exec1;
			break;
	}
}
