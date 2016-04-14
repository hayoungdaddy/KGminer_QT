#ifndef BINDER_H
#define BINDER_H

#include <QDialog>

#include "common.h"

namespace Ui {
    class Binder;
}

class Binder : public QDialog
{
    Q_OBJECT

public:
    explicit Binder(CFG cfg, bool _korean = 0, QWidget *parent = 0);
    ~Binder();

    bool korean;
    CFG c;
    void setLanguageEn();
    void setLanguageKo();
    void setup();

private:
    Ui::Binder *ui;

    QTextCodec *codec;

    QString dspace;
    QString grdlat1, grdlat2;
    QString grdlon1, grdlon2;
    QString grdz1, grdz2;
    QString rstack;
    QString tstack;
    QString stack;
    QString thresh;
    QString focus;

private slots:
    void genButtonClicked();

signals:

};

#endif // BINDER_H
