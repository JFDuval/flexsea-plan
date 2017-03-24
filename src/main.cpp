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
	[This file] main: FlexSEA Plan project: GUI app to control FlexSEA slaves
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include <flexsea_global_structs.h>
#include <stdio.h>
#include "main.h"
#include <peripherals.h>
#include "mainwindow.h"
#include <QApplication>

//****************************************************************************
// Main
//****************************************************************************

int main(int argc, char *argv[])
{
	#ifdef UNIT_TEST

		(void)argc;
		(void)argv;

		printf("\nPlan-GUI compiled under 'Test' mode. Program will execute unit tests and exit. Use 'Release' or 'Debug' for normal operation.\n");
		printf("\nStart of tests...\n");
		printf("===================\n\n");
		fflush(stdout);

		//Unit tests, FlexSEA stack:
		printf(">>> flexsea_comm:\n\n");
		fflush(stdout);
		flexsea_comm_test();
		printf("\n>>> flexsea_system:\n\n");
		fflush(stdout);
		flexsea_system_test();
		printf("\n>>> flexsea_user:\n\n");
		fflush(stdout);
		flexsea_user_test();

		printf("\n===========================\n");
		printf("Tests completed. Exiting...\n");
		fflush(stdout);

		return 0;

	#else

		initLocalComm();
		initializeGlobalStructs();

		QApplication a(argc, argv);
		MainWindow w;
		w.show();
		return a.exec();

	#endif
}
