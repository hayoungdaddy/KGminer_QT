#include "makeorigin.h"
#include "ui_makeorigin.h"

MakeOrigin::MakeOrigin(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MakeOrigin)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );

    c = cfg;
    korean = _korean;

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    this->model = new QSqlQueryModel();
}

MakeOrigin::~MakeOrigin()
{
    delete ui;
}
void MakeOrigin::setup(QString evid, QString firstorid)
{
    ui->evidLE->setText(evid);
    ui->latLE->clear();
    ui->lonLE->clear();
    ui->depthLE->clear();
    ui->timeLE->clear();
    ui->alLE->clear();

    EVID = evid;
    FIRSTORID = firstorid;

    ui->editButton->setEnabled(false);
    ui->nllocButton->setEnabled(false);
    ui->inputDBButton->setEnabled(false);
    ui->mapButton->setEnabled(false);

    ui->progressBar->setValue(0);
}

void MakeOrigin::setLanguageEn()
{
    setWindowTitle("Make a new origin");
    ui->originInfo->setTitle("New origin record Info.");
    ui->showButton->setText("Show wave");
    ui->saveButton->setText("Save Picklists");
    ui->editButton->setText("Edit NLLoc params");
    ui->nllocButton->setText("Run NLLoc");
    ui->mapButton->setText("Map");
    ui->inputDBButton->setText("Save");
    ui->quitButton->setText("Cancel");
}

void MakeOrigin::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("신규 origin 생성"));
    ui->originInfo->setTitle(codec->toUnicode("신규 origin 레코드 정보"));
    ui->showButton->setText(codec->toUnicode("파형 보기"));
    ui->saveButton->setText(codec->toUnicode("Pick 리스트 저장"));
    ui->editButton->setText(codec->toUnicode("NLLoc 파라메터 수정"));
    ui->nllocButton->setText(codec->toUnicode("NLLoc 실행"));
    ui->mapButton->setText(codec->toUnicode("지도"));
    ui->inputDBButton->setText(codec->toUnicode("저장"));
    ui->quitButton->setText(codec->toUnicode("취소"));
}

/*
void MakeOrigin::closeEvent(QCloseEvent *event)
{
    QString cmd = "pkill geotool";
    system(cmd.toLatin1().data());

    cmd = "rm -rf " + c.EVENTDIR + "/" + EVID + "/" + ORID;
    system(cmd.toLatin1().data());
}
*/

void MakeOrigin::on_quitButton_clicked()
{
    if(!korean)
    {
        if( !QMessageBox::question( this,
                                    "Warning",
                    "Are you sure?",
                    "Yes",
                    "No",
                    QString::null, 1, 1 ) )
        {
            QString cmd = "pkill geotool";
            system(cmd.toLatin1().data());

            QDir dir;
            dir.setPath(c.EVENTDIR + "/" + EVID + "/NEWORIGIN");
            dir.removeRecursively();

            accept();
        }
        else
            return;
    }
    else
    {
        if( !QMessageBox::question( this,
                                    "Warning",
                    codec->toUnicode("종료하시겠습니까?"),
                    "Yes",
                    "No",
                    QString::null, 1, 1 ) )
        {
            QString cmd = "pkill geotool";
            system(cmd.toLatin1().data());

            QDir dir;
            dir.setPath(c.EVENTDIR + "/" + EVID + "/NEWORIGIN");
            dir.removeRecursively();

            accept();
        }
        else
            return;
    }

}

void MakeOrigin::on_showButton_clicked()
{
    QString cmd = c.SCRIPTDIR + "/viewWave.sh " + EVID + " NEWORIGIN >> /dev/null 2>&1 &";
    system(cmd.toLatin1().data());
}

void MakeOrigin::on_saveButton_clicked()
{
    QString cmd = "cp " + c.EVENTDIR + "/" + EVID + "/NEWORIGIN/data/css/css.arrival " + c.EVENTDIR + "/" + EVID + "/NEWORIGIN/data/css/.css.arrival";
    system(cmd.toLatin1().data());

    cmd = c.SCRIPTDIR + "/cssarrivalTopicklist.sh " + EVID + " NEWORIGIN";
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    if(!korean)
        msgBox.setText("Saved picklists.");
    else
        msgBox.setText(codec->toUnicode("Pick 리스트 저장 완료"));
    msgBox.exec();

    ui->editButton->setEnabled(true);
    ui->nllocButton->setEnabled(true);
}

void MakeOrigin::on_editButton_clicked()
{
    NLLoc *nlloc = new NLLoc(c, korean, EVID, "NEWORIGIN", "1", this);
    nlloc->show();  
}

void MakeOrigin::on_nllocButton_clicked()
{
    // 1. change ID to EVID, ORID string
    // 2. run NLLOC
    // 3. change realID
    ui->progressBar->setValue(0);

    QDir d;
    d.setPath(c.EVENTDIR + "/" + EVID + "/NEWORIGIN/LOC");
    if(d.exists())
        d.removeRecursively();
    d.mkpath(".");

    QFile file;
    file.setFileName(c.EVENTDIR + "/" + EVID + "/NEWORIGIN/NLLOC/type");
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

    if(TYPE.startsWith("SVM"))
    {
        rechangeEVIDORID(c.EVENTDIR + "/" + EVID + "/NEWORIGIN/NLLOC", "grid_p.in", EVID.toInt(), FIRSTORID.toInt());
        rechangeEVIDORID(c.EVENTDIR + "/" + EVID + "/NEWORIGIN/NLLOC", "nlloc.in", EVID.toInt(), FIRSTORID.toInt());
    }
    else if(TYPE.startsWith("MVM"))
    {
        float minVel = minv.toFloat();
        float maxVel = maxv.toFloat();

        for(float i=minVel;i<=maxVel;i=i+0.1)
        {
            rechangeEVIDORID(c.EVENTDIR + "/" + EVID + "/NEWORIGIN/NLLOC/" + QString::number(i, 'f', 1), "grid_p.in", EVID.toInt(), FIRSTORID.toInt());
            rechangeEVIDORID(c.EVENTDIR + "/" + EVID + "/NEWORIGIN/NLLOC/" + QString::number(i, 'f', 1), "nlloc.in", EVID.toInt(), FIRSTORID.toInt());
        }
    }

    ui->progressBar->setValue(20);

    if(TYPE.startsWith("SVM"))
        runNLLoc(c.EVENTDIR, EVID.toInt(), 0, "NEWORIGIN", TYPE, 0, 0);
    else if(TYPE.startsWith("MVM"))
        runNLLoc(c.EVENTDIR, EVID.toInt(), 0, "NEWORIGIN", TYPE, minv.toFloat(), maxv.toFloat());

    ui->progressBar->setValue(80);

    file.setFileName(c.EVENTDIR + "/" + EVID + "/NEWORIGIN/LOC/origin");

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
                    /*
                    ui->timeLE->setText(_line.section(' ', 2, 2) + "-" + _line.section(' ', 3, 3) + "-"
                                        + _line.section(' ', 4, 4) + " " + _line.section(' ', 5, 5) + ":"
                                        + _line.section(' ', 6, 6) + ":" + _line.section(' ', 7, 7).section('.', 0, 0));
                                        */

                    if(_line.section(' ', 7, 7).section('.', 0, 0) == "0")
                    {
                        EVNAME = _line.section(' ', 2, 2) + "-" + _line.section(' ', 3, 3) + "-" + _line.section(' ', 4, 4)
                            + " " + _line.section(' ', 5, 5) + ":" + _line.section(' ', 6, 6) + ":" + "00";

                        ORITIME = _line.section(' ', 2, 2) + _line.section(' ', 3, 3) + _line.section(' ', 4, 4)
                                + _line.section(' ', 5, 5) + _line.section(' ', 6, 6) + "00." + _line.section(' ', 7, 7).section('.', 1, 1);
                    }
                    else if(_line.section(' ', 7, 7).section('.', 0, 0).toInt() > 0 && _line.section(' ', 7, 7).section('.', 0, 0).toInt() < 10 )
                    {
                        EVNAME = _line.section(' ', 2, 2) + "-" + _line.section(' ', 3, 3) + "-" + _line.section(' ', 4, 4)
                            + " " + _line.section(' ', 5, 5) + ":" + _line.section(' ', 6, 6) + ":" + "0" + _line.section(' ', 7, 7).section('.', 0, 0);

                        ORITIME = _line.section(' ', 2, 2) + _line.section(' ', 3, 3) + _line.section(' ', 4, 4)
                                + _line.section(' ', 5, 5) + _line.section(' ', 6, 6) + "0" + _line.section(' ', 7, 7).section('.', 0, 0) + "."+ _line.section(' ', 7, 7).section('.', 1, 1);
                    }
                    else
                    {
                        EVNAME = _line.section(' ', 2, 2) + "-" + _line.section(' ', 3, 3) + "-" + _line.section(' ', 4, 4)
                            + " " + _line.section(' ', 5, 5) + ":" + _line.section(' ', 6, 6) + ":" + _line.section(' ', 7, 7).section('.', 0, 0);

                        ORITIME = _line.section(' ', 2, 2) + _line.section(' ', 3, 3) + _line.section(' ', 4, 4)
                                + _line.section(' ', 5, 5) + _line.section(' ', 6, 6) + _line.section(' ', 7, 7);
                    }

                    ui->timeLE->setText(EVNAME);

                    if(TYPE == "SVM")
                        ui->alLE->setText("NLLoc_SVM");
                    else if(TYPE == "MVM")
                        ui->alLE->setText("NLLoc_MVM");
                }
            }
        }
        file.close();
    }
    ui->progressBar->setValue(100);

    QMessageBox msgBox;
    if(!korean)
        msgBox.setText("Finished NLLoc.");
    else
        msgBox.setText(codec->toUnicode("NLLOC 실행 완료."));
    msgBox.exec();

    ui->mapButton->setEnabled(true);
    ui->inputDBButton->setEnabled(true);

}

void MakeOrigin::on_inputDBButton_clicked()
{
    this->model = new QSqlQueryModel();
    model->setQuery("SELECT max(orid) FROM origin");
    int maxorid = model->record(0).value("max(orid)").toInt() + 1;
    model->setQuery("SELECT max(arid) FROM assoc");
    int maxarid = model->record(0).value("max(arid)").toInt() + 1;
    QDate today = QDate::currentDate();
    QString insertQuery;
    QFile file;
    file.setFileName(c.EVENTDIR + "/" + EVID + "/NEWORIGIN/picklist");
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream( &file );
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            insertQuery = "INSERT INTO assoc (arid, orid, sta, chan, time, phase, p_algorithm, lddate) VALUES ("
                    + QString::number(maxarid) + ", " + QString::number(maxorid) + ", '" + _line.section(' ', 0, 0) + "', '"
                    + _line.section(' ', 2, 2) + "', '" + _line.section(' ', 3, 3).right(12)
                    + _line.section(' ', 4, 4).left(2) + "." + _line.section(' ', 4, 4).right(2)
                    + "', '" + "P" + "', 'FP', '" + today.toString("yyyyMMdd") + "')";
            //qDebug() << insertQuery;
            model->setQuery(insertQuery);
            maxarid++;

        }
        file.close();
    }
    QString jdate;
    QDate date;
    date.setDate(ui->timeLE->text().section('-', 0, 0).toInt(), ui->timeLE->text().section('-', 1, 1).toInt(),
                 ui->timeLE->text().section('-', 2, 2).section(' ', 0, 0).toInt());
    jdate =     jdate = QString::number(date.year()) + QString::number(date.dayOfYear());
    insertQuery = "INSERT INTO origin (lat, lon, depth, time, orid, evid, jdate, l_algorithm, lddate) VALUES ('"
            + ui->latLE->text() + "', '" + ui->lonLE->text() + "', '" + ui->depthLE->text()
            + "', '" + ORITIME + "', " + QString::number(maxorid) + ", "
            + EVID + ", '" + jdate + "', '" + ui->alLE->text() + "', '" + today.toString("yyyyMMdd") + "')";
    //qDebug() << insertQuery;
    model->setQuery(insertQuery);

    QString cmd;
    cmd = "pkill geotool";
    system(cmd.toLatin1().data());

    cmd = "mv " + c.EVENTDIR + "/" + EVID + "/NEWORIGIN " + c.EVENTDIR + "/" + EVID + "/" + QString::number(maxorid);
    system(cmd.toLatin1().data());

    // to /media/sf_KGminer/EVENTLOC/EVID/ORID/location.txt
    // 2016-03-25 15:32:03 37.353215 127.534312
    QDir d;
    QString locdir = c.LOCDIR + "/" + EVID;
    d.setPath(locdir);
    if(!d.exists()) d.mkpath(".");
    locdir = c.LOCDIR + "/" + EVID + "/" + QString::number(maxorid);
    d.setPath(locdir);
    if(!d.exists()) d.mkpath(".");
    file.setFileName(c.LOCDIR + "/" + EVID + "/" + QString::number(maxorid) + "/location.txt");
    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;
        stream <<  EVNAME << " " << ui->latLE->text() << " " << ui->lonLE->text();
        file.close() ;
    }

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Added new origin.");
    else msgBox.setText(codec->toUnicode("신규 Origin 저장 완료."));
    msgBox.exec();

    emit resetTable();

    accept();
}

void MakeOrigin::on_mapButton_clicked()
{
    QString cmd;
    cmd = "rm /usr/local/tomcat/webapps/eventviewer/origin.info"; system(cmd.toLatin1().data());
    cmd = "grep -v File " + c.EVENTDIR + "/" + EVID + "/sta.info | grep -v Desc > /usr/local/tomcat/webapps/eventviewer/sta.info"; system(cmd.toLatin1().data());

    QString tot = "Not_yet_decided. " + ui->latLE->text() + " " + ui->lonLE->text() + " " + ui->depthLE->text() + " " +
            ui->timeLE->text() + " " + ui->alLE->text() + " ";

    cmd = "echo \"" + EVID + " " + tot + "\" >> /usr/local/tomcat/webapps/eventviewer/origin.info"; system(cmd.toLatin1().data());

    cmd = "firefox 127.0.0.1:8080/eventviewer &";
    system(cmd.toLatin1().data());
}
