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
#include "serialdriver.h"

#define REFRESH_PERIOD		100 //Port list refresh in ms
#define BT_CONF_DELAY		500
#define BT_FIELDS1			9
#define BT_FIELDS2			10

#define TCT_GUI				0
#define TCT_BWC				1

#define COM_OPEN_TRIES		3
#define COM_OPEN_DELAY_US	2000.0
#define BT_DELAY_MS			4000.0
#define COM_BAR_RES			15.0		// Increment resolution of the progress bar

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
	explicit W_Config(QWidget *parent = 0, QStringList *initFavoritePort = 0);
	~W_Config();

	DataSource getDataSourceStatus(void) {return dataSourceState;}

private slots:
	void refreshComList(bool forceRefresh = false, \
						bool keepCurrentSelection = false);
	QString getCOMnickname(const QSerialPortInfo *c);
	void btConfig();
	void btConfig2();
	void on_openComButton_clicked();
	void on_closeComButton_clicked();
	void on_pbLoadLogFile_clicked();
	void on_pbCloseLogFile_clicked();
	void on_pbBTmode_clicked();
	void on_pbBTgui_clicked();
	void on_pbBTbwc_clicked();
	void on_pbBTfactory_clicked();
	void on_pbBTreset_clicked();
	void on_pbBTfast_clicked();
	void on_checkBoxFavoritePort_clicked();

public slots:
	void on_openStatusUpdate(SerialPortStatus status, int nbTries);
	void refreshComTimeout();
	void refresh();
	void serialAboutToClose();
	void progressUpdate();

private:
	//Variables & Objects:
	Ui::W_Config *ui;
	DataSource dataSourceState;
	bool btDataMode;
	int btConfigField;
	int lastComPortCounts = 0;
	QString noPortString = "No Port";
	QStringList *favoritePort;
	int progressTries, progressCnt;
	int timerConnectedTo;

	QTimer *comPortRefreshTimer, *btConfigTimer, *openProgressTimer;

	//Function(s):
	void initCom(void);
	void defaultComOffUi(void);
	void enableBluetoothCommandButtons(void);
	void disableBluetoothCommandButtons(void);
	void closingPortRoutine(void);
	void toggleBtDataMode(bool forceDataMode = false);
	void favoritePortManagement(int ComPortCounts);

signals:
	void openCom(QString name, int tries, int delay, bool *success);
	void closeCom(void);
	void openReadingFile(bool *, FlexseaDevice **);
	void createLogKeypad(DataSource, FlexseaDevice *);
	void closeReadingFile(void);
	void updateDataSourceStatus(DataSource , FlexseaDevice *);
	void windowClosed(void);
	//void writeSerial(uint8_t bytes_to_send, uint8_t *serial_tx_data);
	void writeCommand(uint8_t numb, uint8_t *tx_data, uint8_t r_w);
	void write(uint8_t bytes_to_send, uint8_t *serial_tx_data);
	void flush();

};

#endif // W_CONFIG_H
