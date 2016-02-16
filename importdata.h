#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <QDialog>
#include <QFileDialog>
#include <QProgressBar>

#include "common.h"

namespace Ui {
    class ImportData;
}

class ImportData : public QDialog
{
    Q_OBJECT

public:
    explicit ImportData(QWidget *parent = 0);
    ~ImportData();

    bool korean;
    void setup();

    void setLanguageEn();
    void setLanguageKo();

private:
    Ui::ImportData *ui;
    QTextCodec *codec;

    QStringList scnl;

public slots:


private slots:
    void openFileBrowser();
    void genButtonClicked();
    void viewWaveformClicked();
    void viewWaveClicked(int, int);

signals:

};

#endif // IMPORTDATA_H
