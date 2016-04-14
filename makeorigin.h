#ifndef MAKEORIGIN_H
#define MAKEORIGIN_H

#include <QDialog>

#include "nlloc.h"
#include "common.h"

namespace Ui {
    class MakeOrigin;
}

class MakeOrigin : public QDialog
{
    Q_OBJECT

public:
    explicit MakeOrigin(CFG cfg, bool _korean = 0, QString evid = 0, QString orid = 0, QWidget *parent = 0);
    ~MakeOrigin();

    void setup(QString, QString);

    bool korean;
    CFG c;

    QString EVID;
    QString ORID;
    QString TYPE;

    void setLanguageEn();
    void setLanguageKo();

private:
    Ui::MakeOrigin *ui;

    QTextCodec *codec;

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
