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
    explicit DataReciever(CFG cfg, bool _korean = 0, QWidget *parent = 0);
    ~DataReciever();

    CFG c;

    bool korean;
    void setLanguageEn();
    void setLanguageKo();

private:
    Ui::DataReciever *ui;

    QTextCodec *codec;

public slots:
    void extract();
    void extracButtonClicked();
    void simulButtonClicked();
    void reset();
    void viewWaveForm();
};

#endif
