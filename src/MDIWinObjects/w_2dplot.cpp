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

W_2DPlot::W_2DPlot(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_2DPlot)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

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
		else
		{
			val[item] = (*vtp[item].decodedPtr);
		}
	}

	saveNewPoints(val);
}

void W_2DPlot::refresh2DPlot(void)
{
	//sub sample the clock we receive, which is at 1kHz. We want 33Hz
	static double ticks = 0;
	const double REFRESH_PERIOD = 1000.0 / 33.0;
	ticks += 1.0;
	if(ticks > REFRESH_PERIOD)
	{
		ticks -= REFRESH_PERIOD;
	}
	else
	{
		return;
	}

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
	nullVar32s = 0;
	nullVar16u = 0;

	for(int i = 0; i < VAR_NUM; i++)
	{
		vtp[i].decodedPtr = &nullVar32s;
		vtp[i].rawGenPtr = &nullVar32s;
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
		FlexSEA_Generic::populateSlaveComboBox((*cbVarSlave[i]), SL_BASE_ALL, \
												SL_LEN_ALL);
	}

	//Variable comboBoxes:
	saveCurrentSettings();  //Needed for the 1st var_list

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

	saveCurrentSettings();

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
void W_2DPlot::saveCurrentSettings(void)
{
	for(int i = 0; i < VAR_NUM; i++)
	{
		//Slave:
		slaveIndex[i] = (*cbVarSlave[i])->currentIndex();
		slaveAddr[i] = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, slaveIndex[i]);
		slaveBType[i] = FlexSEA_Generic::getSlaveBoardType(SL_BASE_ALL, \
														   slaveIndex[i]);
		//Variable:
		varIndex[i] = (*cbVar[i])->currentIndex();

		//Decode:
		vtp[i].decode = (*ckbDecode[i])->isChecked();
	}
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
	QStringList var_list, toolTipList;

	uint8_t bType = slaveBType[item];

	//Build the string:
	switch(bType)
	{
		case FLEXSEA_PLAN_BASE:
			var_list << "****";
			toolTipList << "";
			break;
		case FLEXSEA_MANAGE_BASE:
			var_list << "**Unused**" << "Accel X" << "Accel Y" << "Accel Z" \
					<< "Gyro X" << "Gyro Y" << "Gyro Z" << "Pushbutton" \
					<< "Digital Inputs" << "Analog[0]" << "Analog[1]" \
					<< "Analog[2]" << "Analog[3]" << "Analog[4]" \
					<< "Analog[5]" << "Analog[6]" << "Analog[7]" << "Status";
			toolTipList << "Unused" << "Decoded: mg" << "Decoded: mg" << "Decoded: mg" \
						<< "Decoded: deg/s" << "Decoded: deg/s" << "Decoded: deg/s" << "Raw Value Only" \
						<< "Raw Value Only" << "Decoded: mV" << "Decoded: mV" \
						<< "Decoded: mV" << "Decoded: mV" << "Decoded: mV" \
						<< "Decoded: mV" << "Decoded: mV" << "Decoded: mV" << "Raw Value Only";
			break;
		case FLEXSEA_EXECUTE_BASE:
			var_list << "**Unused**" << "Accel X" << "Accel Y" << "Accel Z" \
					<< "Gyro X" << "Gyro Y" << "Gyro Z" << "Encoder Display" \
					<< "Encoder Control" << "Encoder Commutation" \
					<< "Motor current" << "Analog[0]" << "Analog[1]" \
					<< "Strain" << "Battery Voltage" << "Int. voltage" \
					<< "Temperature" << "Status 1" << "Status 2" \
					<< "Setpoint (square)" << "Setpoint (trapezoidal)" \
					<< "Fake Data";
			toolTipList << "Unused" << "Decoded: mg" << "Decoded: mg" << "Decoded: mg" \
					<< "Decoded: deg/s" << "Decoded: deg/s" << "Decoded: deg/s" << "Raw Value Only" \
					<< "Raw value only" << "Raw value only" \
					<< "Decoded: mA" << "Decoded: mV" << "Decoded: mV" \
					<< "Decoded: ±100%" << "Decoded: mV" << "Decoded: mV" \
					<< "Decoded: 10x C" << "Raw value only" << "Raw value only" \
					<< "Raw value only" << "Raw value only" \
					<< "Raw value only";
			break;
		case FLEXSEA_BATTERY_BASE:
			var_list << "**Unused**" << "Battery Voltage" << "Battery Current" \
					<< "Power" << "Temperature" << "Pushbutton" << "Status";
			toolTipList << "Unused" << "Decoded: mV" << "Decoded: mA" \
					<< "Decoded: mW" << "Decoded: 10x C" << "Raw Values Only" \
					<< "Raw Values Only";
			break;
		case FLEXSEA_STRAIN_BASE:
			var_list << "**Unused**" << "Strain ch[1]" << "Strain ch[2]" \
					<< "Strain ch[3]" << "Strain ch[4]" << "Strain ch[5]" \
					<< "Strain ch[6]";
			toolTipList << "Unused" << "Decoded: ±100%" << "Decoded: ±100%" \
					<< "Decoded: ±100%" << "Decoded: ±100%" << "Decoded: ±100%" \
					<< "Decoded: ±100%";
			break;
		case FLEXSEA_GOSSIP_BASE:
			var_list << "**Unused**" << "Accel X" << "Accel Y" << "Accel Z" \
					<< "Gyro X" << "Gyro Y" << "Gyro Z" << "Magneto X" \
					<< "Magneto Y" << "Magneto Z" << "IO[1]" << "IO[2]" \
					<< "CapSense[1]" << "CapSense[2]" << "CapSense[3]" \
					<< "CapSense[4]" << "Status";
			toolTipList << "Unused" << "Decoded: mg" << "Decoded: mg" << "Decoded: mg" \
					<< "Decoded: deg/s" << "Decoded: deg/s" << "Decoded: deg/s" << "Decoded: uT" \
					<< "Decoded: uT" << "Decoded: uT" << "Raw Values Only" << "Raw Values Only" \
					<< "Raw Values Only" << "Raw Values Only" << "Raw Values Only" \
					<< "Raw Values Only" << "Raw Values Only";
			break;
		case FLEXSEA_VIRTUAL_BASE:
			//TODO: Virtual can be many things. For now we only use it with
			//RIC/NU. Generalize.
			var_list << "**Unused**" << "Accel X" << "Accel Y" << "Accel Z" \
					<< "Gyro X" << "Gyro Y" << "Gyro Z" << "Encoder Motor" \
					<< "Encoder Joint" << "Motor current" << "Strain[0]" << "Strain[1]" \
					<< "Strain[2]" << "Strain[3]" << "Strain[4]" << "Strain[5]" \
					<< "PWM";
			toolTipList << "Unused" << "Decoded: mg" << "Decoded: mg" << "Decoded: mg" \
					<< "Decoded: deg/s" << "Decoded: deg/s" << "Decoded: deg/s" << "Raw Value Only" \
					<< "Raw value only" << "Decoded: mA" << "Decoded: ±100%" << "Decoded: ±100%" \
					<< "Decoded: ±100%"<< "Decoded: ±100%"<< "Decoded: ±100%"\
					<< "Decoded: ±100%" << "PWM, -1024 to 1024";
			break;
		default:
			var_list << "Invalid";
			toolTipList << "Invalid";
	}

	//var_list & toolTipList need to be the same length:
	if(var_list.length() != toolTipList.length())
	{
		qDebug() << "Error in updateVarList()!";
	}

	//Fill the comboBox:
	(*cbVar[item])->clear();
	(*cbVar[item])->setToolTipDuration(350);
	for(int index = 0; index < var_list.count(); index++)
	{
		(*cbVar[item])->addItem(var_list.at(index));
		(*cbVar[item])->setItemData(index, toolTipList.at(index), Qt::ToolTipRole);
	}
}

//Assigns a pointer to the desired variable. This function is called whenever
//we change Slave or Variable. The runtime plotting function will then use the
//pointer.
void W_2DPlot::assignVariable(uint8_t var)
{
	switch(slaveBType[var])
	{
		case FLEXSEA_PLAN_BASE:
			//ToDo, if needed
			break;
		case FLEXSEA_MANAGE_BASE:
			struct manage_s *mnPtr;
			FlexSEA_Generic::assignManagePtr(&mnPtr, SL_BASE_ALL, \
											   slaveIndex[var]);
			assignVariableMn(var, mnPtr);
			break;
		case FLEXSEA_EXECUTE_BASE:
			struct execute_s *exPtr;
			FlexSEA_Generic::assignExecutePtr(&exPtr, SL_BASE_ALL, \
											   slaveIndex[var]);
			assignVariableEx(var, exPtr);
			break;
		case FLEXSEA_BATTERY_BASE:
			struct battery_s *baPtr;
			FlexSEA_Generic::assignBatteryPtr(&baPtr, SL_BASE_ALL, \
											   slaveIndex[var]);
			assignVariableBa(var, baPtr);
			break;
		case FLEXSEA_STRAIN_BASE:
			struct strain_s *stPtr;
			FlexSEA_Generic::assignStrainPtr(&stPtr, SL_BASE_ALL, \
											   slaveIndex[var]);
			assignVariableSt(var, stPtr);
			break;
		case FLEXSEA_GOSSIP_BASE:
			struct gossip_s *goPtr;
			FlexSEA_Generic::assignGossipPtr(&goPtr, SL_BASE_ALL, \
											   slaveIndex[var]);
			assignVariableGo(var, goPtr);
			break;
		case FLEXSEA_VIRTUAL_BASE:
			//TODO Generalize for other projects than RIC/NU
			struct ricnu_s *myPtr;
			FlexSEA_Generic::assignRicnuPtr(&myPtr, SL_BASE_ALL, \
											   slaveIndex[var]);
			assignVariableRicnu(var, myPtr);
			break;
		default:
			break;
	}
}

//TODO move this to the w_board files

//Assigns a pointer to the desired variable - Execute boards
void W_2DPlot::assignVariableEx(uint8_t var, struct execute_s *myPtr)
{
	//'Used' as default, 'false' when set at Unused
	vtp[var].used = true;
	vtp[var].format = FORMAT_32S;

	//Assign pointer:
	switch(varIndex[var])
	{
		/*Format: (every Case except Unused)
		 * Line 1: data format, raw variable
		 * Line 2: raw variable
		 * Line 3: decoded variable (always int32),
					null if not decoded  */
		case 0: //"**Unused**"
			vtp[var].used = false;
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 1: //"Accel X"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->accel.x;
			vtp[var].decodedPtr = &myPtr->decoded.accel.x;
			break;
		case 2: //"Accel Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->accel.y;
			vtp[var].decodedPtr = &myPtr->decoded.accel.y;
			break;
		case 3: //"Accel Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->accel.z;
			vtp[var].decodedPtr = &myPtr->decoded.accel.z;
			break;
		case 4: //"Gyro X"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->gyro.x;
			vtp[var].decodedPtr = &myPtr->decoded.gyro.x;
			break;
		case 5: //"Gyro Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->gyro.y;
			vtp[var].decodedPtr = &myPtr->decoded.gyro.y;
			break;
		case 6: //"Gyro Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->gyro.z;
			vtp[var].decodedPtr = &myPtr->decoded.gyro.z;
			break;
		case 7: //"Encoder Display"
			vtp[var].format = FORMAT_32S;
            vtp[var].rawGenPtr = myPtr->enc_ang;
			vtp[var].decodedPtr = &nullVar32s;
			break;
        case 8: //"Encoder Control"
            break;
        case 9: //"Encoder Commutation"
            break;
		case 10: //"Motor current"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->current;
			vtp[var].decodedPtr = &myPtr->decoded.current;
			break;
		case 11: //"Analog[0]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[0];
			vtp[var].decodedPtr = &myPtr->decoded.analog[0];
			break;
		case 12: //Analog[1]
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[1];
			vtp[var].decodedPtr = &myPtr->decoded.analog[1];
			break;
		case 13: //"Strain"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->strain;
			vtp[var].decodedPtr = &myPtr->decoded.strain;
			break;
		case 14: //"+VB"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->volt_batt;
			vtp[var].decodedPtr = &myPtr->decoded.volt_batt;
			break;
		case 15: //"+VG"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->volt_int;
			vtp[var].decodedPtr = &myPtr->decoded.volt_int;
			break;
		case 16: //"Temp"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->temp;
			vtp[var].decodedPtr = &myPtr->decoded.temp;
			break;
		case 17: //"Status 1"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->status1;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 18: //"Status 2"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->status2;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 19: //"Setpoint (square)"
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;//ctrl_setpoint);   //ToDo Fix
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 20: //"Setpoint (trap)"
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;//ctrl_setpoint_trap);  //ToDo Fix
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 21: //"Fake Data"
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &myFakeData;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			vtp[var].used = false;
			break;
	}
}

//Assigns a pointer to the desired variable - RINC/NU (Execute) boards
void W_2DPlot::assignVariableRicnu(uint8_t var, struct ricnu_s *myPtr)
{
	//'Used' as default, 'false' when set at Unused
	vtp[var].used = true;
	vtp[var].format = FORMAT_32S;

	//Assign pointer:
	switch(varIndex[var])
	{
		/*Format: (every Case except Unused)
		 * Line 1: data format, raw variable
		 * Line 2: raw variable
		 * Line 3: decoded variable (always int32),
					null if not decoded  */
		case 0: //"**Unused**"
			vtp[var].used = false;
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 1: //"Accel X"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->ex.accel.x;
			vtp[var].decodedPtr = &myPtr->ex.decoded.accel.x;
			break;
		case 2: //"Accel Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->ex.accel.y;
			vtp[var].decodedPtr = &myPtr->ex.decoded.accel.y;
			break;
		case 3: //"Accel Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->ex.accel.z;
			vtp[var].decodedPtr = &myPtr->ex.decoded.accel.z;
			break;
		case 4: //"Gyro X"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->ex.gyro.x;
			vtp[var].decodedPtr = &myPtr->ex.decoded.gyro.x;
			break;
		case 5: //"Gyro Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->ex.gyro.y;
			vtp[var].decodedPtr = &myPtr->ex.decoded.gyro.y;
			break;
		case 6: //"Gyro Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->ex.gyro.z;
			vtp[var].decodedPtr = &myPtr->ex.decoded.gyro.z;
			break;
		case 7: //"Encoder Motor"
			vtp[var].format = FORMAT_32S;
            vtp[var].rawGenPtr = &myPtr->enc_motor;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 8: //"Encoder Control"
			vtp[var].format = FORMAT_32S;
            vtp[var].rawGenPtr = &myPtr->enc_joint;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 9: //"Motor current"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->ex.current;
			vtp[var].decodedPtr = &myPtr->ex.decoded.current;
			break;
		case 10: //"Strain[0]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->st.ch[0].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.ext_strain[0];
			break;
		case 11: //"Strain[1]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->st.ch[1].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.ext_strain[1];
			break;
		case 12: //"Strain[2]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->st.ch[2].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.ext_strain[2];
			break;
		case 13: //"Strain[3]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->st.ch[3].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.ext_strain[3];
			break;
		case 14: //"Strain[4]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->st.ch[4].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.ext_strain[4];
			break;
		case 15: //"Strain[5]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->st.ch[5].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.ext_strain[5];
			break;
		case 16: //"PWM"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->ex.sine_commut_pwm;
			vtp[var].decodedPtr = &nullVar32s;
			break;
	}
}

//Assigns a pointer to the desired variable - Manage boards
void W_2DPlot::assignVariableMn(uint8_t var, struct manage_s *myPtr)
{
	//'Used' as default, 'false' when set at Unused
	vtp[var].used = true;
	vtp[var].format = FORMAT_32S;

	//Assign pointer:
	switch(varIndex[var])
	{
		/*Format: (every Case except Unused)
		 * Line 1: data format, raw variable
		 * Line 2: raw variable
		 * Line 3: decoded variable (always int32),
					null if not decoded  */
		case 0: //"**Unused**"
			vtp[var].used = false;
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 1: //"Accel X"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->accel.x;
			vtp[var].decodedPtr = &myPtr->decoded.accel.x;
			break;
		case 2: //"Accel Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->accel.y;
			vtp[var].decodedPtr = &myPtr->decoded.accel.y;
			break;
		case 3: //"Accel Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->accel.z;
			vtp[var].decodedPtr = &myPtr->decoded.accel.z;
			break;
		case 4: //"Gyro X"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->gyro.x;
			vtp[var].decodedPtr = &myPtr->decoded.gyro.x;
			break;
		case 5: //"Gyro Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->gyro.y;
			vtp[var].decodedPtr = &myPtr->decoded.gyro.y;
			break;
		case 6: //"Gyro Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->gyro.z;
			vtp[var].decodedPtr = &myPtr->decoded.gyro.z;
			break;
		case 7: //"Pushbutton"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->sw1;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 8: //"Digital inputs"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->digitalIn;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 9: //"Analog[0]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[0];
			vtp[var].decodedPtr = &myPtr->decoded.analog[0];
			break;
		case 10: //Analog[1]
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[1];
			vtp[var].decodedPtr = &myPtr->decoded.analog[1];
			break;
		case 11: //"Analog[2]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[2];
			vtp[var].decodedPtr = &myPtr->decoded.analog[2];
			break;
		case 12: //Analog[3]
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[3];
			vtp[var].decodedPtr = &myPtr->decoded.analog[3];
			break;
		case 13: //"Analog[4]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[4];
			vtp[var].decodedPtr = &myPtr->decoded.analog[4];
			break;
		case 14: //Analog[5]
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[5];
			vtp[var].decodedPtr = &myPtr->decoded.analog[5];
			break;
		case 15: //"Analog[6]"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[6];
			vtp[var].decodedPtr = &myPtr->decoded.analog[6];
			break;
		case 16: //Analog[7]
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->analog[7];
			vtp[var].decodedPtr = &myPtr->decoded.analog[7];
			break;
		case 17: //"Status"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->status1;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			vtp[var].used = false;
			break;
	}
}

//Assigns a pointer to the desired variable - Gossip boards
void W_2DPlot::assignVariableGo(uint8_t var, struct gossip_s *myPtr)
{
	//'Used' as default, 'false' when set at Unused
	vtp[var].used = true;
	vtp[var].format = FORMAT_32S;

	//Assign pointer:
	switch(varIndex[var])
	{
		/*Format: (every Case except Unused)
		 * Line 1: data format, raw variable
		 * Line 2: raw variable
		 * Line 3: decoded variable (always int32),
					null if not decoded  */
		case 0: //"**Unused**"
			vtp[var].used = false;
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 1: //"Accel X"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->accel.x;
			vtp[var].decodedPtr = &myPtr->decoded.accel.x;
			break;
		case 2: //"Accel Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->accel.y;
			vtp[var].decodedPtr = &myPtr->decoded.accel.y;
			break;
		case 3: //"Accel Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->accel.z;
			vtp[var].decodedPtr = &myPtr->decoded.accel.z;
			break;
		case 4: //"Gyro X"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->gyro.x;
			vtp[var].decodedPtr = &myPtr->decoded.gyro.x;
			break;
		case 5: //"Gyro Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->gyro.y;
			vtp[var].decodedPtr = &myPtr->decoded.gyro.y;
			break;
		case 6: //"Gyro Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->gyro.z;
			vtp[var].decodedPtr = &myPtr->decoded.gyro.z;
			break;
		case 7: //"Magneto X"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->magneto.x;
			vtp[var].decodedPtr = &myPtr->decoded.magneto.x;
			break;
		case 8: //"Magneto Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->magneto.y;
			vtp[var].decodedPtr = &myPtr->decoded.magneto.y;
			break;
		case 9: //"Magneto Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->magneto.z;
			vtp[var].decodedPtr = &myPtr->decoded.magneto.z;
			break;
		case 10: //"IO 1"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->io[0];
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 11: //"IO 2"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->io[1];
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 12: //"Capsense 1"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->capsense[0];
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 13: //"Capsense 2"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->capsense[1];
			vtp[var].decodedPtr = &nullVar32s;
		case 14: //"Capsense 3"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->capsense[2];
			vtp[var].decodedPtr = &nullVar32s;
		case 15: //"Capsense 4"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->capsense[3];
			vtp[var].decodedPtr = &nullVar32s;
		case 16: //"Status"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->status;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			vtp[var].used = false;
			break;
	}
}

//Assigns a pointer to the desired variable - Battery boards
void W_2DPlot::assignVariableBa(uint8_t var, struct battery_s *myPtr)
{
	//'Used' as default, 'false' when set at Unused
	vtp[var].used = true;
	vtp[var].format = FORMAT_32S;

	//Assign pointer:
	switch(varIndex[var])
	{
		/*Format: (every Case except Unused)
		 * Line 1: data format, raw variable
		 * Line 2: raw variable
		 * Line 3: decoded variable (always int32),
					null if not decoded  */
		case 0: //"**Unused**"
			vtp[var].used = false;
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 1: //"Voltage"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->voltage;
			vtp[var].decodedPtr = &myPtr->decoded.voltage;
			break;
		case 2: //"Current"
			vtp[var].format = FORMAT_16S;
			vtp[var].rawGenPtr = &myPtr->current;
			vtp[var].decodedPtr = &myPtr->decoded.current;
			break;
		case 3: //"Power"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &nullVar16u;
			vtp[var].decodedPtr = &myPtr->decoded.power;
			break;
		case 4: //"Temperature"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->temp;
			vtp[var].decodedPtr = &myPtr->decoded.temp;
			break;
		case 5: //"Pushbutton"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->pushbutton;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 6: //"Status"
			vtp[var].format = FORMAT_8U;
			vtp[var].rawGenPtr = &myPtr->status;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			vtp[var].used = false;
			break;
	}
}

//Assigns a pointer to the desired variable - Strain boards
void W_2DPlot::assignVariableSt(uint8_t var, struct strain_s *myPtr)
{
	//'Used' as default, 'false' when set at Unused
	vtp[var].used = true;
	vtp[var].format = FORMAT_32S;

	//Assign pointer:
	switch(varIndex[var])
	{
		/*Format: (every Case except Unused)
		 * Line 1: data format, raw variable
		 * Line 2: raw variable
		 * Line 3: decoded variable (always int32),
					null if not decoded  */

		case 0: //"**Unused**"
			vtp[var].used = false;
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			break;
		case 1: //"Ch 1"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->ch[0].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.strain[0];
			break;
		case 2: //"Ch 2"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->ch[1].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.strain[1];
			break;
		case 3: //"Ch 3"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->ch[2].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.strain[2];
			break;
		case 4: //"Ch 4"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->ch[3].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.strain[3];
			break;
		case 5: //"Ch 5"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->ch[4].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.strain[4];
			break;
		case 6: //"Ch 6"
			vtp[var].format = FORMAT_16U;
			vtp[var].rawGenPtr = &myPtr->ch[5].strain_filtered;
			vtp[var].decodedPtr = &myPtr->decoded.strain[5];
			break;
/*
		case 7: //"Status"
			vtp[var].format = FORMAT_8U;
			vtp[var].gptr = &myPtr->status;
			vtp[var].ptrD32s = &nullVar32s;
			break;
*/

		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].rawGenPtr = &nullVar32s;
			vtp[var].decodedPtr = &nullVar32s;
			vtp[var].used = false;
			break;
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
		saveCurrentSettings();

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
		saveCurrentSettings();
		updateVarList(1);
		assignVariable(1);
	}
}

void W_2DPlot::on_cBoxvar3slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		updateVarList(2);
		assignVariable(2);
	}
}

void W_2DPlot::on_cBoxvar4slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		updateVarList(3);
		assignVariable(3);
	}
}

void W_2DPlot::on_cBoxvar5slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		updateVarList(4);
		assignVariable(4);
	}
}

void W_2DPlot::on_cBoxvar6slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
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
		saveCurrentSettings();
		assignVariable(0);
	}
}

void W_2DPlot::on_cBoxvar2_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(1);
	}
}

void W_2DPlot::on_cBoxvar3_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(2);
	}
}

void W_2DPlot::on_cBoxvar4_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(3);
	}
}

void W_2DPlot::on_cBoxvar5_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(4);
	}
}

void W_2DPlot::on_cBoxvar6_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(5);
	}
}

//Decode checkboxes:

void W_2DPlot::on_checkBoxD1_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(0);
	}
}

void W_2DPlot::on_checkBoxD2_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(1);
	}
}

void W_2DPlot::on_checkBoxD3_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(2);
	}
}

void W_2DPlot::on_checkBoxD4_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(3);
	}
}

void W_2DPlot::on_checkBoxD5_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		assignVariable(4);
	}
}

void W_2DPlot::on_checkBoxD6_stateChanged(int arg1)
{
	(void)arg1;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
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
