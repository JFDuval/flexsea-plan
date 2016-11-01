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
    inc/MDIWinObjects \
    inc/Wrappers \
    flexsea-comm/inc \
    flexsea-system/inc \

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/MDIWinObjects/w_2dplot.cpp \
    src/MDIWinObjects/w_anycommand.cpp \
    src/MDIWinObjects/w_battery.cpp \
    src/MDIWinObjects/w_calibration.cpp \
    src/MDIWinObjects/w_control.cpp \
    src/MDIWinObjects/w_converter.cpp \
    src/MDIWinObjects/w_execute.cpp \
    src/MDIWinObjects/w_gossip.cpp \
    src/MDIWinObjects/w_manage.cpp \
    src/MDIWinObjects/w_ricnu.cpp \
    src/MDIWinObjects/w_slavecomm.cpp \
    src/MDIWinObjects/w_strain.cpp \
    src/Wrappers/WinSlaveComm.cpp \
    src/datalogger.cpp \
    src/flexsea_generic.cpp \
    src/serialdriver.cpp \
    src/flexsea_board.c \
    src/trapez.c \
    flexsea-comm/src/flexsea.c \
    flexsea-comm/src/flexsea_buffers.c \
    flexsea-comm/src/flexsea_comm.c \
    flexsea-comm/src/flexsea_payload.c \
    flexsea-system/src/flexsea_cmd_application.c \
    flexsea-system/src/flexsea_cmd_control.c \
    flexsea-system/src/flexsea_cmd_data.c \
    flexsea-system/src/flexsea_cmd_external.c \
    flexsea-system/src/flexsea_cmd_sensors.c \
    flexsea-system/src/flexsea_system.c \
    src/MDIWinObjects/w_config.cpp


HEADERS  += inc/main.h \
    inc/mainwindow.h \
    inc/flexsea_generic.h \
    inc/datalogger.h \
    inc/trapez.h \
    inc/serialdriver.h \
    inc/flexsea_board.h \
    inc/MDIWinObjects/w_2dplot.h \
    inc/MDIWinObjects/w_anycommand.h \
    inc/MDIWinObjects/w_battery.h \
    inc/MDIWinObjects/w_calibration.h \
    inc/MDIWinObjects/w_control.h \
    inc/MDIWinObjects/w_converter.h \
    inc/MDIWinObjects/w_execute.h \
    inc/MDIWinObjects/w_gossip.h \
    inc/MDIWinObjects/w_manage.h \
    inc/MDIWinObjects/w_ricnu.h \
    inc/MDIWinObjects/w_slavecomm.h \
    inc/MDIWinObjects/w_strain.h \
    inc/Wrappers/WinSlaveComm.h \
    flexsea-comm/inc/flexsea.h \
    flexsea-comm/inc/flexsea_buffers.h \
    flexsea-comm/inc/flexsea_comm.h \
    flexsea-comm/inc/flexsea_payload.h \
    flexsea-system/inc/flexsea_cmd_application.h \
    flexsea-system/inc/flexsea_cmd_control.h \
    flexsea-system/inc/flexsea_cmd_data.h \
    flexsea-system/inc/flexsea_cmd_external.h \
    flexsea-system/inc/flexsea_cmd_sensors.h \
    flexsea-system/inc/flexsea_system.h \
    inc/MDIWinObjects/w_config.h \
    inc/define.h

FORMS    += ui/mainwindow.ui \
    ui/w_execute.ui \
    ui/w_control.ui \
    ui/w_2dplot.ui \
    ui/w_slavecomm.ui \
    ui/w_anycommand.ui \
    ui/w_converter.ui \
    ui/w_ricnu.ui \
    ui/w_manage.ui \
    ui/w_calibration.ui \
    ui/w_battery.ui \
    ui/w_gossip.ui \
    ui/w_strain.ui \
    ui/w_config.ui

RESOURCES += \
    misc/icons.qrc

#Enable the next 2 lines to inspect the pre-processor output
#Linked will fail - debug only
#QMAKE_CFLAGS = $$QMAKE_CFLAGS -E
#QMAKE_CXXFLAGS = $$QMAKE_CXXFLAGS -E

DISTFILES += \
    flexsea-comm/flexsea.gitignore \
    flexsea-comm/GPL-3.0.txt \
    flexsea-system/flexsea.gitignore \
    flexsea-system/GPL-3.0.txt \
    flexsea-comm/README.md \
    flexsea-system/README.md
