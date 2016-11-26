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
	[This file] w_gossip.h: Gossip View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

#ifndef W_GOSSIP_H
#define W_GOSSIP_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexsea_generic.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Gossip;
}

class W_Gossip : public QWidget, public Counter<W_Gossip>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Gossip(QWidget *parent = 0);
	~W_Gossip();

	//Function(s):

public slots:
	void refreshDisplayGossip(void);

signals:
	void windowClosed(void);

private:
	//Variables & Objects:
	Ui::W_Gossip *ui;
	int active_slave, active_slave_index;

	//Function(s):
	void init(void);
	void displayGossip(struct gossip_s *go);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_GOSSIP_H
