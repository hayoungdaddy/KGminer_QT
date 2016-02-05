#include "eventgenerator.h"

EventGenerator::EventGenerator( CFG cfg, QString fname )
{
    c = cfg;

    openDB();
    this->model = new QSqlQueryModel();

    model->setQuery("SELECT max(evid) FROM event");
    evid = model->record(0).value("max(evid)").toInt();
    model->setQuery("SELECT max(orid) FROM origin");
    orid = model->record(0).value("max(orid)").toInt();
    model->setQuery("SELECT max(arid) FROM assoc");
    arid = model->record(0).value("max(arid)").toInt();

    qDebug() << QString::number(evid) + " " + QString::number(orid) + " " + QString::number(arid);

    QFile file;
    file.setFileName(PARAMSDIR + "/" + fname);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line;

        // PR20160112095701  --> PR file name
        // G01   -1 HGZ  P?0201512010451 4501  --> picklist
        // G05 HGZ TS -- ?0 P 20160112095740.900 27 0 0 41 68 0 0 0 0 3 --> PR file

        file.close();
    }
}

EventGenerator::~EventGenerator()
{
}

void EventGenerator::openDB()
{
    kgminerdb = QSqlDatabase::addDatabase("QSQLITE");
    kgminerdb.setDatabaseName(c.DBDIR + "/" + c.DBNAME);
    if(!kgminerdb.open())
    {
        QMessageBox msgBox;
        msgBox.setText(kgminerdb.lastError().text());
        msgBox.exec();
        return;
    }
}
