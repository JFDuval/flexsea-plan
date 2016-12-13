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
#include <QtCharts>
#include <QtCharts/QChartView>
#include "flexsea_generic.h"
#include <QtCharts/QXYSeries>

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

//Variable formats:
#define FORMAT_32U					0
#define FORMAT_32S					1
#define FORMAT_16U					2
#define FORMAT_16S					3
#define FORMAT_8U					4
#define FORMAT_8S					5

//Test code:
#define VECLEN	200

//VTP = Variable To Plot
struct vtp_s
{
	int32_t *ptr32s, *ptrD32s;
	uint32_t *ptr32u;
	int16_t *ptr16s;
	uint16_t *ptr16u;
	int8_t *ptr8s;
	uint8_t *ptr8u;
	uint8_t format, decode;
	bool used;
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
	explicit W_2DPlot(QWidget *parent = 0);
	~W_2DPlot();

	//Function(s):

public slots:
	void refresh2DPlot(void);
	void refreshControl(void);

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
	void genTestData(void);
	void on_pbIMU_clicked();
	void on_pbPoints_clicked();

signals:
	void windowClosed(void);

private:
	//Variables & Objects:
	Ui::W_2DPlot *ui;
	QChart *chart;
	QChartView *chartView;
	QLineSeries *qlsData[VAR_NUM];
	int plot_xmin, plot_ymin, plot_xmax, plot_ymax, plot_len;
	uint8_t data_to_plot[VAR_NUM];
	int graph_ylim[2*VAR_NUM];
	bool allChannelUnused(void);
	int plotting_len;
	QStringList var_list_margin;
	bool plotFreezed, initFlag;

	struct vtp_s vtp[6];
	uint8_t varToPlotFormat[6];
	int32_t nullVar32s;
	uint32_t nullVar32u;
	int16_t nullVar16s;
	uint16_t nullVar16u;
	int8_t nullVar8s;
	uint8_t nullVar8u;

	uint8_t slaveIndex[VAR_NUM], slaveAddr[VAR_NUM], slaveBType[VAR_NUM];
	uint8_t varIndex[VAR_NUM];
	int64_t stats[VAR_NUM][STATS_FIELDS];
	int32_t myFakeData;

	//Function(s):
	void initChart(void);
	void initUserInput(void);
	void setChartAxis(void);
	uint8_t select_plot_slave(uint8_t index);
	void addMargins(int *ymin, int *ymax);
	void updateVarList(uint8_t var, QComboBox *myCombo);
	void saveCurrentSettings(void);
	void assignVariable(uint8_t var);
	void assignVariableEx(uint8_t var, struct execute_s *myPtr);
	void assignVariableMn(uint8_t var, struct manage_s *myPtr);
	void assignVariableGo(uint8_t var, struct gossip_s *myPtr);
	void assignVariableBa(uint8_t var, struct battery_s *myPtr);
	void assignVariableSt(uint8_t var, struct strain_s *myPtr);
	void assignVariableRicnu(uint8_t var, struct ricnu_s *myPtr);
	void initStats(void);
	void refreshStats(void);

	//Test 12/12/2016:
	void saveNewPoints(int myDataPoints[6]);
	void setChartAxisAutomatic(void);
	QVector<int> myVector0, myVectorX;
	QDateTime *myTime;
	float getRefreshRate(void);
	bool pointsVisible;
	void computeStats(void);
	QLineSeries qlsDataBuffer[6];
	int globalYmin, globalYmax;
	int vecLen;
	void initData(void);
};



#endif // W_2DPLOT_H
