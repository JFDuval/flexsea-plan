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
	[This file] w_planconfig.h: Configuration Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef W_PLANCONFIG_H
#define W_PLANCONFIG_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_PlanConfig;
}

class W_PlanConfig : public QWidget
{
    Q_OBJECT

public:
	//Constructor & Destructor:
    explicit W_PlanConfig(QWidget *parent = 0);
    ~W_PlanConfig();

private slots:
    void on_comPortComboBox_currentIndexChanged(int index);
    void on_openComButton_clicked();
    void on_closeComButton_clicked();
    void on_pushButtonRefresh_clicked();
    void on_pbOpenLog1_clicked();
    void on_pbOpenLog2_clicked();
    void on_pbOpenLog3_clicked();
    void on_pbOpenLog4_clicked();

public slots:
    void setComProgress(int val, int rst);
    void setLogFileStatus(QString status);

private:
	//Variables & Objects:
    Ui::W_PlanConfig *ui;
	QStringList comPortList;
	int flagComInitDone, flagManualEntry;
	
	//Function(s):
	void initCom(void);
	void initLog(void);
    void getComList(void);
    void defaultComOffUi(void); 

 signals:
    void openCom(QString name, int tries, int delay);
    void closeCom(void);
    void openLogFile(uint8_t);
};

#endif // W_PLANCONFIG_H
