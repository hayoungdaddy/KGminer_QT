#include "eventmon.h"
#include "ui_eventmon.h"

EventMon::EventMon(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventMon)
{
    ui->setupUi(this);
    //codec = QTextCodec::codecForName( "utf8" );

    this->model = new QSqlQueryModel();

    openDB();

    setEventTable();
    model->setQuery("SELECT max(evid) FROM event");
    evid = model->record(0).value("max(evid)").toString();
    model->setQuery("SELECT min(orid) FROM origin where evid = " + evid);
    orid = model->record(0).value("min(orid)").toString();
    setOriginTable(evid);
    setAssocTable(orid);

    makeorigin = new MakeOrigin("0", "0", this);
    makeorigin->hide();

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
    kgminerdb.close();
    delete ui;
}

void EventMon::openDB()
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

void EventMon::setup()
{
    QModelIndex index1 = ui->eventTable->currentIndex();
    setEventTable();
    //eventmodel->setQuery("SELECT max(evid) FROM event");
    //evid = eventmodel->record(0).value("max(evid)").toString();
    //originmodel->setQuery("SELECT min(orid) FROM origin where evid = " + evid);
    //orid = originmodel->record(0).value("min(orid)").toString();
    //setOriginTable(evid);
    //setAssocTable(orid);

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
        ui->originTable->setItem(i, 7, new QTableWidgetItem("View"));
        ui->originTable->setItem(i, 8, new QTableWidgetItem("View"));
        ui->originTable->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 3)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 4)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 5)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 6)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 7)->setTextAlignment(Qt::AlignCenter);
        ui->originTable->item(i, 8)->setTextAlignment(Qt::AlignCenter);
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
        QString cmd = SCRIPTDIR + "/viewWave.sh " + evid + " >> /dev/null 2>&1 &";
        system(cmd.toLatin1().data());
    }
    else if(col == 3)
    {
        QString cmd;
        cmd = "rm /usr/local/tomcat/webapps/eventviewer/origin.info"; system(cmd.toLatin1().data());
        evid = ui->eventTable->item(row, 0)->text();
        //cmd = "cp " + EVENTDIR + "/" + evid + "/sta.info /usr/local/tomcat/webapps/eventviewer/sta.info"; system(cmd.toLatin1().data());
        cmd = "grep -v File " + EVENTDIR + "/" + evid + "/sta.info | grep -v Desc > /usr/local/tomcat/webapps/eventviewer/sta.info"; system(cmd.toLatin1().data());
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
        QString str = "Do you want to delete this event?";
        if( !QMessageBox::question( this,
                                    "Warning",
                    str,
                    "Yes",
                    "No",
                    QString::null, 1, 1 ) )
        {
            QString cmd;
            cmd = "sqlite3 " + DB + " \"delete from event where evid=" + ui->eventTable->item(row, 0)->text() + "\"";
            system(cmd.toLatin1().data());
            cmd = "sqlite3 " + DB + " \"delete from origin where evid=" + ui->eventTable->item(row, 0)->text() + "\"";
            system(cmd.toLatin1().data());
            cmd = "sqlite3 " + DB + " \"delete from assoc where orid=" + ui->originTable->item(row, 0)->text() + "\"";
            system(cmd.toLatin1().data());
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
        QString cmd = SCRIPTDIR + "/viewWave.sh " + evid + " " + orid + " >> /dev/null 2>&1 &";
        system(cmd.toLatin1().data());
    }
    else if(col == 7)
    {
        QTextEdit *edit = new QTextEdit();
        edit->setWindowTitle(EVENTDIR + "/" + evid + "/" + orid + "/LOC/summary");
        edit->setMinimumWidth(1200); //edit->setMaximumWidth(1000);
        edit->setMinimumHeight(600); //edit->setMaximumHeight(800);
        QFile file;
        file.setFileName(EVENTDIR + "/" + evid + "/" + orid + "/LOC/summary");
        if( file.open( QIODevice::ReadOnly ) )
        {
            QTextStream stream(&file);
            QString line;

            while(!stream.atEnd())
            {
                line = stream.readLine();
                edit->append(line);
            }
            file.close();
        }

        edit->show();
    }
    else if(col == 8)
    {
        QTextEdit *edit = new QTextEdit();
        edit->setWindowTitle(EVENTDIR + "/" + evid + "/" + orid + "/NLLOC/*.log");
        edit->setMinimumWidth(1200); //edit->setMaximumWidth(1000);
        edit->setMinimumHeight(600); //edit->setMaximumHeight(800);
        QFile file;

        for(int i=0;i<3;i++)
        {
            if(i==0) file.setFileName(EVENTDIR + "/" + evid + "/" + orid + "/NLLOC/Vel2Grid.log");
            else if(i==1) file.setFileName(EVENTDIR + "/" + evid + "/" + orid + "/NLLOC/Grid2Time.log");
            else if(i==2) file.setFileName(EVENTDIR + "/" + evid + "/" + orid + "/NLLOC/NLLoc.log");
            if( file.open( QIODevice::ReadOnly ) )
            {
                QTextStream stream(&file);
                QString line;

                edit->append("<< " + file.fileName() + " >>");

                while(!stream.atEnd())
                {
                    line = stream.readLine();
                    edit->append(line);
                }
                file.close();

                edit->append("-----------------------------------------------------------------\n");
            }
        }
        edit->show();
    }
    else
    {
        setAssocTable(orid);
    }
}


void EventMon::on_makeOrigin_clicked()
{
    //qDebug() << ui->eventTable->currentRow();
    QString str = "Do you want to make new origin using evid " + evid + " data?\n\nAre you sure to close all Geotool windows?";
    if( !QMessageBox::question( this,
                                "Warning",
                str,
                "Yes",
                "No",
                QString::null, 1, 1 ) )
    {
        QString cmd = "pkill geotool";
        system(cmd.toLatin1().data());

        model->setQuery("SELECT max(orid) FROM origin");
        int t = model->record(0).value("max(orid)").toInt();
        t = t+1;
        QString neworid;
        neworid = neworid.setNum(t, 10);

        cmd = SCRIPTDIR + "/makeNewOrigin.sh " + evid + " " + neworid + " >> /dev/null 2>&1 &";
        system(cmd.toLatin1().data());

        //makeorigin = new MakeOrigin(evid, neworid, this);
        makeorigin->setup(evid, neworid);
        makeorigin->show();

    }
    else
        return;
}

void EventMon::_resetTable()
{
    setup();
}
