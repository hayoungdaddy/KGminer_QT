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
    qDebug() << evname;

    this->model = new QSqlQueryModel();

    model->setQuery("SELECT max(evid) FROM event");
    evid = model->record(0).value("max(evid)").toInt() + 1;
    model->setQuery("SELECT max(orid) FROM origin");
    orid = model->record(0).value("max(orid)").toInt() + 1;
    model->setQuery("SELECT max(arid) FROM assoc");
    arid = model->record(0).value("max(arid)").toInt() + 1;

    qDebug() << QString::number(evid) + " " + QString::number(orid) + " " + QString::number(arid);

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

        insertQuery = "INSERT INTO origin (lat, lon, depth, time, orid, evid, jdate, l_algorithm, lddate) VALUES ('0', '0', '0', '0', "
                + QString::number(orid) + ", " + QString::number(evid)
                + ", '0', '0', '" + today.toString("yyyyMMdd") + "')";
        //qDebug() << insertQuery;
        model->setQuery(insertQuery);

        insertQuery = "INSERT INTO event (evid, evname, lddate) VALUES ("
                + QString::number(evid) + ", '" + pickinfo.evName + "', '" + today.toString("yyyyMMdd") + "')";
        //qDebug() << insertQuery;
        model->setQuery(insertQuery);
    }
        /* run NLLOC */
    /*
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
        {
            runNLLoc(c.EVENTDIR, evid, orid, c.PARAMSDIR + "/NLLOC/1", type.section(':',0,0), type.section(':',1,1).toFloat(), type.section(':',2,2).toFloat());
        }

    //int evid, int orid, int setnum, QString type, float minVel, float maxVel;
    */
}


