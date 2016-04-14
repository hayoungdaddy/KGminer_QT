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
    viewlog.cpp \
    selectstafile.cpp \
    makeorigin.cpp \
    viewstation.cpp \
    importdata.cpp \
    datareciever.cpp \
    datamonitor.cpp \
    picklist.cpp \
    filegenerator.cpp \
    util.cpp \
    checkewprocs.cpp \
    eventgenerator.cpp

HEADERS  += mainwindow.h \
    configstation.h \
    filterpicker.h \
    binder.h \
    nlloc.h \
    util.h \
    latencymon.h \
    eventmon.h \
    dataextractor.h \
    viewlog.h \
    selectstafile.h \
    makeorigin.h \
    viewstation.h \
    importdata.h \
    datareciever.h \
    datamonitor.h \
    picklist.h \
    filegenerator.h \
    util.h \
    common.h \
    checkewprocs.h \
    eventgenerator.h \
    locator.h

FORMS    += mainwindow.ui \
    binder.ui \
    nlloc.ui \
    latencymon.ui \
    eventmon.ui \
    dataextractor.ui \
    viewlog.ui \
    selectstafile.ui \
    makeorigin.ui \
    importdata.ui \
    datareciever.ui \
    datamonitor.ui \
    picklist.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libmseed/release/ -lmseed
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libmseed/debug/ -lmseed
else:unix: LIBS += -L$$PWD/../libmseed/ -lmseed

INCLUDEPATH += $$PWD/../libmseed
DEPENDPATH += $$PWD/../libmseed

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libmseed/release/libmseed.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libmseed/debug/libmseed.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libmseed/release/mseed.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libmseed/debug/mseed.lib
else:unix: PRE_TARGETDEPS += $$PWD/../libmseed/libmseed.a
