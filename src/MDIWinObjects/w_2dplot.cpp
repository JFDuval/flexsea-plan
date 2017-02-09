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
	[This file] w_2dplot.h: 2D Plot window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	* 2016-09-12 | jfduval | Added Freeze/Release
	* 2016-12-1x | jfduval | Major refactoring
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_2dplot.h"
#include "w_execute.h"
#include "ui_w_2dplot.h"
#include <QApplication>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QDebug>
#include <QDateTime>
#include "flexsea_generic.h"
#include "main.h"

QT_CHARTS_USE_NAMESPACE

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_2DPlot::W_2DPlot(QWidget *parent,
				   QList<FlexseaDevice*> *devListInit) :
	QWidget(parent),
	ui(new Ui::W_2DPlot)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	devList = devListInit;
	initFlag = true;
	initPtr();
	initStats();
	initUserInput();
	initChart();
	useOpenGL(false);

	//Timers:
	timerRefreshDisplay = new QDateTime;
	timerRefreshData = new QDateTime;
}

W_2DPlot::~W_2DPlot()
{
	emit windowClosed();
	delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void W_2DPlot::receiveNewData(void)
{
	uint8_t item = 0;
	int val[6] = {0,0,0,0,0,0};

	dataRate = getRefreshRateData();

	//For every variable:
	for(item = 0; item < VAR_NUM; item++)
	{
		if(vtp[item].decode == false)
		{
			if((vtp[item].rawGenPtr) == nullptr)
			{
				val[item] = 0;
			}
			else
			{
				switch(vtp[item].format)
				{
					case FORMAT_32S:
						val[item] = (*(int32_t*)vtp[item].rawGenPtr);
						break;
					case FORMAT_32U:
						val[item] = (int)(*(uint32_t*)vtp[item].rawGenPtr);
						break;
					case FORMAT_16S:
						val[item] = (int)(*(int16_t*)vtp[item].rawGenPtr);
						break;
					case FORMAT_16U:
						val[item] = (int)(*(uint16_t*)vtp[item].rawGenPtr);
						break;
					case FORMAT_8S:
						val[item] = (int)(*(int8_t*)vtp[item].rawGenPtr);
						break;
					case FORMAT_8U:
						val[item] = (int)(*(uint8_t*)vtp[item].rawGenPtr);
						break;
					default:
						val[item] = 0;
						break;
				}
			}
		}
		else
		{
			if((vtp[item].decodedPtr) == nullptr)
			{
				val[item] = 0;
			}
			else
			{
				val[item] = (*vtp[item].decodedPtr);
			}
		}
	}

	saveNewPoints(val);
}

void W_2DPlot::refresh2DPlot(void)
{
	uint8_t index = 0;

	//Refresh Stat Bar:
	refreshStatBar(getRefreshRateDisplay(), dataRate);


	//For every variable:
	for(index = 0; index < VAR_NUM; index++)
	{
		if(vtp[index].used == false)
		{
			//This channel isn't used, we make it invisible
			qlsChart[index]->setVisible(false);
		}
		else
		{
			qlsChart[index]->setVisible(true);
		}
	}

	//And now update the display:
	if(plotFreezed == false)
	{
		// Apparently, using pointsVector is much faster (see documentation)
		qlsChart[0]->replace(vDataBuffer[0]);
		qlsChart[1]->replace(vDataBuffer[1]);
		qlsChart[2]->replace(vDataBuffer[2]);
		qlsChart[3]->replace(vDataBuffer[3]);
		qlsChart[4]->replace(vDataBuffer[4]);
		qlsChart[5]->replace(vDataBuffer[5]);

		computeGlobalMinMax();
		refreshStats();
		setChartAxisAutomatic();
	}
}

//We use this function for the trapeze setpoints. Refreshed at 100Hz.
void W_2DPlot::refreshControl(void)
{

}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_2DPlot::initPtr(void)
{
	lbT[0] = &ui->label_t1;
	lbT[1] = &ui->label_t2;
	lbT[2] = &ui->label_t3;
	lbT[3] = &ui->label_t4;
	lbT[4] = &ui->label_t5;
	lbT[5] = &ui->label_t6;

	cbVar[0] = &ui->cBoxvar1;
	cbVar[1] = &ui->cBoxvar2;
	cbVar[2] = &ui->cBoxvar3;
	cbVar[3] = &ui->cBoxvar4;
	cbVar[4] = &ui->cBoxvar5;
	cbVar[5] = &ui->cBoxvar6;

	cbVarSlave[0] = &ui->cBoxvar1slave;
	cbVarSlave[1] = &ui->cBoxvar2slave;
	cbVarSlave[2] = &ui->cBoxvar3slave;
	cbVarSlave[3] = &ui->cBoxvar4slave;
	cbVarSlave[4] = &ui->cBoxvar5slave;
	cbVarSlave[5] = &ui->cBoxvar6slave;

	ckbDecode[0] = &ui->checkBoxD1;
	ckbDecode[1] = &ui->checkBoxD2;
	ckbDecode[2] = &ui->checkBoxD3;
	ckbDecode[3] = &ui->checkBoxD4;
	ckbDecode[4] = &ui->checkBoxD5;
	ckbDecode[5] = &ui->checkBoxD6;

	lbMin[0] = &ui->label_1_min;
	lbMin[1] = &ui->label_2_min;
	lbMin[2] = &ui->label_3_min;
	lbMin[3] = &ui->label_4_min;
	lbMin[4] = &ui->label_5_min;
	lbMin[5] = &ui->label_6_min;

	lbMax[0] = &ui->label_1_max;
	lbMax[1] = &ui->label_2_max;
	lbMax[2] = &ui->label_3_max;
	lbMax[3] = &ui->label_4_max;
	lbMax[4] = &ui->label_5_max;
	lbMax[5] = &ui->label_6_max;

	lbAvg[0] = &ui->label_1_avg;
	lbAvg[1] = &ui->label_2_avg;
	lbAvg[2] = &ui->label_3_avg;
	lbAvg[3] = &ui->label_4_avg;
	lbAvg[4] = &ui->label_5_avg;
	lbAvg[5] = &ui->label_6_avg;
}

void W_2DPlot::initChart(void)
{
	vecLen = 0;

	for(int i = 0; i < VAR_NUM; ++i)
	{
		//Data series:
		qlsChart[i] = new QLineSeries();
		qlsChart[i]->append(0, 0);
	}

	initData();

	//Chart:
	chart = new QChart();
	chart->legend()->hide();

	for(int i = 0; i < VAR_NUM; ++i)
	{
		chart->addSeries(qlsChart[i]);
	}

	chart->createDefaultAxes();
	chart->axisX()->setRange(plot_xmin, plot_xmax);
	chart->axisY()->setRange(plot_ymin, plot_ymax);

	//Colors:
	chart->setTheme(QChart::ChartThemeDark);
	qlsChart[5]->setColor(Qt::red);  //Color[5] was ~= [0], too similar, now red

	//Update labels based on theme colors:
	QString msg;
	for(int u = 0; u < VAR_NUM; u++)
	{
		int r = 0, g = 0, b = 0;
		r = qlsChart[u]->color().red();
		g = qlsChart[u]->color().green();
		b = qlsChart[u]->color().blue();
		msg = "QLabel { background-color: black; color: rgb(" + \
				QString::number(r) + ',' + QString::number(g) + ','+ \
				QString::number(b) + ");}";
		(*lbT[u])->setStyleSheet(msg);
	}

	//Chart view:
	chartView = new QChartView(chart);
	ui->gridLayout_test->addWidget(chartView, 0,0);
	chartView->setRenderHint(QPainter::Antialiasing);
	chartView->setBaseSize(600,300);
	chartView->setMinimumSize(500,300);
	chartView->setMaximumSize(4000,2500);
	chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	chart->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
	QPixmapCache::setCacheLimit(100000);
	//Data indicator:
	connect(qlsChart[0], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler0(QPointF, bool)));
	connect(qlsChart[1], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler1(QPointF, bool)));
	connect(qlsChart[2], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler2(QPointF, bool)));
	connect(qlsChart[3], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler3(QPointF, bool)));
	connect(qlsChart[4], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler4(QPointF, bool)));
	connect(qlsChart[5], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler5(QPointF, bool)));
}

//Fills the fields and combo boxes:
void W_2DPlot::initUserInput(void)
{

	for(int i = 0; i < VAR_NUM; i++)
	{
		vtp[i].decodedPtr = nullptr;
		vtp[i].rawGenPtr = nullptr;
		vtp[i].used = false;
	}

	//Axis, limits, etc.:
	//=====================

	plot_len = INIT_PLOT_LEN;
	plot_xmin = INIT_PLOT_XMIN;
	plot_xmax = INIT_PLOT_XMAX;
	plot_ymin = INIT_PLOT_YMIN;
	plot_ymax = INIT_PLOT_YMAX;

	ui->radioButtonXA->setChecked(1);
	ui->radioButtonXM->setChecked(0);
	ui->radioButtonYA->setChecked(1);
	ui->radioButtonYM->setChecked(0);

	ui->lineEditXMin->setText(QString::number(plot_xmin));
	ui->lineEditXMax->setText(QString::number(plot_xmax));
	ui->lineEditYMin->setText(QString::number(plot_ymin));
	ui->lineEditYMax->setText(QString::number(plot_ymax));
	ui->lineEditXMin->setDisabled(true);
	ui->lineEditXMax->setDisabled(true);
	ui->lineEditYMin->setDisabled(true);
	ui->lineEditYMax->setDisabled(true);

	plotting_len = 0;

	ui->comboBoxMargin->clear();
	var_list_margin.clear();

	//Margin options:
	var_list_margin << "±2%" << "±5%" << "±10%" << "±10 ticks" << "±25 ticks" \
					<< "±100 ticks" << "±1000 ticks";
	for(int i = 0; i < var_list_margin.length(); i++)
	{
		ui->comboBoxMargin->addItem(var_list_margin.at(i));
	}

	//Plot Freeze:
	ui->pushButtonFreeze->setText("Freeze!");
	plotFreezed = false;

	//Data fields and variables:
	//==========================

	//Note: Color coded labels will be defined based on the chart.


	//Slave combo box:
	for(int i = 0; i < VAR_NUM; i++)
	{
		(*cbVarSlave[i])->blockSignals(true);
		(*cbVarSlave[i])->clear();
		(*cbVarSlave[i])->blockSignals(false);

		for(int ii = 0; ii < devList->length(); ++ii)
		{
			(*cbVarSlave[i])->addItem((*devList)[ii]->slaveName);
		}

		//Variable comboBoxes:
		saveCurrentSettings(i);  //Needed for the 1st var_list
	}

	//Decode Checkbox tooltips:
	QString ttip = "<html><head/><body><p>Plot data in physical units (instead \
					of ticks)</p></body></html>";

	for(int i = 0; i < VAR_NUM; i++)
	{
		updateVarList(i);
		(*ckbDecode[i])->setToolTip(ttip);
	}

	//By default, we track Slave 1:
	ui->checkBoxTrack->setChecked(true);


	pointsVisible = false;
	globalYmin = 0;
	globalYmin = 0;

	//Stats bar:
	ui->label_refreshRateData->setTextFormat(Qt::RichText);
	ui->label_refreshRateData->setText("-- Hz");
	ui->label_refreshRateDisplay->setTextFormat(Qt::RichText);
	ui->label_refreshRateDisplay->setText("-- Hz");
	ui->label_pointHovered->setTextFormat(Qt::RichText);
	ui->label_pointHovered->setText("<font color=#808080>(--, --)</font>");

	//No OpenGL by default:
	ui->checkBoxOpenGL->setChecked(false);
	ttip = "<html><head/><body><p>Experimental. Can be faster, but has limited \
			features (ex.: no visible points). To turn it off, close the \
			window...</p></body></html>";
	ui->checkBoxOpenGL->setToolTip(ttip);

	dataRate = 0;

	//Init flag:
	initFlag = false;
}

//Empties all the lists
void W_2DPlot::initData(void)
{
	vecLen = 0;
	for(int i = 0; i < VAR_NUM; i++)
	{
		vDataBuffer[i].clear();
		qlsChart[i]->replace(vDataBuffer[i]);
	}

	initStats();
}

//Updates 6 buffers, and compute stats (min/max/avg/...)
void W_2DPlot::saveNewPoints(int myDataPoints[6])
{
	QPointF min, max;
	QPointF temp;
	QPoint tempInt;
	long long avg = 0;

	if(vecLen <= plot_len-1)
	{
		//First VECLEN points: append
		//For each variable:
		for(int i = 0; i < VAR_NUM; i++)
		{
			vDataBuffer[i].append(QPointF(vecLen, myDataPoints[i]));

			min.setY(vDataBuffer[i].at(0).y());
			max.setY(vDataBuffer[i].at(0).y());
			avg = 0;
			for(int j = 0; j < vecLen; j++)
			{

				//Minimum:
				if(vDataBuffer[i].at(j).y() < min.y())
				{
					min.setY(vDataBuffer[i].at(j).y());
				}

				//Maximum:
				if(vDataBuffer[i].at(j).y() > max.y())
				{
					max.setY(vDataBuffer[i].at(j).y());
				}

				//Average - sum:
				tempInt = vDataBuffer[i].at(j).toPoint();
				avg += tempInt.y();

			}

			if(vecLen > 0)
			{
				//Average - result:
				avg = avg / vecLen;

				//Save:
				tempInt = min.toPoint();
				stats[i][STATS_MIN] = tempInt.y();
				tempInt = max.toPoint();
				stats[i][STATS_MAX] = tempInt.y();
				stats[i][STATS_AVG] = (int64_t) avg;
			}
		}

		vecLen++;
	}
	else
	{
		//For each variable:
		for(int i = 0; i < VAR_NUM; i++)
		{
			//For each point:
			min.setY(vDataBuffer[i].at(0).y());
			max.setY(vDataBuffer[i].at(0).y());
			avg = 0;
			int index = 0;
			for(int j = 1; j < plot_len+1; j++)
			{
				index = j-1;
				//qDebug() << "Index:" << index << "Plot len:" << plot_len;

				//Minimum:
				if(vDataBuffer[i].at(index).y() < min.y())
				{
					min.setY(vDataBuffer[i].at(index).y());
				}

				//Maximum:
				if(vDataBuffer[i].at(index).y() > max.y())
				{
					max.setY(vDataBuffer[i].at(index).y());
				}

				//Average - sum:
				tempInt = vDataBuffer[i].at(index).toPoint();
				avg += tempInt.y();

				//Shift by one position (all but last point):
				if(j < plot_len)
				{
					temp = vDataBuffer[i].at(j);
					vDataBuffer[i].replace(index, QPointF(index, temp.ry()));
				}
			}

			//Average - result:
			avg = avg / vecLen;

			//Save:
			tempInt = min.toPoint();
			stats[i][STATS_MIN] = tempInt.y();
			tempInt = max.toPoint();
			stats[i][STATS_MAX] = tempInt.y();
			stats[i][STATS_AVG] = (int64_t) avg;

			//Last (new):
			vDataBuffer[i].replace(plot_len-1, QPointF(plot_len-1, myDataPoints[i]));
		}
	}

	//qDebug() << "Test, qlsData length =" << qlsDataBuffer[0].count() << "Veclen =" << vecLen;
	plotting_len = vecLen;
}

//Get global min & max:
void W_2DPlot::computeGlobalMinMax(void)
{
	//Stats for all channels:

	if(allChannelUnused() == true)
	{
		globalYmin = -10;
		globalYmax = 10;
	}
	else
	{
		//First, we use the 1st used channel to initialize the global min/max:
		for(int i = 0; i < VAR_NUM; i++)
		{
			if(vtp[i].used == true)
			{
				globalYmin = stats[i][STATS_MIN];
				globalYmax = stats[i][STATS_MAX];
				break;
			}
		}

		//Now we can compare:
		for(int i = 0; i < VAR_NUM; i++)
		{
			//We only use the 'used' channels for the global min/max:
			if(vtp[i].used == true)
			{
				//Minimum:
				if(stats[i][STATS_MIN] < globalYmin)
				{
					globalYmin = stats[i][STATS_MIN];
				}

				//Maximum:
				if(stats[i][STATS_MAX] > globalYmax)
				{
					globalYmax = stats[i][STATS_MAX];
				}
			}
		}
	}
}

//Returns the rate at which it is called, in Hz
//Average of 8 values
float W_2DPlot::getRefreshRateDisplay(void)
{
	static qint64 oldTime = 0;
	qint64 newTime = 0, diffTime = 0;
	float t_s = 0.0, f = 0.0, avg = 0.0;
	static int counter = 0;
	static float fArray[8] = {0,0,0,0,0,0,0,0};

	//Actual frequency:
	newTime = timerRefreshDisplay->currentMSecsSinceEpoch();
	diffTime = newTime - oldTime;
	oldTime = newTime;
	t_s = diffTime/1000.0;
	f = 1/t_s;

	//Average:
	counter++;
	counter %=8;
	fArray[counter] = f;
	avg = 0;
	for(int i = 0; i < 8; i++)
	{
		avg += fArray[i];
	}
	avg = avg / 8;

	return avg;
}

//Returns the rate at which it is called, in Hz. Average of 8 values
//Different approach than for the Display function because 200Hz
//is fast for a ms timer.
float W_2DPlot::getRefreshRateData(void)
{
	static qint64 oldTime = 0;
	qint64 newTime = 0, diffTime = 0;
	float t_s = 0.0, avg = 0.0;
	static float f = 0.0;
	static int counter = 0;
	static float fArray[8] = {0,0,0,0,0,0,0,0};
	static int callCounter = 0;

	callCounter++;
	callCounter %= 10;
	if(!callCounter)
	{
		newTime = timerRefreshData->currentMSecsSinceEpoch();
		diffTime = newTime - oldTime;
		oldTime = newTime;

		t_s = diffTime/10/1000.0;
		f = 1/t_s;
	}

	//Average:
	counter++;
	counter %=4;
	fArray[counter] = f;
	avg = 0;
	for(int i = 0; i < 4; i++)
	{
		avg += fArray[i];
	}
	avg = avg / 4;

	return avg;
}

//Based on the current state of comboBoxes, saves the info in variables
void W_2DPlot::saveCurrentSettings(int item)
{
	//Slave: // TODO remove the & when the flexlist will be used.
	selectedDevList[item] = (*devList)[(*cbVarSlave[item])->currentIndex()];

	//Variable:
	varIndex[item] = (*cbVar[item])->currentIndex();

	//Decode:
	vtp[item].decode = (*ckbDecode[item])->isChecked();
}

//We use a bigger Y scale than the minimum span to make it clearer
void W_2DPlot::addMargins(int *ymin, int *ymax)
{
	switch(ui->comboBoxMargin->currentIndex())
	{
		case 0: //2%
			*ymin = (*ymin-(abs(*ymin)/50));
			*ymax = (*ymax+(abs(*ymax)/50));
			break;
		case 1: //5%
			*ymin = (*ymin-(abs(*ymin)/20));
			*ymax = (*ymax+(abs(*ymax)/20));
			break;
		case 2: //10%
			*ymin = (*ymin-(abs(*ymin)/10));
			*ymax = (*ymax+(abs(*ymax)/10));
			break;
		case 3: //10 ticks
			*ymin = (*ymin-10);
			*ymax = (*ymax+10);
			break;
		case 4: //25 ticks
			*ymin = (*ymin-25);
			*ymax = (*ymax+25);
			break;
		case 5: //100 ticks
			*ymin = (*ymin-100);
			*ymax = (*ymax+100);
			break;
		case 6: //1000 ticks
			*ymin = (*ymin-1000);
			*ymax = (*ymax+1000);
			break;
	}
}

//Manages the chart axis, including auto-scaling
//This function is called when the user makes a change.
//setChartAxisAutomatic() will be refreshed by a timer
void W_2DPlot::setChartAxis(void)
{
	static int lastPlotLen = 0;

	//X:
	if(ui->radioButtonXM->isChecked())
	{
		//Enable manual input fields:
		ui->lineEditXMin->setEnabled(true);
		ui->lineEditXMax->setEnabled(true);

		//Manual:
		//=======

		QString xMinText = ui->lineEditXMin->text();
		QString xMaxText = ui->lineEditXMax->text();

		//Empty field?

		if(xMinText.length() <= 0)
		{
			xMinText = "0";
		}

		if(xMaxText.length() <= 0)
		{
			xMaxText = "2";
		}

		//Convert to numbers:
		int tmpXmin = xMinText.toInt();
		int tmpXmax = xMaxText.toInt();

		//Numbers can't be negative:

		if(tmpXmin < 0)
		{
			tmpXmin = 0;
		}

		if(tmpXmax < 0)
		{
			tmpXmax = 0;
		}

		//Numbers can't be too long:

		if(tmpXmin > PLOT_BUF_LEN-5)
		{
			tmpXmin = PLOT_BUF_LEN-5;
		}

		if(tmpXmax > PLOT_BUF_LEN)
		{
			tmpXmax = PLOT_BUF_LEN;
		}

		//Max can't be smaller than min:

		if(tmpXmax <= tmpXmin)
		{
			tmpXmax = tmpXmin + 1;
		}

		//Update displays:
		ui->lineEditXMin->setText(QString::number(tmpXmin));
		ui->lineEditXMax->setText(QString::number(tmpXmax));

		//Save values:
		plot_xmin = tmpXmin;
		plot_xmax = tmpXmax;
		plot_len = 1+ plot_xmax - plot_xmin;

		if(plot_len < lastPlotLen)
		{
			initData();
		}
		lastPlotLen = plot_len;
	}
	else if(ui->radioButtonXA->isChecked())
	{
		//Disable manual input fields:
		ui->lineEditXMin->setEnabled(false);
		ui->lineEditXMax->setEnabled(false);

		//Auto scale axis
		plot_xmin = 0;
		//plot_xmax = plotting_len;	//ToDo remove?
		plot_xmax = vecLen;

		//Notify user of value used:
		ui->lineEditXMin->setText(QString::number(plot_xmin));;
		ui->lineEditXMax->setText(QString::number(plot_xmax));;
	}

	//Y:
	if(ui->radioButtonYM->isChecked())
	{
		//Enable manual input fields:
		ui->lineEditYMin->setEnabled(true);
		ui->lineEditYMax->setEnabled(true);

		//Manual
		plot_ymin = ui->lineEditYMin->text().toInt();
		plot_ymax = ui->lineEditYMax->text().toInt();
	}
	else if(ui->radioButtonYA->isChecked())
	{
		//Disable manual input fields:
		ui->lineEditYMin->setEnabled(false);
		ui->lineEditYMax->setEnabled(false);

		//Auto scale Y axis:
		//==================
		//(all done in Automatic function)
	}

	//Update chart:
	chart->axisX()->setRange(plot_xmin, plot_xmax);
	chart->axisY()->setRange(plot_ymin, plot_ymax);
}

void W_2DPlot::setChartAxisAutomatic(void)
{
	if(ui->radioButtonYA->isChecked())
	{
		plot_ymin = globalYmin;
		plot_ymax = globalYmax;

		// If the range is going to be equal, or overlapping,
		// We adjust the axis to some minimum height
		while(plot_ymin >= plot_ymax)
		{
			plot_ymin -= 5;
			plot_ymax += 5;
		}

		//Apply a margin:
		addMargins(&plot_ymin, &plot_ymax);

		//Update chart:
		//chart->axisX()->setRange(plot_xmin, plot_xmax);
		chart->axisY()->setRange(plot_ymin, plot_ymax);

		//Display values used:
		ui->lineEditYMin->setText(QString::number(plot_ymin));
		ui->lineEditYMax->setText(QString::number(plot_ymax));
	}
}

bool W_2DPlot::allChannelUnused(void)
{
	for(int i = 0; i < VAR_NUM; i++)
	{
		if(vtp[i].used == true)
		{
			return false;
		}
	}
	return true;
}

//Init stats: all 0
void W_2DPlot::initStats(void)
{
	memset(&stats, 0, sizeof stats);

	for(int i = 0; i < VAR_NUM; ++i)
	{
		(*lbMin[i])->setText(QString::number(0));
		(*lbMax[i])->setText(QString::number(0));
		(*lbAvg[i])->setText(QString::number(0));
	}
}

void W_2DPlot::refreshStats(void)
{
	for(int i = 0; i < VAR_NUM; ++i)
	{
		(*lbMin[i])->setText(QString::number(stats[i][STATS_MIN]));
		(*lbMax[i])->setText(QString::number(stats[i][STATS_MAX]));
		(*lbAvg[i])->setText(QString::number(stats[i][STATS_AVG]));
	}
}

//Displays the 2 refresh frequencies
void W_2DPlot::refreshStatBar(float fDisp, float fData)
{
	QString txt, num;

	num = QString::number(fDisp, 'f', 0);
	txt = "<font color=#808080>Display: " + num + " Hz </font>";
	ui->label_refreshRateDisplay->setText(txt);

	num = QString::number(fData, 'f', 0);
	txt = "<font color=#808080>Data: " + num + " Hz </font>";
	ui->label_refreshRateData->setText(txt);
}

//Each board type has a different variable list.
//ToDo: those lists should come from the w_BoardName files
void W_2DPlot::updateVarList(uint8_t item)
{
	//Fill the comboBox:
	(*cbVar[item])->clear();
	(*cbVar[item])->setToolTipDuration(350);

	QStringList headerList(selectedDevList[item]->getHeaderList());
	QStringList headerDecList(selectedDevList[item]->getHeaderDecList());

	(*cbVar[item])->addItem("**Unused**");
	(*cbVar[item])->setItemData(0, "Unused", Qt::ToolTipRole);
	for(int i = 2; i < headerList.length(); i++)
	{
		(*cbVar[item])->addItem(headerList[i]);
		(*cbVar[item])->setItemData(i - 1, headerDecList[i], Qt::ToolTipRole);
	}
}

//Assigns a pointer to the desired variable. This function is called whenever
//we change Slave or Variable. The runtime plotting function will then use the
//pointer.
void W_2DPlot::assignVariable(uint8_t item)
{

	struct std_variable varHandle = selectedDevList[item]->getSerializedVar(varIndex[item]+2);

	if(varIndex[item] == 0)
	{
		vtp[item].used = false;
		vtp[item].format = NULL_PTR;
		vtp[item].rawGenPtr = nullptr;
		vtp[item].decodedPtr = nullptr;
	}
	else
	{
		vtp[item].used = true;
		vtp[item].format = varHandle.format;
		vtp[item].rawGenPtr = varHandle.rawGenPtr;
		vtp[item].decodedPtr = varHandle.decodedPtr;
	}
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

//Generates a sine wave.
void W_2DPlot::genTestData(void)
{
	static double phase = 0.0;
	double res_f = 0;

	phase += PHASE_INCREMENT;
	res_f = A_GAIN * sin(phase + 0);

	//Save to global:
	myFakeData = (int32_t)res_f;
}

void W_2DPlot::on_radioButtonXA_clicked()
{
	setChartAxis();
}

void W_2DPlot::on_radioButtonXM_clicked()
{
	setChartAxis();
}

void W_2DPlot::on_radioButtonYA_clicked()
{
	setChartAxis();
}

void W_2DPlot::on_radioButtonYM_clicked()
{
	setChartAxis();
}

void W_2DPlot::on_lineEditXMin_returnPressed()
{
	setChartAxis();
}

void W_2DPlot::on_lineEditXMax_returnPressed()
{
	setChartAxis();
}

void W_2DPlot::on_lineEditYMin_returnPressed()
{
	setChartAxis();
}

void W_2DPlot::on_lineEditYMax_returnPressed()
{
	setChartAxis();
}

void W_2DPlot::on_pushButtonFreeze_clicked()
{
	//Toggle between Freeze and Release
	if(plotFreezed == true)
	{
		//We Release (normal operation)
		plotFreezed = false;
		ui->pushButtonFreeze->setText("Freeze!");
	}
	else
	{
		//We Freeze
		plotFreezed = true;
		ui->pushButtonFreeze->setText("Release");
	}
}

//Slave comboBoxes:

void W_2DPlot::on_cBoxvar1slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(0);

		if(ui->checkBoxTrack->isChecked() == false)
		{
			//If the Tracking box isn't checked we only change #1:
			updateVarList(0);
			assignVariable(0);
			//qDebug() << "Only change #1";
		}
		else
		{
			//Tracking, let's update all channels at once:
			qDebug() << "Change all slaves (tracking #1)";

			for(int item = 0; item < VAR_NUM; ++item)
			{
				(*cbVarSlave[item])->setCurrentIndex((*cbVarSlave[0])->currentIndex());
				updateVarList(item);
				assignVariable(item);
			}
		}
	}
}

void W_2DPlot::on_cBoxvar2slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(1);
		updateVarList(1);
		assignVariable(1);
	}
}

void W_2DPlot::on_cBoxvar3slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(2);
		updateVarList(2);
		assignVariable(2);
	}
}

void W_2DPlot::on_cBoxvar4slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(3);
		updateVarList(3);
		assignVariable(3);
	}
}

void W_2DPlot::on_cBoxvar5slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(4);
		updateVarList(4);
		assignVariable(4);
	}
}

void W_2DPlot::on_cBoxvar6slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(5);
		updateVarList(5);
		assignVariable(5);
	}
}

//Variable comboBoxes:

void W_2DPlot::on_cBoxvar1_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(0);
		assignVariable(0);
	}
}

void W_2DPlot::on_cBoxvar2_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(1);
		assignVariable(1);
	}
}

void W_2DPlot::on_cBoxvar3_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(2);
		assignVariable(2);
	}
}

void W_2DPlot::on_cBoxvar4_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(3);
		assignVariable(3);
	}
}

void W_2DPlot::on_cBoxvar5_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(4);
		assignVariable(4);
	}
}

void W_2DPlot::on_cBoxvar6_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(5);
		assignVariable(5);
	}
}

//Decode checkboxes:

void W_2DPlot::on_checkBoxD1_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(0);
		assignVariable(0);
	}
}

void W_2DPlot::on_checkBoxD2_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(1);
		assignVariable(1);
	}
}

void W_2DPlot::on_checkBoxD3_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(2);
		assignVariable(2);
	}
}

void W_2DPlot::on_checkBoxD4_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(3);
		assignVariable(3);
	}
}

void W_2DPlot::on_checkBoxD5_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(4);
		assignVariable(4);
	}
}

void W_2DPlot::on_checkBoxD6_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings(5);
		assignVariable(5);
	}
}

void W_2DPlot::on_pushButtonClear_clicked()
{
	qDebug() << "Clear plot!";

	initData();
}

//Reset the 2D plot to default setting
void W_2DPlot::on_pbReset_clicked()
{
	initUserInput();
	initStats();
}

//Sets all channels to the IMU:
void W_2DPlot::on_pbIMU_clicked()
{
	initUserInput();
	initStats();
	for(int item = 0; item < VAR_NUM; ++item)
	{
		(*cbVar[item])->setCurrentIndex(item + 1);
	}
}

void W_2DPlot::on_pbPoints_clicked()
{
	if(pointsVisible == false)
	{
		pointsVisible = true;
		ui->pbPoints->setText("Hide Points");
	}
	else
	{
		pointsVisible = false;
		ui->pbPoints->setText("Show Points");
	}

	for(int i = 0; i < VAR_NUM; i++)
	{
		qlsChart[i]->setPointsVisible(pointsVisible);
	}
}

//Point hovered, channel 0
void W_2DPlot::myHoverHandler0(QPointF pt, bool state)
{
	QPoint curP = QCursor::pos();	//Cursor position
	myHoverHandlerAll(0, pt, curP, state);
}

//Point hovered, channel 1
void W_2DPlot::myHoverHandler1(QPointF pt, bool state)
{
	QPoint curP = QCursor::pos();	//Cursor position
	myHoverHandlerAll(1, pt, curP, state);
}

//Point hovered, channel 2
void W_2DPlot::myHoverHandler2(QPointF pt, bool state)
{
	QPoint curP = QCursor::pos();	//Cursor position
	myHoverHandlerAll(2, pt, curP, state);
}

//Point hovered, channel 3
void W_2DPlot::myHoverHandler3(QPointF pt, bool state)
{
	QPoint curP = QCursor::pos();	//Cursor position
	myHoverHandlerAll(3, pt, curP, state);
}

//Point hovered, channel 4
void W_2DPlot::myHoverHandler4(QPointF pt, bool state)
{
	QPoint curP = QCursor::pos();	//Cursor position
	myHoverHandlerAll(4, pt, curP, state);
}

//Point hovered, channel 5
void W_2DPlot::myHoverHandler5(QPointF pt, bool state)
{
	QPoint curP = QCursor::pos();	//Cursor position
	myHoverHandlerAll(5, pt, curP, state);
}

//Point hovered, all channels (individual channels call this)
void W_2DPlot::myHoverHandlerAll(uint8_t ch, QPointF pt, QPoint cursor, \
									bool state)
{
	int x = 0, y = 0;
	QPoint xy = pt.toPoint();
	QString txt;
	(void)cursor;

	if(state == true)
	{
		//qDebug() << "Hovered over" << pt << "(ch" << ch << ") ";
		x = (int)xy.x();
		y = (int)xy.y();
		txt = "<font color=#808080>Point = CH" + QString::number(ch) + \
				"(" + QString::number(x) + ',' + QString::number(y) + ")</font>";

	}
	else
	{
		txt = "<font color=#808080>Point = CHx(--,--)</font>";
	}

	ui->label_pointHovered->setText(txt);
}

void W_2DPlot::on_checkBoxOpenGL_clicked(bool checked)
{
	useOpenGL(checked);
}

void W_2DPlot::useOpenGL(bool yesNo)
{
	for(int item = 0; item < VAR_NUM; ++item)
	{
		qlsChart[item]->setUseOpenGL(yesNo);
	}

	if(yesNo == true)
	{
		qDebug() << "OpenGL Enabled";
	}
	else
	{
		qDebug() << "OpenGL Disabled";
	}
}
