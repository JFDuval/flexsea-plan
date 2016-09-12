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

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "flexsea_generic.h"
#include <QComboBox>
#include <QDebug>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

FlexSEA_Generic::FlexSEA_Generic(QWidget *parent) : QWidget(parent)
{

}

//****************************************************************************
// Public function(s):
//****************************************************************************

void FlexSEA_Generic::init(void)
{
    //Execute only:
    //=============

    var_list_slave_ex << "Execute 1" << "Execute 2" << "Execute 3" << \
                       "Execute 4";

    //Lookup from list to actual slave number (FlexSEA convention):
    list_to_slave_ex[0] = FLEXSEA_EXECUTE_1;
    list_to_slave_ex[1] = FLEXSEA_EXECUTE_2;
    list_to_slave_ex[2] = FLEXSEA_EXECUTE_3;
    list_to_slave_ex[3] = FLEXSEA_EXECUTE_4;

    //All slaves:
    //===========

    var_list_slave_all << "Execute 1" << "Execute 2" << "Execute 3" << \
                       "Execute 4" << "Manage 1" << "Strain Amp 1" << \
                       "Gossip 1" << "Plan 1";

    //Lookup from list to actual slave number (FlexSEA convention):
    list_to_slave_all[0] = FLEXSEA_EXECUTE_1;
    list_to_slave_all[1] = FLEXSEA_EXECUTE_2;
    list_to_slave_all[2] = FLEXSEA_EXECUTE_3;
    list_to_slave_all[3] = FLEXSEA_EXECUTE_4;
    list_to_slave_all[4] = FLEXSEA_MANAGE_1;
    list_to_slave_all[5] = FLEXSEA_STRAIN_1;
    list_to_slave_all[6] = FLEXSEA_GOSSIP_1;
    list_to_slave_all[7] = FLEXSEA_PLAN_1;
}

void FlexSEA_Generic::assignExecutePtr(struct execute_s **ex_ptr, uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(slave)
    {
        case 0:
            *ex_ptr = &exec1;
            break;
        case 1:
            *ex_ptr = &exec2;
            break;
        case 2:
            *ex_ptr = &exec3;
            break;
        case 3:
            *ex_ptr = &exec4;
            break;
        default:
            *ex_ptr = &exec1;
            break;
    }
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
    for(int i = 0; i < numb; i++)
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

//Execute only:
//==============

uint8_t FlexSEA_Generic::getSlaveCodeEx(uint8_t index)
{
	return list_to_slave_ex[index];
}

void FlexSEA_Generic::getSlaveNameEx(uint8_t index, QString *name)
{
    (*name) = var_list_slave_ex.at(index);
}

uint8_t FlexSEA_Generic::getSlaveLenEx(void)
{
    return var_list_slave_ex.count();
}

void FlexSEA_Generic::populateComboBoxEx(QComboBox *cbox)
{
	QString slave_name;
	
	init();
	
    for(int index = 0; index < getSlaveLenEx(); index++)
    {
        getSlaveNameEx(index, &slave_name);
        cbox->addItem(slave_name);
    }	
}

//All slaves:
//===========

uint8_t FlexSEA_Generic::getSlaveCodeAll(uint8_t index)
{
	return list_to_slave_all[index];
}

void FlexSEA_Generic::getSlaveNameAll(uint8_t index, QString *name)
{
    (*name) = var_list_slave_all.at(index);
}

uint8_t FlexSEA_Generic::getSlaveLenAll(void)
{
    return var_list_slave_all.count();
}

void FlexSEA_Generic::populateComboBoxAll(QComboBox *cbox)
{
    QString slave_name;

    //init();

    for(int index = 0; index < getSlaveLenAll(); index++)
    {
        getSlaveNameAll(index, &slave_name);
        cbox->addItem(slave_name);
    }
}


