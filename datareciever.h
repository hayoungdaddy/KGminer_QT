#ifndef DATARECIEVER_H
#define DATARECIEVER_H

#include <QDialog>
#include <QDate>
#include <QDateTime>
#include <QProcess>

#include "common.h"

namespace Ui {
    class DataReciever;
}

class DataReciever : public QDialog
{
    Q_OBJECT

public:
    explicit DataReciever(QWidget *parent = 0);
    ~DataReciever();

    bool korean;

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
