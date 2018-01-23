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
#include <QElapsedTimer>
#include "flexsea_generic.h"
#include "main.h"

QT_CHARTS_USE_NAMESPACE

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_2DPlot::W_2DPlot(QWidget *parent,
				   FlexseaDevice* devLogInit,
				   DisplayMode mode,
				   QList<FlexseaDevice*> *devListInit,
				   QString activeSlave) :
	QWidget(parent),
	ui(new Ui::W_2DPlot)
{
	ui->setupUi(this);

	setWindowTitle(this->getDescription());
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	liveDevList = devListInit;

	logIndex = 0;

	initFlag = true;
	initPtr();
	initChart();

	// Big part of the init have been moved to this function.
	lastDisplayMode = (DisplayMode)1000; // To force the init
	updateDisplayMode(mode, devLogInit);

	useOpenGL(false);
	drawingTimer = new QTimer(this);
	drawingTimer->setTimerType(Qt::CoarseTimer);
	drawingTimer->setInterval(40);
	drawingTimer->setSingleShot(false);
	connect(drawingTimer, &QTimer::timeout, this, &W_2DPlot::refresh2DPlot);

	selectSlave(activeSlave);
}

W_2DPlot::~W_2DPlot()
{
	emit windowClosed();
	delete drawingTimer;
	drawingTimer = nullptr;
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
	int val;
	//For every variable:
	for(int row = 0; row < VAR_NUM; row++)
	{
		if(!vtp[row].used) continue;

		if(!vtp[row].decode)
		{
			if((vtp[row].rawGenPtr) == nullptr)
			{
				val = 0;
			}
			else
			{
				switch(vtp[row].format)
				{
					case FORMAT_32S:
						val = (*(int32_t*)vtp[row].rawGenPtr);
						scale(row, &val);
						break;
					case FORMAT_32U:
						val = (int)(*(uint32_t*)vtp[row].rawGenPtr);
						scale(row, &val);
						break;
					case FORMAT_16S:
						val = (int)(*(int16_t*)vtp[row].rawGenPtr);
						scale(row, &val);
						break;
					case FORMAT_16U:
						val = (int)(*(uint16_t*)vtp[row].rawGenPtr);
						scale(row, &val);
						break;
					case FORMAT_8S:
						val = (int)(*(int8_t*)vtp[row].rawGenPtr);
						scale(row, &val);
						break;
					case FORMAT_8U:
						val = (int)(*(uint8_t*)vtp[row].rawGenPtr);
						scale(row, &val);
						break;
					default:
						val = 0;
						break;
				}
			}
		}
		else
		{
			if((vtp[row].decodedPtr) == nullptr)
			{
				val = 0;
			}
			else
			{
				val = (*vtp[row].decodedPtr);
			}
		}

		saveNewPoint(row, val);
	}
	dataRate = getRefreshRateData();
}

void W_2DPlot::refresh2DPlot(void)
{
	uint8_t index = 0;

	//Refresh Stat Bar:
	refreshStatBar(getRefreshRateDisplay(), dataRate);

	//For every variable:
	for(index = 0; index < VAR_NUM; index++)
	{
		qlsChart[index]->setVisible(vtp[index].used);
	}

	//And now update the display:
	if(!plotFreezed)
	{
		computeStats();
		computeGlobalMinMax();
		setChartAxisAutomatic();

		for(int i = 0; i < VAR_NUM; i++)
		{
			if(vtp[i].used && !vDataBuffer[i].isEmpty())
			{
				qlsChart[i]->replace(vDataBuffer[i]);
			}

			(*lbMin[i])->setText(QString::number(stats[i][STATS_MIN]));
			(*lbMax[i])->setText(QString::number(stats[i][STATS_MAX]));
			(*lbDiff[i])->setText(QString::number(stats[i][STATS_DIFF]));
			(*lbAvg[i])->setText(QString::number(stats[i][STATS_AVG]));
		}
	}
}

void W_2DPlot::refreshDisplayLog(int index, FlexseaDevice * devPtr)
{
	(void)devPtr;
	logIndex = index;

	saveNewPointsLog(index);
	refresh2DPlot();
}

void W_2DPlot::updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr)
{
	displayMode = mode;
	if(displayMode != lastDisplayMode)
	{
		if(displayMode == DisplayLogData)
		{
			logDevList.clear();
			logDevList.append(devPtr);
			currentDevList = &logDevList;

			initUserInput();
			initData();

			ui->pushButtonFreeze->setDisabled(true);
			ui->radioButtonXA->setChecked(0);
			ui->radioButtonXM->setChecked(1);
			ui->radioButtonXA->setDisabled(true);
			ui->radioButtonXM->setDisabled(true);
			ui->lineEditXMin->setDisabled(false);
			ui->lineEditXMax->setDisabled(false);
		}
		else
		{
			currentDevList = liveDevList;

			initUserInput();
			initData();

			ui->pushButtonFreeze->setDisabled(false);
			ui->radioButtonXA->setChecked(1);
			ui->radioButtonXM->setChecked(0);
			ui->radioButtonXA->setDisabled(false);
			ui->radioButtonXM->setDisabled(false);
			ui->lineEditXMin->setDisabled(true);
			ui->lineEditXMax->setDisabled(true);
		}
	}

	lastDisplayMode = displayMode;
}

void W_2DPlot::activeSlaveStreaming(QString slaveName)
{
	selectSlave(slaveName);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_2DPlot::saveScreenshot(void)
{
	QPixmap originalPixmap = this->grab();

	const QString format = "png";
	QString path = QDir::currentPath();
	QString dateTime =	QDate::currentDate().toString("yyyy-MM-dd_") + \
						QTime::currentTime().toString("HH'h'mm'm'ss's'");
	path += tr("/Plan-GUI-Logs/")+ dateTime + "." + format;

	if (!originalPixmap.save(path)) {
		QMessageBox::warning(this, tr("Save Error"), tr("The image could not be saved to \"%1\".")
							 .arg(QDir::toNativeSeparators(path)));
	}
}

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

	lbDiff[0]= &ui->label_diff_1;
	lbDiff[1]= &ui->label_diff_2;
	lbDiff[2]= &ui->label_diff_3;
	lbDiff[3]= &ui->label_diff_4;
	lbDiff[4]= &ui->label_diff_5;
	lbDiff[5]= &ui->label_diff_6;

	lbAvg[0] = &ui->label_1_avg;
	lbAvg[1] = &ui->label_2_avg;
	lbAvg[2] = &ui->label_3_avg;
	lbAvg[3] = &ui->label_4_avg;
	lbAvg[4] = &ui->label_5_avg;
	lbAvg[5] = &ui->label_6_avg;
}

void W_2DPlot::initChart(void)
{
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

	const QValidator *validator = new QIntValidator(-10000000, 10000000, this);
	ui->lineEditXMin->setValidator(validator);
	ui->lineEditXMax->setValidator(validator);
	ui->lineEditYMin->setValidator(validator);
	ui->lineEditYMax->setValidator(validator);

	chart->createDefaultAxes();
	chart->axisX()->setRange(INIT_PLOT_XMIN, INIT_PLOT_XMAX);
	chart->axisY()->setRange(INIT_PLOT_YMIN, INIT_PLOT_YMAX);

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
	connect(qlsChart[0],	&QLineSeries::hovered,\
			this,			&W_2DPlot::myHoverHandler0);
	connect(qlsChart[1],	&QLineSeries::hovered,\
			this,			&W_2DPlot::myHoverHandler1);
	connect(qlsChart[2],	&QLineSeries::hovered,\
			this,			&W_2DPlot::myHoverHandler2);
	connect(qlsChart[3],	&QLineSeries::hovered,\
			this,			&W_2DPlot::myHoverHandler3);
	connect(qlsChart[4],	&QLineSeries::hovered,\
			this,			&W_2DPlot::myHoverHandler4);
	connect(qlsChart[5],	&QLineSeries::hovered,\
			this,			&W_2DPlot::myHoverHandler5);
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

	chart->axisX()->setRange(plot_xmin, plot_xmax);
	chart->axisY()->setRange(plot_ymin, plot_ymax);

	if(displayMode == DisplayLogData)
	{
		ui->radioButtonXA->setChecked(0);
		ui->radioButtonXM->setChecked(1);
		ui->lineEditXMin->setDisabled(false);
		ui->lineEditXMax->setDisabled(false);
	}
	else
	{
		ui->radioButtonXA->setChecked(1);
		ui->radioButtonXM->setChecked(0);
		ui->lineEditXMin->setDisabled(true);
		ui->lineEditXMax->setDisabled(true);
	}
	ui->radioButtonYA->setChecked(1);
	ui->radioButtonYM->setChecked(0);

	ui->lineEditXMin->setText(QString::number(plot_xmin));
	ui->lineEditXMax->setText(QString::number(plot_xmax));
	ui->lineEditYMin->setText(QString::number(plot_ymin));
	ui->lineEditYMax->setText(QString::number(plot_ymax));

	ui->lineEditYMin->setDisabled(true);
	ui->lineEditYMax->setDisabled(true);

	ui->comboBoxMargin->clear();
	var_list_margin.clear();

	//Margin options:
	var_list_margin << "±2%" << "±5%" << "±10%" << "±5 ticks" << "±10 ticks" \
					<< "±25 ticks" << "±100 ticks" << "±1000 ticks";
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


		for(int j = 0; j < currentDevList->length(); ++j)
		{
			(*cbVarSlave[i])->addItem((*currentDevList)[j]->slaveName);
		}
		(*cbVarSlave[i])->blockSignals(false);

		//Variable comboBoxes:
		saveCurrentSettings(i);  //Needed for the 1st var_list
	}

	//Decode Checkbox tooltips:
	QString ttip = "<html><head/><body><p>Plot data in physical units (instead \
					of ticks)</p></body></html>";

	for(int item = 0; item < VAR_NUM; item++)
	{
		updateVarList(item);
		(*ckbDecode[item])->setToolTip(ttip);
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

	//Scaling:
	initScaling();

	//Init flag:
	initFlag = false;
}

//Empties all the lists
void W_2DPlot::initData(void)
{
	for(int i = 0; i < VAR_NUM; i++)
	{
		vDataBuffer[i].clear();
		qlsChart[i]->replace(vDataBuffer[i]);
	}

	initStats();
}

//Initialize the scaling boxes (y=mx+b)
void W_2DPlot::initScaling(void)
{
	const QValidator *validator = new QIntValidator(-1000000, 1000000, this);

	//Default: unity gain, no offset:
	ui->lineEditM1->setText(QString::number(SCALE_DEFAULT_M));
	ui->lineEditM2->setText(QString::number(SCALE_DEFAULT_M));
	ui->lineEditM3->setText(QString::number(SCALE_DEFAULT_M));
	ui->lineEditM4->setText(QString::number(SCALE_DEFAULT_M));
	ui->lineEditM5->setText(QString::number(SCALE_DEFAULT_M));
	ui->lineEditM6->setText(QString::number(SCALE_DEFAULT_M));
	ui->lineEditB1->setText(QString::number(SCALE_DEFAULT_B));
	ui->lineEditB2->setText(QString::number(SCALE_DEFAULT_B));
	ui->lineEditB3->setText(QString::number(SCALE_DEFAULT_B));
	ui->lineEditB4->setText(QString::number(SCALE_DEFAULT_B));
	ui->lineEditB5->setText(QString::number(SCALE_DEFAULT_B));
	ui->lineEditB6->setText(QString::number(SCALE_DEFAULT_B));

	for(int i = 0; i < VAR_NUM; i++)
	{
		scaling[i][0] = SCALE_DEFAULT_M;
		scaling[i][1] = SCALE_DEFAULT_B;
	}

	//Numbers only:
	ui->lineEditB1->setValidator(validator);
	ui->lineEditB2->setValidator(validator);
	ui->lineEditB3->setValidator(validator);
	ui->lineEditB4->setValidator(validator);
	ui->lineEditB5->setValidator(validator);
	ui->lineEditB6->setValidator(validator);
	ui->lineEditM1->setValidator(validator);
	ui->lineEditM2->setValidator(validator);
	ui->lineEditM3->setValidator(validator);
	ui->lineEditM4->setValidator(validator);
	ui->lineEditM5->setValidator(validator);
	ui->lineEditM6->setValidator(validator);
}

//Updates 6 buffers, and compute stats (min/max/avg/...)
void W_2DPlot::saveNewPoint(int row, int data)
{
	if(row < 0 || row >= VAR_NUM || !vtp[row].used) return;

	// add point if plot lenght not reached
	if(vDataBuffer[row].length() < plot_len)
	{
		//First VECLEN points: append
		vDataBuffer[row].append(QPointF(vDataBuffer[row].length(), data));
	}
	// replace point if max length reached
	else
	{
		vDataBuffer[row].removeFirst();
		vDataBuffer[row].append(QPointF(plot_len-1, data));
	}

	// Fill the x value properly
	int bufLength = vDataBuffer[row].size();

	for(int j = 0; j < bufLength; j++)
	{
		vDataBuffer[row][j].setX(j);
	}
}

void W_2DPlot::saveNewPointsLog(int index)
{
	struct std_variable varHandle;
	int point;

	initData();

	for(int item = 0; item < VAR_NUM; item++)
	{

		// Manage the starting point for parsing the data
		// set the data iterator to start with the index desired
		int dataIter = index;

		// Set the plot iterator to the begining
		int graphIter = 0;

		// Add data until one of the limit is reached.
		while(dataIter < selectedDevList[item]->length() &&
			  graphIter < plot_len &&
			  varIndex[item] > 0)
		{
			varHandle = selectedDevList[item]->getSerializedVar(varIndex[item] + 1, dataIter);

			if(vtp[item].decode == false)
			{
				if(varHandle.rawGenPtr == nullptr)
				{
					point = 0;
				}
				else
				{
					switch(varHandle.format)
					{
						case FORMAT_32S:
							point = (*(int32_t*)varHandle.rawGenPtr);
							scale(item, &point);
							break;
						case FORMAT_32U:
							point = (int)(*(uint32_t*)varHandle.rawGenPtr);
							scale(item, &point);
							break;
						case FORMAT_16S:
							point = (int)(*(int16_t*)varHandle.rawGenPtr);
							scale(item, &point);
							break;
						case FORMAT_16U:
							point = (int)(*(uint16_t*)varHandle.rawGenPtr);
							scale(item, &point);
							break;
						case FORMAT_8S:
							point = (int)(*(int8_t*)varHandle.rawGenPtr);
							scale(item, &point);
							break;
						case FORMAT_8U:
							point = (int)(*(uint8_t*)varHandle.rawGenPtr);
							scale(item, &point);
							break;
						default:
							point = 0;
							qDebug() << "Generic pointer format not supported";
							break;
					}
				}
			}
			else
			{
				if((varHandle.decodedPtr) == nullptr)
				{
					point = 0;
				}
				else
				{
					point = (*varHandle.decodedPtr);
				}
			}

			// Append the proper value
			vDataBuffer[item].append(QPointF(graphIter, point));
			++dataIter;
			++graphIter;
		}
	}
}

void W_2DPlot::computeStats(void)
{
	qreal min, max;
	qreal avg = 0;

	// Compute the Stats
	for(int i = 0; i < VAR_NUM; i++)
	{
		if(vDataBuffer[i].length() > 0 && vtp[i].used)
		{
			min = vDataBuffer[i].at(0).y();
			max = vDataBuffer[i].at(0).y();
			avg = 0;

			for(int j = 0; j < vDataBuffer[i].length(); j++)
			{
				//Minimum:
				if(vDataBuffer[i].at(j).y() < min)
				{
					min = vDataBuffer[i].at(j).y();
				}

				//Maximum:
				if(vDataBuffer[i].at(j).y() > max)
				{
					max = vDataBuffer[i].at(j).y();
				}

				//Average - sum:
				avg += vDataBuffer[i].at(j).y();

			}
			//Average - result:
			avg = avg / vDataBuffer[i].length();

			//Save:
			stats[i][STATS_MIN] = (int64_t) min;
			stats[i][STATS_MAX] = (int64_t) max;
			stats[i][STATS_DIFF]= (int64_t) max - min;
			stats[i][STATS_AVG] = (int64_t) avg;
		}
		else
		{
			stats[i][STATS_MIN] = 0;
			stats[i][STATS_MAX] = 0;
			stats[i][STATS_DIFF]= 0;
			stats[i][STATS_AVG] = 0;
		}
	}
}

//Get global min & max:
void W_2DPlot::computeGlobalMinMax(void)
{
	//Stats for all channels:

	if(allChannelUnused())
	{
		globalYmin = -10;
		globalYmax = 10;
	}
	else
	{
		int i;
		//First, we use the 1st used channel to initialize the global min/max:
		for(i = 0; i < VAR_NUM; i++)
		{
			if(vtp[i].used)
			{
				globalYmin = stats[i][STATS_MIN];
				globalYmax = stats[i][STATS_MAX];
				break;
			}
		}

		//Now we can compare, starting at the next value of i:
		for(i = i+1; i < VAR_NUM; i++)
		{
			//We only use the 'used' channels for the global min/max:
			if(vtp[i].used)
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
	const int SIZE_AVG = 8;

	static QElapsedTimer timer;
	if(!timer.isValid())
	{
		timer.start();
		return -1;
	}

	int64_t msec = timer.elapsed();
	timer.restart();

	float t_s = 0.0, f = 0.0, avg = 0.0;
	static int counter = 0;
	static float fArray[SIZE_AVG] = {0};

	//Actual frequency:
	t_s = msec;
	f = 1000.0f/t_s;

	//Average:
	fArray[counter] = f;
	counter++;
	counter %=SIZE_AVG;
	avg = 0;
	for(int i = 0; i < SIZE_AVG; i++)
	{
		avg += fArray[i];
	}
	avg = avg / SIZE_AVG;

	return avg;
}

//Returns the rate at which it is called, in Hz. Average of 8 values
//Different approach than for the Display function because 200Hz
//is fast for a ms timer.
float W_2DPlot::getRefreshRateData(void)
{
	const int SIZE_AVG = 8;
	static int counter = 0;
	static float fArray[SIZE_AVG] = {0};
	static int callCounter = 0;

	static QElapsedTimer timer;
	if(!timer.isValid())
	{
		timer.start();
		return -1;
	}

	if(!callCounter)
	{
		int64_t msec = timer.elapsed();
		timer.restart();

		float t_s = msec / 10.0f;
		float f = 1000.0f/t_s;

		//place into frequency array
		fArray[counter] = f;
		counter++;
		counter%=SIZE_AVG;
	}
	callCounter++;
	callCounter%= 10;

	//compute average
	float avg = 0;
	for(int i = 0; i < SIZE_AVG; i++)
	{
		avg += fArray[i];
	}
	avg = avg / SIZE_AVG;

	return avg;
}

//Based on the current state of comboBoxes, saves the info in variables
void W_2DPlot::saveCurrentSettings(int item)
{
	//Slave:
	selectedDevList[item] = (*currentDevList)[(*cbVarSlave[item])->currentIndex()];

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
		case 3: //5 ticks
			*ymin = (*ymin-5);
			*ymax = (*ymax+5);
			break;
		case 4: //10 ticks
			*ymin = (*ymin-10);
			*ymax = (*ymax+10);
			break;
		case 5: //25 ticks
			*ymin = (*ymin-25);
			*ymax = (*ymax+25);
			break;
		case 6: //100 ticks
			*ymin = (*ymin-100);
			*ymax = (*ymax+100);
			break;
		case 7: //1000 ticks
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

		//Save values:
		plot_xmin = tmpXmin;
		plot_xmax = tmpXmax;
	}
	else if(ui->radioButtonXA->isChecked())
	{
		//Disable manual input fields:
		ui->lineEditXMin->setEnabled(false);
		ui->lineEditXMax->setEnabled(false);

		//Auto scale axis
		plot_xmin = 0;
		plot_xmax = vDataBuffer[0].length();

		// Limit the minimum value
		if(plot_xmax < 1)
		{
			plot_xmax = 1;
		}
	}

	//Notify user of value used:
	ui->lineEditXMin->setText(QString::number(plot_xmin));;
	ui->lineEditXMax->setText(QString::number(plot_xmax));;

	plot_len = 1+ plot_xmax - plot_xmin;

	if(plot_len < lastPlotLen)
	{
		initData();
	}
	lastPlotLen = plot_len;

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
		while(plot_ymax <= plot_ymin)
		{
			plot_ymin -= 5;
			plot_ymax += 5;
		}

		//Apply a margin:
		addMargins(&plot_ymin, &plot_ymax);

		//Update chart:
		chart->axisX()->setRange(plot_xmin, plot_xmax);
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
		if(vtp[i].used)
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
		(*lbDiff[i])->setText(QString::number(0));
		(*lbAvg[i])->setText(QString::number(0));
	}
}

void W_2DPlot::refreshStats(void)
{
	for(int i = 0; i < VAR_NUM; ++i)
	{
		(*lbMin[i])->setText(QString::number(stats[i][STATS_MIN]));
		(*lbMax[i])->setText(QString::number(stats[i][STATS_MAX]));
		(*lbDiff[i])->setText(QString::number(stats[i][STATS_DIFF]));
		(*lbAvg[i])->setText(QString::number(stats[i][STATS_AVG]));
	}
}

//Displays the 2 refresh frequencies
void W_2DPlot::refreshStatBar(float fDisp, float fData)
{
	QString txt, num;

	num = displayMode == DisplayLogData ? "--" : QString::number(fDisp, 'f', 0);
	txt = "<font color=#808080>Display: " + num + " Hz </font>";
	ui->label_refreshRateDisplay->setText(txt);


	num = displayMode == DisplayLogData ? "--" : QString::number(fData, 'f', 0);
	txt = "<font color=#808080>Data: " + num + " Hz </font>";
	ui->label_refreshRateData->setText(txt);
}

//Each board type has a different variable list.
void W_2DPlot::updateVarList(uint8_t item)
{
	//Fill the comboBox:
	(*cbVar[item])->clear();
	(*cbVar[item])->setToolTipDuration(350);

	QStringList headerList(selectedDevList[item]->getHeader());
	QStringList headerDecList(selectedDevList[item]->getHeaderUnit());

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
	struct std_variable varHandle = selectedDevList[item]->getSerializedVar(varIndex[item] + 1);

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

	if(displayMode == DisplayLogData)
	{
		saveNewPointsLog(logIndex);
		refresh2DPlot();
	}

	if(allChannelUnused()) drawingTimer->stop();
	else if(!drawingTimer->isActive()) drawingTimer->start();
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
	initStats();

	for(int item = 0; item < VAR_NUM; item++)
	{
		//Special case for Rigid:
		if((*cbVarSlave[item])->count()-1 == (*cbVarSlave[item])->currentIndex())
			(*cbVar[item])->setCurrentIndex(item + 4);
		else
			(*cbVar[item])->setCurrentIndex(item + 2);
	}
}

void W_2DPlot::on_pbScreenShot_clicked()
{
	saveScreenshot();
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

//All the scaling lineEdit slots redirect here
void W_2DPlot::updateScalingFactors(uint8_t var, uint8_t param, QString txt)
{
	int32_t num = txt.toInt();

	if((param > 1) || (var > (VAR_NUM-1)))
	{
		qDebug() << "Invalid parameter, scaling unchaged.";
		return;
	}

	//Change array:
	scaling[var][param] = num;
}

//Apply the scaling factors to the variable
void W_2DPlot::scale(uint8_t item, int *value)
{
	(*value) = (*value)*scaling[item][0] + scaling[item][1];
}

void W_2DPlot::selectSlave(QString slaveName)
{
	for(int i = 0; i < currentDevList->length(); ++i)
	{
		if((*currentDevList)[i]->slaveName == slaveName)
		{
			(*cbVarSlave[0])->setCurrentIndex(i);
		}
	}
}

void W_2DPlot::on_lineEditM1_textEdited(const QString &arg1)
{
	updateScalingFactors(0, 0, arg1);
}

void W_2DPlot::on_lineEditM2_textEdited(const QString &arg1)
{
	updateScalingFactors(1, 0, arg1);
}

void W_2DPlot::on_lineEditM3_textEdited(const QString &arg1)
{
	updateScalingFactors(2, 0, arg1);
}

void W_2DPlot::on_lineEditM4_textEdited(const QString &arg1)
{
	updateScalingFactors(3, 0, arg1);
}

void W_2DPlot::on_lineEditM5_textEdited(const QString &arg1)
{
	updateScalingFactors(4, 0, arg1);
}

void W_2DPlot::on_lineEditM6_textEdited(const QString &arg1)
{
	updateScalingFactors(5, 0, arg1);
}

void W_2DPlot::on_lineEditB1_textEdited(const QString &arg1)
{
	updateScalingFactors(0, 1, arg1);
}

void W_2DPlot::on_lineEditB2_textEdited(const QString &arg1)
{
	updateScalingFactors(1, 1, arg1);
}

void W_2DPlot::on_lineEditB3_textEdited(const QString &arg1)
{
	updateScalingFactors(2, 1, arg1);
}

void W_2DPlot::on_lineEditB4_textEdited(const QString &arg1)
{
	updateScalingFactors(3, 1, arg1);
}

void W_2DPlot::on_lineEditB5_textEdited(const QString &arg1)
{
	updateScalingFactors(4, 1, arg1);
}

void W_2DPlot::on_lineEditB6_textEdited(const QString &arg1)
{
	updateScalingFactors(5, 1, arg1);
}
