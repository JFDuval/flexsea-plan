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
	[This file] w_control.cpp: Control Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

//#include "main.h"
#include <unistd.h>
#include <flexsea_system.h>
#include <flexsea_board.h>
#include "w_control.h"
#include "flexsea_generic.h"
#include "trapez.h"
#include "ui_w_control.h"
#include <QTimer>
#include <QDebug>
#include <flexsea_comm.h>
//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Control::W_Control(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_Control)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	initControl();
	initTimers();
}

W_Control::~W_Control()
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

void W_Control::initControl(void)
{
	initTabToggle();
	initTabSlider();
	ui->tabWidget->setCurrentIndex(0);

	//Populates Slave list:
	FlexSEA_Generic::populateSlaveComboBox(ui->comboBox_slave, SL_BASE_EX, \
											SL_LEN_EX);
	//Variables:
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_EX, active_slave_index);


	//Variable option lists:
	var_list_controllers << "**Null**" << "Open" << "Position" << "Current" \
						 << "Impedance" << "Other/custom";
	for(int index = 0; index < var_list_controllers.count(); index++)
	{
		ui->comboBox_ctrl_list->addItem(var_list_controllers.at(index));
	}

	//Gains:
	init_ctrl_gains();  //All 0 when we start
	ui->control_g0->setText("0");
	ui->control_g1->setText("0");
	ui->control_g2->setText("0");
	ui->control_g3->setText("0");
	ui->control_g4->setText("0");
	ui->control_g5->setText("0");
	refreshStatusGain();
	ui->statusGains->setTextFormat(Qt::RichText);

	ui->statusController->setText("Active controller: none/not selected via GUI.");

	//Display control encoder:
	var_list_enc_disp << "Execute's" << "RIC/NU - Motor" << "RIC/NU - Joint";
	for(int index = 0; index < var_list_enc_disp.count(); index++)
	{
		ui->comboBoxDispSel->addItem(var_list_enc_disp.at(index));
	}
	ui->labelDispEncoder->setText("No data");   //Initial
}

void W_Control::initTabToggle(void)
{
	//Limit input fields:
	const QValidator *validInt = new QIntValidator(-10000000, 10000000, this);
	const QValidator *validUint = new QIntValidator(0, 10000000, this);
	ui->control_setp_a->setValidator(validInt);
	ui->control_setp_b->setValidator(validInt);
	ui->control_toggle_delayA->setValidator(validUint);
	ui->control_toggle_delayB->setValidator(validUint);
	ui->control_trapeze_spd->setValidator(validUint);
	ui->control_trapeze_acc->setValidator(validUint);

	//Setpoints:
	ui->control_setp_a->setText("0");
	ui->control_setp_b->setText("0");
	ui->control_toggle_delayA->setText("1000");
	ui->control_toggle_delayB->setText("1000");
	ui->control_trapeze_spd->setText("10000");
	ui->control_trapeze_acc->setText("10000");
	ui->label_actualSetpoint->setText("0");

	//Toggle:
	ctrl_toggle_state = 0;
}

void W_Control::initTabSlider(void)
{
	//Limit input fields:
	const QValidator *validator = new QIntValidator(-1000000, 1000000, this);
	ui->control_slider_min->setValidator(validator);
	ui->control_slider_max->setValidator(validator);

	ui->control_slider_min->setText("0");
	ui->control_slider_max->setText("0");
	ui->hSlider_Ctrl->setMinimum(ui->control_slider_min->text().toInt());
	ui->hSlider_Ctrl->setMaximum(ui->control_slider_max->text().toInt());
}

void W_Control::initTimers(void)
{
	timerCtrl = new QTimer(this);
	connect(timerCtrl, SIGNAL(timeout()), this, SLOT(timerCtrlEvent()));

	timerDisplay = new QTimer(this);
	connect(timerDisplay, SIGNAL(timeout()), this, SLOT(timerDisplayEvent()));
	timerDisplay->start(75);
}

void W_Control::init_ctrl_gains(void)
{
	int i = 0, j = 0;
	for(i = 0; i < CONTROLLERS; i++)
	{
		for(j = 0; j < GAIN_FIELDS; j++)
		{
			//All gains = 0:
			ctrl_gains[i][j] = 0;
		}
	}
}

void W_Control::save_ctrl_gains(int controller, int16_t *gains)
{
	int i = 0;
	for(i = 0; i < GAIN_FIELDS; i++)
	{
		ctrl_gains[controller][i] = gains[i];
	}
}

void W_Control::controller_setpoint(int val)
{
	uint16_t numb = 0, valid = 0;
	uint8_t info[2] = {PORT_USB, PORT_USB};

	qDebug() << "Entered controller_setpoint()";

	switch(wanted_controller)
	{
		case 0: //Null
			valid = 0;
			break;
		case 1: //Open
			valid = 1;
			tx_cmd_ctrl_o_w(TX_N_DEFAULT, val);
			qDebug() << "Open: " << val << "mV";
			break;
		case 2: //Position
		case 4: //Impedance
			valid = 1;
			tx_cmd_ctrl_p_w(TX_N_DEFAULT, trap_pos, trap_posi, trap_posf, \
							trap_spd, trap_acc);
			trapez_steps = trapez_gen_motion_1(trap_posi, trap_posf, trap_spd, \
											   trap_acc);
			qDebug() << "Pos/Z: posi = " << trap_posi << ", posf = " << \
						trap_posf << ", spd = " << trap_spd << ", trap_acc = " \
					 << trap_acc;
			 break;
		case 3: //Current
			valid = 1;
			tx_cmd_ctrl_i_w(TX_N_DEFAULT, val);
			qDebug() << "Current: " << val << "mA";
			break;
		//case 4: //Impedance
			//Done with position (see above)
		case 5: //Custom/other
			valid = 0;
			break;
		default:
			valid = 0;
			break;
	}

	if(valid)
	{
		//Common for all gain functions:
		pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
		emit writeCommand(numb, comm_str_usb, WRITE);
	}
	else
	{
		qDebug() << "Invalid controller selected.";
	}
}

void W_Control::stream_ctrl(void)
{
	struct execute_s *ex_ptr;
	FlexSEA_Generic::assignExecutePtr(&ex_ptr, SL_BASE_EX, active_slave_index);

	if(ui->comboBoxDispSel->currentIndex() == 0)    //Execute's
	{
		ui->labelDispEncoder->setText(QString::number(*ex_ptr->enc_ang));
	}
	else if(ui->comboBoxDispSel->currentIndex() == 1)   //RIC/NU - Motor
	{
		ui->labelDispEncoder->setText(QString::number(ricnu_1.enc_motor));
	}
	else if(ui->comboBoxDispSel->currentIndex() == 2)   //RIC/NU - Joint
	{
		ui->labelDispEncoder->setText(QString::number(ricnu_1.enc_joint));
	}
	else
	{
		ui->labelDispEncoder->setText("Invalid.");
	}

	//Update Toggle's setpoint field:
	ui->label_actualSetpoint->setText(QString::number(ctrl_setpoint));
}

void W_Control::control_trapeze(void)
{
	//Call 10x to match 1ms & 100Hz timebases:
	for(int i = 0; i < 10; i++)
	{
		trapez_get_pos(trapez_steps);
	}
	ctrl_setpoint_trap = trapez_get_pos(trapez_steps);
}

void W_Control::timerCtrlEvent(void)
{
	toggle_output_state ^= 1;

	qDebug() << "Control Toggle Timer Event, output = " << toggle_output_state;

	if(toggle_output_state)
	{
		ctrl_setpoint = ui->control_setp_a->text().toInt();
		trap_posi = ui->control_setp_b->text().toInt();
		trap_posf = ui->control_setp_a->text().toInt();
		trap_pos = ctrl_setpoint;

		timerCtrl->setInterval(ui->control_toggle_delayA->text().toInt());
	}
	else
	{
		ctrl_setpoint = ui->control_setp_b->text().toInt();
		trap_posi = ui->control_setp_a->text().toInt();
		trap_posf = ui->control_setp_b->text().toInt();
		trap_pos = ctrl_setpoint;

		timerCtrl->setInterval(ui->control_toggle_delayB->text().toInt());
	}

	trap_spd = ui->control_trapeze_spd->text().toInt();
	trap_acc = ui->control_trapeze_acc->text().toInt();

	controller_setpoint(ctrl_setpoint);
}

void W_Control::timerDisplayEvent(void)
{
	stream_ctrl();
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_Control::on_pushButton_SetController_clicked()
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;
	int16_t ctrl = CTRL_NONE;

	selected_controller = ui->comboBox_ctrl_list->currentIndex();
	wanted_controller = selected_controller;

	switch(wanted_controller)
	{
		case 0: //Null
			ctrl = CTRL_NONE;
			break;
		case 1: //Open
			ctrl = CTRL_OPEN;
			break;
		case 2: //Position
			ctrl = CTRL_POSITION;
			 break;
		case 3: //Current
			ctrl = CTRL_CURRENT;
			break;
		case 4: //Impedance
			ctrl = CTRL_IMPEDANCE;
			break;
		case 5: //Custom/other
			ctrl = CTRL_NONE;
			break;
		default:
			ctrl = CTRL_NONE;
			break;
	}

	//Prepare and send command:
	tx_cmd_ctrl_mode_w(TX_N_DEFAULT, ctrl);
	pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, WRITE);

	//Notify user:
	QString msg;
	msg = "Active controller: " + var_list_controllers.at(wanted_controller);
	ui->statusController->setText(msg);
}

void W_Control::on_pushButton_setp_a_go_clicked()
{
	int val = 0;
	val = ui->control_setp_a->text().toInt();

	ctrl_setpoint = ui->control_setp_a->text().toInt();
	trap_posi = ui->labelDispEncoder->text().toInt();
	trap_posf = ui->control_setp_a->text().toInt();
	trap_pos = ctrl_setpoint;

	trap_spd = ui->control_trapeze_spd->text().toInt();
	trap_acc = ui->control_trapeze_acc->text().toInt();

	controller_setpoint(val);
}

void W_Control::on_pushButton_setp_b_go_clicked()
{
	int val = 0;
	val = ui->control_setp_b->text().toInt();

	ctrl_setpoint = ui->control_setp_b->text().toInt();
	trap_posi = ui->labelDispEncoder->text().toInt();
	trap_posf = ui->control_setp_b->text().toInt();
	trap_pos = ctrl_setpoint;

	trap_spd = ui->control_trapeze_spd->text().toInt();
	trap_acc = ui->control_trapeze_acc->text().toInt();

	controller_setpoint(val);
}

void W_Control::on_pushButton_toggle_clicked()
{
	//Toggle:
	ctrl_toggle_state ^= 1;

	if(!ctrl_toggle_state)
	{
		//We are in Toggle OFF Mode.
		ui->pushButton_toggle->setText("Toggle ON");

		//Enable GO A & B, values, etc:
		ui->pushButton_setp_a_go->setEnabled(true);
		ui->pushButton_setp_b_go->setEnabled(true);
		ui->control_setp_a->setEnabled(true);
		ui->control_setp_b->setEnabled(true);
		ui->control_toggle_delayA->setEnabled(true);
		ui->control_toggle_delayB->setEnabled(true);
		ui->control_trapeze_acc->setEnabled(true);
		ui->control_trapeze_spd->setEnabled(true);

		//Stop timer:
		timerCtrl->stop();
	}
	else
	{
		//We are in Toggle ON Mode.
		ui->pushButton_toggle->setText("Toggle OFF");

		//Enable GO A & B, values, etc:
		ui->pushButton_setp_a_go->setEnabled(false);
		ui->pushButton_setp_b_go->setEnabled(false);
		ui->control_setp_a->setEnabled(false);
		ui->control_setp_b->setEnabled(false);
		ui->control_toggle_delayA->setEnabled(false);
		ui->control_toggle_delayB->setEnabled(false);
		ui->control_trapeze_acc->setEnabled(false);
		ui->control_trapeze_spd->setEnabled(false);

		//Start at Setpoint A, going to B:
		toggle_output_state = 1;

		//Start timer:
		timerCtrl->start(ui->control_toggle_delayA->text().toInt());
	}
}

void W_Control::on_pushButton_CtrlMinMax_clicked()
{
	//Get min & max, update slider limits:
	int min = ui->control_slider_min->text().toInt();
	int max = ui->control_slider_max->text().toInt();

	//Safety:
	if(min > max)
	{
		min = max;
		ui->control_slider_min->setText(QString::number(min));
	}

	ui->hSlider_Ctrl->setMinimum(min);
	ui->hSlider_Ctrl->setMaximum(max);

	//Default position:
	if(min < 0)	{ui->hSlider_Ctrl->setValue(0);}
	else {ui->hSlider_Ctrl->setValue(min);}

	//Reset button's color:
	ui->pushButton_CtrlMinMax->setStyleSheet("");
}

void W_Control::on_hSlider_Ctrl_valueChanged(int value)
{
	(void)value;	//Unused for now

	int val = 0;
	val = ui->hSlider_Ctrl->value();
	ui->disp_slider->setText(QString::number(val));
	ctrl_setpoint = val;

	//When we move the slider we do not use trapeze, we just "slip" the setpoint
	trap_pos = val;
	trap_posi = val;
	trap_posf = val;
	controller_setpoint(val);

	//Wait 2ms to avoid sending a million packets when we move the slider
	usleep(2000);
}

void W_Control::on_pushButton_SetGains_clicked()
{
	int16_t gains[6] = {0,0,0,0,0,0};
	uint16_t numb = 0, valid = 0;
	uint8_t info[2] = {PORT_USB, PORT_USB};

	//Save gains in temp variables:
	gains[0] = ui->control_g0->text().toInt();
	gains[1] = ui->control_g1->text().toInt();
	gains[2] = ui->control_g2->text().toInt();
	gains[3] = ui->control_g3->text().toInt();
	gains[4] = ui->control_g4->text().toInt();
	gains[5] = ui->control_g5->text().toInt();

	//Send command to hardware:

	//Different controllers have different gain functions:
	selected_controller = ui->comboBox_ctrl_list->currentIndex();
	switch(selected_controller)
	{
		case 0: //Null
			valid = 0;
			break;
		case 1: //Open
			valid = 0;
			break;
		case 2: //Position
			valid = 1;
			save_ctrl_gains(selected_controller, gains);
			tx_cmd_ctrl_p_g_w(TX_N_DEFAULT, gains[0], gains[1], gains[2]);
			 break;
		case 3: //Current
			valid = 1;
			save_ctrl_gains(selected_controller, gains);
			tx_cmd_ctrl_i_g_w(TX_N_DEFAULT, gains[0], gains[1], gains[2]);
			break;
		case 4: //Impedance
			valid = 1;
			save_ctrl_gains(selected_controller, gains);
			tx_cmd_ctrl_z_g_w(TX_N_DEFAULT, gains[0], gains[1], gains[2]);
			break;
		case 5: //Custom/other
			valid = 0;
			break;
		default:
			valid = 0;
			break;
	}

	refreshStatusGain();

	if(valid)
	{
		qDebug() << "Valid controller.";

		//Common for all gain functions:
		pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
		emit writeCommand(numb, comm_str_usb, WRITE);
	}
	else
	{
		qDebug() << "Invalid controller, no gains set.";
	}
}

void W_Control::on_comboBox_ctrl_list_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	selected_controller = ui->comboBox_ctrl_list->currentIndex();

	switch(selected_controller)
	{
		case 0: //Null
			ui->control_g0->setText("0");
			ui->control_g1->setText("0");
			ui->control_g2->setText("0");
			ui->control_g3->setText("0");
			ui->control_g4->setText("0");
			ui->control_g5->setText("0");
			ui->control_g0->setDisabled(1);
			ui->control_g1->setDisabled(1);
			ui->control_g2->setDisabled(1);
			ui->control_g3->setDisabled(1);
			ui->control_g4->setDisabled(1);
			ui->control_g5->setDisabled(1);
			break;
		case 1: //Open
			ui->control_g0->setText("0");
			ui->control_g1->setText("0");
			ui->control_g2->setText("0");
			ui->control_g3->setText("0");
			ui->control_g4->setText("0");
			ui->control_g5->setText("0");
			ui->control_g0->setDisabled(1);
			ui->control_g1->setDisabled(1);
			ui->control_g2->setDisabled(1);
			ui->control_g3->setDisabled(1);
			ui->control_g4->setDisabled(1);
			ui->control_g5->setDisabled(1);
			break;
		case 2: //Position
			ui->control_g0->setText(QString::number(ctrl_gains[selected_controller][0]));
			ui->control_g1->setText(QString::number(ctrl_gains[selected_controller][1]));
			ui->control_g2->setText(QString::number(ctrl_gains[selected_controller][2]));
			ui->control_g3->setText(QString::number(ctrl_gains[selected_controller][3]));
			ui->control_g4->setText(QString::number(ctrl_gains[selected_controller][4]));
			ui->control_g5->setText(QString::number(ctrl_gains[selected_controller][5]));
			ui->control_g0->setEnabled(1);
			ui->control_g1->setEnabled(1);
			ui->control_g2->setEnabled(1);
			ui->control_g3->setDisabled(1);
			ui->control_g4->setDisabled(1);
			ui->control_g5->setDisabled(1);
			break;

		case 3: //Current
			ui->control_g0->setText(QString::number(ctrl_gains[selected_controller][0]));
			ui->control_g1->setText(QString::number(ctrl_gains[selected_controller][1]));
			ui->control_g2->setText(QString::number(ctrl_gains[selected_controller][2]));
			ui->control_g3->setText(QString::number(ctrl_gains[selected_controller][3]));
			ui->control_g4->setText(QString::number(ctrl_gains[selected_controller][4]));
			ui->control_g5->setText(QString::number(ctrl_gains[selected_controller][5]));
			ui->control_g0->setEnabled(1);
			ui->control_g1->setEnabled(1);
			ui->control_g2->setEnabled(1);
			ui->control_g3->setDisabled(1);
			ui->control_g4->setDisabled(1);
			ui->control_g5->setDisabled(1);
			break;
		case 4: //Impedance
			ui->control_g0->setText(QString::number(ctrl_gains[selected_controller][0]));
			ui->control_g1->setText(QString::number(ctrl_gains[selected_controller][1]));
			ui->control_g2->setText(QString::number(ctrl_gains[selected_controller][2]));
			ui->control_g3->setText(QString::number(ctrl_gains[selected_controller][3]));
			ui->control_g4->setText(QString::number(ctrl_gains[selected_controller][4]));
			ui->control_g5->setText(QString::number(ctrl_gains[selected_controller][5]));
			ui->control_g0->setEnabled(1);
			ui->control_g1->setEnabled(1);
			ui->control_g2->setEnabled(1);
			ui->control_g3->setDisabled(1);
			ui->control_g4->setDisabled(1);
			ui->control_g5->setDisabled(1);
			break;
		case 5: //Custom/other
			ui->control_g0->setText("0");
			ui->control_g1->setText("0");
			ui->control_g2->setText("0");
			ui->control_g3->setText("0");
			ui->control_g4->setText("0");
			ui->control_g5->setText("0");
			ui->control_g0->setDisabled(1);
			ui->control_g1->setDisabled(1);
			ui->control_g2->setDisabled(1);
			ui->control_g3->setDisabled(1);
			ui->control_g4->setDisabled(1);
			ui->control_g5->setDisabled(1);
			break;
		default:
			ui->control_g0->setText("0");
			ui->control_g1->setText("0");
			ui->control_g2->setText("0");
			ui->control_g3->setText("0");
			ui->control_g4->setText("0");
			ui->control_g5->setText("0");
			ui->control_g0->setDisabled(1);
			ui->control_g1->setDisabled(1);
			ui->control_g2->setDisabled(1);
			ui->control_g3->setDisabled(1);
			ui->control_g4->setDisabled(1);
			ui->control_g5->setDisabled(1);
			break;
	}
}

void W_Control::on_comboBox_slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	qDebug() << "Changed active slave";
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_EX, active_slave_index);
}

void W_Control::refreshStatusGain(void)
{
	QString str;

	str = "<i>Gains: I = [" + \
			QString::number(ctrl_gains[3][0]) + ", " + \
			QString::number(ctrl_gains[3][1]) + ", " + \
			QString::number(ctrl_gains[3][2]) + "], P = [" + \
			QString::number(ctrl_gains[2][0]) + ", " + \
			QString::number(ctrl_gains[2][1]) + ", " + \
			QString::number(ctrl_gains[2][2]) + "], Z = [" + \
			QString::number(ctrl_gains[4][0]) + ", " + \
			QString::number(ctrl_gains[4][1]) + ", " + \
			QString::number(ctrl_gains[4][2]) + "].</i>";

	ui->statusGains->setText(str);
	qDebug() << str;
}

void W_Control::on_control_slider_min_textEdited(const QString &arg1)
{
	(void)arg1;
	minMaxTextChanged();
}

void W_Control::on_control_slider_max_textEdited(const QString &arg1)
{
	(void)arg1;
	minMaxTextChanged();
}

void W_Control::minMaxTextChanged(void)
{
	ui->pushButton_CtrlMinMax->setStyleSheet("background-color: rgb(255, 255, 0); \
											   color: rgb(0, 0, 0)");
}
