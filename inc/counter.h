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
	[This file] counter: Counter template
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-11-25 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef COUNTER_H_
#define COUNTER_H_

#include <QString>

// This template/ class implement a counter that can be use to count the number
// of instance of a specific class.
//
// To used it simply inherit from Counter to count objects don't forget to
// change Widget class for your own base class.
//
//		class Widget: public Counter<Widget> {
//			.....
//		};
// It is based on this article
// http://www.drdobbs.com/cpp/counting-objects-in-c/184403484

template<typename T>
class Counter {
public:
	Counter() { ++count; }
	Counter(const Counter&) { ++count; }
	~Counter() { --count; }

	static int howManyInstance(){ return count; }
	static int getMaxWindow(void) { return maxWindow;}
	static void setMaxWindow(int value) { maxWindow = value;}
	static QString getDescription(void) { return description;}
	static void setDescription(QString value) { description = value;}

private:
	static int count;
	static int maxWindow;
	static QString description;
};

template<typename T> int Counter<T>::count = 0;
template<typename T> int Counter<T>::maxWindow = 0;
template<typename T> QString Counter<T>::description = "";

#endif // COUNTER_H_
