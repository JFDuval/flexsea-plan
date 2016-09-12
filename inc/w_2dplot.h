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
	*
****************************************************************************/

#ifndef W_2DPLOT_H
#define W_2DPLOT_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QApplication>
#include <QWidget>
#include <QtCharts>
#include <QtCharts/QChartView>
#include "flexsea_generic.h"

//****************************************************************************
// Definition(s)
//****************************************************************************

#define INIT_PLOT_XMIN              0
#define INIT_PLOT_XMAX              200
#define INIT_PLOT_YMIN              -10000
#define INIT_PLOT_YMAX              10000
#define INIT_PLOT_LEN               ((INIT_PLOT_XMAX-INIT_PLOT_XMIN)+1)
#define VAR_NUM                     6
#define PLOT_BUF_LEN                1000

#define TWO_PI                      (2*3.14159)
#define PHASE_INCREMENT             (TWO_PI/75)
#define A_GAIN                      1000

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class W_2DPlot;
}

class W_2DPlot : public QWidget
{
    Q_OBJECT

public:
	//Constructor & Destructor:
    explicit W_2DPlot(QWidget *parent = 0);
    ~W_2DPlot();
	
	//Function(s):
    void refresh2DPlot(void);

public slots:

private slots:
    void on_radioButtonXA_clicked();
    void on_radioButtonXM_clicked();
	void on_radioButtonYA_clicked();
    void on_radioButtonYM_clicked();
    void on_lineEditXMin_returnPressed();
    void on_lineEditXMax_returnPressed();
    void on_lineEditYMin_returnPressed();
    void on_lineEditYMax_returnPressed();

private:
	//Variables & Objects:
    Ui::W_2DPlot *ui;
	QChart *chart;
	QChartView *chartView;
	FlexSEA_Generic myFlexSEA_Generic;
    QLineSeries *qlsData[VAR_NUM];
    QLineSeries *mySeriesTest;
	int graph_xarray[PLOT_BUF_LEN];
    int graph_yarray[VAR_NUM][PLOT_BUF_LEN];
    int plot_xmin, plot_ymin, plot_xmax, plot_ymax, plot_len;
    uint8_t data_to_plot[VAR_NUM];
    int graph_ylim[2*VAR_NUM];
    bool isChannelUsed[VAR_NUM];
    bool allChannelUnused(void);
    int plotting_len;
    QStringList var_list_margin;

	//Function(s):
    void initChart(void);
    void initUserInput(void);
    void gen_graph_xarray(void);
    void init_yarrays(void);
    void setChartAxis(void);
    void refreshData2DPlot(int *x, int *y, int len, uint8_t plot_index);
    void update_plot_buf_single(int *buf, int *idx, int new_data);
    void update_graph_array(int graph, int new_data);
    uint8_t select_plot_slave(uint8_t index);
    int gen_test_data(int phaseShift);
    void array_minmax(int *arr, int len, int *min, int *max);
    void addMargins(int *ymin, int *ymax);
};

#endif // W_2DPLOT_H
