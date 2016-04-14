#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlDatabase>>
#include <QSqlRecord>
#include <QSqlError>

#include "common.h"
#include "selectstafile.h"
#include "nlloc.h"

namespace Ui {
    class DataExtractor;
}

class DataExtractor : public QDialog
{
    Q_OBJECT

public:
    explicit DataExtractor(CFG cfg, bool _korean = 0, QWidget *parent = 0);
    ~DataExtractor();

    bool korean;
    CFG c;

    void setLanguageEn();
    void setLanguageKo();

    STAFILE stafile;

    void setup();

    QString EVID, ORID, MAINDIR, TYPE;

private:
    Ui::DataExtractor *ui;
    QTextCodec *codec;

    QSqlDatabase db;
    QSqlQueryModel *model;

    SelectStaFile *selectstafile;
    NLLoc *nlloc;

public slots:


private slots:
    void loadStaInfoBClicked();
    void rvSignalfromDataExtractor();
    void viewStaLocBClicked();

    void extractBClicked();
    void resetBClicked();

    void runGeoBClicked();    
    void savePickBClicked();

    void editParamNLLocBClicked();
    void runNLLocBClicked();

    void saveBClicked();

signals:

};

#endif // DATAEXTRACTOR_H
