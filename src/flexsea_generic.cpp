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
    * 2016-09-12 | jfduval | Moved the status display here, from w_execute
****************************************************************************/

//ToDo: this code is getting ugly: combine functions!

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
    //Slaves - Execute only:
    //======================

    var_list_slave_ex.clear();
    var_list_slave_ex << "Execute 1" << "Execute 2" << "Execute 3" << \
                       "Execute 4";

    //Lookup from list to actual slave number (FlexSEA convention):
    list_to_slave_ex[0] = FLEXSEA_EXECUTE_1;
    list_to_slave_ex[1] = FLEXSEA_EXECUTE_2;
    list_to_slave_ex[2] = FLEXSEA_EXECUTE_3;
    list_to_slave_ex[3] = FLEXSEA_EXECUTE_4;

    //Slaves - Manage only:
    //======================

    var_list_slave_mn.clear();
    var_list_slave_mn << "Manage 1" << "Manage 2";

    //Lookup from list to actual slave number (FlexSEA convention):
    list_to_slave_mn[0] = FLEXSEA_MANAGE_1;
    list_to_slave_mn[1] = FLEXSEA_MANAGE_2;

    //Slaves - All:
    //=============

    var_list_slave_all.clear();
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

    //Experiments:
    //============

    var_list_exp.clear();
    var_list_exp << "Read All (Barebone)" << "In Control" \
                    << "Strain Amp" << "RIC/NU Knee" << "CSEA Knee" \
                    << "2DOF Ankle" << "[Your project]";
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

void FlexSEA_Generic::assignManagePtr(struct manage_s **mn_ptr, uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(slave)
    {
        case 0:
            *mn_ptr = &manag1;
            break;
        case 1:
            *mn_ptr = &manag2;
            break;
        /*
        case 2:
            *mn_ptr = &manag3;
            break;
        case 3:
            *mn_ptr = &manag4;
            break;
            */
        default:
            *mn_ptr = &manag1;
            break;
    }
}

void FlexSEA_Generic::assignRicnuPtr(struct ricnu_s **ricnu_ptr, uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(slave)
    {
        case 0:
            *ricnu_ptr = &ricnu_1;
            break;
        /*
        case 1:
            *ricnu_ptr = &ricnu_2;
            break;
        case 2:
            *ricnu_ptr = &ricnu_3;
            break;
        case 3:
            *ricnu_ptr = &ricnu_4;
            break;
        */
        default:
            *ricnu_ptr = &ricnu_1;
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

uint8_t FlexSEA_Generic::getLenExp(void)
{
    return var_list_exp.count();
}

void FlexSEA_Generic::getNameExp(uint8_t index, QString *name)
{
    (*name) = var_list_exp.at(index);
}

void FlexSEA_Generic::populateComboBoxExp(QComboBox *cbox)
{
    QString exp;

    init();

    for(int index = 0; index < getLenExp(); index++)
    {
        getNameExp(index, &exp);
        cbox->addItem(exp);
    }
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

void FlexSEA_Generic::execStatusBytes(uint8_t stat1, uint8_t stat2, QString *str1)
{
    //QString str1;
    uint8_t mod = 0;
    (*str1) = "";

    //WDCLK:
    if(GET_WDCLK_FLAG(stat1))
    {
        (*str1) += QString("Co-Processor Error");
        mod++;
    }

    //Disconnected battery:
    if(GET_DISCON_FLAG(stat1) == BATT_DISCONNECTED)
    {
        if(mod){(*str1) += QString(" | ");};
        (*str1) += QString("Disconnected battery");
        mod++;
    }

    //Temperature:
    if(GET_OVERTEMP_FLAG(stat1) == T_WARNING)
    {
        if(mod){(*str1) += QString(" | ");};
        (*str1) += QString("Temp. Near Limit");
        mod++;
    }
    else if(GET_OVERTEMP_FLAG(stat1) == T_ERROR)
    {
        if(mod){(*str1) += QString(" | ");};
        (*str1) += QString("Temp. Error");
        mod++;
    }

    //Voltage - VB:
    if(GET_VB_FLAG(stat1) == V_LOW)
    {
        if(mod){(*str1) += QString(" | ");};
        (*str1) += QString("VB Low");
        mod++;
    }
    else if(GET_VB_FLAG(stat1) == V_HIGH)
    {
        if(mod){(*str1) += QString(" | ");};
        (*str1) += QString("VB High");
        mod++;
    }

    //Voltage - VG:
    if(GET_VG_FLAG(stat1) == V_LOW)
    {
        if(mod){(*str1) += QString(" | ");};
        (*str1) += QString("VG Low");
        mod++;
    }
    else if(GET_VG_FLAG(stat1) == V_HIGH)
    {
        if(mod){(*str1) += QString(" | ");};
        (*str1) += QString("VG High");
        mod++;
    }

    //If nothing is wrong:
    if(mod == 0)
    {
        (*str1) = QString("Status: OK");
    }
}

//Manage:
//===========

uint8_t FlexSEA_Generic::getSlaveCodeMn(uint8_t index)
{
    return list_to_slave_mn[index];
}

void FlexSEA_Generic::getSlaveNameMn(uint8_t index, QString *name)
{
    (*name) = var_list_slave_mn.at(index);
}

uint8_t FlexSEA_Generic::getSlaveLenMn(void)
{
    return var_list_slave_mn.count();
}

void FlexSEA_Generic::populateComboBoxMn(QComboBox *cbox)
{
    QString slave_name;

    init();

    for(int index = 0; index < getSlaveLenMn(); index++)
    {
        getSlaveNameMn(index, &slave_name);
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

    init();

    for(int index = 0; index < getSlaveLenAll(); index++)
    {
        getSlaveNameAll(index, &slave_name);
        cbox->addItem(slave_name);
    }
}


