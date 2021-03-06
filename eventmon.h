#ifndef EVENTMON_H
#define EVENTMON_H

#include <QDialog>
#include <QSql>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QTimer>
#include <QTextEdit>

#include "common.h"
#include "makeorigin.h"
#include "locator.h"

namespace Ui {
    class EventMon;
}

class EventMon : public QDialog
{
    Q_OBJECT

public:
    explicit EventMon(CFG cfg, bool _korean = 0, QWidget *parent = 0);
    ~EventMon();

    void setup();

    bool korean;
    CFG c;
    void setLanguageEn();
    void setLanguageKo();

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

    QTextCodec *codec;

public slots:
    void clickEventTable(int, int);
    void clickOriginTable(int, int);
    void _resetTable();

private slots:
    void on_makeOrigin_clicked();

signals:

};

#endif // EVENTMON_H
