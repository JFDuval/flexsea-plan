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
	[This file] w_strain.h: Strain View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_strain.h"
#include "flexsea_generic.h"
#include "ui_w_strain.h"
#include "main.h"
#include <QString>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Strain::W_Strain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_Strain)
{
    ui->setupUi(this);

    setWindowTitle("6ch StrainAmp - Barebone");
    setWindowIcon(QIcon(":icons/d_logo_small.png"));

    init();
}

W_Strain::~W_Strain()
{
    delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//Call this function to refresh the display
void W_Strain::refresh(void)
{
    struct strain_s *stPtr;
    myFlexSEA_Generic.assignStrainPtr(&stPtr, SL_BASE_ALL, \
                                       ui->comboBoxSlave->currentIndex());
    displayStrain(stPtr);
}

//****************************************************************************
// Public slot(s):
//****************************************************************************

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Strain::init(void)
{
    //Populates Slave list:
    myFlexSEA_Generic.populateSlaveComboBox(ui->comboBoxSlave, \
                                            SL_BASE_STRAIN, SL_LEN_STRAIN);
}

void W_Strain::displayStrain(struct strain_s *st)
{
    int combined_status = 0;

    //Raw values:
    //===========

    //ToDo Wrong
    ui->disp_strain_ch1->setText(QString::number(st->strain_filtered));
    ui->disp_strain_ch2->setText(QString::number(st->strain_filtered));
    ui->disp_strain_ch3->setText(QString::number(st->strain_filtered));
    ui->disp_strain_ch4->setText(QString::number(st->strain_filtered));
    ui->disp_strain_ch5->setText(QString::number(st->strain_filtered));
    ui->disp_strain_ch6->setText(QString::number(st->strain_filtered));

    //Decoded values:
    //===================

    /*
    ui->disp_strain_ch1_d->setText(QString::number(st->decoded.strain_filtered,'i',0));
    ui->disp_strain_ch2_d->setText(QString::number(st->decoded.strain_filtered)'i',0);
    ui->disp_strain_ch3_d->setText(QString::number(st->decoded.strain_filtered)'i',0);
    ui->disp_strain_ch4_d->setText(QString::number(st->decoded.strain_filtered)'i',0);
    ui->disp_strain_ch5_d->setText(QString::number(st->decoded.strain_filtered)'i',0);
    ui->disp_strain_ch6_d->setText(QString::number(st->decoded.strain_filtered)'i',0);
    */

    //==========
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
