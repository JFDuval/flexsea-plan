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
#include <QDebug>
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

    initFlag = true;
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

void W_2DPlot::refresh2DPlot(void)
{
    uint8_t index = 0;

    //For every variable:
    for(index = 0; index < VAR_NUM; index++)
    {
        if(varUsed[index] == false)
        {
            //This channel isn't used, we make it invisible
            qlsData[index]->setVisible(false);
        }
        else
        {
            qlsData[index]->setVisible(true);
        }

        if(varDecode[index] == false)
        {
            switch(varToPlotFormat[index])
            {
                case FORMAT_32S:
                    update_graph_array(index, (*varToPlotPtr32s[index]));
                    //qDebug() << "Index: " << index << (*varToPlotPtr32s[index]);
                    break;
                case FORMAT_32U:
                    update_graph_array(index, (int)(*varToPlotPtr32u[index]));
                    break;
                case FORMAT_16S:
                    update_graph_array(index, (int)(*varToPlotPtr16s[index]));
                    break;
                case FORMAT_16U:
                    update_graph_array(index, (int)(*varToPlotPtr16u[index]));
                    break;
                case FORMAT_8S:
                    update_graph_array(index, (int)(*varToPlotPtr8s[index]));
                    break;
                case FORMAT_8U:
                    update_graph_array(index, (int)(*varToPlotPtr8u[index]));
                    break;
                default:
                    update_graph_array(index, 0);
                    break;
            }
        }
        else
        {
            update_graph_array(index, (*varToPlotPtrD32s[index]));
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
    //Data series:
    qlsData[0] = new QLineSeries();
    qlsData[0]->append(0, 0);
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
    nullVar32s = 0;
    nullVar32u = 0;
    nullVar16s = 0;
    nullVar16u = 0;
    nullVar8s = 0;
    nullVar8u = 0;

    for(int i = 0; i < VAR_NUM; i++)
    {
        varToPlotPtr32s[i] = &nullVar32s;
        varToPlotPtrD32s[i] = &nullVar32s;
        varToPlotPtr32u[i] = &nullVar32u;
        varToPlotPtr16s[i] = &nullVar16s;
        varToPlotPtr16u[i] = &nullVar16u;
        varToPlotPtr8s[i] = &nullVar8s;
        varToPlotPtr8u[i] = &nullVar8u;
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
        data_to_plot[h] = 0;
        varUsed[h] = false;
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

    //Color coded labels will be defined based on the chart. Default = B&W
    ui->label_t1->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t2->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t3->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t4->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t5->setStyleSheet("QLabel { background-color: black; color: white;}");
    ui->label_t6->setStyleSheet("QLabel { background-color: black; color: white;}");

    //Slave combo box:
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar1slave, SL_BASE_ALL, \
                                            SL_LEN_ALL);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar2slave, SL_BASE_ALL, \
                                          SL_LEN_ALL);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar3slave, SL_BASE_ALL, \
                                          SL_LEN_ALL);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar4slave, SL_BASE_ALL, \
                                          SL_LEN_ALL);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar5slave, SL_BASE_ALL, \
                                          SL_LEN_ALL);
    myFlexSEA_Generic.populateSlaveComboBox(ui->cBoxvar6slave, SL_BASE_ALL, \
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

    saveCurrentSettings();
}

//Each board type has a different variable list.
void W_2DPlot::updateVarList(uint8_t var, QComboBox *myCombo)
{
    QStringList var_list, toolTipList;

    uint8_t bType = slaveBType[var];

    //qDebug() << "Updating Var List for index =" << var << "bType = " << bType;

    //Build the string:
    switch(bType)
    {
        case FLEXSEA_PLAN_BASE:
            var_list << "**Unused**";
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

            break;
        case FLEXSEA_MANAGE_BASE:
            struct manageD_s *mnDPtr;
            myFlexSEA_Generic.assignManagePtr(&mnDPtr, SL_BASE_ALL, \
                                               slaveIndex[var]);
            assignVariableMn(var, mnDPtr);
            break;
        case FLEXSEA_EXECUTE_BASE:
            struct executeD_s *exDPtr;
            myFlexSEA_Generic.assignExecutePtr(&exDPtr, SL_BASE_ALL, \
                                               slaveIndex[var]);
            assignVariableEx(var, exDPtr);
            break;
        case FLEXSEA_BATTERY_BASE:
            struct batteryD_s *baDPtr;
            myFlexSEA_Generic.assignBatteryPtr(&baDPtr, SL_BASE_ALL, \
                                               slaveIndex[var]);
            assignVariableBa(var, baDPtr);
            break;
        case FLEXSEA_STRAIN_BASE:
        /* ToDo
            struct strainD_s *stDPtr;
            myFlexSEA_Generic.assignStrainPtr(&stDPtr, SL_BASE_ALL, \
                                               slaveIndex[var]);
            assignVariableSt(var, stDPtr);
        */
            break;

        case FLEXSEA_GOSSIP_BASE:
            struct gossipD_s *goDPtr;
            myFlexSEA_Generic.assignGossipPtr(&goDPtr, SL_BASE_ALL, \
                                               slaveIndex[var]);
            assignVariableGo(var, goDPtr);
            break;
        default:
            break;
    }
}

//Assigns a pointer to the desired variable - Execute boards
void W_2DPlot::assignVariableEx(uint8_t var, struct executeD_s *myPtr)
{
    //'Used' as default, 'false' when set at Unused
    varUsed[var] = true;
    varToPlotFormat[var] = FORMAT_32S;

    //Assign pointer:
    switch(varIndex[var])
    {
        /*Format: (every Case except Unused)
         * Line 1: data format, raw variable
         * Line 2: raw variable
         * Line 3: decoded variable (always int32),
                    null if not decoded  */
        case 0: //"**Unused**"
            varUsed[var] = false;
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 1: //"Accel X"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->exRaw.accel.x;
            varToPlotPtrD32s[var] = &myPtr->accel.x;
            break;
        case 2: //"Accel Y"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->exRaw.accel.y;
            varToPlotPtrD32s[var] = &myPtr->accel.y;
            break;
        case 3: //"Accel Z"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->exRaw.accel.z;
            varToPlotPtrD32s[var] = &myPtr->accel.z;
            break;
        case 4: //"Gyro X"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->exRaw.gyro.x;
            varToPlotPtrD32s[var] = &myPtr->gyro.x;
            break;
        case 5: //"Gyro Y"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->exRaw.gyro.y;
            varToPlotPtrD32s[var] = &myPtr->gyro.y;
            break;
        case 6: //"Gyro Z"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->exRaw.gyro.z;
            varToPlotPtrD32s[var] = &myPtr->gyro.z;
            break;
        case 7: //"Encoder Display"
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &myPtr->exRaw.enc_display;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 8: //"Encoder Control"
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &myPtr->exRaw.enc_control;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 9: //"Encoder Commutation"
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &myPtr->exRaw.enc_commut;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 10: //"Motor current"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->exRaw.current;
            varToPlotPtrD32s[var] = &myPtr->current;
            break;
        case 11: //"Analog[0]"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->exRaw.analog[0];
            varToPlotPtrD32s[var] = &myPtr->analog[0];
            break;
        case 12: //Analog[1]
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->exRaw.analog[1];
            varToPlotPtrD32s[var] = &myPtr->analog[1];
            break;
        case 13: //"Strain"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->exRaw.strain;
            varToPlotPtrD32s[var] = &myPtr->strain;
            break;
        case 14: //"+VB"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->exRaw.volt_batt;
            varToPlotPtrD32s[var] = &myPtr->volt_batt;
            break;
        case 15: //"+VG"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->exRaw.volt_int;
            varToPlotPtrD32s[var] = &myPtr->volt_int;
            break;
        case 16: //"Temp"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->exRaw.temp;
            varToPlotPtrD32s[var] = &myPtr->temp;
            break;
        case 17: //"Status 1"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->exRaw.status1;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 18: //"Status 2"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->exRaw.status2;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 19: //"Setpoint (square)"
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;//ctrl_setpoint);   //ToDo Fix
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 20: //"Setpoint (trap)"
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;//ctrl_setpoint_trap);  //ToDo Fix
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 21: //"Fake Data"
        /*
            phaseShift = (TWO_PI/VAR_NUM)*index;
            update_graph_array(index, gen_test_data((phaseShift)));
            */
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;   //***ToDo***
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        default:
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            varUsed[var] = false;
            break;
    }
}

//Assigns a pointer to the desired variable - Manage boards
void W_2DPlot::assignVariableMn(uint8_t var, struct manageD_s *myPtr)
{
    //'Used' as default, 'false' when set at Unused
    varUsed[var] = true;
    varToPlotFormat[var] = FORMAT_32S;

    //Assign pointer:
    switch(varIndex[var])
    {
        /*Format: (every Case except Unused)
         * Line 1: data format, raw variable
         * Line 2: raw variable
         * Line 3: decoded variable (always int32),
                    null if not decoded  */
        case 0: //"**Unused**"
            varUsed[var] = false;
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 1: //"Accel X"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->mnRaw.accel.x;
            varToPlotPtrD32s[var] = &myPtr->accel.x;
            break;
        case 2: //"Accel Y"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->mnRaw.accel.y;
            varToPlotPtrD32s[var] = &myPtr->accel.y;
            break;
        case 3: //"Accel Z"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->mnRaw.accel.z;
            varToPlotPtrD32s[var] = &myPtr->accel.z;
            break;
        case 4: //"Gyro X"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->mnRaw.gyro.x;
            varToPlotPtrD32s[var] = &myPtr->gyro.x;
            break;
        case 5: //"Gyro Y"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->mnRaw.gyro.y;
            varToPlotPtrD32s[var] = &myPtr->gyro.y;
            break;
        case 6: //"Gyro Z"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->mnRaw.gyro.z;
            varToPlotPtrD32s[var] = &myPtr->gyro.z;
            break;
        case 7: //"Pushbutton"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->mnRaw.sw1;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 8: //"Digital inputs"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->mnRaw.digitalIn;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 9: //"Analog[0]"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->mnRaw.analog[0];
            varToPlotPtrD32s[var] = &myPtr->analog[0];
            break;
        case 10: //Analog[1]
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->mnRaw.analog[1];
            varToPlotPtrD32s[var] = &myPtr->analog[1];
            break;
        case 11: //"Analog[2]"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->mnRaw.analog[2];
            varToPlotPtrD32s[var] = &myPtr->analog[2];
            break;
        case 12: //Analog[3]
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->mnRaw.analog[3];
            varToPlotPtrD32s[var] = &myPtr->analog[3];
            break;
        case 13: //"Analog[4]"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->mnRaw.analog[4];
            varToPlotPtrD32s[var] = &myPtr->analog[4];
            break;
        case 14: //Analog[5]
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->mnRaw.analog[5];
            varToPlotPtrD32s[var] = &myPtr->analog[5];
            break;
        case 15: //"Analog[6]"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->mnRaw.analog[6];
            varToPlotPtrD32s[var] = &myPtr->analog[6];
            break;
        case 16: //Analog[7]
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->mnRaw.analog[7];
            varToPlotPtrD32s[var] = &myPtr->analog[7];
            break;
        case 17: //"Status"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->mnRaw.status1;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        default:
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            varUsed[var] = false;
            break;
    }
}

//Assigns a pointer to the desired variable - Gossip boards
void W_2DPlot::assignVariableGo(uint8_t var, struct gossipD_s *myPtr)
{
    //'Used' as default, 'false' when set at Unused
    varUsed[var] = true;
    varToPlotFormat[var] = FORMAT_32S;

    //Assign pointer:
    switch(varIndex[var])
    {
        /*Format: (every Case except Unused)
         * Line 1: data format, raw variable
         * Line 2: raw variable
         * Line 3: decoded variable (always int32),
                    null if not decoded  */
        case 0: //"**Unused**"
            varUsed[var] = false;
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 1: //"Accel X"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->goRaw.accel.x;
            varToPlotPtrD32s[var] = &myPtr->accel.x;
            break;
        case 2: //"Accel Y"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->goRaw.accel.y;
            varToPlotPtrD32s[var] = &myPtr->accel.y;
            break;
        case 3: //"Accel Z"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->goRaw.accel.z;
            varToPlotPtrD32s[var] = &myPtr->accel.z;
            break;
        case 4: //"Gyro X"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->goRaw.gyro.x;
            varToPlotPtrD32s[var] = &myPtr->gyro.x;
            break;
        case 5: //"Gyro Y"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->goRaw.gyro.y;
            varToPlotPtrD32s[var] = &myPtr->gyro.y;
            break;
        case 6: //"Gyro Z"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->goRaw.gyro.z;
            varToPlotPtrD32s[var] = &myPtr->gyro.z;
            break;
        case 7: //"Magneto X"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->goRaw.magneto.x;
            varToPlotPtrD32s[var] = &myPtr->magneto.x;
            break;
        case 8: //"Magneto Y"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->goRaw.magneto.y;
            varToPlotPtrD32s[var] = &myPtr->magneto.y;
            break;
        case 9: //"Magneto Z"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->goRaw.magneto.z;
            varToPlotPtrD32s[var] = &myPtr->magneto.z;
            break;
        case 10: //"IO 1"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->goRaw.io[0];
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 11: //"IO 2"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->goRaw.io[1];
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 12: //"Capsense 1"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->goRaw.capsense[0];
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 13: //"Capsense 2"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->goRaw.capsense[1];
            varToPlotPtrD32s[var] = &nullVar32s;
        case 14: //"Capsense 3"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->goRaw.capsense[2];
            varToPlotPtrD32s[var] = &nullVar32s;
        case 15: //"Capsense 4"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->goRaw.capsense[3];
            varToPlotPtrD32s[var] = &nullVar32s;
        case 16: //"Status"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->goRaw.status;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        default:
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            varUsed[var] = false;
            break;
    }
}

//Assigns a pointer to the desired variable - Battery boards
void W_2DPlot::assignVariableBa(uint8_t var, struct batteryD_s *myPtr)
{
    //'Used' as default, 'false' when set at Unused
    varUsed[var] = true;
    varToPlotFormat[var] = FORMAT_32S;

    //Assign pointer:
    switch(varIndex[var])
    {
        /*Format: (every Case except Unused)
         * Line 1: data format, raw variable
         * Line 2: raw variable
         * Line 3: decoded variable (always int32),
                    null if not decoded  */
        case 0: //"**Unused**"
            varUsed[var] = false;
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 1: //"Voltage"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &myPtr->baRaw.voltage;
            varToPlotPtrD32s[var] = &myPtr->voltage;
            break;
        case 2: //"Current"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->baRaw.current;
            varToPlotPtrD32s[var] = &myPtr->current;
            break;
        case 3: //"Power"
            varToPlotFormat[var] = FORMAT_16U;
            varToPlotPtr16u[var] = &nullVar16u;
            varToPlotPtrD32s[var] = &myPtr->power;
            break;
        case 4: //"Temperature"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->baRaw.temp;
            varToPlotPtrD32s[var] = &myPtr->temp;
            break;
        case 5: //"Pushbutton"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->baRaw.pushbutton;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 6: //"Status"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->baRaw.status;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        default:
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            varUsed[var] = false;
            break;
    }
}

//***ToDo*** incomplete
//Assigns a pointer to the desired variable - Strain boards
void W_2DPlot::assignVariableSt(uint8_t var, struct strainD_s *myPtr)
{
    //'Used' as default, 'false' when set at Unused
    varUsed[var] = true;
    varToPlotFormat[var] = FORMAT_32S;

    //Assign pointer:
    switch(varIndex[var])
    {
        /*Format: (every Case except Unused)
         * Line 1: data format, raw variable
         * Line 2: raw variable
         * Line 3: decoded variable (always int32),
                    null if not decoded  */

    /*
        case 0: //"**Unused**"
            varUsed[var] = false;
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;
        case 1: //"Ch 1"
            varToPlotFormat[var] = FORMAT_16S;
            varToPlotPtr16s[var] = &myPtr->stRaw.strain_filtered;
            varToPlotPtrD32s[var] = &myPtr->;
            break;



        case 7: //"Status"
            varToPlotFormat[var] = FORMAT_8U;
            varToPlotPtr8u[var] = &myPtr->goRaw.status;
            varToPlotPtrD32s[var] = &nullVar32s;
            break;

    */
        default:
            varToPlotFormat[var] = FORMAT_32S;
            varToPlotPtr32s[var] = &nullVar32s;
            varToPlotPtrD32s[var] = &nullVar32s;
            varUsed[var] = false;
            break;
    }
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
        slaveAddr[i] = myFlexSEA_Generic.getSlaveID(SL_BASE_ALL, slaveIndex[i]);
        slaveBType[i] = myFlexSEA_Generic.getSlaveBoardType(SL_BASE_ALL, \
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
    varDecode[0] = ui->checkBoxD1->isChecked();
    varDecode[1] = ui->checkBoxD2->isChecked();
    varDecode[2] = ui->checkBoxD3->isChecked();
    varDecode[3] = ui->checkBoxD4->isChecked();
    varDecode[4] = ui->checkBoxD5->isChecked();
    varDecode[5] = ui->checkBoxD6->isChecked();
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
                if(varUsed[k] == true)
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
                if(varUsed[k] == false)
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
        if(varUsed[i] == true)
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

//Slave comboBoxes:

void W_2DPlot::on_cBoxvar1slave_currentIndexChanged(int index)
{
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
    if(initFlag == false)
    {
        saveCurrentSettings();
        updateVarList(1, ui->cBoxvar2);
        assignVariable(1);
    }
}

void W_2DPlot::on_cBoxvar3slave_currentIndexChanged(int index)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        updateVarList(2, ui->cBoxvar3);
        assignVariable(2);
    }
}

void W_2DPlot::on_cBoxvar4slave_currentIndexChanged(int index)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        updateVarList(3, ui->cBoxvar4);
        assignVariable(3);
    }
}

void W_2DPlot::on_cBoxvar5slave_currentIndexChanged(int index)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        updateVarList(4, ui->cBoxvar5);
        assignVariable(4);
    }
}

void W_2DPlot::on_cBoxvar6slave_currentIndexChanged(int index)
{
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
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(0);
    }
}

void W_2DPlot::on_cBoxvar2_currentIndexChanged(int index)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(1);
    }
}

void W_2DPlot::on_cBoxvar3_currentIndexChanged(int index)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(2);
    }
}

void W_2DPlot::on_cBoxvar4_currentIndexChanged(int index)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(3);
    }
}

void W_2DPlot::on_cBoxvar5_currentIndexChanged(int index)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(4);
    }
}

void W_2DPlot::on_cBoxvar6_currentIndexChanged(int index)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(5);
    }
}

//Decode checkboxes:

void W_2DPlot::on_checkBoxD1_stateChanged(int arg1)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(0);
    }
}

void W_2DPlot::on_checkBoxD2_stateChanged(int arg1)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(1);
    }
}

void W_2DPlot::on_checkBoxD3_stateChanged(int arg1)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(2);
    }
}

void W_2DPlot::on_checkBoxD4_stateChanged(int arg1)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(3);
    }
}

void W_2DPlot::on_checkBoxD5_stateChanged(int arg1)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(4);
    }
}

void W_2DPlot::on_checkBoxD6_stateChanged(int arg1)
{
    if(initFlag == false)
    {
        saveCurrentSettings();
        assignVariable(5);
    }
}

void W_2DPlot::on_pushButtonClear_clicked()
{
    qDebug() << "Clear plot!";
    init_yarrays();
}
