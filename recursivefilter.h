#ifndef RECURSIVEFILTER_H
#define RECURSIVEFILTER_H

#include <QDialog>

#include "util.h"

namespace Ui {
    class RecursiveFilter;
}

class RecursiveFilter : public QDialog
{
    Q_OBJECT

public:
    explicit RecursiveFilter(QWidget *parent = 0);
    ~RecursiveFilter();

    void setup();

private:
    Ui::RecursiveFilter *RecursiveFilter_ui;

    QString accel;
    QString veloA;
    QString veloV;
    QString disp;
    QString lambda;
    QString snr;
    QString signal;
    QString tauk;
    QString noise;
    QString gain;

private slots:
    void genButtonClicked();

signals:

};

#endif // RECURSIVEFILTER_H
