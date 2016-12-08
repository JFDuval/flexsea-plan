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
														FLEXSEA_VIRTUAL_1};
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

	(void)stat2;	//Unused at this point

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

//TODO these decoding functions should be in the w_board files

//Decodes some of Execute's fields
void FlexSEA_Generic::decodeExecute(uint8_t base, uint8_t index)
{
	struct execute_s *exPtr;
	assignExecutePtr(&exPtr, base, index);
	decodeExecute(exPtr);
}

void FlexSEA_Generic::decodeExecute(struct execute_s *exPtr)
{
	//Accel in mG
	exPtr->decoded.accel.x = (1000*exPtr->accel.x)/8192;
	exPtr->decoded.accel.y = (1000*exPtr->accel.y)/8192;
	exPtr->decoded.accel.z = (1000*exPtr->accel.z)/8192;

	//Gyro in degrees/s
	exPtr->decoded.gyro.x = (100*exPtr->gyro.x)/164;
	exPtr->decoded.gyro.y = (100*exPtr->gyro.y)/164;
	exPtr->decoded.gyro.z = (100*exPtr->gyro.z)/164;

	//exPtr->decoded.current = (185*exPtr->current)/10;   //mA
	exPtr->decoded.current = exPtr->current;   //1mA/bit for sine comm.

	exPtr->decoded.volt_batt = (int32_t)1000*P4_ADC_SUPPLY*((16*\
						(float)exPtr->volt_batt/3 + 302 ) \
						/P4_ADC_MAX) / 0.0738;          //mV

	exPtr->decoded.volt_int = (int32_t)1000*P4_ADC_SUPPLY*((26*\
						(float)exPtr->volt_int/3 + 440 ) \
						/P4_ADC_MAX) / 0.43;            //mV

	exPtr->decoded.temp = (int32_t)10*((((2.625*(float)exPtr->temp + 41) \
					  /P4_ADC_MAX)*P4_ADC_SUPPLY) - P4_T0) / P4_TC; //C*10

	exPtr->decoded.analog[0] = (int32_t)1000*((float)exPtr->analog[0]/ \
						P5_ADC_MAX)*P5_ADC_SUPPLY;
}

//RIC/NU is a special case of Execute board. It use the first struct of execute
// and strain.
void FlexSEA_Generic::decodeRicnu(uint8_t base, uint8_t index)
{
	(void)base;
	(void)index;
	ricnu_1.ex = exec1;
	ricnu_1.st = strain1;
	decodeRicnu(&ricnu_1);
}

void FlexSEA_Generic::decodeRicnu(struct ricnu_s *riPtr)
{
	decodeExecute(&riPtr->ex);
	decodeStrain(&riPtr->st);
}

void FlexSEA_Generic::decodeManage(uint8_t base, uint8_t index)
{
	struct manage_s *mnPtr;
	assignManagePtr(&mnPtr, base, index);
	decodeManage(mnPtr);
}

//Decodes some of Manage's fields
void FlexSEA_Generic::decodeManage(struct manage_s *mnPtr)
{
	//Accel in mG
	mnPtr->decoded.accel.x = (1000*mnPtr->accel.x)/8192;
	mnPtr->decoded.accel.y = (1000*mnPtr->accel.y)/8192;
	mnPtr->decoded.accel.z = (1000*mnPtr->accel.z)/8192;

	//Gyro in degrees/s
	mnPtr->decoded.gyro.x = (100*mnPtr->gyro.x)/164;
	mnPtr->decoded.gyro.y = (100*mnPtr->gyro.y)/164;
	mnPtr->decoded.gyro.z = (100*mnPtr->gyro.z)/164;

	mnPtr->decoded.analog[0] = (int32_t)1000*((float)mnPtr->analog[0]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[1] = (int32_t)1000*((float)mnPtr->analog[1]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[2] = (int32_t)1000*((float)mnPtr->analog[2]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[3] = (int32_t)1000*((float)mnPtr->analog[3]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[4] = (int32_t)1000*((float)mnPtr->analog[4]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[5] = (int32_t)1000*((float)mnPtr->analog[5]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[6] = (int32_t)1000*((float)mnPtr->analog[6]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
	mnPtr->decoded.analog[7] = (int32_t)1000*((float)mnPtr->analog[7]/ \
						STM32_ADC_MAX)*STM32_ADC_SUPPLY;
}

//Decodes some of Gossip's fields
void FlexSEA_Generic::decodeGossip(uint8_t base, uint8_t index)
{
	struct gossip_s *goPtr;
	assignGossipPtr(&goPtr, base, index);
	decodeGossip(goPtr);
}

//Decodes some of Gossip's fields
void FlexSEA_Generic::decodeGossip(struct gossip_s *goPtr)
{
	//Accel in mG
	goPtr->decoded.accel.x = (1000*goPtr->accel.x)/8192;
	goPtr->decoded.accel.y = (1000*goPtr->accel.y)/8192;
	goPtr->decoded.accel.z = (1000*goPtr->accel.z)/8192;

	//Gyro in degrees/s
	goPtr->decoded.gyro.x = (100*goPtr->gyro.x)/164;
	goPtr->decoded.gyro.y = (100*goPtr->gyro.y)/164;
	goPtr->decoded.gyro.z = (100*goPtr->gyro.z)/164;

	//Magneto in uT (0.15uT/LSB)
	goPtr->decoded.magneto.x = (15*goPtr->magneto.x)/100;
	goPtr->decoded.magneto.y = (15*goPtr->magneto.y)/100;
	goPtr->decoded.magneto.z = (15*goPtr->magneto.z)/100;
}

void FlexSEA_Generic::decodeBattery(uint8_t base, uint8_t index)
{
	struct battery_s *baPtr;
	assignBatteryPtr(&baPtr, base, index);
	decodeBattery(baPtr);
}

//Decodes some of Battery's fields
void FlexSEA_Generic::decodeBattery(struct battery_s *baPtr)
{
	baPtr->decoded.voltage = baPtr->voltage;
	baPtr->decoded.current = baPtr->current;
	baPtr->decoded.power = baPtr->decoded.voltage * baPtr->decoded.current;
	baPtr->decoded.temp = baPtr->temp;
}

//Decodes some of Strain's fields
void FlexSEA_Generic::decodeStrain(uint8_t base, uint8_t index)
{
	struct strain_s *stPtr;
	assignStrainPtr(&stPtr, base, index);
	decodeStrain(stPtr);
}

void FlexSEA_Generic::decodeStrain(struct strain_s *stPtr)
{
	stPtr->decoded.strain[0] = (100*(stPtr->ch[0].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[1] = (100*(stPtr->ch[1].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[2] = (100*(stPtr->ch[2].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[3] = (100*(stPtr->ch[3].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[4] = (100*(stPtr->ch[4].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
	stPtr->decoded.strain[5] = (100*(stPtr->ch[5].strain_filtered-STRAIN_MIDPOINT)/STRAIN_MIDPOINT);
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
			decodeManage(base, index);
			break;
		case FLEXSEA_EXECUTE_BASE:
			decodeExecute(base, index);
			decodeRicnu(base, index);
			break;
		case FLEXSEA_BATTERY_BASE:
			decodeBattery(base, index);
			break;
		case FLEXSEA_STRAIN_BASE:
			decodeStrain(base, index);
			break;
		case FLEXSEA_GOSSIP_BASE:
			decodeGossip(base, index);
			break;
		default:
			break;
	}
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

//Assign pointer
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

//Assign pointer
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

//Assign pointer
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

//Assign pointer
void FlexSEA_Generic::assignStrainPtr(struct strain_s **myPtr, uint8_t base, \
									  uint8_t slave)
{
	//Based on selected slave, what structure do we use?
	switch(list_to_slave[base+slave])
	{
		case FLEXSEA_STRAIN_1:
			*myPtr = &strain1;
			break;
		default:
			*myPtr = &strain1;
			break;
	}
}

//Assign pointer
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
