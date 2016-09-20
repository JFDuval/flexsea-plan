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
	[This file] w_execute.h: Execute View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_execute.h"
#include "flexsea_generic.h"
#include "ui_w_execute.h"
#include "main.h"
#include <QString>
#include <QTextStream>
#include <QDebug>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Execute::W_Execute(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_Execute)
{
    ui->setupUi(this);

    setWindowTitle("Execute - Barebone");
    setWindowIcon(QIcon(":icons/d_logo_small.png"));

    init();
}

W_Execute::~W_Execute()
{
    delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//Call this function to refresh the display
void W_Execute::refresh(void)
{
    struct execute_s *exPtr;
    myFlexSEA_Generic.assignExecutePtr(&exPtr, SL_BASE_ALL, \
                                       ui->comboBox_slave->currentIndex());
    displayExecute(exPtr);
}

//****************************************************************************
// Public slot(s):
//****************************************************************************

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Execute::init(void)
{
    //Populates Slave list:
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBox_slave, \
                                            SL_BASE_EX, SL_LEN_EX);
}

void W_Execute::displayExecute(struct execute_s *ex)
{
    int combined_status = 0;

    //Raw values:
    //===========

    ui->disp_accx->setText(QString::number(ex->accel.x));
    ui->disp_accy->setText(QString::number(ex->accel.y));
    ui->disp_accz->setText(QString::number(ex->accel.z));
    ui->disp_gyrox->setText(QString::number(ex->gyro.x));
    ui->disp_gyroy->setText(QString::number(ex->gyro.y));
    ui->disp_gyroz->setText(QString::number(ex->gyro.z));

    ui->disp_encDisplay->setText(QString::number(ex->enc_display));
    ui->disp_encControl->setText(QString::number(ex->enc_control));
    ui->disp_encCommut->setText(QString::number(ex->enc_commut));

    ui->disp_strain->setText(QString::number(ex->strain));
    ui->disp_ana->setText(QString::number(ex->analog[0]));
    ui->disp_ana1->setText(QString::number(ex->analog[1]));

    ui->disp_current->setText(QString::number(ex->current));

    ui->disp_vb->setText(QString::number(ex->volt_batt));
    ui->disp_vg->setText(QString::number(ex->volt_int));
    ui->disp_temp->setText(QString::number(ex->temp));

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
    myFlexSEA_Generic.decodeStatus(SL_BASE_EX, ui->comboBox_slave->currentIndex(), \
                                      ex->status1, ex->status2, &myStr);
    ui->label_status1->setText(myStr);

    //==========
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
