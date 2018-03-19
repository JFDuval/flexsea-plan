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

#include "betterchartview.h"
#include <QtGui/QMouseEvent>
#include <QDebug>
#include <QLineSeries>
#include <qabstractaxis.h>
#include <QValueAxis>

BetterChartView::BetterChartView(QChart *chart, QWidget *parent) :
	QChartView(chart, parent),
	m_isTouching(false)
{
	setRubberBand(QChartView::RectangleRubberBand);


	cursorPen = QPen(QColor(220, 220, 220),2, Qt::SolidLine, Qt::FlatCap);
	// todo fin a way to put the cursor at the center of the graph
	// Cursor #1
	cursorPos.append(QPointF(100,100));
	cursorName.append("a");
	// Cursor #2
	cursorPos.append(QPointF(200,100));
	cursorName.append("b");

	cursorPosXDiff = cursorPos[1].x() - cursorPos[0].x();

	activeCursor = -1;
	clickableRadius = 12;
}

void BetterChartView::drawForeground(QPainter* painter, const QRectF &rect)
{
	QChartView::drawForeground(painter, rect);

	if(cursorMode == NoCursor)
	{
		return;
	}

	QPointF cursorValue;
	QVector<QPointF> waveformPointPosition , waveformPointValue;
	QVector<QPointF> lineSeriesVector;
	QValueAxis *axisx = ((QValueAxis*)chart()->axisX());
	QValueAxis *axisy = ((QValueAxis*)chart()->axisY());
	QRectF axisRect = QRectF(chart()->mapToPosition(QPointF(axisx->min(),axisy->max())),
						   chart()->mapToPosition(QPointF(axisx->max(),axisy->min())));

	const int cursorWidth = 13;
	const int labelRadius = 8;
	painter->setBrush(Qt::NoBrush);

	//******************
	//* Cursor
	//******************
	for(int i = 0; i < cursorPos.length(); i++)
	{
		// Paint vertical line
		cursorPen.setColor(lineSeries[lSIndex]->color());
		painter->setPen(cursorPen);
		painter->drawLine(QPointF(cursorPos[i].x(),
								  axisRect.top()),
						  QPointF(cursorPos[i].x(),
								  axisRect.bottom()));

		// Paint horizontal line
		if(lineSeries[lSIndex] != NULL &&
		   lineSeries[lSIndex]->pointsVector().isEmpty() == false)
		{
			cursorValue = chart()->mapToValue(cursorPos[i]);
			lineSeriesVector = lineSeries[lSIndex]->pointsVector();

			// Find the x point on the wavform coresponding to
			// the x cursor position
			waveformPointValue.append(QPointF(0,0));
			for(int j = 1; j < lineSeriesVector.length(); j++)
			{
				if(lineSeriesVector[j].x() >= cursorValue.x())
				{
					waveformPointValue.replace(i,lineSeriesVector[j-1]);
					break;
				}
			}

			// Compute the position of the point and draw the horizontal line
			waveformPointPosition.append(chart()->mapToPosition(waveformPointValue[i]));
			painter->drawLine(QPointF(cursorPos[i].x() + cursorWidth,
									  waveformPointPosition[i].y()),
							  QPointF(cursorPos[i].x() - cursorWidth,
									  waveformPointPosition[i].y()));
		}
		else
		{
			waveformPointPosition.append(cursorPos[i]);
			waveformPointValue.append(chart()->mapToValue(cursorPos[i]));
			waveformPointValue[i] = chart()->mapToValue(cursorPos[i]);
			painter->drawLine(QPointF(cursorPos[i].x() + cursorWidth,
									  cursorPos[i].y()),
							  QPointF(cursorPos[i].x() - cursorWidth,
									  cursorPos[i].y()));
		}

		// Paint cursor label
		painter->setPen(QPen(Qt::black));
		painter->setBrush(QBrush(QColor(204, 86, 0)));
		painter->drawEllipse(QPointF(cursorPos[i].x(), axisRect.top() - 12),
							 labelRadius, labelRadius);
		painter->setBrush(Qt::NoBrush);
		painter->drawText(QPointF(cursorPos[i].x() - 4, axisRect.top() - 7), cursorName[i]);
	}

	//******************
	//* Statistics
	//******************
	const int boxWidth = 200;
	const int boxHeight = 52;
	const int lineOffset = 16;
	QPointF boxOrigin = QPointF(this->geometry().topRight().x() - boxWidth - 30,
								this->geometry().topRight().y() + 30);
	// Paint the box
	painter->setPen(QPen(QColor(220, 220, 220)));
	painter->setBrush(QBrush(QColor("#272932")));
	QRectF rectangle(boxOrigin.x(), boxOrigin.y(),
					 boxWidth, boxHeight + (cursorMode == CursorLinked?lineOffset:0));
	painter->drawRoundedRect(rectangle, 5.0, 5.0);
	painter->setBrush(Qt::NoBrush);

	// Paint Cursor value
	int i;
	for(i = 0; i < cursorPos.length(); i++)
	{
		// Paint cursor circle label
		painter->setPen(QPen(Qt::black));
		painter->setBrush(QBrush(QColor(204, 86, 0)));
		painter->drawEllipse(QPointF(boxOrigin.x() + 20,
									 boxOrigin.y() + 10 + lineOffset*i),
							 labelRadius, labelRadius);
		painter->setBrush(Qt::NoBrush);
		painter->drawText(QPointF(boxOrigin.x() + 16,
								  boxOrigin.y() + 15 + lineOffset*i), cursorName[i]);

		// Text cursor xy value
		painter->setPen(QPen(lineSeries[lSIndex]->color()));
		QString text;
		text.sprintf(" %10.2f     %10.2f", waveformPointValue[i].x(), waveformPointValue[i].y());
		painter->drawText(QPointF(boxOrigin.x() + 30,
								  boxOrigin.y() + 15 + lineOffset*i), text);
	}

	// Paint the differences
	QString text;
	text.sprintf("\u0394%10.2f     \u0394%10.2f",
				 waveformPointValue[1].x() - waveformPointValue[0].x(),
				 waveformPointValue[1].y() - waveformPointValue[0].y());
	painter->drawText(QPointF(boxOrigin.x() + 25,
							  boxOrigin.y() + 15 + lineOffset*i), text);
	i++;
	// Paint the cursor linked if needed
	if(cursorMode == CursorLinked)
	{
		painter->drawText(QPointF(boxOrigin.x() + 25 + 30,
								  boxOrigin.y() + 15 + lineOffset*i),
						  "Cursors Linked");
	}
}

bool BetterChartView::viewportEvent(QEvent *event)
{
	if (event->type() == QEvent::TouchBegin) {
		// By default touch events are converted to mouse events. So
		// after this event we will get a mouse event also but we want
		// to handle touch events as gestures only. So we need this safeguard
		// to block mouse events that are actually generated from touch.
		m_isTouching = true;

		// Turn off animations when handling gestures they
		// will only slow us down.
		chart()->setAnimationOptions(QChart::NoAnimation);
	}
	return QChartView::viewportEvent(event);
}

void BetterChartView::mousePressEvent(QMouseEvent *event)
{
	if((event->buttons() & Qt::MiddleButton) != 0)
	{
		chart()->zoomReset();
		return;
	}

	if((event->buttons() & Qt::RightButton) != 0)
	{
		return;
	}

	if(cursorMode != NoCursor &&
	   (event->buttons() & Qt::LeftButton) != 0 &&
	   activeCursor <= -1)
	{
		QPointF p = event->pos();
		QPointF cp;
		static int distThresh = clickableRadius * clickableRadius;
		float xdist, dist;

		for(int i = 0; i < cursorPos.length(); i++)
		{
			cp = cursorPos[i];

			xdist = (p.x() - cp.x());

			dist = xdist * xdist;

			if( dist < distThresh )
			{
				activeCursor = i;
				cursorPosXDiff = cursorPos[1].x() - cursorPos[0].x();
				return;
			}
		}
	}

	if (m_isTouching)
		return;

	QChartView::mousePressEvent(event);
}

void BetterChartView::mouseDoubleClickEvent(QMouseEvent *event )
{
	if ( event->button() == Qt::LeftButton )
	{
		chart()->zoomReset();
	}
	QChartView::mouseDoubleClickEvent(event);
}

void BetterChartView::mouseMoveEvent(QMouseEvent *event)
{
	static QPointF lastPos = event->pos();

	if((event->buttons() & Qt::RightButton) != 0)
	{
		chart()->scroll(-(event->pos().x() - lastPos.x()), event->pos().y() - lastPos.y());
		lastPos = event->pos();
		return;
	}

	lastPos = event->pos();

	if(cursorMode != NoCursor &&
	   activeCursor > -1)
	{
		//Normal points
		QPointF mp = event->pos();
		QPointF cp = cursorPos[activeCursor];

		QValueAxis *axisx = ((QValueAxis*)chart()->axisX());
		QValueAxis *axisy = ((QValueAxis*)chart()->axisY());
		QRectF axisRect = QRectF(chart()->mapToPosition(QPointF(axisx->min(),axisy->max())),
							   chart()->mapToPosition(QPointF(axisx->max(),axisy->min())));

		float dist = (mp - cp).manhattanLength();

		static float distThresh = 0.25*clickableRadius;

		if(dist > distThresh)
		{
			cursorPos[activeCursor] = event->pos();

			// Limit to stay within the graph
			if(cursorPos[activeCursor].x() > axisRect.right())
				cursorPos[activeCursor].setX(axisRect.right());
			else if(cursorPos[activeCursor].x() < axisRect.left())
				cursorPos[activeCursor].setX(axisRect.left());

			if(cursorPos[activeCursor].y() < axisRect.top())
				cursorPos[activeCursor].setY(axisRect.top());
			else if(cursorPos[activeCursor].y() > axisRect.bottom())
				cursorPos[activeCursor].setY(axisRect.bottom());

			// Handle linked cursor mode
			if(activeCursor == 0 && cursorMode == CursorLinked)
			{
				cursorPos[1] = event->pos();
				cursorPos[1].setX(cursorPos[1].x() + cursorPosXDiff);

				// Limit to stay within the graph
				if(cursorPos[1].x() > axisRect.right())
					cursorPos[1].setX(axisRect.right());
				else if(cursorPos[1].x() < axisRect.left())
					cursorPos[1].setX(axisRect.left());

				if(cursorPos[1].y() < axisRect.top())
					cursorPos[1].setY(axisRect.top());
				else if(cursorPos[1].y() > axisRect.bottom())
					cursorPos[1].setY(axisRect.bottom());
			}

			// Redraw the graph
			this->update();
			chart()->update();
		}
		return;
	}

	if (m_isTouching)
		return;

	QChartView::mouseMoveEvent(event);
}

void BetterChartView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_isTouching)
		m_isTouching = false;

	if(activeCursor > -1)
		activeCursor = -1;

	if((event->button() == Qt::RightButton))
	{
		return;
	}

	QChartView::mouseReleaseEvent(event);
}

void BetterChartView::wheelEvent(QWheelEvent* event)
{
	if(event->angleDelta().y() > 0)
	{
		chart()->zoomIn();
	}
	else if(event->angleDelta().y() < 0)
	{
		chart()->zoomOut();
	}

}

void BetterChartView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Plus:
		chart()->zoomIn();
		break;
	case Qt::Key_Minus:
		chart()->zoomOut();
		break;
	case Qt::Key_Left:
		chart()->scroll(-10, 0);
		break;
	case Qt::Key_Right:
		chart()->scroll(10, 0);
		break;
	case Qt::Key_Up:
		chart()->scroll(0, 10);
		break;
	case Qt::Key_Down:
		chart()->scroll(0, -10);
		break;
	default:
		QGraphicsView::keyPressEvent(event);
		break;
	}

}

void BetterChartView::addViewSeries(QLineSeries *serie)
{
	lineSeries.append(serie);
}

void BetterChartView::setLineSeriesIndex(int index)
{
	if(index < lineSeries.length())
	{
		lSIndex = index;
		// Redraw the graph
		this->update();
		chart()->update();
	}
}

void BetterChartView::setCursorMode(CursorMode mode)
{
	cursorMode = mode;
	this->update();
	chart()->update();
}

BetterChartView::CursorMode BetterChartView::getCursorMode()
{
	return cursorMode;
}
