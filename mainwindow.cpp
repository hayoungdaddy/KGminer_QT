#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags( Qt::WindowTitleHint | Qt::CustomizeWindowHint );
    codec = QTextCodec::codecForName( "utf8" );
    korean = false;

    posx = 0; posy = 0;

    /* get cfg */
    util = new Util();
    cfg = util->readCfg();

    /* open Database */
    openDB();

    /* check ew process */
    procscheckthred = new QThread(this);
    checkewprocs = new CheckEWProcs(cfg);
    checkewprocs->moveToThread(procscheckthred);
    procscheckthred->start();

    /* check Event */
    eventcheckthred = new QThread(this);
    eventgenerator = new EventGenerator(cfg);
    eventgenerator->moveToThread(eventcheckthred);
    eventcheckthred->start();

    /* ActionMenu Dialogs */
    importdata = new ImportData(cfg, this);
    importdata->hide();
    datamonitor = new DataMonitor(cfg, this);
    datamonitor->hide();
    selectstafile = new SelectStaFile(false, this);
    selectstafile->hide();

    /* Mainwindow GUI Dialog */
    latencymon = new LatencyMon(ui->statusFrame);
    latencymon->show();
    picklist = new PickList(ui->pickframe);
    picklist->show();

    /* Process Dialogs */
    binder = new Binder(this);
    binder->hide();
    viewlogFP = new ViewLog(this );
    viewlogFP->hide();
    viewlogBI = new ViewLog(this );
    viewlogBI->hide();
    viewlogNL = new ViewLog(this );
    viewlogNL->hide();
    viewlogTK = new ViewLog(this );
    viewlogTK->hide();


    /* Mainbutton Dialogs */

    /* Process Submenu */
    xmenu = new QMenu();
    submenu = xmenu->addMenu("Process_Submenu");
    //sub1 = submenu->addAction("Stop");
    sub2 = submenu->addAction("Restart");
    sub3 = submenu->addAction("Edit Parameters");
    //sub4 = submenu->addAction("View Logs");
    submenu->hide();

    /* Main Menu Connection */
    connect(ui->stainfoButton, SIGNAL(clicked()), this, SLOT(stainfoViewerShow()));
    connect(ui->swarmButton, SIGNAL(clicked()), this, SLOT(showWave()));
    connect(ui->eventViewerButton, SIGNAL(clicked()), this, SLOT(eventViewerShow()));
    connect(ui->dataExtractorButton, SIGNAL(clicked()), this, SLOT(dataExtractorShow()));

    /* Process Menu Connection */
    connect(ui->startEWButton, SIGNAL(clicked()), this, SLOT(startEWprocess()));
    connect(ui->stopEWButton, SIGNAL(clicked()), this, SLOT(stopEWprocess()));
    connect(ui->filterpickerB, SIGNAL(clicked()), this, SLOT(filterpickerButtonClicked()));
    connect(ui->binderB, SIGNAL(clicked()), this, SLOT(binderButtonClicked()));
    connect(ui->loc1B, SIGNAL(clicked()), this, SLOT(loc1ButtonClicked()));
    connect(ui->datarecieverB, SIGNAL(clicked()), this, SLOT(datarecieverButtonClicked()));

    /* Check EW Procs Thread */
    connect(checkewprocs, SIGNAL(sendEWModuleListToMainwindow(EWMODULEINFO)), this, SLOT(recvEWModuleList(EWMODULEINFO)));

    /* Action Menu Connection */
    connect(ui->actionConfigStaInfo, SIGNAL(triggered()), this, SLOT(actionConfigStaInfoClicked()));
    connect(ui->actionLoadStaFile, SIGNAL(triggered()), this, SLOT(actionLoadStaFileClicked()));
    connect(ui->actionImportData, SIGNAL(triggered()), this, SLOT(actionImportDataClicked()));
    connect(ui->actionDataMonitor, SIGNAL(triggered()), this, SLOT(actionDataMonitorClicked()));
    connect(ui->actionEnglish, SIGNAL(triggered()), this, SLOT(actionEnglishClicked()));
    connect(ui->actionKorean, SIGNAL(triggered()), this, SLOT(actionKoreanClicked()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(actionExitClicked()));

    /* SubMenu Connection */
    //connect(sub1, SIGNAL(triggered()), this, SLOT(killProcess()));
    connect(sub2, SIGNAL(triggered()), this, SLOT(restartProcess()));
    connect(sub3, SIGNAL(triggered()), this, SLOT(changeParameterDialogShow()));
    //connect(sub4, SIGNAL(triggered()), this, SLOT(viewLog()));

    ui->datarecieverB->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
    ui->filterpickerB->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
    ui->binderB->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
    ui->loc1B->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
}

MainWindow::~MainWindow()
{
    procscheckthred->terminate();
    delete ui;
}

void MainWindow::openDB()
{
    kgminerdb = QSqlDatabase::addDatabase("QSQLITE");
    kgminerdb.setDatabaseName(DB);
    if(!kgminerdb.open())
    {
        QMessageBox msgBox;
        msgBox.setText(kgminerdb.lastError().text());
        msgBox.exec();
        return;
    }
}

void MainWindow::recvEWModuleList(EWMODULEINFO ewmoduleinfo)
{
    //qDebug() << ewmoduleinfo.prName;

    ui->datarecieverB->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
    ui->filterpickerB->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
    ui->binderB->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
    ui->loc1B->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");

    for(int i=0;i<ewmoduleinfo.prName.count();i++)
    {
        if(ewmoduleinfo.prName[i].startsWith("tankplayer") && ewmoduleinfo.status[i].startsWith("Alive"))
            ui->datarecieverB->setStyleSheet("background-color: rgb(170, 255, 127);");
        else if(ewmoduleinfo.prName[i].startsWith("slink2ew") && ewmoduleinfo.status[i].startsWith("Alive"))
            ui->datarecieverB->setStyleSheet("background-color: rgb(170, 255, 127);");
        else if(ewmoduleinfo.prName[i].startsWith("pick_FP") && ewmoduleinfo.status[i].startsWith("Alive"))
            ui->filterpickerB->setStyleSheet("background-color: rgb(170, 255, 127);");
        else if(ewmoduleinfo.prName[i].startsWith("binder_ew") && ewmoduleinfo.status[i].startsWith("Alive"))
            ui->binderB->setStyleSheet("background-color: rgb(170, 255, 127);");
        else if(ewmoduleinfo.prName[i].startsWith("eqproc") && ewmoduleinfo.status[i].startsWith("Alive"))
            ui->loc1B->setStyleSheet("background-color: rgb(170, 255, 127);");
    }
}

/* Process slots */
void MainWindow::startEWprocess()
{
    QString cmd = "source " + PARAMSDIR + "/ew_linux.bash";
    system(cmd.toLatin1().data());
    cmd = "sh " + SCRIPTDIR + "/autostart.sh &";
    system(cmd.toLatin1().data());

    latencymon->setup();
    picklist->clear();
    picklist->setup();
}

void MainWindow::stopEWprocess()
{
    if(!korean)
    {
        if( !QMessageBox::question( this,
                    "Exit",
                    "Do you want to stop all process?",
                    "Confirm",
                    "Cancel",
                    QString::null, 1, 1 ) )
        {
            QString cmd = BINDIR + "/pau";
            //system(cmd.toLatin1().data());
        }
    }
    else
    {
        if( !QMessageBox::question( this,
                    codec->toUnicode("프로세스 종료"),
                    codec->toUnicode("모든 분석 프로세스를 종료합니다."),
                    codec->toUnicode("확인"),
                    codec->toUnicode("취소"),
                    QString::null, 1, 1 ) )
        {
            QString cmd = BINDIR + "/pau";
            system(cmd.toLatin1().data());
        }
    }
}

void MainWindow::datarecieverButtonClicked()
{
    setPosition();
    submenu->move(posx, posy);
    if(!sub3->isEnabled())
        sub3->setEnabled(true);
    submenu->show();
    parameterFileName = "tankplayer";
}

void MainWindow::filterpickerButtonClicked()
{
    setPosition();
    submenu->move(posx, posy);
    if(!sub3->isEnabled())
        sub3->setEnabled(true);
    submenu->show();
    parameterFileName = "filterpicker";
}

void MainWindow::binderButtonClicked()
{
    setPosition();
    submenu->move(posx, posy);
    if(!sub3->isEnabled())
        sub3->setEnabled(true);
    submenu->show();
    parameterFileName = "binder";
}

void MainWindow::loc1ButtonClicked()
{
    setPosition();
    submenu->move(posx, posy);
    if(!sub3->isEnabled())
        sub3->setEnabled(true);
    submenu->show();
    if(ui->loc1B->text() == "NonLinLoc")
        parameterFileName = "nlloc1";
    else
        parameterFileName = "grid1";
}


/* Sub Menu slots */
void MainWindow::killProcess()
{
    QFile file;
    QString cmd = "";

    if(parameterFileName == "tankplayer")
        file.setFileName(SCRIPTDIR + "/pid/tankplayer.pid");
    else if(parameterFileName == "binder")
        file.setFileName(SCRIPTDIR + "/pid/binder.pid");
    else if(parameterFileName == "nlloc1" || parameterFileName == "grid1")
        file.setFileName(SCRIPTDIR + "/pid/loc1.pid");
    else if(parameterFileName == "filterpicker")
        file.setFileName(SCRIPTDIR + "/pid/pickfp.pid");
    else if(parameterFileName == "ew2mseed")
        file.setFileName(SCRIPTDIR + "/pid/ew2mseed.pid");

    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        line = stream.readLine();

        cmd = BINDIR + "/pidpau " + line;
        system(cmd.toLatin1().data());
        cmd = BINDIR + "/status";
        system(cmd.toLatin1().data());
        file.close();
    }
}

void MainWindow::restartProcess()
{
    QFile file;
    QString cmd = "";

    if(parameterFileName == "tankplayer")
    {
        file.setFileName(SCRIPTDIR + "/pid/tankplayer.pid");
        latencymon->setup();
        picklist->clear();
        picklist->setup();
    }
    else if(parameterFileName == "binder")
        file.setFileName(SCRIPTDIR + "/pid/binder.pid");
    else if(parameterFileName == "nlloc1" || parameterFileName == "grid1")
        file.setFileName(SCRIPTDIR + "/pid/loc1.pid");
    else if(parameterFileName == "filterpicker")
        file.setFileName(SCRIPTDIR + "/pid/pickfp.pid");
    else if(parameterFileName == "ew2mseed")
        file.setFileName(SCRIPTDIR + "/pid/ew2mseed.pid");

    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        line = stream.readLine();

        cmd = BINDIR + "/restart " + line;
        system(cmd.toLatin1().data());

        file.close();
    }

}

void MainWindow::changeParameterDialogShow()
{
    if(parameterFileName == "filterpicker")
    {
        filterpicker = new FilterPicker( this );
        filterpicker->show();
    }
    else if(parameterFileName == "binder")
    {

        binder->setup();
        binder->show();
    }
    else if(parameterFileName == "nlloc1")
    {
        nlloc1 = new NLLoc( "0", "0", "1", this );
        nlloc1->setup();
        nlloc1->show();
    }
    else if(parameterFileName == "tankplayer")
    {
        datareciever = new DataReciever( this );
        datareciever->reset();
        datareciever->show();
    }
}

void MainWindow::viewLog()
{
    if(parameterFileName == "filterpicker")
    {
        viewlogFP->setup("pick_FP");
        viewlogFP->show();
    }
    else if(parameterFileName == "binder")
    {
        viewlogBI->setup("binder_ew");
        viewlogBI->show();
    }
    else if(parameterFileName == "nlloc1")
    {
        viewlogNL->setup("NLLoc1");
        viewlogNL->show();
    }
    else if(parameterFileName == "tankplayer")
    {
        viewlogTK->setup("tankplayer");
        viewlogTK->show();
    }
}

void MainWindow::setPosition()
{
    QPoint p = QCursor::pos();
    posx = p.x();
    posy = p.y();
}

/* Action Menu Slots */
void MainWindow::actionLoadStaFileClicked()
{
    selectstafile->show();
}

void MainWindow::actionConfigStaInfoClicked()
{
    configstation = new ConfigStation(this);
    configstation->show();
}

void MainWindow::actionImportDataClicked()
{
    importdata->setup();
    importdata->show();
}

void MainWindow::actionDataMonitorClicked()
{
    QDate date = QDate::currentDate();
    datamonitor->setup();
    datamonitor->clickCalendar(date);
    datamonitor->changePage(date.year(), date.month());
    datamonitor->show();
}

void MainWindow::actionEnglishClicked()
{
    ui->stainfoButton->setText("Station Info.\nViewer");
    ui->swarmButton->setText("Waveform\nViewer");
    ui->eventViewerButton->setText("Event\nViewer");
    ui->dataExtractorButton->setText("Event\nMaker");

    ui->startEWButton->setText("Start all");
    ui->stopEWButton->setText("Stop All");

    ui->incomingLabel->setText("Data Feeding");
    ui->processLabel->setText("Process Layout");
    ui->pickLabel->setText("Pick Status");

    ui->actionStation->setTitle("Config statoin Info.");
    ui->actionConfigStaInfo->setText("Config new stations Info.");
    ui->actionLoadStaFile->setText("Load a station file");
    ui->actionImportData->setText("Import data");
    ui->actionDataMonitor->setText("Data availability monitor");
    ui->actionLanguage->setTitle("Language");
    ui->actionEnglish->setText("English");
    ui->actionKorean->setText("Korean");
    ui->actionExit->setText("Exit");

    importdata->setLanguageEn();
    datamonitor->setLanguageEn();
    /*
    binder->setLanguageEn();
    selectstafile->setLanguageEn();

    importdata->korean = false;
    datamonitor->korean = false;
    binder->korean = false;
    selectstafile->korean = false;
    */
    korean = false;
}

void MainWindow::actionKoreanClicked()
{
    ui->stainfoButton->setText(codec->toUnicode("관측소 정보"));
    ui->swarmButton->setText(codec->toUnicode("데이터\n모니터링"));
    ui->eventViewerButton->setText(codec->toUnicode("이벤트\n모니터링"));
    ui->dataExtractorButton->setText(codec->toUnicode("이벤트 생성"));

    ui->startEWButton->setText(codec->toUnicode("프로세스 실행"));
    ui->stopEWButton->setText(codec->toUnicode("프로세스 정지"));

    ui->incomingLabel->setText(codec->toUnicode("데이터 수신 리스트"));
    ui->processLabel->setText(codec->toUnicode("프로세스 관리"));
    ui->pickLabel->setText(codec->toUnicode("피킹 정보"));

    ui->actionStation->setTitle(codec->toUnicode("관측소 정보 설정"));
    ui->actionConfigStaInfo->setText(codec->toUnicode("신규 관측소 정보 설정"));
    ui->actionLoadStaFile->setText(codec->toUnicode("관측소 정보 파일 불러오기"));
    ui->actionImportData->setText(codec->toUnicode("지진 자료 입력"));
    ui->actionDataMonitor->setText(codec->toUnicode("지진 자료 가용 상태 확인"));
    ui->actionLanguage->setTitle(codec->toUnicode("언어"));
    ui->actionEnglish->setText(codec->toUnicode("영어"));
    ui->actionKorean->setText(codec->toUnicode("한국어"));
    ui->actionExit->setText(codec->toUnicode("프로그램 종료"));

    importdata->setLanguageKo();
    datamonitor->setLanguageKo();
    /*
    binder->setLanguageKo();
    selectstafile->setLanguageKo();

    importdata->korean = true;
    datamonitor->korean = true;
    binder->korean = true;
    selectstafile->korean = true;
    */
    korean = true;
}

void MainWindow::actionExitClicked()
{
    if(!korean)
    {
        if( !QMessageBox::question( this,
                    "Exit",
                    "All programs will be exit.",
                    "Confirm",
                    "Cancel",
                    QString::null, 1, 1 ) )
        {
            /*
            QString cmd = BINDIR + "/pau";
            system(cmd.toLatin1().data());
            cmd = "kill `ps -ef | grep eventviewer | grep -v grep | awk '{print $2}'`";
            system(cmd.toLatin1().data());
            cmd = "kill `ps -ef | grep swarm | grep -v grep | grep -v Swarm | awk '{print $2}'`";
            system(cmd.toLatin1().data());
            cmd = "kill `ps -ef | grep swarm | grep -v grep |awk '{print $2}'`";
            system(cmd.toLatin1().data());
            cmd = "kill `ps -ef | grep SeisGram | grep -v grep | awk '{print $2}'";
            system(cmd.toLatin1().data());
            */

            procscheckthred->quit();
            //procscheckthred->terminate();
            close();
        }
    }
    else
    {
        if( !QMessageBox::question( this,
                    codec->toUnicode("프로그램 종료"),
                    codec->toUnicode("모든 관련 프로세스를 종료합니다."),
                    codec->toUnicode("확인"),
                    codec->toUnicode("취소"),
                    QString::null, 1, 1 ) )
        {
            QString cmd = BINDIR + "/pau";
            system(cmd.toLatin1().data());
            cmd = "kill `ps -ef | grep eventviewer | grep -v grep | awk '{print $2}'`";
            system(cmd.toLatin1().data());
            cmd = "kill `ps -ef | grep swarm | grep -v grep | grep -v Swarm | awk '{print $2}'`";
            system(cmd.toLatin1().data());
            cmd = "kill `ps -ef | grep swarm | grep -v grep |awk '{print $2}'`";
            system(cmd.toLatin1().data());
            cmd = "kill `ps -ef | grep SeisGram | grep -v grep | awk '{print $2}'";
            system(cmd.toLatin1().data());

            procscheckthred->quit();
            //procscheckthred->terminate();
            close();
        }
    }
}

/* Main menu slots */
void MainWindow::stainfoViewerShow()
{
    viewstation = new ViewStation(this);
    viewstation->show();
}

void MainWindow::showWave()
{
    QMessageBox mbox;
    mbox.setStandardButtons( QMessageBox::Yes  | QMessageBox::No );
    mbox.setButtonText(QMessageBox::Yes, "geotool");
    mbox.setButtonText(QMessageBox::No, "swarm (realtime mode only)");
    mbox.setDefaultButton( mbox.addButton( QMessageBox::Cancel ) );
    if(!korean)
        mbox.setText( "Which program do you want to run?" );
    else
        mbox.setText( codec->toUnicode("파형 뷰어 프로그램을 선택하십시오.") );

    switch (mbox.exec()) {
        case QMessageBox::Yes:
            runG();
            break;
        case QMessageBox::No:
            runSwarm();
            break;
        case QMessageBox::Cancel:
            break;
        default:
            // should never be reached
            break;
    }
}

void MainWindow::runG()
{
    QString cmd = SCRIPTDIR + "/runGeo.sh >> /dev/null 2>&1 &";
    system(cmd.toLatin1().data());
}

void MainWindow::runSwarm()
{
    QString cmd = "/opt/swarm/swarm.sh &";
    system(cmd.toLatin1().data());
}

void MainWindow::eventViewerShow()
{
    eventmon = new EventMon( this );
    eventmon->show();
}

void MainWindow::dataExtractorShow()
{
    dataextractor = new DataExtractor( korean, this );
    dataextractor->show();
}
