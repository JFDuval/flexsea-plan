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
****************************************************************************/

#ifndef W_2DPLOT_H
#define W_2DPLOT_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QApplication>
#include <QWidget>
#include "counter.h"
#include <QTimer>
#include <QtCharts>
#include <QtCharts/QChartView>
#include "flexsea_generic.h"
#include "flexseaDevice.h"
#include <QtCharts/QXYSeries>
#include "executeDevice.h"
#include "define.h"

//****************************************************************************
// Definition(s)
//****************************************************************************

#define INIT_PLOT_XMIN				0
#define INIT_PLOT_XMAX				200
#define INIT_PLOT_YMIN				-1000
#define INIT_PLOT_YMAX				1000
#define INIT_PLOT_LEN				((INIT_PLOT_XMAX-INIT_PLOT_XMIN)+1)
#define VAR_NUM						6
#define PLOT_BUF_LEN				1000

#define TWO_PI						(2*3.14159)
#define PHASE_INCREMENT				(TWO_PI/75)
#define A_GAIN						1000

//Stats:
#define STATS_FIELDS				4
#define STATS_MIN					0
#define STATS_MAX					1
#define STATS_AVG					2
#define STATS_RMS					3

//Scaling:
#define SCALE_DEFAULT_M				1
#define SCALE_DEFAULT_B				0

//Test code:
//#define VECLEN	200

//VTP = Variable To Plot
struct vtp_s
{
	void *rawGenPtr;
	uint8_t format;
	int32_t *decodedPtr;
	bool used;
	bool decode;
};

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class W_2DPlot;
}

class W_2DPlot : public QWidget, public Counter<W_2DPlot>
{
	Q_OBJECT

public:

	//Constructor & Destructor:
	explicit W_2DPlot(QWidget *parent = 0,
					  FlexseaDevice* devLogInit = nullptr,
					  DisplayMode mode = DisplayLiveData,
					  QList<FlexseaDevice*> *devListInit = nullptr);
	~W_2DPlot();

public slots:

	void receiveNewData(void);
	void refresh2DPlot(void);
	void refreshDisplayLog(int index, FlexseaDevice * devPtr);
	void updateDisplayMode(DisplayMode mode, FlexseaDevice* devPtr);

private slots:

	void on_radioButtonXA_clicked();
	void on_radioButtonXM_clicked();
	void on_radioButtonYA_clicked();
	void on_radioButtonYM_clicked();
	void on_lineEditXMin_returnPressed();
	void on_lineEditXMax_returnPressed();
	void on_lineEditYMin_returnPressed();
	void on_lineEditYMax_returnPressed();
	void on_pushButtonFreeze_clicked();
	void on_cBoxvar1slave_currentIndexChanged(int index);
	void on_cBoxvar2slave_currentIndexChanged(int index);
	void on_cBoxvar3slave_currentIndexChanged(int index);
	void on_cBoxvar4slave_currentIndexChanged(int index);
	void on_cBoxvar5slave_currentIndexChanged(int index);
	void on_cBoxvar6slave_currentIndexChanged(int index);
	void on_cBoxvar1_currentIndexChanged(int index);
	void on_cBoxvar2_currentIndexChanged(int index);
	void on_cBoxvar3_currentIndexChanged(int index);
	void on_cBoxvar4_currentIndexChanged(int index);
	void on_cBoxvar5_currentIndexChanged(int index);
	void on_cBoxvar6_currentIndexChanged(int index);
	void on_checkBoxD1_stateChanged(int arg1);
	void on_checkBoxD2_stateChanged(int arg1);
	void on_checkBoxD3_stateChanged(int arg1);
	void on_checkBoxD4_stateChanged(int arg1);
	void on_checkBoxD5_stateChanged(int arg1);
	void on_checkBoxD6_stateChanged(int arg1);
	void on_pushButtonClear_clicked();
	void on_pbReset_clicked();
	void on_pbIMU_clicked();
	void on_pbPoints_clicked();
	void genTestData(void);
	void myHoverHandler0(QPointF pt, bool state);
	void myHoverHandler1(QPointF pt, bool state);
	void myHoverHandler2(QPointF pt, bool state);
	void myHoverHandler3(QPointF pt, bool state);
	void myHoverHandler4(QPointF pt, bool state);
	void myHoverHandler5(QPointF pt, bool state);
	void myHoverHandlerAll(uint8_t ch, QPointF pt, QPoint cursor, bool state);
	void on_checkBoxOpenGL_clicked(bool checked);

	void on_lineEditM1_textEdited(const QString &arg1);
	void on_lineEditM2_textEdited(const QString &arg1);
	void on_lineEditM3_textEdited(const QString &arg1);
	void on_lineEditM4_textEdited(const QString &arg1);
	void on_lineEditM5_textEdited(const QString &arg1);
	void on_lineEditM6_textEdited(const QString &arg1);

	void on_lineEditB1_textEdited(const QString &arg1);
	void on_lineEditB2_textEdited(const QString &arg1);
	void on_lineEditB3_textEdited(const QString &arg1);
	void on_lineEditB4_textEdited(const QString &arg1);
	void on_lineEditB5_textEdited(const QString &arg1);
	void on_lineEditB6_textEdited(const QString &arg1);

signals:

	void windowClosed(void);

private:
	QTimer* drawingTimer;

	DisplayMode displayMode;

	// GUI Pointer table
	QLabel **lbT[VAR_NUM];
	QComboBox **cbVar[VAR_NUM];
	QComboBox **cbVarSlave[VAR_NUM];
	QCheckBox **ckbDecode[VAR_NUM];
	QLabel **lbMin[VAR_NUM];
	QLabel **lbMax[VAR_NUM];
	QLabel **lbAvg[VAR_NUM];

	//Variables & Objects:

	QList<FlexseaDevice*> *liveDevList;

	QList<FlexseaDevice*> logDevList;

	QList<FlexseaDevice*> *currentDevList;

	int logIndex;

	FlexseaDevice* selectedDevList[VAR_NUM];

	Ui::W_2DPlot *ui;
	QChart *chart;
	QChartView *chartView;
	QLineSeries *qlsChart[VAR_NUM];
	QVector<QPointF> vDataBuffer[VAR_NUM];
	QDateTime *timerRefreshDisplay, *timerRefreshData;
	int plot_xmin, plot_ymin, plot_xmax, plot_ymax, plot_len;
	int globalYmin, globalYmax;

	QStringList var_list_margin;
	bool plotFreezed, initFlag;
	bool pointsVisible;

	struct vtp_s vtp[6];
	uint8_t varToPlotFormat[6];

	uint8_t varIndex[VAR_NUM];
	int64_t stats[VAR_NUM][STATS_FIELDS];
	int32_t myFakeData;
	float dataRate;

	//Scaling:
	int32_t scaling[VAR_NUM][2];

	//Function(s):

	void initChart(void);
	void initUserInput(void);
	void saveNewPoint(int row, int data);
	void saveNewPointsLog(int index);
	void computeStats(void);
	void computeGlobalMinMax(void);
	float getRefreshRateDisplay(void);
	float getRefreshRateData(void);
	void initData(void);
	void initScaling(void);
	void saveCurrentSettings(int item);
	void addMargins(int *ymin, int *ymax);
	void setChartAxis(void);
	void setChartAxisAutomatic(void);
	bool allChannelUnused(void);
	void initStats(void);
	void initPtr(void);
	void refreshStats(void);
	void refreshStatBar(float fDisp, float fData);
	void useOpenGL(bool yesNo);
	void updateScalingFactors(uint8_t var, uint8_t param, QString txt);
	void scale(uint8_t item, int *value);

	void updateVarList(uint8_t var);
	void assignVariable(uint8_t var);
	void assignVariableEx(uint8_t var, struct execute_s *myPtr);
	void assignVariableMn(uint8_t var, struct manage_s *myPtr);
	void assignVariableGo(uint8_t var, struct gossip_s *myPtr);
	void assignVariableBa(uint8_t var, struct battery_s *myPtr);
	void assignVariableSt(uint8_t var, struct strain_s *myPtr);
	void assignVariableRicnu(uint8_t var, struct ricnu_s *myPtr);
};

#endif // W_2DPLOT_H
