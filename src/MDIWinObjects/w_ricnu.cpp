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
	[This file] WinViewRicnu: RIC/NU Kneww View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-12 | jfduval | New file
    * 2016-09-12 | jfduval | Enabling stream
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_ricnu.h"
#include "flexsea_generic.h"
#include "ui_w_ricnu.h"
#include "main.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Ricnu::W_Ricnu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_Ricnu)
{
    ui->setupUi(this);

    setWindowTitle("RIC/NU Knee");
    setWindowIcon(QIcon(":icons/d_logo_small.png"));

    init();
}

W_Ricnu::~W_Ricnu()
{
    delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//Call this function to refresh the display
void W_Ricnu::refresh(void)
{
    struct ricnu_s *ricnuPtr;
    myFlexSEA_Generic.assignRicnuPtr(&ricnuPtr, SL_BASE_EX, \
                                     ui->comboBox_slave->currentIndex());
    displayRicnu(ricnuPtr);
}

//****************************************************************************
// Public slot(s):
//****************************************************************************

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Ricnu::init(void)
{
    //Populates Slave list:
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBox_slave, SL_BASE_EX, \
                                            SL_LEN_EX);

    //Variables:
    active_slave_index = ui->comboBox_slave->currentIndex();
    active_slave = myFlexSEA_Generic.getSlaveID(SL_BASE_EX, active_slave_index);
}

void W_Ricnu::displayRicnu(struct ricnu_s *ricnu)
{
    //int combined_status = 0;

    //Raw values:
    //===========

    ui->disp_accx->setText(QString::number(ricnu->ex.accel.x));
    ui->disp_accy->setText(QString::number(ricnu->ex.accel.y));
    ui->disp_accz->setText(QString::number(ricnu->ex.accel.z));
    ui->disp_gyrox->setText(QString::number(ricnu->ex.gyro.x));
    ui->disp_gyroy->setText(QString::number(ricnu->ex.gyro.y));
    ui->disp_gyroz->setText(QString::number(ricnu->ex.gyro.z));

    ui->enc_mot->setText(QString::number(ricnu->ex.enc_commut));
    ui->enc_joint->setText(QString::number(ricnu->ex.enc_control));

    ui->strain1->setText(QString::number(ricnu->ext_strain[0]));
    ui->strain2->setText(QString::number(ricnu->ext_strain[1]));
    ui->strain3->setText(QString::number(ricnu->ext_strain[2]));
    ui->strain4->setText(QString::number(ricnu->ext_strain[3]));
    ui->strain5->setText(QString::number(ricnu->ext_strain[4]));
    ui->strain6->setText(QString::number(ricnu->ext_strain[5]));

    ui->disp_current->setText(QString::number(ricnu->ex.current));

    ui->disp_vb->setText(QString::number(ricnu->ex.volt_batt));

    //combined_status = (ex->status2 << 8) & ex->status1;
    //ui->disp_stat1->setText(QString::number(combined_status));

    //Decode some of them:
    //===================

    //***ToDo*** this shouldn't be done here - mimic w_execute

    ui->disp_current_d->setText(QString::number((float)ricnu->ex.current*18.5, 'i',0));
    ui->disp_vb_d->setText(QString::number(P4_ADC_SUPPLY*((16*(float)ricnu->ex.volt_batt/3 + 302 )/P4_ADC_MAX) / 0.0738, 'f',2));

    ui->disp_accx_d->setText(QString::number((double)ricnu->ex.accel.x/8192, 'f', 2));
    ui->disp_accy_d->setText(QString::number((double)ricnu->ex.accel.y/8192, 'f', 2));
    ui->disp_accz_d->setText(QString::number((double)ricnu->ex.accel.z/8192, 'f', 2));
    ui->disp_gyrox_d->setText(QString::number((double)ricnu->ex.gyro.x/16.4, 'i', 0));
    ui->disp_gyroy_d->setText(QString::number((double)ricnu->ex.gyro.y/16.4, 'i', 0));
    ui->disp_gyroz_d->setText(QString::number((double)ricnu->ex.gyro.z/16.4, 'i', 0));

    ui->strain1d->setText(QString::number(((double)(ricnu->ext_strain[0]-32768)/32768)*100, 'i', 0));
    ui->strain2d->setText(QString::number(((double)(ricnu->ext_strain[1]-32768)/32768)*100, 'i', 0));
    ui->strain3d->setText(QString::number(((double)(ricnu->ext_strain[2]-32768)/32768)*100, 'i', 0));
    ui->strain4d->setText(QString::number(((double)(ricnu->ext_strain[3]-32768)/32768)*100, 'i', 0));
    ui->strain5d->setText(QString::number(((double)(ricnu->ext_strain[4]-32768)/32768)*100, 'i', 0));
    ui->strain6d->setText(QString::number(((double)(ricnu->ext_strain[5]-32768)/32768)*100, 'i', 0));


    /*
    QString myStr;
    myFlexSEA_Generic.execStatusBytes(ex->status1, ex->status2, &myStr);
    ui->label_status1->setText(myStr);
    */
    //==========

    //***ToDo: this is an ugly hack***
    //To be able to plot RIC/NU values we copy them in the exec & strain structures
    exec1.accel.x = ricnu->ex.accel.x;
    exec1.accel.y = ricnu->ex.accel.y;
    exec1.accel.z = ricnu->ex.accel.z;

    exec1.gyro.x = ricnu->ex.gyro.x;
    exec1.gyro.y = ricnu->ex.gyro.y;
    exec1.gyro.z = ricnu->ex.gyro.z;

    exec1.current = ricnu->ex.current;
    exec1.volt_batt = ricnu->ex.volt_batt;

    strain[0].strain_filtered = ricnu->ext_strain[0];
    strain[1].strain_filtered = ricnu->ext_strain[1];
    strain[2].strain_filtered = ricnu->ext_strain[2];
    strain[3].strain_filtered = ricnu->ext_strain[3];
    strain[4].strain_filtered = ricnu->ext_strain[4];
    strain[5].strain_filtered = ricnu->ext_strain[5];
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
