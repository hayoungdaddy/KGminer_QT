#include "dataextractor.h"
#include "ui_dataextractor.h"

DataExtractor::DataExtractor(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataExtractor)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;
    c = cfg;

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    this->model = new QSqlQueryModel();

    QString rootDir = c.EVENTDIR + "/TMP";
    QDir d;
    d.setPath(rootDir);
    if(d.exists())
    {
        QString cmd = "rm -rf " + rootDir; system(cmd.toLatin1().data());
    }

    d.mkpath(".");
    d.setPath(rootDir + "/data/mseed");
    d.mkpath(".");
    d.setPath(rootDir + "/data/css");
    d.mkpath(".");
    d.setPath(rootDir + "/TMP/LOC");
    d.mkpath(".");
    d.setPath(rootDir + "/TMP/NLLOC1/model");
    d.mkpath(".");
    d.setPath(rootDir + "/TMP/NLLOC1/time");
    d.mkpath(".");

    selectstafile = new SelectStaFile(cfg, korean, true, this);
    selectstafile->hide();

    ui->pickInfoTW->setColumnWidth(2, 248);

    QDateTime now = QDateTime::currentDateTime();
    now = now.addSecs(-(3600*9));
    ui->dateTimeEdit->setDateTime(now);
    ui->durationED->setText("120");

    /* step 1 */
    connect(ui->loadStaInfoB, SIGNAL(clicked()), this, SLOT(loadStaInfoBClicked()));
    connect(selectstafile, SIGNAL(sendSignaltoDataExtractor()), this, SLOT(rvSignalfromDataExtractor()));
    connect(ui->viewStaLocB, SIGNAL(clicked()), this, SLOT(viewStaLocBClicked()));

    /* step 2 */
    connect(ui->extractB, SIGNAL(clicked()), this, SLOT(extractBClicked()));
    connect(ui->resetB, SIGNAL(clicked()), this, SLOT(resetBClicked()));

    /* step 3 */
    connect(ui->runGeoB, SIGNAL(clicked()), this, SLOT(runGeoBClicked()));
    connect(ui->savePickB, SIGNAL(clicked()), this, SLOT(savePickBClicked()));

    /* step 4 */
    connect(ui->editParamNLLocB, SIGNAL(clicked()), this, SLOT(editParamNLLocBClicked()));
    connect(ui->runNLLocB, SIGNAL(clicked()), this, SLOT(runNLLocBClicked()));

    /* final */
    connect(ui->saveB, SIGNAL(clicked()), this, SLOT(saveBClicked()));
    connect(ui->quitB, SIGNAL(clicked()), this, SLOT(accept()));
}

DataExtractor::~DataExtractor()
{
    delete ui;
}

void DataExtractor::setLanguageEn()
{
    setWindowTitle("New Event Maker");
    ui->step1GB->setTitle("Step 1. Load stations Infomation");
    ui->step2GB->setTitle("Step 2. Extract data");
    ui->step3GB->setTitle("Step 3. Pick using Geotool");
    ui->step4GB->setTitle("Step 4. Run Locator");
    ui->resultGB->setTitle("Result (Database Infomation)");
    ui->loadStaInfoB->setText("Load a station file");
    ui->viewStaLocB->setText("View stations location");
    ui->extractB->setText("Extract data");
    ui->resetB->setText("Reset");
    ui->runGeoB->setText("View waveform");
    ui->savePickB->setText("Save picks");
    ui->editParamNLLocB->setText("Edit parameters for NLLoc");
    ui->runNLLocB->setText("Run NLLoc");
    ui->saveB->setText("Save");
    ui->quitB->setText("Quit");
    ui->staFileNameLB->setText("File name");
    ui->descLB->setText("Description");
    ui->timeLB->setText("Start time (GMT)");
    ui->durationLB->setText("Duration");
}

void DataExtractor::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("신규 이벤트 생성"));
    ui->step1GB->setTitle(codec->toUnicode("Step 1. 관측소 정보 입력"));
    ui->step2GB->setTitle(codec->toUnicode("Step 2. 데이터 추출"));
    ui->step3GB->setTitle(codec->toUnicode("Step 3. P파 피킹"));
    ui->step4GB->setTitle(codec->toUnicode("Step 4. 진앙지 결정"));
    ui->resultGB->setTitle(codec->toUnicode("결과 (Database 정보)"));
    ui->loadStaInfoB->setText(codec->toUnicode("관측소 정보 파일 불러오기"));
    ui->viewStaLocB->setText(codec->toUnicode("관측소 위치 보기"));
    ui->extractB->setText(codec->toUnicode("데이터 추출"));
    ui->resetB->setText(codec->toUnicode("리셋"));
    ui->runGeoB->setText(codec->toUnicode("파형 보기"));
    ui->savePickB->setText(codec->toUnicode("피킹정보 저장"));
    ui->editParamNLLocB->setText(codec->toUnicode("NLLoc 설정값 수정"));
    ui->runNLLocB->setText(codec->toUnicode("NLLoc 실행"));
    ui->saveB->setText(codec->toUnicode("저장"));
    ui->quitB->setText(codec->toUnicode("취소"));
    ui->staFileNameLB->setText(codec->toUnicode("파일명"));
    ui->descLB->setText(codec->toUnicode("설명"));
    ui->timeLB->setText(codec->toUnicode("시작 시간(GMT)"));
    ui->durationLB->setText(codec->toUnicode("기간"));
}

void DataExtractor::loadStaInfoBClicked()
{
    selectstafile->show();
}

void DataExtractor::rvSignalfromDataExtractor()
{
    ui->staInfoTW->clear();

    if(stafile.staName.count() != 0)
    {
        stafile.staName.clear();
        stafile.chanName.clear();
        stafile.netName.clear();
        stafile.locName.clear();
        stafile.latD.clear();
        stafile.lonD.clear();
        stafile.elevKm.clear();
    }
    QFile file;

    file.setFileName(c.EVENTDIR + "/TMP/sta.info");
    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        while(!stream.atEnd())
        {
            line = stream.readLine();

            if(line.startsWith("#") || line.isNull() || line.startsWith("["))
                continue;
            else if(line.startsWith("Filename"))
            {
                stafile.filename = line.section(":",1,1);
            }
            else if(line.startsWith("Description"))
            {
                stafile.description = line.section(":",1,1);
            }
            else
            {
                stafile.staName << line.section(" ", 0, 0);
                stafile.chanName << line.section(" ", 1, 1);
                stafile.locName << line.section(" ", 2, 2);
                stafile.netName << line.section(" ", 3, 3);
                stafile.latD << line.section(" ", 4, 4);
                stafile.lonD << line.section(" ", 5, 5);
                stafile.elevKm << line.section(" ", 6, 6);
            }
        }
        file.close();
    }

    ui->staFileNameLE->setText(stafile.filename);
    ui->descLE->setText(stafile.description);

    ui->staInfoTW->setRowCount(stafile.staName.count());
    for(int i=0;i<stafile.staName.count();i++)
    {
        ui->staInfoTW->setItem(i, 0, new QTableWidgetItem(stafile.staName[i]));
        ui->staInfoTW->setItem(i, 1, new QTableWidgetItem(stafile.chanName[i]));
        ui->staInfoTW->setItem(i, 2, new QTableWidgetItem(stafile.locName[i]));
        ui->staInfoTW->setItem(i, 3, new QTableWidgetItem(stafile.netName[i]));
        ui->staInfoTW->setItem(i, 4, new QTableWidgetItem(stafile.latD[i]));
        ui->staInfoTW->setItem(i, 5, new QTableWidgetItem(stafile.lonD[i]));
        ui->staInfoTW->setItem(i, 6, new QTableWidgetItem(stafile.elevKm[i]));
        ui->staInfoTW->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->staInfoTW->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->staInfoTW->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        ui->staInfoTW->item(i, 3)->setTextAlignment(Qt::AlignCenter);
        ui->staInfoTW->item(i, 4)->setTextAlignment(Qt::AlignCenter);
        ui->staInfoTW->item(i, 5)->setTextAlignment(Qt::AlignCenter);
        ui->staInfoTW->item(i, 6)->setTextAlignment(Qt::AlignCenter);
    }
}

void DataExtractor::viewStaLocBClicked()
{
    QString cmd;
    cmd = "grep -v File " + c.EVENTDIR + "/TMP/sta.info | grep -v Desc > /usr/local/tomcat/webapps/viewstaloc/sta.info";
    system(cmd.toLatin1().data());
    cmd = "firefox 127.0.0.1:8080/viewstaloc/index.jsp &";
    system(cmd.toLatin1().data());
}

/* step 2 */
void DataExtractor::resetBClicked()
{
    QDateTime now = QDateTime::currentDateTime();
    now = now.addSecs(-(3600*9));
    ui->dateTimeEdit->setDateTime(now);
    ui->durationED->setText("120");
}

void DataExtractor::extractBClicked()
{
    QDateTime st( ui->dateTimeEdit->dateTime() ) ;
    QString ststr = st.toString("yyyy/MM/dd,hh:mm:ss");
    QString year = st.toString("yyyy");
    QDate date = QDate::fromString(st.toString("yyyyMMdd"), "yyyyMMdd");
    QString jday = QString::number(date.dayOfYear());

    for(int i=0;i<stafile.staName.count();i++)
    {
        QString scn = c.MSEEDDIR + "/" + year + "/" + stafile.netName[i] + "/" + stafile.staName[i] + "/" + stafile.chanName[i] + ".D/*";
        QString cmd;
        cmd = "qmerge -T -f " + ststr + " -s " + ui->durationED->text() + " -b 512 -o " + c.EVENTDIR + "/TMP/data/mseed/" +
                stafile.staName[i] + ".mseed" + " " + scn;
        system(cmd.toLatin1().data());
    }

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Completed extrating data");
    else msgBox.setText(codec->toUnicode("관측소 정보 파일 수정 완료."));
    msgBox.exec();
}

void DataExtractor::runGeoBClicked()
{
    QString cmd;
    cmd = c.SCRIPTDIR + "/mseed2cssUsingGeotool.sh TMP";
    system(cmd.toLatin1().data());
    cmd = c.SCRIPTDIR + "/viewWave.sh TMP >> /dev/null 2>&1 &";
    system(cmd.toLatin1().data());
}

void DataExtractor::savePickBClicked()
{
    QString cmd = "cp " + c.EVENTDIR + "/TMP/data/css/css.arrival " + c.EVENTDIR + "/TMP/data/css/.css.arrival";
    system(cmd.toLatin1().data());

    cmd = c.SCRIPTDIR + "/makePickforNLLoc.sh TMP";
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    msgBox.setText("Saved picklists.");
    msgBox.exec();

    ui->pickInfoTW->setRowCount(0);

    cmd = "cp " + c.EVENTDIR + "/TMP/picklist " + c.EVENTDIR + "/TMP/TMP/picklist";
    system(cmd.toLatin1().data());

    QFile file;
    file.setFileName(c.EVENTDIR + "/TMP/picklist");
    if(file.open( QIODevice::ReadOnly ))
    {
        QTextStream stream(&file);
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            ui->pickInfoTW->setRowCount(ui->pickInfoTW->rowCount() + 1);
            ui->pickInfoTW->setItem(ui->pickInfoTW->rowCount()-1, 0, new QTableWidgetItem(_line.section(' ', 0, 0)));
            ui->pickInfoTW->setItem(ui->pickInfoTW->rowCount()-1, 1, new QTableWidgetItem(_line.section(' ', 2, 2)));
            ui->pickInfoTW->setItem(ui->pickInfoTW->rowCount()-1, 2, new QTableWidgetItem(_line.section(' ', 3, 3).mid(3,4) + "-" +
                                                                                          _line.section(' ', 3, 3).mid(7,2) + "-" +
                                                                                        _line.section(' ', 3, 3).mid(9,2) + " " +
                                                                                        _line.section(' ', 3, 3).mid(11,2) + ":" +
                                                                                        _line.section(' ', 3, 3).mid(13,2) + ":" +
                                                                                        _line.section(' ', 4, 4).left(2) + "." +
                                                                                        _line.section(' ', 4, 4).right(2)));
            ui->pickInfoTW->item(ui->pickInfoTW->rowCount()-1, 0)->setTextAlignment(Qt::AlignCenter);
            ui->pickInfoTW->item(ui->pickInfoTW->rowCount()-1, 1)->setTextAlignment(Qt::AlignCenter);
            ui->pickInfoTW->item(ui->pickInfoTW->rowCount()-1, 2)->setTextAlignment(Qt::AlignCenter);

        }
        file.close();
    }
}

void DataExtractor::editParamNLLocBClicked()
{
    nlloc = new NLLoc(c, korean, "TMP", "TMP", "1", this);
    nlloc->show();
}

void DataExtractor::runNLLocBClicked()
{
    ui->runNLLocB->setEnabled(false);

    QFile file;
    file.setFileName(c.EVENTDIR + "/TMP/TMP/NLLOC/type");
    QString minv,maxv;

    if(file.open( QIODevice::ReadOnly ))
    {
        QTextStream stream(&file);
        QString line;

        line = stream.readLine();
        TYPE = line.section(":", 0, 0);

        if(TYPE == "MVM")
        {
            minv = line.section(":", 1, 1);
            maxv = line.section(":", 2, 2);
        }

        file.close();
    }

    QString cmd = "cp " + c.EVENTDIR + "/TMP/data/css/css.arrival " + c.EVENTDIR + "/TMP/data/css/.css.arrival";
    system(cmd.toLatin1().data());

    if(TYPE == "SVM")
    {
        cmd = c.SCRIPTDIR + "/runNLLocSVM.sh TMP TMP 0";
    }
    else if(TYPE == "MVM")
    {
        cmd = c.SCRIPTDIR + "/runNLLocMVM.sh TMP TMP 0 " + minv + " " + maxv;
    }
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    msgBox.setText("Finished NLLoc.");
    msgBox.exec();

    ui->runNLLocB->setEnabled(true);

    file.setFileName(c.EVENTDIR + "/TMP/TMP/LOC/NLLOC.origin");

    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            if(_line.startsWith("#") || _line.isNull() || _line.startsWith("[") )
                continue;
            else
            {
                if(_line.startsWith("GEOGRAPHIC"))
                {
                    ui->latLE->setText(_line.section(' ', 9, 9));
                    ui->lonLE->setText(_line.section(' ', 11, 11));
                    ui->depthLE->setText(_line.section(' ', 13, 13));
                    ui->timeLE->setText(_line.section(' ', 2, 2) + "-" + _line.section(' ', 3, 3) + "-"
                                        + _line.section(' ', 4, 4) + " " + _line.section(' ', 5, 5) + ":"
                                        + _line.section(' ', 6, 6) + ":" + _line.section(' ', 7, 7).section('.', 0, 0));
                    if(TYPE == "SVM")
                        ui->algoLE->setText("NLLoc_SVM");
                    else if(TYPE == "MVM")
                        ui->algoLE->setText("NLLoc_MVM");
                }
            }
        }
        file.close();
    }
    ui->saveB->setEnabled(true);

    model->setQuery("SELECT max(evid) FROM event");
    int temp = model->record(0).value("max(evid)").toInt();
    temp = temp + 1;
    EVID = QString::number(temp);

    ui->evidLE->setText(EVID);
    model->setQuery("SELECT max(orid) FROM origin");
    temp = model->record(0).value("max(orid)").toInt();
    temp = temp + 1;
    ORID = QString::number(temp);
    ui->oridLE->setText(ORID);
}

void DataExtractor::saveBClicked()
{
    QString cmd;
    cmd = "mv " + c.EVENTDIR + "/TMP " + c.EVENTDIR + "/" + EVID; system(cmd.toLatin1().data());
    cmd = "mv " + c.EVENTDIR + "/" + EVID + "/TMP " + c.EVENTDIR + "/" + EVID + "/" + ORID; system(cmd.toLatin1().data());
    cmd = "cp -R " + c.EVENTDIR + "/" + EVID + "/data " + c.EVENTDIR + "/" + EVID + "/" + ORID + "/"; system(cmd.toLatin1().data());

    cmd = c.SCRIPTDIR + "/inputDatatoDB.sh " + EVID + " " + ORID + " Manual " + TYPE;
    //qDebug() << cmd;
    system(cmd.toLatin1().data());

    cmd = "pkill geotool";
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    msgBox.setText("Added new event & origin.");
    msgBox.exec();

    accept();
}
