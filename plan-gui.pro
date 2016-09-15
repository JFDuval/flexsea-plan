#-------------------------------------------------
#
# Project created by QtCreator 2016-08-23T15:35:07
#
#-------------------------------------------------

QT       += core gui charts serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = plan-gui
TEMPLATE = app

INCLUDEPATH += inc \
    ../flexsea-comm/inc \
    ../flexsea-system/inc

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/w_2dplot.cpp \
    src/w_control.cpp \
    src/w_execute.cpp \
    src/w_planconfig.cpp \
    src/WinConfig.cpp \
    src/WinControlControl.cpp \
    src/WinView2DPlot.cpp \
    src/WinViewExecute.cpp \
    src/flexsea_board.c \
    ../flexsea-comm/src/flexsea.c \
    ../flexsea-comm/src/flexsea_buffers.c \
    ../flexsea-comm/src/flexsea_comm.c \
    ../flexsea-comm/src/flexsea_payload.c \
    ../flexsea-system/src/flexsea_cmd_application.c \
    ../flexsea-system/src/flexsea_cmd_control.c \
    ../flexsea-system/src/flexsea_cmd_external.c \
    ../flexsea-system/src/flexsea_cmd_sensors.c \
    ../flexsea-system/src/flexsea_system.c \
    ../flexsea-system/src/flexsea_cmd_data.c \
    src/w_slavecomm.cpp \
    src/WinSlaveComm.cpp \
    src/serialdriver.cpp \
    src/flexsea_generic.cpp \
    src/trapez.c \
    src/w_anycommand.cpp \
    src/WinAnyCommand.cpp \
    src/w_converter.cpp \
    src/WinConverter.cpp \
    src/datalogger.cpp \
    src/WinViewRicnu.cpp \
    src/w_ricnu.cpp \
    src/WinCalibration.cpp \
    src/w_calibration.cpp \
    src/WinViewManage.cpp \
    src/w_manage.cpp

HEADERS  += inc/main.h \
    inc/mainwindow.h \
    inc/flexsea_board.h \
    ../flexsea-comm/inc/flexsea.h \
    ../flexsea-comm/inc/flexsea_buffers.h \
    ../flexsea-comm/inc/flexsea_comm.h \
    ../flexsea-comm/inc/flexsea_payload.h \
    ../flexsea-system/inc/flexsea_cmd_application.h \
    ../flexsea-system/inc/flexsea_cmd_control.h \
    ../flexsea-system/inc/flexsea_cmd_external.h \
    ../flexsea-system/inc/flexsea_cmd_sensors.h \
    ../flexsea-system/inc/flexsea_system.h \
    ../flexsea-system/inc/flexsea_cmd_data.h \
    inc/WinViewExecute.h \
    inc/w_execute.h \
    inc/w_control.h \
    inc/WinControlControl.h \
    inc/w_2dplot.h \
    inc/WinView2DPlot.h \
    inc/w_planconfig.h \
    inc/WinConfig.h \
    inc/w_slavecomm.h \
    inc/WinSlaveComm.h \
    inc/serialdriver.h \
    inc/flexsea_generic.h \
    inc/trapez.h \
    inc/w_anycommand.h \
    inc/WinAnyCommand.h \
    inc/w_converter.h \
    inc/WinConverter.h \
    inc/datalogger.h \
    inc/WinViewRicnu.h \
    inc/w_ricnu.h \
    inc/WinCalibration.h \
    inc/w_calibration.h \
    inc/WinViewManage.h \
    inc/w_manage.h

FORMS    += ui/mainwindow.ui \
    ui/w_execute.ui \
    ui/w_control.ui \
    ui/w_2dplot.ui \
    ui/w_planconfig.ui \
    ui/w_slavecomm.ui \
    ui/w_anycommand.ui \
    ui/w_converter.ui \
    ui/w_ricnu.ui \
    ui/w_manage.ui \
    ui/w_calibration.ui

RESOURCES += \
    misc/icons.qrc

#Enable the next 2 lines to inspect the pre-processor output
#Linked will fail - debug only
#QMAKE_CFLAGS = $$QMAKE_CFLAGS -E
#QMAKE_CXXFLAGS = $$QMAKE_CXXFLAGS -E
