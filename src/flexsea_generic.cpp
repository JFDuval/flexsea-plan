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
    //Slaves:
    //=======

    var_list_slave.clear();
    var_list_slave << "Execute 1" << "Execute 2" << "Execute 3" << \
                      "Execute 4" << "Manage 1" << "Manage 2" << "Plan 1" << \
                      "Gossip 1" << "Gossip 2" <<"Battery 1" << "Strain 1" << \
					  "RIC/NU 1";

    //Lookup from list to actual slave number (FlexSEA convention):
    list_to_slave[0] = FLEXSEA_EXECUTE_1;
    list_to_slave[1] = FLEXSEA_EXECUTE_2;
    list_to_slave[2] = FLEXSEA_EXECUTE_3;
    list_to_slave[3] = FLEXSEA_EXECUTE_4;
    list_to_slave[4] = FLEXSEA_MANAGE_1;
    list_to_slave[5] = FLEXSEA_MANAGE_2;
    list_to_slave[6] = FLEXSEA_PLAN_1;
    list_to_slave[7] = FLEXSEA_GOSSIP_1;
    list_to_slave[8] = FLEXSEA_GOSSIP_2;
    list_to_slave[9] = FLEXSEA_BATTERY_1;
    list_to_slave[10] = FLEXSEA_STRAIN_1;
	list_to_slave[11] = FLEXSEA_EXECUTE_1;	//RIC/NU == Execute (for now)

    //Experiments:
    //============

    var_list_exp.clear();
    var_list_exp << "Read All (Barebone)" << "In Control" \
                    << "Strain Amp" << "RIC/NU Knee" << "CSEA Knee" \
                    << "2DOF Ankle" << "[Your project]";
}

//Populates a Slave List ComboBox
void FlexSEA_Generic::populateSlaveComboBox(QComboBox *cbox, uint8_t base, \
                                            uint8_t len)
{
    QString slave_name;

    init();

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

    init();

    for(int index = 0; index < var_list_exp.length(); index++)
    {
        exp_name = var_list_exp.at(index);
        cbox->addItem(exp_name);
    }
}

//Sometimes we need to know if a board is an Execute, without caring about
//if it's Execute 1, 2 or N. This function returns the base code.
uint8_t FlexSEA_Generic::getSlaveBoardType(uint8_t base, uint8_t index)
{
    //Board type? Extract base via address&integer trick
    uint8_t tmp = 0, bType = 0;
    tmp = list_to_slave[base + index] / 10;
    bType = tmp * 10;

    return bType;
}

//Returns the slave name, as a QString
void FlexSEA_Generic::getSlaveName(uint8_t base, uint8_t index, \
                                   QString *slaveName)
{
    *slaveName = var_list_slave.at(base+index);
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

//Decode status byte(s), return a user-friendly QString
void FlexSEA_Generic::decodeStatus(uint8_t base, uint8_t index, uint8_t stat1, \
                                   uint8_t stat2, QString *str1)
{
    uint8_t mod = 0, bType = getSlaveBoardType(base, index);
    (*str1) = "";

    if(bType == FLEXSEA_EXECUTE_BASE)
    {
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
    else
    {
        (*str1) = QString("No decoding available for this board");
    }
}

//Decodes some of Execute's fields
void FlexSEA_Generic::decodeExecute(uint8_t base, uint8_t index)
{
    struct executeD_s *exDPtr;
    assignExecutePtr(&exDPtr, base, index);

    //Accel in mG
    exDPtr->accel.x = (1000*exDPtr->exRaw.accel.x)/8192;
    exDPtr->accel.y = (1000*exDPtr->exRaw.accel.y)/8192;
    exDPtr->accel.z = (1000*exDPtr->exRaw.accel.z)/8192;

    //Gyro in degrees/s
    exDPtr->gyro.x = (100*exDPtr->exRaw.gyro.x)/164;
    exDPtr->gyro.y = (100*exDPtr->exRaw.gyro.y)/164;
    exDPtr->gyro.z = (100*exDPtr->exRaw.gyro.z)/164;

    exDPtr->current = (185*exDPtr->exRaw.current)/10;   //mA

    exDPtr->volt_batt = (int32_t)1000*P4_ADC_SUPPLY*((16*\
                        (float)exDPtr->exRaw.volt_batt/3 + 302 ) \
                        /P4_ADC_MAX) / 0.0738;          //mV

    exDPtr->volt_int = (int32_t)1000*P4_ADC_SUPPLY*((26*\
                        (float)exDPtr->exRaw.volt_int/3 + 440 ) \
                        /P4_ADC_MAX) / 0.43;            //mV

    exDPtr->temp = (int32_t)10*((((2.625*(float)exDPtr->exRaw.temp + 41) \
                      /P4_ADC_MAX)*P4_ADC_SUPPLY) - P4_T0) / P4_TC; //C*10

    exDPtr->analog[0] = (int32_t)1000*((float)exDPtr->exRaw.analog[0]/ \
                        P5_ADC_MAX)*P5_ADC_SUPPLY;
}

//Decodes some of Manage's fields
void FlexSEA_Generic::decodeManage(uint8_t base, uint8_t index)
{
    /* ToDo
    struct executeD_s *exDPtr;
    assignExecutePtr(&exDPtr, base, index);

    //Accel in mG
    exDPtr->accel.x = (1000*exDPtr->exRaw.accel.x)/8192;
    exDPtr->accel.y = (1000*exDPtr->exRaw.accel.y)/8192;
    exDPtr->accel.z = (1000*exDPtr->exRaw.accel.z)/8192;

    //Gyro in degrees/s
    exDPtr->gyro.x = (100*exDPtr->exRaw.gyro.x)/164;
    exDPtr->gyro.y = (100*exDPtr->exRaw.gyro.y)/164;
    exDPtr->gyro.z = (100*exDPtr->exRaw.gyro.z)/164;

    exDPtr->analog[0] = (int32_t)1000*((float)exDPtr->exRaw.analog[0]/ \
                        P5_ADC_MAX)*P5_ADC_SUPPLY;
                        */
}

//Decodes some of Gossip's fields
void FlexSEA_Generic::decodeGossip(uint8_t base, uint8_t index)
{
    /* ToDo
    struct executeD_s *exDPtr;
    assignExecutePtr(&exDPtr, base, index);

    //Accel in mG
    exDPtr->accel.x = (1000*exDPtr->exRaw.accel.x)/8192;
    exDPtr->accel.y = (1000*exDPtr->exRaw.accel.y)/8192;
    exDPtr->accel.z = (1000*exDPtr->exRaw.accel.z)/8192;

    //Gyro in degrees/s
    exDPtr->gyro.x = (100*exDPtr->exRaw.gyro.x)/164;
    exDPtr->gyro.y = (100*exDPtr->exRaw.gyro.y)/164;
    exDPtr->gyro.z = (100*exDPtr->exRaw.gyro.z)/164;

    exDPtr->analog[0] = (int32_t)1000*((float)exDPtr->exRaw.analog[0]/ \
                        P5_ADC_MAX)*P5_ADC_SUPPLY;
                        */
}


//Decodes some of the slave's fields
void FlexSEA_Generic::decodeSlave(uint8_t base, uint8_t index)
{
    uint8_t bType = getSlaveBoardType(base, index);

    switch(bType)
    {
        case FLEXSEA_PLAN_BASE:

            break;
        case FLEXSEA_MANAGE_BASE:
            //decodeManage(base, index);
            break;
        case FLEXSEA_EXECUTE_BASE:
            decodeExecute(base, index);
            break;
        case FLEXSEA_BATTERY_BASE:

            break;
        case FLEXSEA_STRAIN_BASE:

            break;
        case FLEXSEA_GOSSIP_BASE:

            break;
        default:
            break;
    }
}

//Assign pointer - raw data
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

//Assign pointer - decoded data
void FlexSEA_Generic::assignExecutePtr(struct executeD_s **myPtr, uint8_t base, \
                                       uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_EXECUTE_1:
            *myPtr = &execD1;
            break;
        case FLEXSEA_EXECUTE_2:
            *myPtr = &execD2;
            break;
        case FLEXSEA_EXECUTE_3:
            *myPtr = &execD3;
            break;
        case FLEXSEA_EXECUTE_4:
            *myPtr = &execD4;
            break;
        default:
            *myPtr = &execD1;
            break;
    }
}

//Assign pointer - raw data
void FlexSEA_Generic::assignManagePtr(struct manage_s **myPtr, uint8_t base, \
                                      uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_MANAGE_1:
            *myPtr = &manag1;
            break;
        case FLEXSEA_MANAGE_2:
            *myPtr = &manag2;
            break;
        default:
            *myPtr = &manag1;
            break;
    }
}


//Assign pointer - decoded data
void FlexSEA_Generic::assignManagePtr(struct manageD_s **myPtr, uint8_t base, \
                                      uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_MANAGE_1:
            *myPtr = &managD1;
            break;
        case FLEXSEA_MANAGE_2:
            *myPtr = &managD2;
            break;
        default:
            *myPtr = &managD1;
            break;
    }
}

//Assign pointer - raw data
void FlexSEA_Generic::assignRicnuPtr(struct ricnu_s **myPtr, uint8_t base, \
                                     uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_EXECUTE_1: //RIC/NU is the same as Execute
            *myPtr = &ricnu_1;
            break;
        default:
            *myPtr = &ricnu_1;
            break;
    }
}

//Assign pointer - decoded data
void FlexSEA_Generic::assignRicnuPtr(struct ricnuD_s **myPtr, uint8_t base, \
                                     uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_EXECUTE_1: //RIC/NU is the same as Execute
            *myPtr = &ricnuD_1;
            break;
        default:
            *myPtr = &ricnuD_1;
            break;
    }
}

//Assign pointer - raw data
void FlexSEA_Generic::assignGossipPtr(struct gossip_s **myPtr, uint8_t base, \
                                      uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_GOSSIP_1:
            *myPtr = &gossip1;
            break;
        case FLEXSEA_GOSSIP_2:
            *myPtr = &gossip2;
            break;
        default:
            *myPtr = &gossip1;
            break;
    }
}

//Assign pointer - decoded data
void FlexSEA_Generic::assignGossipPtr(struct gossipD_s **myPtr, uint8_t base, \
                                      uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_GOSSIP_1:
            *myPtr = &gossipD1;
            break;
        case FLEXSEA_GOSSIP_2:
            *myPtr = &gossipD2;
            break;
        default:
            *myPtr = &gossipD1;
            break;
    }
}

//Assign pointer - raw data
void FlexSEA_Generic::assignStrainPtr(struct strain_s **myPtr, uint8_t base, \
                                      uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_STRAIN_1:
            *myPtr = &strain[0];    //***ToDo 99% sure this is wrong!
            break;
        default:
            *myPtr = &strain[0];
            break;
    }
}

//Assign pointer - decoded data
void FlexSEA_Generic::assignStrainPtr(struct strainD_s **myPtr, uint8_t base, \
                                      uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_STRAIN_1:
            *myPtr = &strainD[0];    //***ToDo 99% sure this is wrong!
            break;
        default:
            *myPtr = &strainD[0];
            break;
    }
}

//Assign pointer - raw data
void FlexSEA_Generic::assignBatteryPtr(struct battery_s **myPtr, uint8_t base, \
                                       uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_BATTERY_1:
            *myPtr = &batt1;
            break;
        default:
            *myPtr = &batt1;
            break;
    }
}

//Assign pointer - decoded data
void FlexSEA_Generic::assignBatteryPtr(struct batteryD_s **myPtr, uint8_t base, \
                                       uint8_t slave)
{
    //Based on selected slave, what structure do we use?
    switch(list_to_slave[base+slave])
    {
        case FLEXSEA_BATTERY_1:
            *myPtr = &battD1;
            break;
        default:
            *myPtr = &battD1;
            break;
    }
}
