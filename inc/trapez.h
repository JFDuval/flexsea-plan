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
	[This file] trapez: trapezoidal trajectory generation
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef TRAPEZ_H_
#define TRAPEZ_H_

#ifdef __cplusplus
extern "C" {
#endif

//****************************************************************************
// Include(s):
//****************************************************************************

//****************************************************************************
// Public Function Prototype(s):
//****************************************************************************

long long trapez_gen_motion_1(long long pos_i, long long pos_f, long long spd_max, long long a);
long long trapez_get_pos(long long max_steps);

//****************************************************************************
// Definition(s):
//****************************************************************************

#define TRAPEZ_DT				0.001		//Trapezoidal timebase. Has to match hardware!
#define TRAPEZ_ONE_OVER_DT		1000
#define SPD_FACTOR				10000		//Scaling for integer
#define ACC_FACTOR				10000

#ifdef __cplusplus
}
#endif

#endif // TRAPEZ_H_
