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

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_2dplot.h"
#include "ui_w_2dplot.h"
#include <QApplication>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
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

    setWindowTitle("2D Plot");
    setWindowIcon(QIcon(":icons/d_logo_small.png"));

    initUserInput();
    initChart();
}

W_2DPlot::~W_2DPlot()
{
    delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//Gets called by the timer, updates the plot:
void W_2DPlot::refresh2DPlot(void)
{
    uint8_t index = 0;
    struct execute_s *execute_ptr;
    int phaseShift = 0;

    //We go through the list and we update the appropriate data:
    data_to_plot[0] = ui->cBoxvar1->currentIndex();
    data_to_plot[1] = ui->cBoxvar2->currentIndex();
    data_to_plot[2] = ui->cBoxvar3->currentIndex();
    data_to_plot[3] = ui->cBoxvar4->currentIndex();
    data_to_plot[4] = ui->cBoxvar5->currentIndex();
    data_to_plot[5] = ui->cBoxvar6->currentIndex();

    //For every variable:
    for(index = 0; index < VAR_NUM; index++)
    {
        //Point to the selected slave:
        myFlexSEA_Generic.assignExecutePtr(&execute_ptr, SL_BASE_EX, select_plot_slave(index));

        //'Used' as default, 'false' when set at Unused
        isChannelUsed[index] = true;

        //Update buffers with latest results:
        switch(data_to_plot[index])
        {
            case 0: //"**Unused**"
                isChannelUsed[index] = false;
                update_graph_array(index, 0);
                break;
            case 1: //"Accel X"
                update_graph_array(index, execute_ptr->accel.x);
                break;
            case 2: //"Accel Y"
                update_graph_array(index, execute_ptr->accel.y);
                break;
            case 3: //"Accel Z"
                update_graph_array(index, execute_ptr->accel.z);
                break;
            case 4: //"Gyro X"
                update_graph_array(index, execute_ptr->gyro.x);
                break;
            case 5: //"Gyro Y"
                update_graph_array(index, execute_ptr->gyro.y);
                break;
            case 6: //"Gyro Z"
                update_graph_array(index, execute_ptr->gyro.z);
                break;
            case 7: //"Encoder"
                update_graph_array(index, execute_ptr->enc_display);
                break;
            case 8: //"Motor current"
                update_graph_array(index, execute_ptr->current);
                break;
            case 9: //"Analog[0]"
                update_graph_array(index, (int) execute_ptr->analog[0]);
                break;
            case 10: //Analog[1]
                update_graph_array(index, (int) execute_ptr->analog[1]);    //ToDo move up
                break;
            case 11: //"Strain"
                update_graph_array(index, execute_ptr->strain);
                break;
            case 12: //"+VB"
                update_graph_array(index, execute_ptr->volt_batt);
                break;
            case 13: //"+VG"
                update_graph_array(index, execute_ptr->volt_int);
                break;
            case 14: //"Temp"
                update_graph_array(index, execute_ptr->temp);
                break;
            case 15: //"Fake Data"
                phaseShift = (TWO_PI/VAR_NUM)*index;
                update_graph_array(index, gen_test_data((phaseShift)));
                break;
            case 16: //"Setpoint (square)"
                update_graph_array(index, 0);//ctrl_setpoint);   //ToDo Fix
                break;
            case 17: //"Setpoint (trap)"
                update_graph_array(index, 0);//ctrl_setpoint_trap);  //ToDo Fix
                break;
            case 18: //"Strain ch1"
                update_graph_array(index, strain[0].strain_filtered);
                break;
            case 19: //"Strain ch2"
                update_graph_array(index, strain[1].strain_filtered);
                break;
            case 20: //"Strain ch3"
                update_graph_array(index, strain[2].strain_filtered);
                break;
            case 21: //"Strain ch4"
                update_graph_array(index, strain[3].strain_filtered);
                break;
            case 22: //"Strain ch5"
                update_graph_array(index, strain[4].strain_filtered);
                break;
            case 23: //"Strain ch6"
                update_graph_array(index, strain[5].strain_filtered);
                break;
            case 24: //"AS5047 (Mot.)"
                update_graph_array(index, ricnu_1.ex.enc_commut);
                break;
            case 25: //"AS5048 (Joint)"
                update_graph_array(index, ricnu_1.ex.enc_control);
                break;
            default:
                isChannelUsed[index] = false;
                break;
        }

        if(isChannelUsed[index] == false)
        {
            //This channel isn't used, we make it invisible
            qlsData[index]->setVisible(false);
        }
        else
        {
            qlsData[index]->setVisible(true);
        }

        //Plot it:
        refreshData2DPlot(graph_xarray, graph_yarray[index], plot_len, index);
    }
}

//****************************************************************************
// Public slot(s):
//****************************************************************************

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_2DPlot::initChart(void)
{
    //QPen myPen(Qt::red);
    //Fake data:
    qlsData[0] = new QLineSeries();
   // qlsData[0]->setPen(myPen);
    qlsData[0]->append(0, 0);

    //qlsData[0]->setPen(myPen);

    qlsData[1] = new QLineSeries();
    qlsData[1]->append(0, 0);

    mySeriesTest = new QLineSeries();
    mySeriesTest->append(0, 0);

    qlsData[2] = new QLineSeries();
    qlsData[2]->append(0, 0);

    qlsData[3] = new QLineSeries();
    qlsData[3]->append(0, 0);

    qlsData[4] = new QLineSeries();
    qlsData[4]->append(0, 0);

    qlsData[5] = new QLineSeries();
    qlsData[5]->append(0, 0);
    //--

    //Chart:
    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(qlsData[0]);
    chart->addSeries(qlsData[1]);
    chart->addSeries(qlsData[2]);
    chart->addSeries(qlsData[3]);
    chart->addSeries(qlsData[4]);
    chart->addSeries(qlsData[5]);

    //chart->setTitle("Simple spline chart example");
    chart->createDefaultAxes();
    chart->axisX()->setRange(plot_xmin, plot_xmax);
    chart->axisY()->setRange(plot_ymin, plot_ymax);
    //chart->setAnimationOptions(QChart::AllAnimations);

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
        //qDebug() << r << g << b;
        msg[u] = "QLabel { background-color: black; color: rgb(" + \
                QString::number(r) + ',' + QString::number(g) + ','+ \
                QString::number(b) + ");}";
        //qDebug() << msg[u];
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
}

//Fills the fields and combo boxes:
void W_2DPlot::initUserInput(void)
{
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
        data_to_plot[h] = 0;
        isChannelUsed[h] = false;
    }

    //Limits:
    for(int i = 0; i < 2*VAR_NUM; i++)
    {
        graph_ylim[i] = 0;
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

    gen_graph_xarray();
    init_yarrays();

    //Variable option lists:
    QStringList var_list;
    var_list << "**Unused**" << "Accel X" << "Accel Y" << "Accel Z" << "Gyro X" << "Gyro Y" << "Gyro Z" << "Encoder" \
            << "Motor current" << "Analog[0]" << "Analog[1]" << "Strain" << "+VB" << "+VG" << "Temperature" << "Fake Data" << "Setpoint (square)" \
            << "Setpoint (trapezoidal)" << "Strain ch1" << "Strain ch2" << "Strain ch3" << "Strain ch4" << "Strain ch5" << "Strain ch6" \
            << "AS5047 (Mot.)" << "AS5048B (Joint)";
    for(int index = 0; index < var_list.count(); index++)
    {
        //All boxes have the same list:
        ui->cBoxvar1->addItem(var_list.at(index));
        ui->cBoxvar2->addItem(var_list.at(index));
        ui->cBoxvar3->addItem(var_list.at(index));
        ui->cBoxvar4->addItem(var_list.at(index));
        ui->cBoxvar5->addItem(var_list.at(index));
        ui->cBoxvar6->addItem(var_list.at(index));
    }

    //Color coded labels will be defined based on the chart. Default = B&W
    ui->label_t1->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t2->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t3->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t4->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t5->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t6->setStyleSheet("QLabel { background-color: black; color: white;}");

    //***ToDo*** should be ALL and not EX, but refresh2DPlot() has to be reworked first
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar1slave, SL_BASE_EX, \
                                            SL_LEN_EX);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar2slave, SL_BASE_EX, \
                                          SL_LEN_EX);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar3slave, SL_BASE_EX, \
                                          SL_LEN_EX);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar4slave, SL_BASE_EX, \
                                          SL_LEN_EX);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar5slave, SL_BASE_EX, \
                                          SL_LEN_EX);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar6slave, SL_BASE_EX, \
                                          SL_LEN_EX);

    //Decoded checkboxes:
    ui->checkBoxD1->setDisabled(true);
    ui->checkBoxD2->setDisabled(true);
    ui->checkBoxD3->setDisabled(true);
    ui->checkBoxD4->setDisabled(true);
    ui->checkBoxD5->setDisabled(true);
    ui->checkBoxD6->setDisabled(true);
}

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


//Buffer management for the N variables that we can plot.
void W_2DPlot::update_graph_array(int graph, int new_data)
{
    static int idx_plot[VAR_NUM] = {0,0,0,0,0,0};

    //Updating buffer with one new data point
    update_plot_buf_single(graph_yarray[graph], &idx_plot[graph], new_data);
}

//Add one byte to the FIFO buffer
//Do not call that function directly
void W_2DPlot::update_plot_buf_single(int *buf, int *idx, int new_data)
{
    if((*idx) < plot_len)
    {
        //Buffer isn't full yet, no need to discard "old" bytes
        buf[(*idx)] = new_data;
        (*idx)++;
    }
    else
    {
        //Shift buffer to clear one spot
        for(int i = 1; i < plot_len; i++)
        {
            buf[i-1] = buf[i];
        }
        //Add last byte to the buffer
        buf[plot_len-1] = new_data;
    }

    //buf[] is now up to date
}

//What slave are we plotting for this variable?
uint8_t W_2DPlot::select_plot_slave(uint8_t index)
{
    uint8_t retval = 0;

    switch(index)
    {
        case 0:
            retval = ui->cBoxvar1slave->currentIndex();
            break;
        case 1:
            retval = ui->cBoxvar2slave->currentIndex();
            break;
        case 2:
            retval = ui->cBoxvar3slave->currentIndex();
            break;
        case 3:
            retval = ui->cBoxvar4slave->currentIndex();
            break;
        case 4:
            retval = ui->cBoxvar5slave->currentIndex();
            break;
        case 5:
            retval = ui->cBoxvar6slave->currentIndex();
            break;
    }

    return retval;
}

//All graphs use the same X data. Call this once at startup.
void W_2DPlot::gen_graph_xarray(void)
{
    for(int i = 0; i < PLOT_BUF_LEN; i++)
    {
        graph_xarray[i] = i; // x goes from 0 to 1
    }
}

//Initialize all the graphs at 0:
void W_2DPlot::init_yarrays(void)
{
    for(int i = 0; i < VAR_NUM; i++)
    {
        for(int j = 0; j < PLOT_BUF_LEN; j++)
        {
            graph_yarray[i][j] = 0;
        }
    }
}

//Manages the chart axis, including auto-scaling
void W_2DPlot::setChartAxis(void)
{
    //X:
    if(ui->radioButtonXM->isChecked())
    {
        //Enable manual input fields:
        ui->lineEditXMin->setEnabled(true);
        ui->lineEditXMax->setEnabled(true);

        //Manual
        plot_xmin = ui->lineEditXMin->text().toInt();
        plot_xmax = ui->lineEditXMax->text().toInt();

        //Few safety checks on that number.
        if(plot_xmax >= PLOT_BUF_LEN)
        {
            plot_len = PLOT_BUF_LEN;
            plot_xmax = PLOT_BUF_LEN;
        }
        else
        {
            plot_len = plot_xmax;
        }

        if(plot_xmin < 0)
        {
            plot_xmin = 0;
        }
        else if(plot_xmin > plot_xmax)
        {
            if(plot_xmax > INIT_PLOT_LEN)
            {
                plot_xmin = plot_xmax - INIT_PLOT_LEN;
            }
            else
            {
                plot_xmin = 0;
            }
        }
        plot_len = plot_xmax - plot_xmin;
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

        //Special case 1: all Unused
        if(allChannelUnused() == true)
        {
            //qDebug() << "All Unused";
            plot_ymin = -10;
            plot_ymax = 10;
        }
        else
        {
            //Special case 2: at least 1 unused. We need a value for it,
            //otherwise it will force a 0.
            int yValMin = 0, yValMax = 0;
            for(int k = 0; k < VAR_NUM; k++)
            {
                if(isChannelUsed[k] == true)
                {
                    //We found one, copy its values:
                    yValMin = graph_ylim[2*k];
                    yValMax = graph_ylim[(2*k)+1];
                    //qDebug() << "graph_ylim:"<< graph_ylim[0] << graph_ylim[1] <<graph_ylim[2] <<graph_ylim[3] ;
                    //qDebug() << "Getting values from ch" << k << "(" << yValMin << "," << yValMax << ")";
                    break;
                }
            }
            //Now we use this for all unused channels:
            for(int k = 0; k < VAR_NUM; k++)
            {
                if(isChannelUsed[k] == false)
                {
                    //Unused, replace its min/max:
                    graph_ylim[2*k] = yValMin;
                    graph_ylim[(2*k)+1] = yValMax;

                    //qDebug() << "Replaced values for ch" << k << "(" << yValMin << "," << yValMax << ")";
                }
            }

            //Now we can find the min/max of all channels:
            array_minmax(graph_ylim, 2*VAR_NUM, &plot_ymin, &plot_ymax);

            //Apply a 5% margin:
            //plot_ymin = (plot_ymin-(abs(plot_ymin)/20));
            //plot_ymax = (plot_ymax+(abs(plot_ymax)/20));
            addMargins(&plot_ymin, &plot_ymax);
        }

        //Notify user of value used:
        ui->lineEditYMin->setText(QString::number(plot_ymin));
        ui->lineEditYMax->setText(QString::number(plot_ymax));
    }

    //Update chart:
    chart->axisX()->setRange(plot_xmin, plot_xmax);
    chart->axisY()->setRange(plot_ymin, plot_ymax);
}

//Returns the min and max of an array. Used for the auto axis
void W_2DPlot::array_minmax(int *arr, int len, int *min, int *max)
{
    (*min) = arr[0];
    (*max) = arr[0];

    for(int i = 0; i < len; i++)
    {
        if(arr[i] < (*min))
            (*min) = arr[i];
        if(arr[i] > (*max))
            (*max) = arr[i];
    }
}

void W_2DPlot::refreshData2DPlot(int *x, int *y, int len, uint8_t plot_index)
{
    if(plotFreezed == false)
    {
        //From array to QLineSeries, + auto-scale limits:
        graph_ylim[2*plot_index] = y[0];
        graph_ylim[2*plot_index+1] = y[0];
        qlsData[plot_index]->clear();
        for(int i = 0; i < len; i++)
        {
            //Add datapoint to series:
            qlsData[plot_index]->append(x[i], y[i]);

            //Min & Max:
            if(y[i] < graph_ylim[2*plot_index])
            {
                graph_ylim[2*plot_index] = y[i];
            }
            if(y[i] > graph_ylim[(2*plot_index) + 1])
            {
                graph_ylim[(2*plot_index) + 1] = y[i];
            }
        }

        plotting_len = len;

        //qDebug() << "refreshData2DPlot ylim:" << graph_ylim[0] << graph_ylim[1] << \
        //              graph_ylim[2] << graph_ylim[3];

        //Update axis:
        setChartAxis();
    }
}

//Generates a sine wave. More channels = faster frequency
//All channels have an offset so we can see them (not superposed)
int W_2DPlot::gen_test_data(int phaseShift)
{
    static double phase = 0.0;
    double res_f = 0;

    phase += PHASE_INCREMENT;
    res_f = A_GAIN * sin(phase + phaseShift);

    return((int)res_f);
}

bool W_2DPlot::allChannelUnused(void)
{
    for(int i = 0; i < VAR_NUM; i++)
    {
        if(isChannelUsed[i] == true)
        {
            return false;
        }
    }
    return true;
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

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
