#ifndef EVENTGENERATOR_H
#define EVENTGENERATOR_H

#include <QObject>

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QDate>
#include <QTimer>
#include <QLinkedList>

#include "common.h"
#include "locator.h"

class EventGenerator : public QObject
{
    Q_OBJECT

public:
    EventGenerator(CFG cfg);
    ~EventGenerator();

    CFG c;

    int evid;
    int orid;
    int arid;

    QLinkedList<QString> evnameList;

private:
    QTimer *timer;
    QSqlQueryModel *model;

    void prParser(QString);

private slots:
    void findEvent();

signals:

};

#endif // EVENTGENERATOR_H
