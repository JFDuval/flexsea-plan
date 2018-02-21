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
	[This file] w_rigid: Rigid View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-04-18 | jfduval | New code
	*
****************************************************************************/

#ifndef W_RIGID_H
#define W_RIGID_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "rigidDevice.h"
#include "define.h"

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
class W_Rigid;
}

class W_Rigid : public QWidget, public Counter<W_Rigid>
{
	Q_OBJECT

public:
	//Constructor & Destructor:
	explicit W_Rigid(QWidget *parent = 0,
					  FlexseaDevice *currentLog = nullptr,
					  RigidDevice *deviceLogPtr = nullptr,
					  DisplayMode mode = DisplayLiveData,
					  QList<RigidDevice> *deviceListPtr = nullptr);
	~W_Rigid();

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
	Ui::W_Rigid *ui;

	DisplayMode displayMode;

	QList<RigidDevice> *deviceList;
	RigidDevice *deviceLog;

	//Function(s):
	void initLive(void);
	void initLog(FlexseaDevice *devPtr);
	void display(RigidDevice *devicePtr, int index);
	//void display(struct rigid_s *ri);
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // W_RIGID_H
