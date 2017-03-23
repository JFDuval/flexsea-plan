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
	[This file] w_converter.h: Converter Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_converter.h"
#include "ui_w_converter.h"
//#include "main.h"
#include <flexsea.h>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Converter::W_Converter(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_Converter)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
}

W_Converter::~W_Converter()
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

void W_Converter::init(void)
{
	//All values at 0:
	ui->label_16b0->setText("0");
	ui->label_16b1->setText("0");
	ui->label_32b0->setText("0");
	ui->label_32b1->setText("0");
	ui->label_32b2->setText("0");
	ui->label_32b3->setText("0");
	ui->lineEdituint16->setText("0");
	ui->lineEdituint32->setText("0");
}

void W_Converter::zero16bitsBytes(void)
{
	//All values at 0:
	ui->label_16b0->setText("0");
	ui->label_16b1->setText("0");
}

void W_Converter::zero32bitsBytes(void)
{
	//All values at 0:
	ui->label_32b0->setText("0");
	ui->label_32b1->setText("0");
	ui->label_32b2->setText("0");
	ui->label_32b3->setText("0");
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

void W_Converter::on_lineEdituint32_returnPressed()
{
	int val_entered = ui->lineEdituint32->text().toInt();
	uint16_t index = 0;
	uint8_t buf[4] = {0,0,0,0};

	//Convert and display
	SPLIT_32((uint32_t)val_entered, buf, &index);
	ui->label_32b0->setText(QString::number(buf[0]));
	ui->label_32b1->setText(QString::number(buf[1]));
	ui->label_32b2->setText(QString::number(buf[2]));
	ui->label_32b3->setText(QString::number(buf[3]));
}

void W_Converter::on_lineEdituint16_returnPressed()
{
	int val_entered = ui->lineEdituint16->text().toInt();
	uint16_t index = 0;
	uint8_t buf[2] = {0,0};

	//Not in range?
	if((val_entered < MIN_16BITS) || (val_entered > MAX_16BITS))
	{
		ui->label_16b0->setText("-");
		ui->label_16b1->setText("-");
		return;
	}

	//Convert and display
	SPLIT_16((uint16_t)val_entered, buf, &index);
	ui->label_16b0->setText(QString::number(buf[0]));
	ui->label_16b1->setText(QString::number(buf[1]));
}

void W_Converter::on_lineEdituint32_textChanged(const QString &arg1)
{
	(void)arg1;
	zero32bitsBytes();
}

void W_Converter::on_lineEdituint16_textChanged(const QString &arg1)
{
	(void)arg1;
	zero16bitsBytes();
}
