#ifndef EVENTMON_H
#define EVENTMON_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlDatabase>>
#include <QSqlRecord>
#include <QSqlError>
#include <QTimer>
#include <QTextEdit>

#include "common.h"
#include "makeorigin.h"

namespace Ui {
    class EventMon;
}

class EventMon : public QDialog
{
    Q_OBJECT

public:
    explicit EventMon(QWidget *parent = 0);
    ~EventMon();

    void setup();

    void setEventTable();
    void setOriginTable(QString);
    void setAssocTable(QString);

    QString evid;
    QString orid;

    MakeOrigin *makeorigin;

    QTimer *resetTableTimer;

private:
    Ui::EventMon *ui;

    QSqlQueryModel *model;

public slots:
    void clickEventTable(int, int);
    void clickOriginTable(int, int);
    void _resetTable();

private slots:
    void on_makeOrigin_clicked();

signals:

};

#endif // EVENTMON_H
