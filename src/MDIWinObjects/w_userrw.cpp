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
	[This file] userrw.h: User Read/Write Tool
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-11-22 | jfduval | New code, initial release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include <flexsea_system.h>
#include <flexsea_buffers.h>
#include <flexsea_comm.h>
#include "w_userrw.h"
#include "flexsea_generic.h"
#include "ui_w_userrw.h"
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
#include <flexsea_board.h>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_UserRW::W_UserRW(QWidget *parent, DynamicUserDataManager* userDataManager) :
	QWidget(parent),
	ui(new Ui::W_UserRW),
	userDataMan(userDataManager)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
}

W_UserRW::~W_UserRW()
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

void W_UserRW::init(void)
{
	//Populates Slave list:
	FlexSEA_Generic::populateSlaveComboBox(ui->comboBox_slave, SL_BASE_ALL, \
											SL_LEN_ALL);
	ui->comboBox_slave->setCurrentIndex(0);	//Execute 1 by default

	//Variables:
	active_slave_index = ui->comboBox_slave->currentIndex();
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);

	//All W boxes to 0:
	ui->w0->setText("0");
	ui->w1->setText("0");
	ui->w2->setText("0");
	ui->w3->setText("0");

	//All R boxes to 0:
	ui->r0->setText("0");
	ui->r1->setText("0");
	ui->r2->setText("0");
	ui->r3->setText("0");

	//Timer used to refresh the received data:
	refreshDelayTimer = new QTimer(this);
	connect(refreshDelayTimer,	&QTimer::timeout,
			this,				&W_UserRW::refreshDisplay);

	userDataMan->requestMetaData(active_slave);
	connect(userDataMan, &DynamicUserDataManager::newData, this, &W_UserRW::receiveNewData);
	connect(ui->planFieldFlagList, &QListWidget::itemChanged, this, &W_UserRW::handlePlanFlagListChange);

	QScrollBar* scrollBar = ui->execFieldFlagList->verticalScrollBar();
	ui->planFieldFlagList->setVerticalScrollBar(scrollBar);
	ui->userCustomStructLabelList->setVerticalScrollBar(scrollBar);
	ui->userCustomTypeList->setVerticalScrollBar(scrollBar);
	ui->userCustomStructValueList->setVerticalScrollBar(scrollBar);
}

void W_UserRW::handlePlanFlagListChange(QListWidgetItem* item)
{
	QListWidget *l = ui->planFieldFlagList;
	int index = l->row(item);
	userDataMan->setPlanFieldFlag(index, item->checkState() == Qt::Checked);
}

//Send a Write command:
void W_UserRW::writeUserData(uint8_t index)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

	//Refresh variable:
	user_data_1.w[0] = (int16_t)ui->w0->text().toInt();
	user_data_1.w[1] = (int16_t)ui->w1->text().toInt();
	user_data_1.w[2] = (int16_t)ui->w2->text().toInt();
	user_data_1.w[3] = (int16_t)ui->w3->text().toInt();

	//qDebug() << "Write user data" << index << ":" << user_data_1.w[index];

	//Prepare and send command:
	tx_cmd_data_user_w(TX_N_DEFAULT, index);
	pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, WRITE);
}

//Send a Read command:
void W_UserRW::readUserData(void)
{
	uint8_t info[2] = {PORT_USB, PORT_USB};
	uint16_t numb = 0;

	//Prepare and send command:
	tx_cmd_data_user_r(TX_N_DEFAULT);
	pack(P_AND_S_DEFAULT, active_slave, info, &numb, comm_str_usb);
	emit writeCommand(numb, comm_str_usb, READ);

	//Display will be refreshed in 75ms:
	refreshDelayTimer->start(75);
}


void W_UserRW::receiveNewData()
{
	QList<QString> newData;
	QList<QString> types;
	if(userDataMan->parseDynamicUserMetadata(&newData, &types))
	{
		ui->userCustomStructLabelList->clear();
		ui->userCustomTypeList->clear();
		ui->userCustomStructValueList->clear();

		for(int i = 0; i < newData.size(); i++)
		{
			ui->userCustomStructLabelList->addItem(newData.at(i));
			ui->userCustomTypeList->addItem(types.at(i));
			ui->userCustomStructValueList->addItem("-");
		}

		QList<bool> execFieldFlags;
		QList<bool> planFieldFlags;
		bool success = userDataMan->getExecFieldFlags(&execFieldFlags);
		success = success && userDataMan->getPlanFieldFlags(&planFieldFlags);

		QListWidget* execFlagListWidget = ui->execFieldFlagList;
		QListWidget* planFlagListWidget = ui->planFieldFlagList;
		execFlagListWidget->clear();
		planFlagListWidget->clear();

		for(int i = 0; i < newData.size(); i++)
		{
			QListWidgetItem* item1 = new QListWidgetItem("", nullptr);
			QListWidgetItem* item2 = new QListWidgetItem("", nullptr);

			bool checked1 = success && execFieldFlags.at(i);
			bool checked2 = success && planFieldFlags.at(i);

			item1->setFlags(item1->flags() | Qt::ItemIsUserCheckable); // set checkable flag
			item1->setCheckState(checked1 ? Qt::Checked : Qt::Unchecked); // AND initialize check state
			item1->setFlags(item1->flags() & (!Qt::ItemIsEnabled));

			item2->setFlags(item2->flags() | Qt::ItemIsUserCheckable); // set checkable flag
			item2->setCheckState(checked2 ? Qt::Checked : Qt::Unchecked); // AND initialize check state

			execFlagListWidget->addItem(item1);
			planFlagListWidget->addItem(item2);
		}

		execFlagListWidget->setFlow(QListView::TopToBottom);
		planFlagListWidget->setFlow(QListView::TopToBottom);
	}

	if(userDataMan->parseDynamicUserData(&newData))
	{
		int uiListLength = ui->userCustomStructValueList->count();
		static int x = 0;
		if(uiListLength != newData.size())
		{
			if(!x)
				qDebug() << "Metadata out of sync with incoming data";
			x++;
			x %= 33;
		}
		else
			for(int i = 0; i < newData.size(); i++)
			{
				ui->userCustomStructValueList->item(i)->setText(newData.at(i));
			}
	}
}

void W_UserRW::comStatusChanged(SerialPortStatus status,int nbTries)
{
	// Not use by this slot.
	(void)nbTries;

	if(status == PortOpeningSucceed)
		userDataMan->requestMetaData(active_slave);
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_UserRW::on_pushButton_w0_clicked()
{
	writeUserData(0);
}

void W_UserRW::on_pushButton_w1_clicked()
{
	writeUserData(1);
}

void W_UserRW::on_pushButton_w2_clicked()
{
	writeUserData(2);
}

void W_UserRW::on_pushButton_w3_clicked()
{
	writeUserData(3);
}

void W_UserRW::on_pushButton_refresh_clicked()
{
	readUserData();
	userDataMan->requestMetaData(active_slave);
}

void W_UserRW::setUserCustomRowHidden(int row, bool shouldHide)
{
	QListWidgetItem* i1 = ui->execFieldFlagList->item(row);
	QListWidgetItem* i2 = ui->planFieldFlagList->item(row);
	QListWidgetItem* i3 = ui->userCustomStructLabelList->item(row);
	QListWidgetItem* i4 = ui->userCustomStructValueList->item(row);
	QListWidgetItem* i5 = ui->userCustomTypeList->item(row);

	if(i1 && i2 && i3 && i4 && i5)
	{
		i1->setHidden(shouldHide);
		i2->setHidden(shouldHide);
		i3->setHidden(shouldHide);
		i4->setHidden(shouldHide);
		i5->setHidden(shouldHide);
	}
	else
	{
		qDebug() << "UserRW::setUserCustomRowHidden: Tried to hide a row for which at least one list could not find corresponding item";
	}
}

void W_UserRW::on_pushButton_hide_clicked()
{
	QListWidget* execFlagList = ui->execFieldFlagList;

	int count = execFlagList->count();
	if(!count) return;
	bool foundFirst = 0, shouldHide;
	for(int i = count-1; i >= 0; i--)
	{
		QListWidgetItem *item = execFlagList->item(i);
		if(item && item->checkState() == Qt::Unchecked)
		{
			if(!foundFirst)
			{
				shouldHide = !(item->isHidden());
				foundFirst = true;
			}

			setUserCustomRowHidden(i, shouldHide);
		}
	}

	if(!foundFirst) return;

	ui->pushButton_hide->setText(shouldHide ? "Show Unchecked" : "Hide Unchecked");
}


//Refreshes the User R values (display only):
void W_UserRW::refreshDisplay(void)
{
	refreshDelayTimer->stop();

	ui->r0->setText(QString::number(user_data_1.r[0]));
	ui->r1->setText(QString::number(user_data_1.r[1]));
	ui->r2->setText(QString::number(user_data_1.r[2]));
	ui->r3->setText(QString::number(user_data_1.r[3]));
}

void W_UserRW::on_comboBox_slave_currentIndexChanged(int index)
{
	active_slave_index = index;
	active_slave = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, active_slave_index);
}
