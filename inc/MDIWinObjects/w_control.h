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

#ifndef W_CONTROL_H
#define W_CONTROL_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexsea_generic.h"
#include "cmd-ActPack.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Control;
}

class W_Control : public QWidget, public Counter<W_Control>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Control(QWidget *parent = 0);
	~W_Control();
	static int getSetp(void);

public slots:

signals:
	void windowClosed(void);
	void writeCommand(uint8_t numb, uint8_t *tx_data, uint8_t r_w);

private slots:
	void timerCtrlEvent(void);
	void timerDisplayEvent(void);
	void on_pushButton_SetController_clicked();
	void on_pushButton_setp_a_go_clicked();
	void on_pushButton_setp_b_go_clicked();
	void on_pushButton_toggle_clicked();
	void on_pushButton_CtrlMinMax_clicked();
	void on_hSlider_Ctrl_valueChanged(int value);
	void on_pushButton_SetGains_clicked();
	void on_comboBox_ctrl_list_currentIndexChanged(int index);
	void on_comboBox_slave_currentIndexChanged(int index);
	void on_control_slider_min_textEdited(const QString &arg1);
	void on_control_slider_max_textEdited(const QString &arg1);
	void on_control_slider_max_editingFinished();
	void on_control_slider_min_editingFinished();

	void on_comboBoxCmdStyle_currentIndexChanged(int index);

	void on_comboBoxFSM2_currentIndexChanged(int index);

private:
	//Variables & Objects:
	Ui::W_Control *ui;
	int active_slave, active_slave_index;
	int wanted_controller = 0, selected_controller = 0, active_controller = 0;
	int trap_pos = 0, trap_posi = 0, trap_posf = 0, trap_spd = 0, trap_acc = 0;
	int ctrl_setpoint = 0, ctrl_setpoint_trap = 0;
	int ctrl_toggle_state = 0;
	int ctrl_gains[6][6];
	int trapez_steps = 0;
	uint8_t toggle_output_state = 0;
	QStringList var_list_controllers, var_list_enc_disp;
	QTimer *timerCtrl, *timerDisplay;
	uint8_t transferBuf[48];
	static int setp;
	struct ActPack_s ActPack;

	//Function(s):
	void initControl(void);
	void initTabSlider(void);
	void initTabToggle(void);
	void init_ctrl_gains(void);
	void save_ctrl_gains(int controller, int16_t *gains);
	void controller_setpoint(int val);
	void initTimers(void);
	void stream_ctrl(void);
	void refreshStatusGain(void);
	void control_trapeze(void);
	void minMaxTextChanged(void);
	void update_CtrlMinMax(void);
	void initActPack(void);
	void sendActPack(void);
	void setController(uint8_t ctrl);
};

#define CONTROLLERS         6
#define GAIN_FIELDS         6

#endif // W_CONTROL_H
