#-------------------------------------------------
#
# Project created by QtCreator 2015-03-21T15:07:44
#
#-------------------------------------------------

QT       += core gui opengl
QT += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = fftestGL
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    myglwidget.cpp \
    qcustomplot.cpp \
    triad.cpp

HEADERS  += widget.h \
    myglwidget.h \
    qcustomplot.h \
    triad.h

FORMS    += widget.ui
