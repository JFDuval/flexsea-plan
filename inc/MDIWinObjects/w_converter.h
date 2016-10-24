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

#ifndef W_CONVERTER_H
#define W_CONVERTER_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Converter;
}

class W_Converter : public QWidget
{
    Q_OBJECT

public:
	//Constructor & Destructor:
    explicit W_Converter(QWidget *parent = 0);
    ~W_Converter();

signals:
    void windowClosed(void);

private slots:
    void on_lineEdituint32_returnPressed();
    void on_lineEdituint16_returnPressed();
    void on_lineEdituint32_textChanged(const QString &arg1);
    void on_lineEdituint16_textChanged(const QString &arg1);

private:
	//Variables & Objects:
    Ui::W_Converter *ui;
	
	//Function(s):
    void init(void);
    void zero16bitsBytes(void);
    void zero32bitsBytes(void);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#define MAX_16BITS  	65536
#define MIN_16BITS  	-32768
#define MAX_32BITS  	4294967295
#define MIN_32BITS  	-2147483648

#endif // W_CONVERTER_H
