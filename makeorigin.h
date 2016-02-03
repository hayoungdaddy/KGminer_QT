#ifndef MAKEORIGIN_H
#define MAKEORIGIN_H

#include <QDialog>

#include "nlloc.h"
#include "util.h"

namespace Ui {
    class MakeOrigin;
}

class MakeOrigin : public QDialog
{
    Q_OBJECT

public:
    explicit MakeOrigin(QString evid = 0, QString orid = 0, QWidget *parent = 0);
    ~MakeOrigin();

    void setup(QString, QString);

    QString EVID;
    QString ORID;
    QString TYPE;

    //NLLoc *nlloc;

private:
    Ui::MakeOrigin *ui;

public slots:

private slots:
    void on_showButton_clicked();
    void on_quitButton_clicked();
    void on_saveButton_clicked();
    void on_editButton_clicked();
    void on_nllocButton_clicked();
    void on_inputDBButton_clicked();

protected slots:
    void closeEvent(QCloseEvent *);

signals:
    void resetTable();

};

#endif // MAKEORIGIN_H
