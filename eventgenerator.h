#ifndef EVENTGENERATOR_H
#define EVENTGENERATOR_H

#include <QObject>

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlDatabase>>
#include <QSqlRecord>
#include <QSqlError>

#include "common.h"

class EventGenerator : public QObject
{
    Q_OBJECT

public:
    EventGenerator(CFG cfg, QString fname = 0);
    ~EventGenerator();

    CFG c;

    int evid;
    int orid;
    int arid;

private:
    void openDB();
    QSqlDatabase kgminerdb;
    QSqlQueryModel *model;

signals:

};

#endif // EVENTGENERATOR_H
