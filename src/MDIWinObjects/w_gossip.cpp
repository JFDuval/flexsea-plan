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
	[This file] w_gossip: Gossip View Window
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
	//Raw values:
	//===========

	ui->disp_accx->setText(QString::number(go->accel.x));
	ui->disp_accy->setText(QString::number(go->accel.y));
	ui->disp_accz->setText(QString::number(go->accel.z));

	ui->disp_gyrox->setText(QString::number(go->gyro.x));
	ui->disp_gyroy->setText(QString::number(go->gyro.y));
	ui->disp_gyroz->setText(QString::number(go->gyro.z));

	ui->disp_magnetox->setText(QString::number(go->magneto.x));
	ui->disp_magnetoy->setText(QString::number(go->magneto.y));
	ui->disp_magnetoz->setText(QString::number(go->magneto.z));

	ui->disp_cs1->setText(QString::number(go->capsense[0]));
	ui->disp_cs2->setText(QString::number(go->capsense[1]));
	ui->disp_cs3->setText(QString::number(go->capsense[2]));
	ui->disp_cs4->setText(QString::number(go->capsense[3]));

	ui->disp_io1->setText(QString::number(go->io[0]));
	ui->disp_io2->setText(QString::number(go->io[1]));

	ui->disp_stat1->setText(QString::number(go->status));

	//Decoded values:
	//===============

	ui->disp_accx_d->setText(QString::number((float)go->decoded.accel.x/1000,'f',2));
	ui->disp_accy_d->setText(QString::number((float)go->decoded.accel.y/1000,'f',2));
	ui->disp_accz_d->setText(QString::number((float)go->decoded.accel.z/1000,'f',2));

	ui->disp_gyrox_d->setText(QString::number(go->decoded.gyro.x, 'i', 0));
	ui->disp_gyroy_d->setText(QString::number(go->decoded.gyro.y, 'i', 0));
	ui->disp_gyroz_d->setText(QString::number(go->decoded.gyro.z, 'i', 0));

	ui->disp_magnetox_d->setText(QString::number(go->decoded.magneto.x, 'i', 0));
	ui->disp_magnetoy_d->setText(QString::number(go->decoded.magneto.y, 'i', 0));
	ui->disp_magnetoz_d->setText(QString::number(go->decoded.magneto.z, 'i', 0));

	//==========
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
