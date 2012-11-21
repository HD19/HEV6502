#-------------------------------------------------
#
# Project created by QtCreator 2012-11-15T16:01:19
#
#-------------------------------------------------

QT       += core gui

TARGET = Visual6502
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../NES/HEV6502/source/cpu/cpu.cpp \
    assembler.cpp

HEADERS  += mainwindow.h \
    ../../NES/HEV6502/source/cpu/cpu.h \
    ../../NES/HEV6502/source/common/common.h \
    assembler.h

FORMS    += mainwindow.ui
