#-------------------------------------------------
#
# Project created by QtCreator 2016-01-06T09:54:12
#
#-------------------------------------------------


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT       += core gui network sql

TARGET = KGminer
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    configstation.cpp \
    filterpicker.cpp \
    binder.cpp \
    nlloc.cpp \
    latencymon.cpp \
    eventmon.cpp \
    dataextractor.cpp \
    importtnk.cpp \
    viewlog.cpp \
    selectstafile.cpp \
    maketnk.cpp \
    makeorigin.cpp \
    viewstation.cpp \
    changelocator.cpp \
    importdata.cpp \
    datareciever.cpp \
    datamonitor.cpp \
    picklist.cpp \
    filegenerator.cpp \
    util.cpp

HEADERS  += mainwindow.h \
    configstation.h \
    filterpicker.h \
    binder.h \
    nlloc.h \
    util.h \
    latencymon.h \
    eventmon.h \
    dataextractor.h \
    importtnk.h \
    viewlog.h \
    selectstafile.h \
    maketnk.h \
    makeorigin.h \
    viewstation.h \
    changelocator.h \
    importdata.h \
    datareciever.h \
    datamonitor.h \
    picklist.h \
    filegenerator.h \
    util.h

FORMS    += mainwindow.ui \
    filterpicker.ui \
    binder.ui \
    nlloc.ui \
    latencymon.ui \
    eventmon.ui \
    dataextractor.ui \
    importtnk.ui \
    viewlog.ui \
    selectstafile.ui \
    maketnk.ui \
    makeorigin.ui \
    changelocator.ui \
    importdata.ui \
    datareciever.ui \
    datamonitor.ui \
    picklist.ui

