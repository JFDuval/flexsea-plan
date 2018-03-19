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

#ifndef W_POCKET_H
#define W_POCKET_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "pocketDevice.h"
#include "define.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Pocket;
}

class W_Pocket : public QWidget, public Counter<W_Pocket>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Pocket(QWidget *parent = 0,
					  FlexseaDevice *currentLog = nullptr,
					  PocketDevice *deviceLogPtr = nullptr,
					  DisplayMode mode = DisplayLiveData,
					  QList<PocketDevice> *deviceListPtr = nullptr);
	~W_Pocket();

	//Function(s):

public slots:
	void refreshDisplay(void);
	void refreshDisplayLog(int index, FlexseaDevice * devPtr);
	void updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr);
	static int getCommandCode();

signals:
	void windowClosed(void);
	void statusChanged(void);

private:
	//Variables & Objects:
	Ui::W_Pocket *ui;

	DisplayMode displayMode;

	QList<PocketDevice> *deviceList;
	PocketDevice *deviceLog;

	//Function(s):
	void initLive(void);
	void initLog(FlexseaDevice *devPtr);
	void display(PocketDevice *devicePtr, int index);
	//void display(struct rigid_s *ri);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_POCKET_H
