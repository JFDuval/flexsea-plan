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
	[This file] w_config.h: Configuration Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef W_CONFIG_H
#define W_CONFIG_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexseaDevice.h"

#define REFRESH_PERIOD 750 //Port list refresh in ms

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui
{
	class W_Config;
}

typedef enum DataSource
{
	None,
	LiveCOM,
	LiveBluetooth,
	FromLogFile
}DataSource;

class W_Config : public QWidget, public Counter<W_Config>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Config(QWidget *parent = 0);
	~W_Config();

	DataSource getDataSourceStatus(void) {return dataSourceState;}


private slots:
	void getComList(void);
	void on_openComButton_clicked();
	void on_closeComButton_clicked();
	void on_pbLoadLogFile_clicked();
	void on_pbCloseLogFile_clicked();

public slots:
	void setComProgress(int val);

private:
	//Variables & Objects:
	Ui::W_Config *ui;
	DataSource dataSourceState;

	QTimer *comPortRefreshTimer;

	//Function(s):
	void initCom(void);
	void defaultComOffUi(void);

 signals:
	void openCom(QString name, int tries, int delay, bool *success);
	void closeCom(void);
	void openReadingFile(bool *, FlexseaDevice **);
	void createLogKeypad(DataSource, FlexseaDevice *);
	void closeReadingFile(void);
	void updateDataSourceStatus(DataSource , FlexseaDevice *);
	void windowClosed(void);

};

#endif // W_CONFIG_H
