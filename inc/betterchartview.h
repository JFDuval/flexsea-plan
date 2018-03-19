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
	[This file] betterchartview: An improve chart view with more functionnality
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2018-02-25 | sbelanger | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef BETTERCHARTVIEW_H
#define BETTERCHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>
#include <QLineSeries>

QT_CHARTS_USE_NAMESPACE

class BetterChartView : public QChartView
{
public:
	enum  CursorMode {
		NoCursor = 0x0,
		CursorUnlinked= 0x1,
		CursorLinked = 0x2,
	};

	BetterChartView(QChart *chart, QWidget *parent = 0);
	void addViewSeries(QLineSeries *serie);
	void setLineSeriesIndex(int index);
	void setCursorMode(CursorMode mode);
	CursorMode getCursorMode();

protected:
	void drawForeground(QPainter* painter, const QRectF &rect);
	bool viewportEvent(QEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent* event);
	void keyPressEvent(QKeyEvent *event);

private:
	bool m_isTouching;
	QPen cursorPen;
	QVector<QString> cursorName;
	QVector<QPointF> cursorPos;
	QVector<QLineSeries *> lineSeries;
	CursorMode cursorMode = NoCursor;
	int cursorPosXDiff;
	int lSIndex = 0;
	int activeCursor;
	int clickableRadius;
};

#endif // BETTERCHARTVIEW_H
