#ifndef NLLOC_H
#define NLLOC_H

#include <QDialog>

#include "util.h"

namespace Ui {
    class NLLoc;
}

class NLLoc : public QDialog
{
    Q_OBJECT

public:
    explicit NLLoc(QString evid = 0, QString orid = 0, QString num = 0, QWidget *parent = 0);
    ~NLLoc();

    void setup();
    QString EVID, ORID, NUMBER, MAINDIR, TYPE, MINV, MAXV;

private:
    Ui::NLLoc *NLLoc_ui;

    QString latOrig;
    QString lonOrig;
    QString fsp;
    QString ssp;
    QString ngx, ngy, ngz;
    QString ogx, ogy, ogz;
    QString gsx, gsy, gsz;
    QString vngx, vngy, vngz;
    QString vogx, vogy, vogz;
    QString vgsx, vgsy, vgsz;

private slots:
    void genButtonClicked();
    void svmbClicked();
    void mvmbClicked();

signals:

};

#endif // NLLOC_H
