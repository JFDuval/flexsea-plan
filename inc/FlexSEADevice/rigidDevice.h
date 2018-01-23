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
	[This file] rigidDevice: Rigid Device Class
	*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-04-18 | jfduval | New code
	*
****************************************************************************/

#ifndef RIGIDDEVICE_H
#define RIGIDDEVICE_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QList>
#include <QString>
#include <flexsea_global_structs.h>
#include <flexsea_user_structs.h>
#include "flexseaDevice.h"

//****************************************************************************
// Definition(s)
//****************************************************************************

//****************************************************************************
// Namespace & Class
//****************************************************************************

namespace Ui
{
	class RigidDevice;
}

class RigidDevice : public FlexseaDevice
{
public:
	// Constructor & Destructor
	explicit RigidDevice(void);
	explicit RigidDevice(rigid_s *devicePtr);
	virtual ~RigidDevice();

	// Interface implementation
	QStringList getHeader(void) {return header;}
	QStringList getHeaderUnit(void) {return headerUnitList;}
	QString getLastDataEntry(void);
	struct std_variable getSerializedVar(int parameter, int index);
	void appendSerializedStr(QStringList *splitLine);
	void decodeLastElement(void);
	void decodeAllElement(void);
	int length(void) {return riList.length();}
	void clear(void);
	void appendEmptyElement(void);

	// Class Function
	QString getStatusStr(int index);
	static void decode(struct rigid_s *riPtr);

	// Class Variable
	QList<struct rigid_s *> riList;
	QList<bool> ownershipList;

private:
	static QStringList header;
	static QStringList headerUnitList;
	int32_t enc_ang, enc_vel;
	int16_t joint_ang, joint_ang_vel, joint_ang_from_mot;
	int16_t ank_ang_deg, ank_ang_from_mot;
};

//****************************************************************************
// Definition(s)
//****************************************************************************

#endif // RIGIDDEVICE_H
