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
	[This file] w_gossip.h: Gossip View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_gossip.h"
#include "flexsea_generic.h"
#include "ui_w_gossip.h"
#include "main.h"
#include <QString>
#include <QTextStream>
#include <QDebug>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Gossip::W_Gossip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_Gossip)
{
    ui->setupUi(this);

    setWindowTitle("Gossip - Barebone");
    setWindowIcon(QIcon(":icons/d_logo_small.png"));

    init();
}

W_Gossip::~W_Gossip()
{
    delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//Call this function to refresh the display
void W_Gossip::refresh(void)
{
    struct gossip_s *goPtr;
    myFlexSEA_Generic.assignGossipPtr(&goPtr, SL_BASE_GOSSIP, \
                                      ui->comboBox_slave->currentIndex());
    displayGossip(goPtr);
}

void W_Gossip::log(QTextStream *filePtr, uint8_t slaveIndex, \
                                char term, qint64 t_ms, QString t_text)
{
    struct gossip_s *goPtr;
    myFlexSEA_Generic.assignGossipPtr(&goPtr, SL_BASE_GOSSIP, slaveIndex);
/*
    (*filePtr) << t_text << ',' << \
                        t_ms << ',' << \
                        mnPtr->accel.x << ',' << \
                        mnPtr->accel.y << ',' << \
                        mnPtr->accel.z << ',' << \
                        mnPtr->gyro.x << ',' << \
                        mnPtr->gyro.y << ',' << \
                        mnPtr->gyro.z << ',' << \
                        mnPtr->digitalIn << ',' << \
                        mnPtr->sw1 << ',' << \
                        mnPtr->analog[0] << ',' << \
                        mnPtr->analog[1] << ',' << \
                        mnPtr->analog[2] << ',' << \
                        mnPtr->analog[3] << ',' << \
                        mnPtr->analog[4] << ',' << \
                        mnPtr->analog[5] << ',' << \
                        mnPtr->analog[6] << ',' << \
                        mnPtr->analog[7] << ',' << \
                        mnPtr->status1 << ',' << \
                        term;
						*/
}

//****************************************************************************
// Public slot(s):
//****************************************************************************

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Gossip::init(void)
{
    //Populates Slave list:
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBox_slave, \
                                            SL_BASE_GOSSIP, SL_LEN_GOSSIP);
}

void W_Gossip::displayGossip(struct gossip_s *go)
{
    int combined_status = 0;

    //Raw values:
    //===========

    ui->disp_accx->setText(QString::number(go->accel.x));
    ui->disp_accy->setText(QString::number(go->accel.y));
    ui->disp_accz->setText(QString::number(go->accel.z));
    ui->disp_gyrox->setText(QString::number(go->gyro.x));
    ui->disp_gyroy->setText(QString::number(go->gyro.y));
    ui->disp_gyroz->setText(QString::number(go->gyro.z));

/*

    combined_status = (ex->status2 << 8) & ex->status1;
    ui->disp_stat1->setText(QString::number(combined_status));

    //Decode some of them:
    //===================

    ui->disp_current_d->setText(QString::number((float)ex->current*18.5, 'i',0));
    ui->disp_vb_d->setText(QString::number(P4_ADC_SUPPLY*((16*(float)ex->volt_batt/3 + 302 )/P4_ADC_MAX) / 0.0738, 'f',2));
    ui->disp_vg_d->setText(QString::number(P4_ADC_SUPPLY*((26*(float)ex->volt_int/3 + 440 )/P4_ADC_MAX) / 0.43, 'f',2));
    ui->disp_temp_d->setText(QString::number(((((2.625*(float)ex->temp + 41)/P4_ADC_MAX)*P4_ADC_SUPPLY) - P4_T0) / P4_TC,'f',1));

    ui->disp_ana_d->setText(QString::number(((float)ex->analog[0]/P5_ADC_MAX)*P5_ADC_SUPPLY,'f',2));
    ui->disp_ana1_d->setText(QString::number(((float)ex->analog[1]/P5_ADC_MAX)*P5_ADC_SUPPLY,'f',2));

    ui->disp_accx_d->setText(QString::number((double)ex->accel.x/8192, 'f', 2));
    ui->disp_accy_d->setText(QString::number((double)ex->accel.y/8192, 'f', 2));
    ui->disp_accz_d->setText(QString::number((double)ex->accel.z/8192, 'f', 2));
    ui->disp_gyrox_d->setText(QString::number((double)ex->gyro.x/16.4, 'i', 0));
    ui->disp_gyroy_d->setText(QString::number((double)ex->gyro.y/16.4, 'i', 0));
    ui->disp_gyroz_d->setText(QString::number((double)ex->gyro.z/16.4, 'i', 0));

    ui->disp_strain_d->setText(QString::number(((double)(ex->strain-32768)/32768)*100, 'i', 0));

    QString myStr;
    myFlexSEA_Generic.execStatusBytes(ex->status1, ex->status2, &myStr);
    ui->label_status1->setText(myStr);
*/

    //==========
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
