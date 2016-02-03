#include "dataextractor.h"
#include "ui_dataextractor.h"

DataExtractor::DataExtractor(bool iskorean, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataExtractor)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean = iskorean;

    this->model = new QSqlQueryModel();

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DB);
    if(!db.open())
    {
        QMessageBox msgBox;
        msgBox.setText(db.lastError().text());
        msgBox.exec();
        return;
    }

    selectstafile = new SelectStaFile(true, this);
    selectstafile->hide();

    ui->pickInfoTW->setColumnWidth(2, 248);


    /* find dir & initial */
    /*
    MAINDIR = EVENTDIR + "/" + EVID;
    QString cmd = "mkdir " + MAINDIR; system(cmd.toLatin1().data());
    cmd = "mkdir " + MAINDIR + "/data"; system(cmd.toLatin1().data());
    MAINDIR = MAINDIR + "/" + ORID;
    cmd = "mkdir " + MAINDIR; system(cmd.toLatin1().data());
    cmd = "mkdir " + MAINDIR + "/LOC"; system(cmd.toLatin1().data());
    cmd = "mkdir " + MAINDIR + "/NLLOC"; system(cmd.toLatin1().data());
    */

    //qDebug() << EVID + " " + ORID;

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

void DataExtractor::loadStaInfoBClicked()
{
    QString cmd = "rm -rf " + EVENTDIR + "/TMP  >> /dev/null 2>&1"; system(cmd.toLatin1().data());
    cmd = "mkdir " + EVENTDIR + "/TMP"; system(cmd.toLatin1().data());
    cmd = "mkdir " + EVENTDIR + "/TMP/data"; system(cmd.toLatin1().data());
    cmd = "mkdir " + EVENTDIR + "/TMP/data/mseed"; system(cmd.toLatin1().data());
    cmd = "mkdir " + EVENTDIR + "/TMP/data/css"; system(cmd.toLatin1().data());

    cmd = "mkdir " + EVENTDIR + "/TMP/TMP"; system(cmd.toLatin1().data());
    cmd = "mkdir " + EVENTDIR + "/TMP/TMP/LOC"; system(cmd.toLatin1().data());
    cmd = "mkdir " + EVENTDIR + "/TMP/TMP/NLLOC"; system(cmd.toLatin1().data());
    cmd = "cp " + PARAMSDIR + "/NLLOC/1/grid_p.in " + EVENTDIR + "/TMP/TMP/NLLOC/"; system(cmd.toLatin1().data());
    cmd = "cp " + PARAMSDIR + "/NLLOC/1/nlloc.in " + EVENTDIR + "/TMP/TMP/NLLOC/"; system(cmd.toLatin1().data());
    cmd = "echo SVM > " + EVENTDIR + "/TMP/TMP/NLLOC/type"; system(cmd.toLatin1().data());

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

    file.setFileName(EVENTDIR + "/TMP/sta.info");
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
    cmd = "grep -v File " + EVENTDIR + "/TMP/sta.info | grep -v Desc > /usr/local/tomcat/webapps/viewstaloc/sta.info";
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
        QString scn = MSEEDDIR + "/" + year + "/" + stafile.netName[i] + "/" + stafile.staName[i] + "/" + stafile.chanName[i] + ".D/*";
        QString cmd;
        cmd = "qmerge -T -f " + ststr + " -s " + ui->durationED->text() + " -b 512 -o " + EVENTDIR + "/TMP/data/mseed/" +
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
    cmd = SCRIPTDIR + "/mseed2cssUsingGeotool.sh TMP";
    system(cmd.toLatin1().data());
    cmd = SCRIPTDIR + "/viewWave.sh TMP >> /dev/null 2>&1 &";
    system(cmd.toLatin1().data());
}

void DataExtractor::savePickBClicked()
{
    QString cmd = "cp " + EVENTDIR + "/TMP/data/css/css.arrival " + EVENTDIR + "/TMP/data/css/.css.arrival";
    system(cmd.toLatin1().data());

    cmd = SCRIPTDIR + "/makePickforNLLoc.sh TMP";
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    msgBox.setText("Saved picklists.");
    msgBox.exec();

    ui->pickInfoTW->setRowCount(0);

    cmd = "cp " + EVENTDIR + "/TMP/picklist " + EVENTDIR + "/TMP/TMP/picklist";
    system(cmd.toLatin1().data());

    QFile file;
    file.setFileName(EVENTDIR + "/TMP/picklist");
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
    nlloc_gen(stafile);
    nlloc = new NLLoc("TMP", "TMP", "", this);
    nlloc->setup();
    nlloc->show();
}

void DataExtractor::nlloc_gen(STAFILE stafile)
{
    /* generate grid_p.in file */
    QString vgout, vggrid, gtfiles, locgrid, locfiles;
    QStringList layer;
    QFile file;

    QString avgLatforNLLoc, avgLonforNLLoc;
    double minlat, maxlat, minlon, maxlon, avglat, avglon;

    int scnCount = stafile.staName.count();

    minlat = 999; maxlat = 0; minlon = 999; maxlon = 0;

    for(int i=0;i<scnCount;i++)
    {
        double fx, fy;

        fx = stafile.latD[i].toDouble();
        fy = stafile.lonD[i].toDouble();
        if(fx < minlat) minlat = fx;
        if(fx > maxlat) maxlat = fx;
        if(fy < minlon) minlon = fy;
        if(fy > maxlon) maxlon = fy;
    }

    avglat = (minlat + maxlat) / 2;
    avglon = (minlon + maxlon) / 2;
    avgLatforNLLoc = avgLatforNLLoc.setNum(avglat, 'f', 6);
    avgLonforNLLoc = avgLonforNLLoc.setNum(avglon, 'f', 6);

    double fst, lst;
    fst = avglat-2;
    lst = avglat+2;

    file.setFileName(EVENTDIR + "/TMP/TMP/NLLOC/grid_p.in");
    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        while(!stream.atEnd())
        {
            line = stream.readLine();

            if(line.startsWith("#") || line.isNull() || line.startsWith("["))
                continue;
            else if(line.startsWith("VGOUT"))
                vgout = line;
            else if(line.startsWith("VGGRID"))
                vggrid = line;
            else if(line.startsWith("LAYER"))
                layer << line;
            else if(line.startsWith("GTFILES"))
                gtfiles = line;
        }
        file.close();
    }
    if( file.open( QIODevice::WriteOnly ))
    {
        QTextStream stream( &file ) ;
        // TRANS  LAMBERT  WGS-84  38.300629 127.320459  37 40  0.0
        stream << "CONTROL 1 54321" << "\n";
        stream << "TRANS LAMBERT WGS-84 " << avgLatforNLLoc << " " << avgLonforNLLoc << " " << QString::number(fst) << " " << QString::number(lst) << " 0.0" << "\n";
        stream << vgout << "\n";
        stream << "VGTYPE P" << "\n";
        stream << vggrid << "\n";
        for(int i=0;i<layer.count();i++)
            stream << layer[i] << "\n";
        stream << gtfiles << "\n";
        stream << "GTMODE GRID3D ANGLES_NO" << "\n";
        for(int i=0;i<scnCount;i++)
            stream << "GTSRCE " << stafile.staName[i] << " LATLON " << stafile.latD[i] << " " << stafile.lonD[i] << " 0 " << stafile.elevKm[i] << "\n";
        stream << "GT_PLFD  1.0e-3  0" << "\n";

        file.close();
    }

    file.setFileName(EVENTDIR + "/TMP/TMP/NLLOC/nlloc.in");
    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        while(!stream.atEnd())
        {
            line = stream.readLine();

            if(line.startsWith("#") || line.isNull() || line.startsWith("["))
                continue;
            else if(line.startsWith("LOCGRID"))
                locgrid = line;
            else if(line.startsWith("LOCFILES"))
                locfiles = line;
        }
        file.close();
    }
    if( file.open( QIODevice::WriteOnly ))
    {
        QTextStream stream( &file ) ;
        stream << "CONTROL 1 54321" << "\n";
        stream << "TRANS LAMBERT WGS-84 " << avgLatforNLLoc << " " << avgLonforNLLoc << " " << QString::number(fst) << " " << QString::number(lst) << " 0.0" << "\n";
        stream << "LOCSIG  KGminer" << "\n";
        stream << "LOCCOM KGminer" << "\n";
        stream << "LOCMETH  EDT_OT_WT 1.0e6 2 25  -1 -1.80 6 -1.0 0" << "\n";
        stream << "LOCHYPOUT  SAVE_NLLOC_ALL SAVE_HYPOINVERSE_Y2000_ARC" << "\n";
        stream << "LOCGAU 0.5 0.0" << "\n";
        stream << "LOCGAU2 0.02 0.1 2.0" << "\n";
        stream << "LOCPHASEID  P   P p G PN Pn P* Pg PG Pb PB" << "\n";
        stream << "LOCPHASEID  S   S s G SN Sn S* Sg SG Sb SB" << "\n";
        stream << "LOCQUAL2ERR 0.1 0.2 0.5 1.0 99999.9" << "\n";
        stream << "LOCSEARCH  OCT 10 10 4 0.01 10000 5000 0 1" << "\n";
        stream << locgrid << "\n";
        stream << "LOCPHSTAT 9999.0 -1 9999.0 1.0 1.0 9999.9 -9999.9 9999.9" << "\n";
        stream << "LOCANGLES ANGLES_NO 5" << "\n";
        stream << "LOCMAG  ML_HB 1.0 1.110 0.00189" << "\n";
        stream << "LOCELEVCORR 1 5.8 3.46" << "\n";
        stream << "LOCSTAWT 0 -1.0" << "\n";
        stream << locfiles << "\n";
        stream << "LOCHYPOUT SAVE_HYPOINVERSE_Y2000_ARC" << "\n";

        file.close();
    }
}

void DataExtractor::runNLLocBClicked()
{
    ui->runNLLocB->setEnabled(false);

    QFile file;
    file.setFileName(EVENTDIR + "/TMP/TMP/NLLOC/type");
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

    QString cmd = "cp " + EVENTDIR + "/TMP/data/css/css.arrival " + EVENTDIR + "/TMP/data/css/.css.arrival";
    system(cmd.toLatin1().data());

    if(TYPE == "SVM")
    {
        cmd = SCRIPTDIR + "/runNLLocSVM.sh TMP TMP 0";
    }
    else if(TYPE == "MVM")
    {
        cmd = SCRIPTDIR + "/runNLLocMVM.sh TMP TMP 0 " + minv + " " + maxv;
    }
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    msgBox.setText("Finished NLLoc.");
    msgBox.exec();

    ui->runNLLocB->setEnabled(true);

    file.setFileName(EVENTDIR + "/TMP/TMP/LOC/NLLOC.origin");

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
    cmd = "mv " + EVENTDIR + "/TMP " + EVENTDIR + "/" + EVID; system(cmd.toLatin1().data());
    cmd = "mv " + EVENTDIR + "/" + EVID + "/TMP " + EVENTDIR + "/" + EVID + "/" + ORID; system(cmd.toLatin1().data());
    cmd = "cp -R " + EVENTDIR + "/" + EVID + "/data " + EVENTDIR + "/" + EVID + "/" + ORID + "/"; system(cmd.toLatin1().data());

    cmd = SCRIPTDIR + "/inputDatatoDB.sh " + EVID + " " + ORID + " Manual " + TYPE;
    qDebug() << cmd;
    system(cmd.toLatin1().data());

    cmd = "pkill geotool";
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    msgBox.setText("Added new event & origin.");
    msgBox.exec();

    accept();
}
