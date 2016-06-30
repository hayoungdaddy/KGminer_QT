#ifndef MAKEORIGIN_H
#define MAKEORIGIN_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include "nlloc.h"
#include "common.h"
#include "locator.h"

namespace Ui {
    class MakeOrigin;
}

class MakeOrigin : public QDialog
{
    Q_OBJECT

public:
    explicit MakeOrigin(CFG cfg, bool _korean = 0, QWidget *parent = 0);
    ~MakeOrigin();

    bool korean;
    CFG c;

    QString EVID;
    QString FIRSTORID;
    QString TYPE;

    void setLanguageEn();
    void setLanguageKo();
    void setup(QString, QString);

private:
    Ui::MakeOrigin *ui;

    QTextCodec *codec;

    QSqlQueryModel *model;

private slots:
    void on_showButton_clicked();
    void on_quitButton_clicked();
    void on_saveButton_clicked();
    void on_editButton_clicked();
    void on_nllocButton_clicked();
    void on_inputDBButton_clicked();
    void on_mapButton_clicked();

protected slots:
    //void closeEvent(QCloseEvent *);

signals:
    void resetTable();

};

#endif // MAKEORIGIN_H
