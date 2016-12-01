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

#ifndef W_ANYCOMMAND_H
#define W_ANYCOMMAND_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexsea_generic.h"
#include <QString>
#include <QList>

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_AnyCommand;
}

class W_AnyCommand : public QWidget, public Counter<W_AnyCommand>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_AnyCommand(QWidget *parent = 0);
	~W_AnyCommand();

private slots:
	void on_comboBoxCmd_currentIndexChanged(int index);

signals:
	void windowClosed(void);

private:
	//Variables & Objects:
	Ui::W_AnyCommand *ui;
	QStringList var_list_commands;
	QList<uint8_t> var_list_codes;

	//Function(s):
	void init(void);
};

#endif // W_ANYCOMMAND_H
