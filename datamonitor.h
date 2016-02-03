#ifndef DATAMONITOR_H
#define DATAMONITOR_H

#include <QDialog>
#include <QFileDialog>
#include <QProcess>
#include <QProgressBar>
#include <QPainter>
#include <QCalendarWidget>
#include <QTextCharFormat>

#include "util.h"

namespace Ui {
    class DataMonitor;
}

class DataMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit DataMonitor(QWidget *parent = 0);
    ~DataMonitor();

    bool korean;
    void setLanguageEn();
    void setLanguageKo();
    void setup();

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
