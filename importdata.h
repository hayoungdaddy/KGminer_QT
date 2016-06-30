#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <QDialog>
#include <QFileDialog>
#include <QProgressBar>
#include <QDateTime>

#include "common.h"
#include "libmseed.h"
#include "lmplatform.h"

namespace Ui {
    class ImportData;
}

class ImportData : public QDialog
{
    Q_OBJECT

public:
    explicit ImportData(CFG cfg, bool _korean = 0, QWidget *parent = 0);
    ~ImportData();

    bool korean;

    CFG c;

    void setLanguageEn();
    void setLanguageKo();

private:
    Ui::ImportData *ui;
    QTextCodec *codec;

    QStringList scnl;
    void readMseedFile(QString, int);

public slots:


private slots:
    void openFileBrowser();
    void genButtonClicked();
    void viewWaveformClicked();
    void viewWaveClicked(int, int);

signals:

};

#endif // IMPORTDATA_H
