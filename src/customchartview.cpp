#include "customchartview.h"

CustomChartView::CustomChartView(QWidget *parent) : QWidget(parent)
{
    initialize();
}

void CustomChartView::updateChartData()
{
    repaint();
}

void CustomChartView::initialize(void)
{
    chartData=new ChartData();
    pickedCursor=0;
    cursorPositionRate1=0.4;
    cursorPositionRate2=0.6;
}

void CustomChartView::addDataProvider(DataProvider* provider)
{
	if(!provider) return;
	chartData->dataProviders.append(provider);
	chartData->streams.append(provider->getXMostRecent(NUM_POINTS_PER_WIDTH));
	chartData->rebuildLegendColorList();
}


void CustomChartView::drawAxis(QPainter &painter)
{
    QPen pen;
    pen.setWidth(2);
    pen.setBrush(QBrush(QColor(Qt::white)));
    painter.setPen(pen);
    painter.fillRect(0,0,width(),height(),QColor(45,45,55));
    painter.drawLine(chartData->translate(chartData->xMin,0,width(),height()),chartData->translate(chartData->xMax,0,width(),height()));
}

void CustomChartView::drawSingleFromDataStream(QPainter& painter, const int &streamIndex, const int &offset)
{
	QList<QuantData> *stream = &(chartData->streams[streamIndex]);
	int numPoints = stream->size();
	QPoint points[numPoints];

	for(int i = 0; i < numPoints; i++)
	{
		QuantData datapoint = stream->at(i);
		QUANT_DATA_T value = datapoint[offset];
		points[i] = chartData->translate(chartData->graphXArray[i], value, width(), height());
	}

	QPen pen;
	pen.setWidth(2);

	QColor brushColor = chartData->getCurrentDrawColor();
	chartData->moveToNextDrawColor();

	pen.setBrush(QBrush(brushColor));
	painter.setPen(pen);
	painter.drawPolyline(points,numPoints);
}

void CustomChartView::drawDoubleFromDataStream(QPainter& painter, const int &streamIndex, const int &offset1, const int &offset2)
{
	QList<QuantData> *stream = &(chartData->streams[streamIndex]);
	int numPoints = stream->size();
	QPoint points[numPoints*2];

	for(int i = 0; i < numPoints; i++)
	{
		QUANT_DATA_T value1 = stream->at(i)[offset1];
		QUANT_DATA_T value2 = stream->at(i)[offset2];
		points[i] =				chartData->translate(chartData->graphXArray[i], value1, width(), height());
		points[numPoints*2-1-i] = chartData->translate(chartData->graphXArray[i], value2, width(), height());
	}

	QColor brushColor = chartData->getCurrentDrawColor();
	chartData->moveToNextDrawColor();

	QPen pen;
	pen.setWidth(2);
	pen.setBrush(QBrush(brushColor));
	painter.setPen(pen);
	painter.setBrush(QBrush(brushColor.lighter(110)));
	painter.drawPolygon(points,numPoints*2);
}

void CustomChartView::drawDataProviderSeries(QPainter& painter, int streamIndex, int numFields)
{
	if(numFields < 1)
	{
		qDebug("CustomChartView::drawDataProviderSeries, dataprovider provided invalid # of fields.");
		return;
	}

	if(numFields % 2 == 0)
	{
		//logic for even number
		;
	}
	else
	{
		for(int i = numFields-1; i > 1; i--)
		{
			drawDoubleFromDataStream(painter, streamIndex, i, i-1);
		}

		drawSingleFromDataStream(painter, streamIndex, 0);
	}
}

void CustomChartView::drawCursor(QPainter &painter)
{
	//updateCursor();
    painter.setPen(QColor(Qt::white));
    painter.setBrush(QBrush(QColor(45,45,55)));
    int cursorPos1=width()*cursorPositionRate1;
    int cursorPos2=width()*cursorPositionRate2;
    painter.drawLine(cursorPos1,0,cursorPos1,height());
    painter.drawLine(cursorPos2,0,cursorPos2,height());

	double trackX, trackY;
	for(int i = 0; i < chartData->streams.size(); i++)
	{
		chartData->trackCursor(cursorPositionRate1,trackX,trackY, i, 0);
		QPoint point = chartData->translate(trackX,trackY,width(),height());
		painter.drawEllipse(point,3,3);

		chartData->trackCursor(cursorPositionRate2,trackX,trackY, i, 0);
		point = chartData->translate(trackX,trackY,width(),height());
		painter.drawEllipse(point,3,3);
	}
}

void CustomChartView::drawLegend(QPainter &painter)
{
	int size = chartData->dataProviders.size();
	//need to figure out the max width given the labels we have
	qreal maxWidth = 0;
	for(int i = 0; i < size; i++)
	{
		QRectF r = painter.boundingRect(QRectF(0, 10, 0, 10), Qt::TextSingleLine,
										chartData->labelAt(i));
		if(r.width() > maxWidth)
			maxWidth = r.width();
	}
	int widthOfChart = this->width();
	//draw a white rectangle under the legend
	painter.setPen(QColor(Qt::gray).darker(150));
	painter.setBrush(QBrush(QColor(Qt::white)));
	painter.drawRect(widthOfChart - maxWidth - 20 - legendRightOffset, legendTopOffset, maxWidth + 20, 10*size);
	for(int i = 0; i < size; i++)
	{
		QColor c = chartData->legendColorAt(i);
		painter.setBrush(QBrush(c));
		painter.drawRect(widthOfChart - maxWidth - 20 - legendRightOffset, legendTopOffset + 10*i, 10, 10);
		painter.drawText(widthOfChart - maxWidth - 5 - legendRightOffset, legendTopOffset + 10*(i+1),
						 chartData->labelAt(i));
	}
}

void CustomChartView::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()!=Qt::LeftButton)
    {
        return;
    }
    double cursorPositionRate=(double)e->pos().x()/width();
    cursorPositionRate=qMin(qMax(cursorPositionRate,0.01),0.99);
    if(pickedCursor==1)
    {
        cursorPositionRate1=cursorPositionRate;
        repaint();
    }
    else if(pickedCursor==2)
    {
        cursorPositionRate2=cursorPositionRate;
        repaint();
    }
}

void CustomChartView::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons()!=Qt::LeftButton)
    {
        return;
    }
    double cursorPositionRate=(double)e->pos().x()/width();
    if(qAbs(cursorPositionRate-cursorPositionRate1)<0.01)
    {
        pickedCursor=1;
        cursorPositionRate1=cursorPositionRate;
        setCursor(Qt::SizeHorCursor);
        repaint();
    }
    else if(qAbs(cursorPositionRate-cursorPositionRate2)<0.01)
    {
        pickedCursor=2;
        cursorPositionRate2=cursorPositionRate;
        setCursor(Qt::SizeHorCursor);
        repaint();
    }
    else
    {
        pickedCursor=0;
        setCursor(Qt::ArrowCursor);
    }
}

void CustomChartView::mouseReleaseEvent(QMouseEvent*)
{
    pickedCursor=0;
    setCursor(Qt::ArrowCursor);
}

void CustomChartView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    drawAxis(painter);

	chartData->setCurrentDrawColorIndex(0);
	for(int i = 0; i < chartData->dataProviders.size(); i++)
	{
		DataProvider* provider = chartData->dataProviders.at(i);
		if(provider)
		{
			chartData->streams[i] = provider->getXMostRecent(NUM_POINTS_PER_WIDTH);
			drawDataProviderSeries(painter, i, provider->getNumFields());
		}
	}
	drawCursor(painter);
	if(shouldDrawLegend)
		drawLegend(painter);
}
