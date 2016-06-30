#ifndef NLLOC_H
#define NLLOC_H

#include <QDialog>

#include "common.h"

namespace Ui {
    class NLLoc;
}

class NLLoc : public QDialog
{
    Q_OBJECT

public:
    explicit NLLoc(CFG cfg, bool _korean = 0, QString evid = 0, QString orid = 0, QString num = 0, QWidget *parent = 0);
    ~NLLoc();

    void setup();

    bool korean;
    CFG c;
    void setLanguageEn();
    void setLanguageKo();

    QString EVID, ORID, NUMBER, MAINDIR, TYPE, MINV, MAXV;

private:
    Ui::NLLoc *ui;

    QTextCodec *codec;

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
    void defaultButtonClicked();
    void svmbClicked();
    void mvmbClicked();

signals:

};

#endif // NLLOC_H
