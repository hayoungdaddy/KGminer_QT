#ifndef DATAMONITOR_H
#define DATAMONITOR_H

#include <QDialog>
#include <QFileDialog>
#include <QProgressBar>
#include <QPainter>
#include <QCalendarWidget>
#include <QTextCharFormat>

#include "common.h"

namespace Ui {
    class DataMonitor;
}

class DataMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit DataMonitor(CFG cfg, bool _korean = 0, QWidget *parent = 0);
    ~DataMonitor();

    bool korean;
    void setLanguageEn();
    void setLanguageKo();
    CFG c;

private:
    Ui::DataMonitor *ui;
    QTextCodec *codec;

public slots:
    void changePage(int, int);
    void clickCalendar(QDate);

private slots:

signals:

};

#endif // DATAMONITOR_H
