#include "eventmon.h"
#include "ui_eventmon.h"

EventMon::EventMon(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventMon)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );

    c = cfg;
    korean = _korean;

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    makeorigin = new MakeOrigin(c, korean, this);
    makeorigin->hide();

    this->model = new QSqlQueryModel();

    setEventTable();
    model->setQuery("SELECT max(evid) FROM event");
    evid = model->record(0).value("max(evid)").toString();
    model->setQuery("SELECT min(orid) FROM origin where evid = " + evid);
    orid = model->record(0).value("min(orid)").toString();
    setOriginTable(evid);
    setAssocTable(orid);

    /* Process Status Setup */
    resetTableTimer = new QTimer(this);
    resetTableTimer->start( 5000 );

    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->eventTable, SIGNAL(cellClicked(int,int)), this, SLOT(clickEventTable(int,int)));
    connect(ui->originTable, SIGNAL(cellClicked(int, int)), this, SLOT(clickOriginTable(int,int)));
    connect(makeorigin, SIGNAL(resetTable()), this, SLOT(_resetTable()));
    connect(resetTableTimer, SIGNAL(timeout()), this, SLOT(_resetTable()));
}

EventMon::~EventMon()
{
    delete ui;
}

void EventMon::setLanguageEn()
{
    setWindowTitle("Event Lists");
    ui->makeOrigin->setText("Make a new origin");
    ui->quitButton->setText("quit");
}

void EventMon::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("이벤트 리스트"));
    ui->makeOrigin->setText(codec->toUnicode("신규 Origin 생성"));
    ui->quitButton->setText(codec->toUnicode("종료"));
}

void EventMon::setup()
{
    QModelIndex index1 = ui->eventTable->currentIndex();
    setEventTable();

    ui->eventTable->setCurrentIndex(index1);
}

void EventMon::setEventTable()
{
    ui->eventTable->verticalHeader()->setVisible(false);
    ui->eventTable->setColumnWidth(0, 50);
    ui->eventTable->setColumnWidth(1, 170);
    ui->eventTable->setColumnWidth(2, 95);
    ui->eventTable->setColumnWidth(3, 85);
    ui->eventTable->setColumnWidth(4, 90);

    model->setQuery("SELECT evid, evname FROM event ORDER BY evid DESC");

    ui->eventTable->setRowCount(0);

    for(int i=0;i<model->rowCount();++i)
    {
        QString evid = model->record(i).value("evid").toString();
        QString evname = model->record(i).value("evname").toString();
        evname = evname.section('.',0,0);
        ui->eventTable->setRowCount(ui->eventTable->rowCount()+1);
        ui->eventTable->setItem(i, 0, new QTableWidgetItem(evid));
        ui->eventTable->setItem(i, 1, new QTableWidgetItem(evname));
        ui->eventTable->setItem(i, 2, new QTableWidgetItem("Link"));
        ui->eventTable->setItem(i, 3, new QTableWidgetItem("View"));
        ui->eventTable->setItem(i, 4, new QTableWidgetItem("Delete"));
        ui->eventTable->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->eventTable->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->eventTable->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        ui->eventTable->item(i, 3)->setTextAlignment(Qt::AlignCenter);
        ui->eventTable->item(i, 4)->setTextAlignment(Qt::AlignCenter);
    }
}

void EventMon::setOriginTable(QString evid)
{
    ui->originTable->verticalHeader()->setVisible(false);
    ui->originTable->setColumnWidth(0, 50);
    ui->originTable->setColumnWidth(4, 168);

    model->setQuery("SELECT lat, lon, depth, time, orid, evid, l_algorithm, lddate FROM origin WHERE evid = " + evid);

    ui->originTable->setRowCount(0);

    for(int i=0;i<model->rowCount();++i)
    {
        QString orid = model->record(i).value("orid").toString();
        double lat = model->record(i).value("lat").toDouble();
        double lon = model->record(i).value("lon").toDouble();
        double depth = model->record(i).value("depth").toDouble();
        QString time = model->record(i).value("time").toString();
        time = time.left(4) + "-" + time.mid(4,2) + "-" + time.mid(6,2) + " " + time.mid(8,2) + ":" + time.mid(10,2) + ":" + time.section('.',0,0).mid(12,2);
        QString l_algorithm = model->record(i).value("l_algorithm").toString();
        ui->originTable->setRowCount(ui->originTable->rowCount()+1);
        ui->originTable->setItem(i, 0, new QTableWidgetItem(orid));
        ui->originTable->setItem(i, 1, new QTableWidgetItem(QString::number(lat)));
        ui->originTable->setItem(i, 2, new QTableWidgetItem(QString::number(lon)));
        ui->originTable->setItem(i, 3, new QTableWidgetItem(QString::number(depth)));
        ui->originTable->setItem(i, 4, new QTableWidgetItem(time));
        ui->originTable->setItem(i, 5, new QTableWidgetItem(l_algorithm));
        ui->originTable->setItem(i, 6, new QTableWidgetItem("Link"));
        ui->originTable->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 3)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 4)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 5)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 6)->setTextAlignment(Qt::AlignCenter);
    }
}

void EventMon::setAssocTable(QString orid)
{
    ui->assocTable->verticalHeader()->setVisible(false);
    ui->assocTable->setColumnWidth(2, 196);
    ui->assocTable->setColumnWidth(3, 80);

    model->setQuery("SELECT arid, orid, sta, chan, time, phase, p_algorithm, lddate FROM assoc WHERE orid = " + orid);

    ui->assocTable->setRowCount(0);

    for(int i=0;i<model->rowCount();++i)
    {
        QString sta = model->record(i).value("sta").toString();
        QString chan = model->record(i).value("chan").toString();
        QString time = model->record(i).value("time").toString();
        time = time.left(4) + "-" + time.mid(4,2) + "-" + time.mid(6,2) + " " + time.mid(8,2) + ":" + time.mid(10,2) + ":" + time.mid(12,2) + "." + time.section('.', 1, 1);
        //55.01
        QString algorithm = model->record(i).value("p_algorithm").toString();
        ui->assocTable->setRowCount(ui->assocTable->rowCount()+1);
        ui->assocTable->setItem(i, 0, new QTableWidgetItem(sta));
        ui->assocTable->setItem(i, 1, new QTableWidgetItem(chan));
        ui->assocTable->setItem(i, 2, new QTableWidgetItem(time));
        ui->assocTable->setItem(i, 3, new QTableWidgetItem(algorithm));
        ui->assocTable->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->assocTable->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->assocTable->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        ui->assocTable->item(i, 3)->setTextAlignment(Qt::AlignCenter);
    }
}

void EventMon::clickEventTable(int row, int col)
{
    evid = ui->eventTable->item(row, 0)->text();
    setOriginTable(evid);
    model->setQuery("SELECT min(orid) FROM origin where evid = " + evid);
    orid = model->record(0).value("min(orid)").toString();
    setAssocTable(orid);

    if(col == 2)
    {
        evid = ui->eventTable->item(row, 0)->text();
        QString cmd = c.SCRIPTDIR + "/viewWave.sh " + evid + " >> /dev/null 2>&1 &";
        system(cmd.toLatin1().data());
    }
    else if(col == 3)
    {
        QString cmd;
        cmd = "rm /usr/local/tomcat/webapps/eventviewer/origin.info"; system(cmd.toLatin1().data());
        evid = ui->eventTable->item(row, 0)->text();
        cmd = "grep -v File " + c.EVENTDIR + "/" + evid + "/sta.info | grep -v Desc > /usr/local/tomcat/webapps/eventviewer/sta.info"; system(cmd.toLatin1().data());
        model->setQuery("SELECT lat, lon, depth, time, orid, evid, l_algorithm, lddate FROM origin WHERE evid = " + evid);
        for(int i=0;i<model->rowCount();++i)
        {
            QString orid = model->record(i).value("orid").toString();
            QString lat = model->record(i).value("lat").toString();
            QString lon = model->record(i).value("lon").toString();
            QString depth = model->record(i).value("depth").toString();
            QString time = model->record(i).value("time").toString();
            time = time.left(4) + "-" + time.mid(4,2) + "-" + time.mid(6,2) + " " + time.mid(8,2) + ":" + time.mid(10,2) + ":" + time.section('.',0,0).mid(12,2);
            QString l_algorithm = model->record(i).value("l_algorithm").toString();
            QString tot = orid + " " + lat + " " + lon + " " + depth + " " + time + " " + l_algorithm + " ";

            cmd = "echo \"" + evid + " " + tot + "\" >> /usr/local/tomcat/webapps/eventviewer/origin.info"; system(cmd.toLatin1().data());
        }

        cmd = "firefox 127.0.0.1:8080/eventviewer &";
        system(cmd.toLatin1().data());
    }
    else if(col == 4)
    {
        QString str;

        if(!korean)
            str = "Do you want to delete this event?";
        else
            str = codec->toUnicode("해당 이벤트를 삭제하시겠습니까?");

        if( !QMessageBox::question( this,
                                    "Warning",
                    str,
                    "Yes",
                    "No",
                    QString::null, 1, 1 ) )
        {
            QSqlQuery qry;
            model->setQuery("SELECT orid FROM origin where evid = " + ui->eventTable->item(row, 0)->text());
            for(int i=0;i<model->rowCount();++i)
            {
                QString orid = model->record(i).value("orid").toString();
                qry.prepare( "DELETE FROM assoc WHERE orid = " + orid );
                qry.exec();
            }
            qry.prepare( "DELETE FROM origin WHERE evid = " + ui->eventTable->item(row, 0)->text() );
            qry.exec();
            qry.prepare( "DELETE FROM event WHERE evid = " + ui->eventTable->item(row, 0)->text() );
            qry.exec();

            setup();
        }
        else
            return;
    }
}

void EventMon::clickOriginTable(int row, int col)
{
    orid = ui->originTable->item(row, 0)->text();
    model->setQuery("SELECT evid FROM origin where orid = " + orid);
    evid = model->record(0).value("evid").toString();

    if(col == 6)
    {
        orid = ui->originTable->item(row, 0)->text();
        QString cmd = c.SCRIPTDIR + "/viewWave.sh " + evid + " " + orid + " >> /dev/null 2>&1 &";
        system(cmd.toLatin1().data());
    }
    else
    {
        setAssocTable(orid);
    }
}


void EventMon::on_makeOrigin_clicked()
{
    QString str;

    if(!korean) str = "Do you want to make a new origin using evid " + evid + " data?\n\nAre you sure to close all Geotool windows?";
    else str = "evid " + evid + codec->toUnicode("번을 사용하여 신규 origin 정보를 만드시겠습니까?\n\n(경고) 모든 geotool 창이 닫힙니다.");
    if( !QMessageBox::question( this,
                                "Warning",
                str,
                "Yes",
                "No",
                QString::null, 1, 1 ) )
    {
        QString cmd = "pkill geotool";
        system(cmd.toLatin1().data());

        model->setQuery("SELECT min(orid) FROM origin WHERE evid="+evid);
        QString firstorid = model->record(0).value("min(orid)").toString();

        QDir dir;
        dir.setPath(c.EVENTDIR + "/" + evid  + "/NEWORIGIN");

        if(dir.exists())
            dir.removeRecursively();

        dir.mkpath(".");

        cmd = "cp -R " + c.EVENTDIR + "/" + evid + "/data " + dir.path();
        system(cmd.toLatin1().data());
        cmd = "cp -R " + c.EVENTDIR + "/" + evid + "/" + firstorid + "/NLLOC " + dir.path();
        system(cmd.toLatin1().data());

        QFile file;
        file.setFileName(dir.path() + "/NLLOC/type");
        QString type;
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            type = stream.readLine();
            file.close();
        }

        file.setFileName(dir.path() + "/data/css/.css.arrival");
        if(file.exists())
            file.remove();

        makeorigin->setup(evid, firstorid);
        makeorigin->show();
    }
    else
        return;
}

void EventMon::_resetTable()
{
    setup();
}
