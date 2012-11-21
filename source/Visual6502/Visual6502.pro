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
    ../cpu/cpu.cpp \
    ../assembler/assembler.cpp

HEADERS  += mainwindow.h \
    ../cpu/cpu.h \
    ../common/common.h \
    ../assembler/assembler.h

FORMS    += mainwindow.ui
