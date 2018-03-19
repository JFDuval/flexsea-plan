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
	[This file] w_pocket: Pocket View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2018-02-21 | sbelanger | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_pocket.h"
#include "ui_w_pocket.h"
#include "flexsea_cmd_user.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Pocket::W_Pocket(QWidget *parent,
				   FlexseaDevice *currentLog,
				   PocketDevice *deviceLogPtr,
				   DisplayMode mode,
				   QList<PocketDevice> *deviceListPtr) :
	QWidget(parent),
	ui(new Ui::W_Pocket)
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

W_Pocket::~W_Pocket()
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
void W_Pocket::refreshDisplay(void)
{
	int index = ui->comboBox_slave->currentIndex();
	display(&((*deviceList)[index]), 0);
}

void W_Pocket::refreshDisplayLog(int index, FlexseaDevice * devPtr)
{
	if(devPtr->slaveName == deviceLog->slaveName)
	{
		if(deviceLog->poList.isEmpty() == false)
		{
			 display(deviceLog, index);
		}
	}
}

void W_Pocket::updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr)
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

int W_Pocket::getCommandCode() { return CMD_READ_ALL_POCKET;}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Pocket::initLive(void)
{
	//Populates Slave list:
	ui->comboBox_slave->clear();

	for(int i = 0; i < (*deviceList).length(); i++)
	{
		ui->comboBox_slave->addItem((*deviceList)[i].slaveName);
	}
}

void W_Pocket::initLog(FlexseaDevice *devPtr)
{
	(void)devPtr;

	//Populates Slave list:
	ui->comboBox_slave->clear();
	ui->comboBox_slave->addItem(deviceLog->slaveName);
}

void W_Pocket::display(PocketDevice *devicePtr, int index)
{
	struct pocket_s *po = devicePtr->poList[index];
	static uint16_t lastStatus = 0;

	//Raw values - Manage:
	//====================

	ui->disp_accx->setText(QString::number(po->mn.accel.x));
	ui->disp_accy->setText(QString::number(po->mn.accel.y));
	ui->disp_accz->setText(QString::number(po->mn.accel.z));
	ui->disp_gyrox->setText(QString::number(po->mn.gyro.x));
	ui->disp_gyroy->setText(QString::number(po->mn.gyro.y));
	ui->disp_gyroz->setText(QString::number(po->mn.gyro.z));

	ui->disp_an0->setText(QString::number(po->mn.analog[0]));
	ui->disp_an1->setText(QString::number(po->mn.analog[1]));

	ui->disp_mn_stat->setText(QString::number(po->mn.status));

	ui->dispGV0->setText(QString::number(po->mn.genVar[0]));
	ui->dispGV1->setText(QString::number(po->mn.genVar[1]));
	ui->dispGV2->setText(QString::number(po->mn.genVar[2]));
	ui->dispGV3->setText(QString::number(po->mn.genVar[3]));
	ui->dispGV4->setText(QString::number(po->mn.genVar[4]));
	ui->dispGV5->setText(QString::number(po->mn.genVar[5]));
	ui->dispGV6->setText(QString::number(po->mn.genVar[6]));
	ui->dispGV7->setText(QString::number(po->mn.genVar[7]));
	ui->dispGV8->setText(QString::number(po->mn.genVar[8]));
	ui->dispGV9->setText(QString::number(po->mn.genVar[9]));

	//Raw values - Execute0:
	//=====================

	ui->disp_mot_ang_0->setText(QString::number(*(po->ex[0].enc_ang)));
	ui->disp_mot_vel_0->setText(QString::number(*(po->ex[0].enc_ang_vel)));
	ui->disp_mot_acc_0->setText(QString::number(po->ex[0].mot_acc));
	ui->disp_strain_0->setText(QString::number(po->ex[0].strain));
	ui->disp_current_mot_0->setText(QString::number(po->ex[0].mot_current));

	ui->disp_joint_angle_0->setText(QString::number(*(po->ex[0].joint_ang)));
	ui->disp_joint_angle_vel_0->setText(QString::number(*(po->ex[0].joint_ang_vel)));
	ui->disp_joint_angle_from_mot_0->setText(QString::number(*(po->ex[0].joint_ang_from_mot)));
	ui->disp_mot_volt_0->setText(QString::number(po->ex[0].mot_volt));

	//Raw values - Execute1:
	//=====================
	ui->disp_mot_ang_1->setText(QString::number(*(po->ex[1].enc_ang)));
	ui->disp_mot_vel_1->setText(QString::number(*(po->ex[1].enc_ang_vel)));
	ui->disp_mot_acc_1->setText(QString::number(po->ex[1].mot_acc));
	ui->disp_strain_1->setText(QString::number(po->ex[1].strain));
	ui->disp_current_mot_1->setText(QString::number(po->ex[1].mot_current));

	ui->disp_joint_angle_1->setText(QString::number(*(po->ex[1].joint_ang)));
	ui->disp_joint_angle_vel_1->setText(QString::number(*(po->ex[1].joint_ang_vel)));
	ui->disp_joint_angle_from_mot_1->setText(QString::number(*(po->ex[1].joint_ang_from_mot)));
	ui->disp_mot_volt_1->setText(QString::number(po->ex[1].mot_volt));

	//Raw values - Regulate:
	//======================

	ui->disp_vb->setText(QString::number((float)po->re.vb/1000,'f',2));
	ui->disp_5v->setText(QString::number((float)po->re.v5/1000,'f',2));

	ui->disp_current_batt->setText(QString::number((float)po->re.current/1000,'f',2));
	ui->disp_temp->setText(QString::number(po->re.temp));
	ui->disp_button->setText(QString::number(po->re.button));

	ui->disp_re_stat->setText(QString::number(po->re.status));

	//Decoded values - Manage:
	//========================

	ui->disp_accx_d->setText(QString::number((float)po->mn.decoded.accel.x/1000,'f',2));
	ui->disp_accy_d->setText(QString::number((float)po->mn.decoded.accel.y/1000,'f',2));
	ui->disp_accz_d->setText(QString::number((float)po->mn.decoded.accel.z/1000,'f',2));

	ui->disp_gyrox_d->setText(QString::number(po->mn.decoded.gyro.x, 'i', 0));
	ui->disp_gyroy_d->setText(QString::number(po->mn.decoded.gyro.y, 'i', 0));
	ui->disp_gyroz_d->setText(QString::number(po->mn.decoded.gyro.z, 'i', 0));

	//Decoded values - Regulate:
	//==========================

	if(po->re.status & 0x80){ui->disp_button->setText("Pressed");}
	else{ui->disp_button->setText("Released");}

	//Notify the Status window that something changed
	if(lastStatus != po->re.status){emit statusChanged();}
	lastStatus = po->re.status;
}

//****************************************************************************
// Private slot(s):
//****************************************************************************
