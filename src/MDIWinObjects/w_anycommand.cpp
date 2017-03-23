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
	[This file] w_anycommand.h: Any Command Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_anycommand.h"
#include "ui_w_anycommand.h"
#include <flexsea_system.h>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_AnyCommand::W_AnyCommand(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_AnyCommand)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
}

W_AnyCommand::~W_AnyCommand()
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

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_AnyCommand::init(void)
{
	//Slave lists:
	FlexSEA_Generic::populateSlaveComboBox(ui->comboBoxSlaveR, SL_BASE_ALL, \
											SL_LEN_ALL);
	FlexSEA_Generic::populateSlaveComboBox(ui->comboBoxSlaveX, SL_BASE_ALL, \
										  SL_LEN_ALL);
	ui->comboBoxSlaveX->setCurrentIndex((ui->comboBoxSlaveX->count()) - 1);

	//Commands:
	ui->spinBoxCMDS->setValue(1);   //Default = 1 command
	ui->lineEditCode->setDisabled(true);
	var_list_commands   << "CMD_READ_ALL" << "CMD_USER_DATA" << \
						   "CMD_ENCODER" << "CMD_STRAIN" << \
						   "CMD_PWRO" << "CMD_CTRL_MODE" << \
						   "CMD_CTRL_I_G" << "CMD_CTRL_P_G" << \
						   "CMD_CTRL_Z_G" << "CMD_CTRL_O" << \
						   "CMD_CTRL_I" << "CMD_CTRL_P" << \
						   "CMD_IN_CONTROL" << "CMD_USER1" << \
						   "CMD_USER2" << "CMD_USER3" << \
						   "CMD_USER4" << "CMD_USER5" << \
						   "Manual Entry";
	var_list_codes      << CMD_READ_ALL << CMD_USER_DATA << \
						   CMD_ENCODER << CMD_STRAIN << \
						   CMD_PWRO << CMD_CTRL_MODE << \
						   CMD_CTRL_I_G << CMD_CTRL_P_G << \
						   CMD_CTRL_Z_G << CMD_CTRL_O << \
						   CMD_CTRL_I << CMD_CTRL_P << \
						   CMD_IN_CONTROL << CMD_USER1 << \
						   CMD_USER2 << CMD_USER3 << \
						   CMD_USER4 << CMD_USER5 << \
						   0;
	ui->comboBoxCmd->addItems(var_list_commands);
	ui->lineEditCode->setText(QString::number(var_list_codes.at(0)));

	//R/W:
	ui->comboBoxRW->addItem("Write");
	ui->comboBoxRW->addItem("Read");

	//Disabled (not programmed yet):
	ui->pushButtonSend->setDisabled(true);
	ui->lineEditResult2->setDisabled(true);
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_AnyCommand::on_comboBoxCmd_currentIndexChanged(int index)
{
	if(ui->comboBoxCmd->currentIndex() == (var_list_commands.length()-1))
	{
		ui->lineEditCode->setDisabled(false);
		ui->lineEditCode->setText("");
	}
	else
	{
		ui->lineEditCode->setDisabled(true);
		ui->lineEditCode->setText(QString::number(var_list_codes.at(index)));
	}
}
