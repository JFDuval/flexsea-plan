/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2017 Dephy, Inc. <http://dephy.com/>
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] w_rigid: Rigid View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-04-18 | jfduval | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_rigid.h"
#include "ui_w_rigid.h"
#include "flexsea_cmd_user.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Rigid::W_Rigid(QWidget *parent,
				   FlexseaDevice *currentLog,
				   RigidDevice *deviceLogPtr,
				   DisplayMode mode,
				   QList<RigidDevice> *deviceListPtr) :
	QWidget(parent),
	ui(new Ui::W_Rigid)
{
	ui->setupUi(this);

	deviceLog  = deviceLogPtr;
	deviceList = deviceListPtr;

	//displayMode = mode;

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	//updateDisplayMode(displayMode, nullptr);
	updateDisplayMode(mode, currentLog);
}

W_Rigid::~W_Rigid()
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
void W_Rigid::refreshDisplay(void)
{
	display(&((*deviceList)[0]), 0);
}

void W_Rigid::refreshDisplayLog(int index, FlexseaDevice * devPtr)
{
	if(devPtr->slaveName == deviceLog->slaveName)
	{
		if(deviceLog->riList.isEmpty() == false)
		{
			 display(deviceLog, index);
		}
	}
}

void W_Rigid::updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr)
{
	//(void)devPtr;
	displayMode = mode;
	if(displayMode == DisplayLogData)
	{
		initLog(devPtr);
	}
	else
	{
		initLive();
	}
}

int W_Rigid::getCommandCode() { return CMD_READ_ALL_RIGID; }

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Rigid::initLive(void)
{
}

void W_Rigid::initLog(FlexseaDevice *devPtr)
{
	(void)devPtr;
}

void W_Rigid::display(RigidDevice *devicePtr, int index)
{
	struct rigid_s *ri = devicePtr->riList[index];

	//Raw values - Manage:
	//====================

	ui->disp_accx->setText(QString::number(ri->mn.accel.x));
	ui->disp_accy->setText(QString::number(ri->mn.accel.y));
	ui->disp_accz->setText(QString::number(ri->mn.accel.z));
	ui->disp_gyrox->setText(QString::number(ri->mn.gyro.x));
	ui->disp_gyroy->setText(QString::number(ri->mn.gyro.y));
	ui->disp_gyroz->setText(QString::number(ri->mn.gyro.z));

	ui->disp_an0->setText(QString::number(ri->mn.analog[0]));
	ui->disp_an1->setText(QString::number(ri->mn.analog[1]));
	ui->disp_an2->setText(QString::number(ri->mn.analog[2]));
	ui->disp_an3->setText(QString::number(ri->mn.analog[3]));

	ui->disp_mn_stat->setText(QString::number(ri->mn.status));
	ui->label_mn_stat->setText("Status: ToDo.");

	ui->dispGV0->setText(QString::number(ri->mn.genVar[0]));
	ui->dispGV1->setText(QString::number(ri->mn.genVar[1]));
	ui->dispGV2->setText(QString::number(ri->mn.genVar[2]));
	ui->dispGV3->setText(QString::number(ri->mn.genVar[3]));
	ui->dispGV4->setText(QString::number(ri->mn.genVar[4]));
	ui->dispGV5->setText(QString::number(ri->mn.genVar[5]));
	ui->dispGV6->setText(QString::number(ri->mn.genVar[6]));
	ui->dispGV7->setText(QString::number(ri->mn.genVar[7]));
	ui->dispGV8->setText(QString::number(ri->mn.genVar[8]));

	//Raw values - Execute:
	//=====================

	ui->disp_mot_ang->setText(QString::number(*(ri->ex.enc_ang)));
	ui->disp_mot_vel->setText(QString::number(*(ri->ex.enc_ang_vel)));
	ui->disp_mot_acc->setText(QString::number(ri->ex.mot_acc));
	ui->disp_strain->setText(QString::number(ri->ex.strain));
	ui->disp_current_mot->setText(QString::number(ri->ex.mot_current));

	ui->disp_joint_angle->setText(QString::number(*(ri->ex.joint_ang)));
	ui->disp_joint_angle_vel->setText(QString::number(*(ri->ex.joint_ang_vel)));
	ui->disp_joint_angle_from_mot->setText(QString::number(*(ri->ex.joint_ang_from_mot)));
	ui->disp_mot_volt->setText(QString::number(ri->ex.mot_volt));

	ui->disp_ex_stat->setText(QString::number(ri->ex.status));
	ui->label_ex_stat->setText("Status: ToDo.");

	//Raw values - Regulate:
	//======================

	ui->disp_vb->setText(QString::number((float)ri->re.vb/1000,'f',2));
	ui->disp_vg->setText(QString::number((float)ri->re.vg/1000,'f',2));
	ui->disp_5v->setText(QString::number((float)ri->re.v5/1000,'f',2));

	ui->disp_current_batt->setText(QString::number((float)ri->re.current/1000,'f',2));
	ui->disp_temp->setText(QString::number(ri->re.temp));
	ui->disp_button->setText(QString::number(ri->re.button));

	ui->disp_re_stat->setText(QString::number(ri->re.status));
	ui->label_re_stat->setText("Status: ToDo.");

	//Decoded values - Manage:
	//========================

	ui->disp_accx_d->setText(QString::number((float)ri->mn.decoded.accel.x/1000,'f',2));
	ui->disp_accy_d->setText(QString::number((float)ri->mn.decoded.accel.y/1000,'f',2));
	ui->disp_accz_d->setText(QString::number((float)ri->mn.decoded.accel.z/1000,'f',2));

	ui->disp_gyrox_d->setText(QString::number(ri->mn.decoded.gyro.x, 'i', 0));
	ui->disp_gyroy_d->setText(QString::number(ri->mn.decoded.gyro.y, 'i', 0));
	ui->disp_gyroz_d->setText(QString::number(ri->mn.decoded.gyro.z, 'i', 0));

	//Decoded values - Regulate:
	//==========================

	if(ri->re.status & 0x80){ui->disp_button->setText("Pressed");}
	else{ui->disp_button->setText("Released");}
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
