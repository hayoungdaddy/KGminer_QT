#ifndef VIEWLOG_H
#define VIEWLOG_H

#include <QDialog>

#include "common.h"

namespace Ui {
    class ViewLog;
}

class ViewLog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewLog(CFG cfg, bool _korean = 0, QWidget *parent = 0);
    ~ViewLog();

    bool korean;
    void setLanguageEn();
    void setLanguageKo();
    void setup(QString);
    CFG c;

private:
    Ui::ViewLog *ui;

    QTextCodec *codec;

private slots:
    void selectFiles(int);
    void viewLogButtonClicked();

signals:

};

#endif // VIEWLOG_H
