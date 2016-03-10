#ifndef DATARECIEVER_H
#define DATARECIEVER_H

#include <QDialog>
#include <QDate>
#include <QDateTime>

#include "common.h"

namespace Ui {
    class DataReciever;
}

class DataReciever : public QDialog
{
    Q_OBJECT

public:
    explicit DataReciever(CFG cfg, QWidget *parent = 0);
    ~DataReciever();

    bool korean;

    CFG c;

private:
    Ui::DataReciever *ui;

    QTextCodec *codec;

public slots:
    void extract();
    void extracButtonClicked();
    void simulButtonClicked();
    void reset();
    void viewWaveForm();

private slots:

signals:

};

#endif
