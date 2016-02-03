#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>

#include "util.h"

#include "configstation.h"
#include "filterpicker.h"
#include "nlloc.h"
#include "latencymon.h"
#include "eventmon.h"
#include "dataextractor.h"
#include "importtnk.h"
#include "maketnk.h"
#include "viewlog.h"
#include "selectstafile.h"
#include "viewstation.h"
#include "changelocator.h"

#include "datareciever.h"

#include "datamonitor.h"
#include "importdata.h"
#include "picklist.h"
#include "binder.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTextCodec *codec;
    bool korean;

    QMenu *xmenu;
    QMenu *submenu;
    QAction *sub1;
    QAction *sub2;
    QAction *sub3;
    QAction *sub4;
    QString parameterFileName;

private:
    Ui::MainWindow *ui;

    /* Widget */    
    LatencyMon *latencymon;
    PickList *picklist;

    ConfigStation *configstation;
    SelectStaFile *selectstafile;
    ImportData *importdata;
    DataMonitor *datamonitor;

    Binder *binder;
    FilterPicker *filterpicker;
    NLLoc *nlloc1; //NLLoc *nlloc2; //NLLoc *nlloc3;
    ViewLog *viewlogFP, *viewlogBI, *viewlogNL, *viewlogTK;

    ViewStation *viewstation;
    EventMon *eventmon;
    DataExtractor *dataextractor;
    DataReciever *datareciever;

    QTimer *checkProcessTimer;
    QTimer *prparserTimer;

    int posx, posy;
    void setPosition();

    QProcess *process;

    QSqlDatabase kgminerdb;
    QSqlQueryModel *model;
    void openDB();

public slots:
    void checkProcess();
    void runPrParser();

private slots:
    /* Main Menu slots */
    void stainfoViewerShow();
    void showWave();
    void runG();
    void runSwarm();
    void eventViewerShow();
    void dataExtractorShow();

    void startEWprocess();
    void stopEWprocess();

    void filterpickerButtonClicked();
    void binderButtonClicked();
    void loc1ButtonClicked();
    void datarecieverButtonClicked();

    void killProcess();
    void restartProcess();
    void changeParameterDialogShow();
    void viewLog();

    /* Action Menu slots */
    void actionConfigStaInfoClicked();
    void actionLoadStaFileClicked();
    void actionImportDataClicked();
    void actionDataMonitorClicked();
    void actionEnglishClicked();
    void actionKoreanClicked();
    void actionExitClicked();
};

#endif // MAINWINDOW_H
