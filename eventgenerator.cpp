#include "eventgenerator.h"

EventGenerator::EventGenerator( CFG cfg )
{
    c = cfg;

    timer = new QTimer(this);
    timer->start( 3000 );

    connect(timer, SIGNAL(timeout()), this, SLOT(findEvent()));
}

EventGenerator::~EventGenerator()
{
}

void EventGenerator::findEvent()
{
    QDir dir;
    dir.setPath( c.PARAMSDIR );
    QStringList files;
    QString filterName = "PR*";
    files = dir.entryList(QStringList(filterName), QDir::Files | QDir::NoSymLinks);

    if(files.count() > 0)
    {
        for(int i=0;i<files.count();i++)
            evnameList.append(files[i]);
    }

    while (!evnameList.isEmpty())
    {
        prParser(evnameList.first());
        QFile file;
        file.setFileName( c.PARAMSDIR + "/" + evnameList.first() );
        file.remove();
        evnameList.removeFirst();
    }
}

void EventGenerator::prParser(QString evname)
{
    //qDebug() << evname;

    this->model = new QSqlQueryModel();

    model->setQuery("SELECT max(evid) FROM event");
    evid = model->record(0).value("max(evid)").toInt() + 1;
    model->setQuery("SELECT max(orid) FROM origin");
    orid = model->record(0).value("max(orid)").toInt() + 1;
    model->setQuery("SELECT max(arid) FROM assoc");
    arid = model->record(0).value("max(arid)").toInt() + 1;

    //qDebug() << QString::number(evid) + " " + QString::number(orid) + " " + QString::number(arid);

    PICKINFO pickinfo;
    QDate today = QDate::currentDate();

    QFile file;
    file.setFileName(c.PARAMSDIR + "/" + evname);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line, _line;

        pickinfo.evName = evname.mid(2, 14);

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            pickinfo.staName << _line.section(' ', 0, 0);
            pickinfo.locName << _line.section(' ', 3, 3);
            pickinfo.chanName << _line.section(' ', 1, 1);
            pickinfo.phase << _line.section(' ', 5, 5);
            pickinfo.dateTime << _line.section(' ', 6, 6).left(12);
            pickinfo.sec << _line.section(' ', 6, 6).section('.', 0, 0).right(2);
            pickinfo.msec << _line.section(' ', 6, 6).section('.', 1, 1).left(2);

            //qDebug() << pickinfo.staName << pickinfo.locName << pickinfo.chanName << pickinfo.phase << pickinfo.dateTime << pickinfo.sec;
        }
        // PR20160112095701  --> PR file name
        // G01   -1 HGZ  P?0201512010451 4501  --> picklist
        // G05 HGZ TS -- ?0 P 20160112095740.900 27 0 0 41 68 0 0 0 0 3 --> PR file

        file.close();
    }

    /* make directories */
    QString rootDir = c.EVENTDIR + "/" + QString::number(evid);

    QDir dir;
    dir.setPath(rootDir);

    if (!dir.exists())
    {
        dir.setPath(rootDir + "/data/sac");
        dir.mkpath(".");
        dir.setPath(rootDir + "/data/mseed");
        dir.mkpath(".");
        dir.setPath(rootDir + "/data/css");
        dir.mkpath(".");
        dir.setPath(rootDir + "/" + QString::number(orid) + "/LOC");
        dir.mkpath(".");
    }

    QString insertQuery;

    /* make picklist */
    file.setFileName(rootDir + "/picklist");
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &file ) ;
        for(int i=0;i<pickinfo.staName.count();i++)
        {
            // G01   -1 HGZ  P?0201512010451 4501  --> picklist
            if(pickinfo.staName[i].count() == 3)
                stream << pickinfo.staName[i] << "  ";
            else if(pickinfo.staName[i].count() == 4)
                stream << pickinfo.staName[i] << " ";
            else if(pickinfo.staName[i].count() == 5)
                stream << pickinfo.staName[i];

            stream << " " << pickinfo.locName[i] << " " << pickinfo.chanName[i] << "  ";
            stream << pickinfo.phase[i] << "?0" << pickinfo.dateTime[i] << " " << pickinfo.sec[i] << pickinfo.msec[i] << "\n";

            /* insert onto assoc */
            insertQuery = "INSERT INTO assoc (arid, orid, sta, chan, time, phase, p_algorithm, lddate) VALUES ("
                    + QString::number(arid) + ", " + QString::number(orid) + ", '" + pickinfo.staName[i] + "', '"
                    + pickinfo.chanName[i] + "', '" + pickinfo.dateTime[i] + pickinfo.sec[i] + "." + pickinfo.msec[i]
                    + "', '" + pickinfo.phase[i] + "', 'FP', '" + today.toString("yyyyMMdd") + "')";

            //qDebug() << insertQuery;
            model->setQuery(insertQuery);
            arid++;
        }
        file.close();
    }

    file.copy(rootDir + "/" + QString::number(orid) + "/picklist");
    file.setFileName(c.PARAMSDIR + "/sta.info");
    file.copy(rootDir + "/sta.info");

    /* run NLLOC */
    file.setFileName(c.PARAMSDIR + "/NLLOC/1/type");
    QString type;
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        type = stream.readLine();
        file.close();
    }

    if(type.startsWith("SVM"))
        runNLLoc(c.EVENTDIR, evid, orid, c.PARAMSDIR + "/NLLOC/1", type, 0, 0);
    else if(type.startsWith("MVM"))
        runNLLoc(c.EVENTDIR, evid, orid, c.PARAMSDIR + "/NLLOC/1", type.section(':',0,0), type.section(':',1,1).toFloat(), type.section(':',2,2).toFloat());

    /* insert onto event, origin */
    file.setFileName(c.EVENTDIR + "/" + QString::number(evid) + "/" + QString::number(orid) + "/LOC/origin");
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line, _line;
        QString temp;

        line = stream.readLine();
        _line = line.simplified();

        //20151201075930.8243
        //GEOGRAPHIC  OT 2015 12 01  05 03   40.5928  Lat 38.304769 Long 127.307725 Depth -0.468438
        if(_line.section(' ', 7, 7).section('.', 0, 0) == "0")
        {
            pickinfo.evName = _line.section(' ', 2, 2) + "-" + _line.section(' ', 3, 3) + "-" + _line.section(' ', 4, 4)
                + " " + _line.section(' ', 5, 5) + ":" + _line.section(' ', 6, 6) + ":" + "00";

            temp = _line.section(' ', 2, 2) + _line.section(' ', 3, 3) + _line.section(' ', 4, 4)
                    + _line.section(' ', 5, 5) + _line.section(' ', 6, 6) + "00." + _line.section(' ', 7, 7).section('.', 1, 1);
        }
        else if(_line.section(' ', 7, 7).section('.', 0, 0).toInt() > 0 && _line.section(' ', 7, 7).section('.', 0, 0).toInt() < 10 )
        {
            pickinfo.evName = _line.section(' ', 2, 2) + "-" + _line.section(' ', 3, 3) + "-" + _line.section(' ', 4, 4)
                + " " + _line.section(' ', 5, 5) + ":" + _line.section(' ', 6, 6) + ":" + "0" + _line.section(' ', 7, 7).section('.', 0, 0);

            temp = _line.section(' ', 2, 2) + _line.section(' ', 3, 3) + _line.section(' ', 4, 4)
                    + _line.section(' ', 5, 5) + _line.section(' ', 6, 6) + "0" + _line.section(' ', 7, 7).section('.', 0, 0) + "."+ _line.section(' ', 7, 7).section('.', 1, 1);
        }
        else
        {
            pickinfo.evName = _line.section(' ', 2, 2) + "-" + _line.section(' ', 3, 3) + "-" + _line.section(' ', 4, 4)
                + " " + _line.section(' ', 5, 5) + ":" + _line.section(' ', 6, 6) + ":" + _line.section(' ', 7, 7).section('.', 0, 0);

            temp = _line.section(' ', 2, 2) + _line.section(' ', 3, 3) + _line.section(' ', 4, 4)
                    + _line.section(' ', 5, 5) + _line.section(' ', 6, 6) + _line.section(' ', 7, 7);
        }
        insertQuery = "INSERT INTO event (evid, evname, lddate) VALUES ("
                + QString::number(evid) + ", '" + pickinfo.evName + "', '" + today.toString("yyyyMMdd") + "')";
        //qDebug() << insertQuery;
        model->setQuery(insertQuery);

        QString lat, lon, depth, jdate, algorithm;
        lat = _line.section(' ', 9, 9);
        lon = _line.section(' ', 11, 11);
        depth = _line.section(' ', 13, 13);
        algorithm = "NLLoc_" + type.section(':',0,0);

        QDate date;
        date.setDate(_line.section(' ', 2, 2).toInt(), _line.section(' ', 3, 3).toInt(), _line.section(' ', 4, 4).toInt());
        jdate = QString::number(date.dayOfYear());

        insertQuery = "INSERT INTO origin (lat, lon, depth, time, orid, evid, jdate, l_algorithm, lddate) VALUES ('"
                + lat + "', '" + lon + "', '" + depth + "', '" + temp + "', "
                + QString::number(orid) + ", " + QString::number(evid)
                + ", '" + jdate + "', '" + algorithm + "', '" + today.toString("yyyyMMdd") + "')";
        //qDebug() << insertQuery;
        model->setQuery(insertQuery);

        file.close();

        // to /media/sf_KGminer/EVENTLOC/EVID/ORID/location.txt
        // 2016-03-25 15:32:03 37.353215 127.534312
        QDir d;
        QString locdir = c.LOCDIR + "/" + QString::number(evid);
        d.setPath(locdir);
        if(!d.exists()) d.mkpath(".");
        locdir = c.LOCDIR + "/" + QString::number(evid) + "/" + QString::number(orid);
        d.setPath(locdir);
        if(!d.exists()) d.mkpath(".");
        file.setFileName(c.LOCDIR + "/" + QString::number(evid) + "/" + QString::number(orid) + "/location.txt");
        if( file.open( QIODevice::WriteOnly ) )
        {
            QTextStream stream( &file ) ;
            stream <<  pickinfo.evName << " " << lat << " " << lon;
            file.close() ;
        }
    }

    /* get data using qmerge */
    QString dataDir = c.MSEEDDIR;
    QString eventDataDir = c.EVENTDIR + "/" + QString::number(evid) + "/data";
    QDateTime eventTime;
    eventTime = QDateTime::fromString(pickinfo.evName, "yyyy-MM-dd hh:mm:ss");
    eventTime = eventTime.addSecs(-30);

    file.setFileName(c.EVENTDIR + "/" + QString::number(evid) + "/sta.info");

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            if(_line.startsWith("Description") || _line.startsWith("Filename"))
                continue;
            else
            {
                // G01 HGZ -- TS 38.308138 127.312229 0.433
                QString net, sta, chan, cmd;
                net = _line.section(' ', 3, 3);
                sta = _line.section(' ', 0, 0);
                chan = _line.section(' ', 1, 1);
                cmd = "qmerge -T -f " + eventTime.toString("yyyy/MM/dd,hh:mm:ss") + " -s 60 -b 512 -o " + eventDataDir + "/mseed/" + sta + ".mseed "
                        + dataDir +"/" + eventTime.toString("yyyy") + "/" + net + "/" + sta + "/" + chan + ".D/* >> /dev/null 2>&1";
                //qDebug() << cmd;
                system(cmd.toLatin1().data());
            }
        }
        file.close();
    }

    QString cmd = c.SCRIPTDIR + "/mseed2css.sh " + QString::number(evid);
    system(cmd.toLatin1().data());
    cmd = c.SCRIPTDIR + "/picklistTocssarrival.sh " + QString::number(evid);
    system(cmd.toLatin1().data());

    cmd = "cp " + c.EVENTDIR + "/" + QString::number(evid) + "/data/css/css.arrival "
            + c.EVENTDIR + "/" + QString::number(evid) + "/data/css/.css.arrival";
    system(cmd.toLatin1().data());
    cmd = "cp -R " + c.EVENTDIR + "/" + QString::number(evid) + "/data " + c.EVENTDIR + "/" + QString::number(evid) + "/" + QString::number(orid);
    system(cmd.toLatin1().data());

    cmd = c.BINDIR + "/KGminer_Warning " + c.EVENTDIR + " " + QString::number(evid) + " " + QString::number(orid) + " >> /dev/null 2>&1 &";
    system(cmd.toLatin1().data());
}
