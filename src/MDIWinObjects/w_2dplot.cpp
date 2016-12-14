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
	initUserInput();
	initChart();
	initStats();

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
	uint8_t index = 0, used = 0;
	int val[6] = {0,0,0,0,0,0};

	dataRate = getRefreshRateData();

	//For every variable:
	for(index = 0; index < VAR_NUM; index++)
	{
		if(vtp[index].decode == false)
		{
			switch(vtp[index].format)
			{
				case FORMAT_32S:
					val[index] = (*vtp[index].ptr32s);
					break;
				case FORMAT_32U:
					val[index] = (int)(*vtp[index].ptr32u);
					break;
				case FORMAT_16S:
					val[index] = (int)(*vtp[index].ptr16s);
					break;
				case FORMAT_16U:
					val[index] = (int)(*vtp[index].ptr16u);
					break;
				case FORMAT_8S:
					val[index] = (int)(*vtp[index].ptr8s);
					break;
				case FORMAT_8U:
					val[index] = (int)(*vtp[index].ptr8u);
					break;
				default:
					val[index] = 0;
					break;
			}
		}
		else
		{
			//if(used)
			{
				val[index] = (*vtp[index].ptrD32s);
			}
		}
	}

	saveNewPoints(val);
}

void W_2DPlot::refresh2DPlot(void)
{
	uint8_t index = 0, used = 0;

	//Refresh Stat Bar:
	refreshStatBar(getRefreshRateDisplay(), dataRate, QPoint(0,0));


	//For every variable:
	for(index = 0; index < VAR_NUM; index++)
	{
		if(vtp[index].used == false)
		{
			//This channel isn't used, we make it invisible
			qlsData[index]->setVisible(false);
			used = 0;
		}
		else
		{
			qlsData[index]->setVisible(true);
			used = 1;
		}
	}

	//And now update the display:
	if(plotFreezed == false)
	{
		qlsData[0]->replace(qlsDataBuffer[0].points());
		qlsData[1]->replace(qlsDataBuffer[1].points());
		qlsData[2]->replace(qlsDataBuffer[2].points());
		qlsData[3]->replace(qlsDataBuffer[3].points());
		qlsData[4]->replace(qlsDataBuffer[4].points());
		qlsData[5]->replace(qlsDataBuffer[5].points());

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

void W_2DPlot::initChart(void)
{
	vecLen = 0;

	//Data series:
	qlsData[0] = new QLineSeries();
	qlsData[0]->append(0, 0);
	qlsData[1] = new QLineSeries();
	qlsData[1]->append(0, 0);
	qlsData[2] = new QLineSeries();
	qlsData[2]->append(0, 0);
	qlsData[3] = new QLineSeries();
	qlsData[3]->append(0, 0);
	qlsData[4] = new QLineSeries();
	qlsData[4]->append(0, 0);
	qlsData[5] = new QLineSeries();
	qlsData[5]->append(0, 0);

	//Chart:
	chart = new QChart();
	chart->legend()->hide();
	chart->addSeries(qlsData[0]);
	chart->addSeries(qlsData[1]);
	chart->addSeries(qlsData[2]);
	chart->addSeries(qlsData[3]);
	chart->addSeries(qlsData[4]);
	chart->addSeries(qlsData[5]);

	chart->createDefaultAxes();
	chart->axisX()->setRange(plot_xmin, plot_xmax);
	chart->axisY()->setRange(plot_ymin, plot_ymax);

	//Colors:
	chart->setTheme(QChart::ChartThemeDark);
	qlsData[5]->setColor(Qt::red);  //Color[5] was ~= [0], too similar, now red
	//Update labels based on theme colors:
	QString msg[VAR_NUM];
	for(int u = 0; u < VAR_NUM; u++)
	{
		int r = 0, g = 0, b = 0;
		r = qlsData[u]->color().red();
		g = qlsData[u]->color().green();
		b = qlsData[u]->color().blue();
		msg[u] = "QLabel { background-color: black; color: rgb(" + \
				QString::number(r) + ',' + QString::number(g) + ','+ \
				QString::number(b) + ");}";
	}

	ui->label_t1->setStyleSheet(msg[0]);
	ui->label_t2->setStyleSheet(msg[1]);
	ui->label_t3->setStyleSheet(msg[2]);
	ui->label_t4->setStyleSheet(msg[3]);
	ui->label_t5->setStyleSheet(msg[4]);
	ui->label_t6->setStyleSheet(msg[5]);

	//Chart view:
	chartView = new QChartView(chart);
	ui->gridLayout_test->addWidget(chartView, 0,0);
	chartView->setRenderHint(QPainter::Antialiasing);
	chartView->setBaseSize(600,300);
	chartView->setMinimumSize(500,300);
	chartView->setMaximumSize(4000,2500);
	chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	//Data indicator:
	connect(qlsData[0], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler0(QPointF, bool)));
	connect(qlsData[1], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler1(QPointF, bool)));
	connect(qlsData[2], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler2(QPointF, bool)));
	connect(qlsData[3], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler3(QPointF, bool)));
	connect(qlsData[4], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler4(QPointF, bool)));
	connect(qlsData[5], SIGNAL(hovered(const QPointF, bool)),\
			this, SLOT(myHoverHandler5(QPointF, bool)));
}

//Fills the fields and combo boxes:
void W_2DPlot::initUserInput(void)
{
	nullVar32s = 0;
	nullVar32u = 0;
	nullVar16s = 0;
	nullVar16u = 0;
	nullVar8s = 0;
	nullVar8u = 0;

	for(int i = 0; i < VAR_NUM; i++)
	{
		vtp[i].ptr32s = &nullVar32s;
		vtp[i].ptrD32s = &nullVar32s;
		vtp[i].ptr32u = &nullVar32u;
		vtp[i].ptr16s = &nullVar16s;
		vtp[i].ptr16u = &nullVar16u;
		vtp[i].ptr8s = &nullVar8s;
		vtp[i].ptr8u = &nullVar8u;
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

	for(int h = 0; h < VAR_NUM; h++)
	{
		vtp[h].used = false;
	}

	plotting_len = 0;

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
	FlexSEA_Generic::populateSlaveComboBox(ui->cBoxvar1slave, SL_BASE_ALL, \
											SL_LEN_ALL);
	FlexSEA_Generic::populateSlaveComboBox(ui->cBoxvar2slave, SL_BASE_ALL, \
										  SL_LEN_ALL);
	FlexSEA_Generic::populateSlaveComboBox(ui->cBoxvar3slave, SL_BASE_ALL, \
										  SL_LEN_ALL);
	FlexSEA_Generic::populateSlaveComboBox(ui->cBoxvar4slave, SL_BASE_ALL, \
										  SL_LEN_ALL);
	FlexSEA_Generic::populateSlaveComboBox(ui->cBoxvar5slave, SL_BASE_ALL, \
										  SL_LEN_ALL);
	FlexSEA_Generic::populateSlaveComboBox(ui->cBoxvar6slave, SL_BASE_ALL, \
										  SL_LEN_ALL);

	//Variable comboBoxes:
	saveCurrentSettings();  //Needed for the 1st var_list
	updateVarList(0, ui->cBoxvar1);
	updateVarList(1, ui->cBoxvar2);
	updateVarList(2, ui->cBoxvar3);
	updateVarList(3, ui->cBoxvar4);
	updateVarList(4, ui->cBoxvar5);
	updateVarList(5, ui->cBoxvar6);

	//By default, we track Slave 1:
	ui->checkBoxTrack->setChecked(true);

	//Init flag:
	initFlag = false;

	//Decode Checkbox tooltips:
	QString ttip = "<html><head/><body><p>Plot data in physical units (instead \
					of ticks)</p></body></html>";
	ui->checkBoxD1->setToolTip(ttip);
	ui->checkBoxD2->setToolTip(ttip);
	ui->checkBoxD3->setToolTip(ttip);
	ui->checkBoxD4->setToolTip(ttip);
	ui->checkBoxD5->setToolTip(ttip);
	ui->checkBoxD6->setToolTip(ttip);

	pointsVisible = false;
	globalYmin = 0;
	globalYmin = 0;

	//Stats bar:
	ui->label_refreshRateData->setTextFormat(Qt::RichText);
	ui->label_refreshRateData->setText("-- Hz");
	ui->label_refreshRateDisplay->setTextFormat(Qt::RichText);
	ui->label_refreshRateDisplay->setText("-- Hz");
	ui->label_pointHovered->setTextFormat(Qt::RichText);
	ui->label_pointHovered->setText("(--, --)");

	saveCurrentSettings();
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
			qlsDataBuffer[i].append(vecLen, myDataPoints[i]);

			min.setY(qlsDataBuffer[i].at(0).y());
			max.setY(qlsDataBuffer[i].at(0).y());
			avg = 0;
			for(int j = 0; j < vecLen; j++)
			{

				//Minimum:
				if(qlsDataBuffer[i].at(j).y() < min.y())
				{
					min.setY(qlsDataBuffer[i].at(j).y());
				}

				//Maximum:
				if(qlsDataBuffer[i].at(j).y() > max.y())
				{
					max.setY(qlsDataBuffer[i].at(j).y());
				}

				//Average - sum:
				tempInt = qlsDataBuffer[i].at(j).toPoint();
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
			min.setY(qlsDataBuffer[i].at(0).y());
			max.setY(qlsDataBuffer[i].at(0).y());
			avg = 0;
			int index = 0;
			for(int j = 1; j < plot_len+1; j++)
			{
				index = j-1;
				//qDebug() << "Index:" << index << "Plot len:" << plot_len;

				//Minimum:
				if(qlsDataBuffer[i].at(index).y() < min.y())
				{
					min.setY(qlsDataBuffer[i].at(index).y());
				}

				//Maximum:
				if(qlsDataBuffer[i].at(index).y() > max.y())
				{
					max.setY(qlsDataBuffer[i].at(index).y());
				}

				//Average - sum:
				tempInt = qlsDataBuffer[i].at(index).toPoint();
				avg += tempInt.y();

				//Shift by one position (all but last point):
				if(j < plot_len)
				{
					temp = qlsDataBuffer[i].at(j);
					qlsDataBuffer[i].replace(index, QPointF(index, temp.ry()));
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
			qlsDataBuffer[i].replace(plot_len-1, QPointF(plot_len-1, myDataPoints[i]));
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
//Average of 10 values
float W_2DPlot::getRefreshRateDisplay(void)
{
	static qint64 oldTime = 0;
	qint64 newTime = 0, diffTime = 0;
	float t_s = 0.0, f = 0.0, avg = 0.0;
	static float oldAvg = 0.0;

	newTime = timerRefreshDisplay->currentMSecsSinceEpoch();
	diffTime = newTime - oldTime;
	oldTime = newTime;

	t_s = diffTime/1000.0;
	f = 1/t_s;

	//Average:
	avg = 0.9*oldAvg;
	avg += 0.1*f;
	oldAvg = avg;

	return avg;
}

//Returns the rate at which it is called, in Hz
//Average of 10 values
float W_2DPlot::getRefreshRateData(void)
{
	static qint64 oldTime = 0;
	qint64 newTime = 0, diffTime = 0;
	float t_s = 0.0, f = 0.0, avg = 0.0;
	static float oldAvg = 0.0;

	newTime = timerRefreshData->currentMSecsSinceEpoch();
	diffTime = newTime - oldTime;
	oldTime = newTime;

	t_s = diffTime/1000.0;
	f = 1/t_s;

	//Average:
	avg = 0.9*oldAvg;
	avg += 0.1*f;
	oldAvg = avg;

	return avg;
}

//Empties all the lists
void W_2DPlot::initData(void)
{
	vecLen = 0;
	for(int i = 0; i < VAR_NUM; i++)
	{
		qlsDataBuffer[i].clear();
		qlsData[i]->replace(qlsDataBuffer[i].points());
	}

	initStats();
}

//Based on the current state of comboBoxes, saves the info in variables
void W_2DPlot::saveCurrentSettings(void)
{
	//Slave:
	slaveIndex[0] = ui->cBoxvar1slave->currentIndex();
	slaveIndex[1] = ui->cBoxvar2slave->currentIndex();
	slaveIndex[2] = ui->cBoxvar3slave->currentIndex();
	slaveIndex[3] = ui->cBoxvar4slave->currentIndex();
	slaveIndex[4] = ui->cBoxvar5slave->currentIndex();
	slaveIndex[5] = ui->cBoxvar6slave->currentIndex();

	for(int i = 0; i < VAR_NUM; i++)
	{
		slaveAddr[i] = FlexSEA_Generic::getSlaveID(SL_BASE_ALL, slaveIndex[i]);
		slaveBType[i] = FlexSEA_Generic::getSlaveBoardType(SL_BASE_ALL, \
														   slaveIndex[i]);
	}

	//Variable:
	varIndex[0] = ui->cBoxvar1->currentIndex();
	varIndex[1] = ui->cBoxvar2->currentIndex();
	varIndex[2] = ui->cBoxvar3->currentIndex();
	varIndex[3] = ui->cBoxvar4->currentIndex();
	varIndex[4] = ui->cBoxvar5->currentIndex();
	varIndex[5] = ui->cBoxvar6->currentIndex();

	//Decode:
	vtp[0].decode = ui->checkBoxD1->isChecked();
	vtp[1].decode = ui->checkBoxD2->isChecked();
	vtp[2].decode = ui->checkBoxD3->isChecked();
	vtp[3].decode = ui->checkBoxD4->isChecked();
	vtp[4].decode = ui->checkBoxD5->isChecked();
	vtp[5].decode = ui->checkBoxD6->isChecked();
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
		plot_xmax = plotting_len;

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
	for(int i = 0; i < VAR_NUM; i++)
	{
		memset(stats[i], 0, STATS_FIELDS);
	}

	ui->label_1_min->setText(QString::number(0));
	ui->label_1_max->setText(QString::number(0));
	ui->label_1_avg->setText(QString::number(0));

	ui->label_2_min->setText(QString::number(0));
	ui->label_2_max->setText(QString::number(0));
	ui->label_2_avg->setText(QString::number(0));

	ui->label_3_min->setText(QString::number(0));
	ui->label_3_max->setText(QString::number(0));
	ui->label_3_avg->setText(QString::number(0));

	ui->label_4_min->setText(QString::number(0));
	ui->label_4_max->setText(QString::number(0));
	ui->label_4_avg->setText(QString::number(0));

	ui->label_5_min->setText(QString::number(0));
	ui->label_5_max->setText(QString::number(0));
	ui->label_5_avg->setText(QString::number(0));

	ui->label_6_min->setText(QString::number(0));
	ui->label_6_max->setText(QString::number(0));
	ui->label_6_avg->setText(QString::number(0));
}

void W_2DPlot::refreshStats(void)
{
	ui->label_1_min->setText(QString::number(stats[0][STATS_MIN]));
	ui->label_1_max->setText(QString::number(stats[0][STATS_MAX]));
	ui->label_1_avg->setText(QString::number(stats[0][STATS_AVG]));

	ui->label_2_min->setText(QString::number(stats[1][STATS_MIN]));
	ui->label_2_max->setText(QString::number(stats[1][STATS_MAX]));
	ui->label_2_avg->setText(QString::number(stats[1][STATS_AVG]));

	ui->label_3_min->setText(QString::number(stats[2][STATS_MIN]));
	ui->label_3_max->setText(QString::number(stats[2][STATS_MAX]));
	ui->label_3_avg->setText(QString::number(stats[2][STATS_AVG]));

	ui->label_4_min->setText(QString::number(stats[3][STATS_MIN]));
	ui->label_4_max->setText(QString::number(stats[3][STATS_MAX]));
	ui->label_4_avg->setText(QString::number(stats[3][STATS_AVG]));

	ui->label_5_min->setText(QString::number(stats[4][STATS_MIN]));
	ui->label_5_max->setText(QString::number(stats[4][STATS_MAX]));
	ui->label_5_avg->setText(QString::number(stats[4][STATS_AVG]));

	ui->label_6_min->setText(QString::number(stats[5][STATS_MIN]));
	ui->label_6_max->setText(QString::number(stats[5][STATS_MAX]));
	ui->label_6_avg->setText(QString::number(stats[5][STATS_AVG]));
}

void W_2DPlot::refreshStatBar(float fDisp, float fData, QPoint xy)
{
	QString txt, num;

	num = QString::number(fDisp, 'f', 2);
	txt = "<font color=#808080>Display: " + num + " Hz </font>";
	ui->label_refreshRateDisplay->setText(txt);

	num = QString::number(fData, 'f', 2);
	txt = "<font color=#808080>Data: " + num + " Hz </font>";
	ui->label_refreshRateData->setText(txt);
}

//Each board type has a different variable list.
//ToDo: those lists should come from the w_BoardName files
void W_2DPlot::updateVarList(uint8_t var, QComboBox *myCombo)
{
	QStringList var_list, toolTipList;

	uint8_t bType = slaveBType[var];

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
					<< "Strain[2]" << "Strain[3]" << "Strain[4]" << "Strain[5]";
			toolTipList << "Unused" << "Decoded: mg" << "Decoded: mg" << "Decoded: mg" \
					<< "Decoded: deg/s" << "Decoded: deg/s" << "Decoded: deg/s" << "Raw Value Only" \
					<< "Raw value only" << "Decoded: mA" << "Decoded: ±100%" << "Decoded: ±100%" \
					<< "Decoded: ±100%"<< "Decoded: ±100%"<< "Decoded: ±100%"\
					<< "Decoded: ±100%";
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
	myCombo->clear();
	myCombo->setToolTipDuration(750);
	for(int index = 0; index < var_list.count(); index++)
	{
		myCombo->addItem(var_list.at(index));
		myCombo->setItemData(index, toolTipList.at(index), Qt::ToolTipRole);
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
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 1: //"Accel X"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->accel.x;
			vtp[var].ptrD32s = &myPtr->decoded.accel.x;
			break;
		case 2: //"Accel Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->accel.y;
			vtp[var].ptrD32s = &myPtr->decoded.accel.y;
			break;
		case 3: //"Accel Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->accel.z;
			vtp[var].ptrD32s = &myPtr->decoded.accel.z;
			break;
		case 4: //"Gyro X"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->gyro.x;
			vtp[var].ptrD32s = &myPtr->decoded.gyro.x;
			break;
		case 5: //"Gyro Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->gyro.y;
			vtp[var].ptrD32s = &myPtr->decoded.gyro.y;
			break;
		case 6: //"Gyro Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->gyro.z;
			vtp[var].ptrD32s = &myPtr->decoded.gyro.z;
			break;
		case 7: //"Encoder Display"
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &myPtr->enc_display;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 8: //"Encoder Control"
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &myPtr->enc_control;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 9: //"Encoder Commutation"
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &myPtr->enc_commut;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 10: //"Motor current"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->current;
			vtp[var].ptrD32s = &myPtr->decoded.current;
			break;
		case 11: //"Analog[0]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[0];
			vtp[var].ptrD32s = &myPtr->decoded.analog[0];
			break;
		case 12: //Analog[1]
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[1];
			vtp[var].ptrD32s = &myPtr->decoded.analog[1];
			break;
		case 13: //"Strain"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->strain;
			vtp[var].ptrD32s = &myPtr->decoded.strain;
			break;
		case 14: //"+VB"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->volt_batt;
			vtp[var].ptrD32s = &myPtr->decoded.volt_batt;
			break;
		case 15: //"+VG"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->volt_int;
			vtp[var].ptrD32s = &myPtr->decoded.volt_int;
			break;
		case 16: //"Temp"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->temp;
			vtp[var].ptrD32s = &myPtr->decoded.temp;
			break;
		case 17: //"Status 1"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->status1;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 18: //"Status 2"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->status2;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 19: //"Setpoint (square)"
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &nullVar32s;//ctrl_setpoint);   //ToDo Fix
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 20: //"Setpoint (trap)"
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &nullVar32s;//ctrl_setpoint_trap);  //ToDo Fix
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 21: //"Fake Data"
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &myFakeData;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
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
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 1: //"Accel X"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->ex.accel.x;
			vtp[var].ptrD32s = &myPtr->ex.decoded.accel.x;
			break;
		case 2: //"Accel Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->ex.accel.y;
			vtp[var].ptrD32s = &myPtr->ex.decoded.accel.y;
			break;
		case 3: //"Accel Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->ex.accel.z;
			vtp[var].ptrD32s = &myPtr->ex.decoded.accel.z;
			break;
		case 4: //"Gyro X"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->ex.gyro.x;
			vtp[var].ptrD32s = &myPtr->ex.decoded.gyro.x;
			break;
		case 5: //"Gyro Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->ex.gyro.y;
			vtp[var].ptrD32s = &myPtr->ex.decoded.gyro.y;
			break;
		case 6: //"Gyro Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->ex.gyro.z;
			vtp[var].ptrD32s = &myPtr->ex.decoded.gyro.z;
			break;
		case 7: //"Encoder Motor"
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &myPtr->ex.enc_motor;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 8: //"Encoder Control"
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &myPtr->ex.enc_joint;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 9: //"Motor current"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->ex.current;
			vtp[var].ptrD32s = &myPtr->ex.decoded.current;
			break;
		case 10: //"Strain[0]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->st.ch[0].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.ext_strain[0];
			break;
		case 11: //"Strain[1]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->st.ch[1].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.ext_strain[1];
			break;
		case 12: //"Strain[2]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->st.ch[2].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.ext_strain[2];
			break;
		case 13: //"Strain[3]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->st.ch[3].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.ext_strain[3];
			break;
		case 14: //"Strain[4]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->st.ch[4].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.ext_strain[4];
			break;
		case 15: //"Strain[5]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->st.ch[5].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.ext_strain[5];
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
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 1: //"Accel X"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->accel.x;
			vtp[var].ptrD32s = &myPtr->decoded.accel.x;
			break;
		case 2: //"Accel Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->accel.y;
			vtp[var].ptrD32s = &myPtr->decoded.accel.y;
			break;
		case 3: //"Accel Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->accel.z;
			vtp[var].ptrD32s = &myPtr->decoded.accel.z;
			break;
		case 4: //"Gyro X"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->gyro.x;
			vtp[var].ptrD32s = &myPtr->decoded.gyro.x;
			break;
		case 5: //"Gyro Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->gyro.y;
			vtp[var].ptrD32s = &myPtr->decoded.gyro.y;
			break;
		case 6: //"Gyro Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->gyro.z;
			vtp[var].ptrD32s = &myPtr->decoded.gyro.z;
			break;
		case 7: //"Pushbutton"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->sw1;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 8: //"Digital inputs"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->digitalIn;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 9: //"Analog[0]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[0];
			vtp[var].ptrD32s = &myPtr->decoded.analog[0];
			break;
		case 10: //Analog[1]
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[1];
			vtp[var].ptrD32s = &myPtr->decoded.analog[1];
			break;
		case 11: //"Analog[2]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[2];
			vtp[var].ptrD32s = &myPtr->decoded.analog[2];
			break;
		case 12: //Analog[3]
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[3];
			vtp[var].ptrD32s = &myPtr->decoded.analog[3];
			break;
		case 13: //"Analog[4]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[4];
			vtp[var].ptrD32s = &myPtr->decoded.analog[4];
			break;
		case 14: //Analog[5]
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[5];
			vtp[var].ptrD32s = &myPtr->decoded.analog[5];
			break;
		case 15: //"Analog[6]"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[6];
			vtp[var].ptrD32s = &myPtr->decoded.analog[6];
			break;
		case 16: //Analog[7]
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->analog[7];
			vtp[var].ptrD32s = &myPtr->decoded.analog[7];
			break;
		case 17: //"Status"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->status1;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
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
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 1: //"Accel X"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->accel.x;
			vtp[var].ptrD32s = &myPtr->decoded.accel.x;
			break;
		case 2: //"Accel Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->accel.y;
			vtp[var].ptrD32s = &myPtr->decoded.accel.y;
			break;
		case 3: //"Accel Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->accel.z;
			vtp[var].ptrD32s = &myPtr->decoded.accel.z;
			break;
		case 4: //"Gyro X"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->gyro.x;
			vtp[var].ptrD32s = &myPtr->decoded.gyro.x;
			break;
		case 5: //"Gyro Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->gyro.y;
			vtp[var].ptrD32s = &myPtr->decoded.gyro.y;
			break;
		case 6: //"Gyro Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->gyro.z;
			vtp[var].ptrD32s = &myPtr->decoded.gyro.z;
			break;
		case 7: //"Magneto X"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->magneto.x;
			vtp[var].ptrD32s = &myPtr->decoded.magneto.x;
			break;
		case 8: //"Magneto Y"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->magneto.y;
			vtp[var].ptrD32s = &myPtr->decoded.magneto.y;
			break;
		case 9: //"Magneto Z"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->magneto.z;
			vtp[var].ptrD32s = &myPtr->decoded.magneto.z;
			break;
		case 10: //"IO 1"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->io[0];
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 11: //"IO 2"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->io[1];
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 12: //"Capsense 1"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->capsense[0];
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 13: //"Capsense 2"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->capsense[1];
			vtp[var].ptrD32s = &nullVar32s;
		case 14: //"Capsense 3"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->capsense[2];
			vtp[var].ptrD32s = &nullVar32s;
		case 15: //"Capsense 4"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->capsense[3];
			vtp[var].ptrD32s = &nullVar32s;
		case 16: //"Status"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->status;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
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
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 1: //"Voltage"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->voltage;
			vtp[var].ptrD32s = &myPtr->decoded.voltage;
			break;
		case 2: //"Current"
			vtp[var].format = FORMAT_16S;
			vtp[var].ptr16s = &myPtr->current;
			vtp[var].ptrD32s = &myPtr->decoded.current;
			break;
		case 3: //"Power"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &nullVar16u;
			vtp[var].ptrD32s = &myPtr->decoded.power;
			break;
		case 4: //"Temperature"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->temp;
			vtp[var].ptrD32s = &myPtr->decoded.temp;
			break;
		case 5: //"Pushbutton"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->pushbutton;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 6: //"Status"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->status;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
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
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
			break;
		case 1: //"Ch 1"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->ch[0].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.strain[0];
			break;
		case 2: //"Ch 2"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->ch[1].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.strain[1];
			break;
		case 3: //"Ch 3"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->ch[2].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.strain[2];
			break;
		case 4: //"Ch 4"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->ch[3].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.strain[3];
			break;
		case 5: //"Ch 5"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->ch[4].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.strain[4];
			break;
		case 6: //"Ch 6"
			vtp[var].format = FORMAT_16U;
			vtp[var].ptr16u = &myPtr->ch[5].strain_filtered;
			vtp[var].ptrD32s = &myPtr->decoded.strain[5];
			break;
/*
		case 7: //"Status"
			vtp[var].format = FORMAT_8U;
			vtp[var].ptr8u = &myPtr->status;
			vtp[var].ptrD32s = &nullVar32s;
			break;
*/

		default:
			vtp[var].format = FORMAT_32S;
			vtp[var].ptr32s = &nullVar32s;
			vtp[var].ptrD32s = &nullVar32s;
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
			updateVarList(0, ui->cBoxvar1);
			assignVariable(0);
			//qDebug() << "Only change #1";
		}
		else
		{
			//Tracking, let's update all channels at once:
			qDebug() << "Change all slaves (tracking #1)";

			ui->cBoxvar2slave->setCurrentIndex(ui->cBoxvar1slave->currentIndex());
			ui->cBoxvar3slave->setCurrentIndex(ui->cBoxvar1slave->currentIndex());
			ui->cBoxvar4slave->setCurrentIndex(ui->cBoxvar1slave->currentIndex());
			ui->cBoxvar5slave->setCurrentIndex(ui->cBoxvar1slave->currentIndex());
			ui->cBoxvar6slave->setCurrentIndex(ui->cBoxvar1slave->currentIndex());

			updateVarList(0, ui->cBoxvar1);
			updateVarList(1, ui->cBoxvar2);
			updateVarList(2, ui->cBoxvar3);
			updateVarList(3, ui->cBoxvar4);
			updateVarList(4, ui->cBoxvar5);
			updateVarList(5, ui->cBoxvar6);
			assignVariable(0);
			assignVariable(1);
			assignVariable(2);
			assignVariable(3);
			assignVariable(4);
			assignVariable(5);
		}
	}
}

void W_2DPlot::on_cBoxvar2slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		updateVarList(1, ui->cBoxvar2);
		assignVariable(1);
	}
}

void W_2DPlot::on_cBoxvar3slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		updateVarList(2, ui->cBoxvar3);
		assignVariable(2);
	}
}

void W_2DPlot::on_cBoxvar4slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		updateVarList(3, ui->cBoxvar4);
		assignVariable(3);
	}
}

void W_2DPlot::on_cBoxvar5slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		updateVarList(4, ui->cBoxvar5);
		assignVariable(4);
	}
}

void W_2DPlot::on_cBoxvar6slave_currentIndexChanged(int index)
{
	(void)index;	//Unused for now

	if(initFlag == false)
	{
		saveCurrentSettings();
		updateVarList(5, ui->cBoxvar6);
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
	ui->cBoxvar1->setCurrentIndex(1);
	ui->cBoxvar2->setCurrentIndex(2);
	ui->cBoxvar3->setCurrentIndex(3);
	ui->cBoxvar4->setCurrentIndex(4);
	ui->cBoxvar5->setCurrentIndex(5);
	ui->cBoxvar6->setCurrentIndex(6);
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
		qlsData[i]->setPointsVisible(pointsVisible);
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
