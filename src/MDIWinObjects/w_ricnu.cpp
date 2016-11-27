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

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
}

W_Ricnu::~W_Ricnu()
{
	emit windowClosed();
	delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

//Call this function to refresh the display
void W_Ricnu::refreshDisplayRicnu(void)
{
	struct ricnu_s *ricnuPtr;
	FlexSEA_Generic::assignRicnuPtr(&ricnuPtr, SL_BASE_EX, \
									 ui->comboBox_slave->currentIndex());
	displayRicnu(ricnuPtr);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Ricnu::init(void)
{
	//Populates Slave list:
	FlexSEA_Generic::populateSlaveComboBox(ui->comboBox_slave, SL_BASE_EX, \
											SL_LEN_EX);

	//Variables:
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_EX, active_slave_index);
}

//TODO: lots of copy&paste from w_strain.
void W_Ricnu::displayRicnu(struct ricnu_s *ricnu)
{
	//Unpack:
	//=======

	unpackCompressed6ch(ricnu->st.compressedBytes, &ricnu->st.ch[0].strain_filtered,
						&ricnu->st.ch[1].strain_filtered, &ricnu->st.ch[2].strain_filtered,
						&ricnu->st.ch[3].strain_filtered, &ricnu->st.ch[4].strain_filtered,
						&ricnu->st.ch[5].strain_filtered);

	//Raw values:
	//===========

	ui->disp_accx->setText(QString::number(ricnu->ex.accel.x));
	ui->disp_accy->setText(QString::number(ricnu->ex.accel.y));
	ui->disp_accz->setText(QString::number(ricnu->ex.accel.z));
	ui->disp_gyrox->setText(QString::number(ricnu->ex.gyro.x));
	ui->disp_gyroy->setText(QString::number(ricnu->ex.gyro.y));
	ui->disp_gyroz->setText(QString::number(ricnu->ex.gyro.z));

	ui->enc_mot->setText(QString::number(ricnu->ex.enc_motor));
	ui->enc_joint->setText(QString::number(ricnu->ex.enc_joint));

	ui->strain1->setText(QString::number(ricnu->st.ch[0].strain_filtered));
	ui->strain2->setText(QString::number(ricnu->st.ch[1].strain_filtered));
	ui->strain3->setText(QString::number(ricnu->st.ch[2].strain_filtered));
	ui->strain4->setText(QString::number(ricnu->st.ch[3].strain_filtered));
	ui->strain5->setText(QString::number(ricnu->st.ch[4].strain_filtered));
	ui->strain6->setText(QString::number(ricnu->st.ch[5].strain_filtered));

	/*
	ui->strain1->setText(QString::number(ricnu->ext_strain[0]));
	ui->strain2->setText(QString::number(ricnu->ext_strain[1]));
	ui->strain3->setText(QString::number(ricnu->ext_strain[2]));
	ui->strain4->setText(QString::number(ricnu->ext_strain[3]));
	ui->strain5->setText(QString::number(ricnu->ext_strain[4]));
	ui->strain6->setText(QString::number(ricnu->ext_strain[5]));
	*/

	ui->disp_current->setText(QString::number(ricnu->ex.current));

	//Decode some of them:
	//===================

	ui->disp_accx_d->setText(QString::number((float)ricnu->ex.decoded.accel.x/1000,'f',2));
	ui->disp_accy_d->setText(QString::number((float)ricnu->ex.decoded.accel.y/1000,'f',2));
	ui->disp_accz_d->setText(QString::number((float)ricnu->ex.decoded.accel.z/1000,'f',2));

	ui->disp_gyrox_d->setText(QString::number((double)ricnu->ex.decoded.gyro.x, 'i', 0));
	ui->disp_gyroy_d->setText(QString::number((double)ricnu->ex.decoded.gyro.y, 'i', 0));
	ui->disp_gyroz_d->setText(QString::number((double)ricnu->ex.decoded.gyro.z, 'i', 0));

	ui->disp_current_d->setText(QString::number(ricnu->ex.decoded.current, 'i',0));

	//TODO change
	ui->strain1d->setText(QString::number(ricnu->st.decoded.strain[0],'i',0));
	ui->strain2d->setText(QString::number(ricnu->st.decoded.strain[1],'i',0));
	ui->strain3d->setText(QString::number(ricnu->st.decoded.strain[2],'i',0));
	ui->strain4d->setText(QString::number(ricnu->st.decoded.strain[3],'i',0));
	ui->strain5d->setText(QString::number(ricnu->st.decoded.strain[4],'i',0));
	ui->strain6d->setText(QString::number(ricnu->st.decoded.strain[5],'i',0));

	strain1 = ricnu->st;
	/*
	strain1.ch[0].strain_filtered = ricnu->ext_strain[0];
	strain1.ch[1].strain_filtered = ricnu->ext_strain[1];
	strain1.ch[2].strain_filtered = ricnu->ext_strain[2];
	strain1.ch[3].strain_filtered = ricnu->ext_strain[3];
	strain1.ch[4].strain_filtered = ricnu->ext_strain[4];
	strain1.ch[5].strain_filtered = ricnu->ext_strain[5];
*/
}

//Unpack from buffer
void W_Ricnu::unpackCompressed6ch(uint8_t *buf, uint16_t *v0, uint16_t *v1, uint16_t *v2, \
							uint16_t *v3, uint16_t *v4, uint16_t *v5)
{
	*v0 = ((*(buf+0) << 8 | *(buf+1)) >> 4);
	*v1 = (((*(buf+1) << 8 | *(buf+2))) & 0xFFF);
	*v2 = ((*(buf+3) << 8 | *(buf+4)) >> 4);
	*v3 = (((*(buf+4) << 8 | *(buf+5))) & 0xFFF);
	*v4 = ((*(buf+6) << 8 | *(buf+7)) >> 4);
	*v5 = (((*(buf+7) << 8 | *(buf+8))) & 0xFFF);
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
