#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlDatabase>>
#include <QSqlRecord>
#include <QSqlError>

#include "util.h"
#include "selectstafile.h"
#include "nlloc.h"

namespace Ui {
    class DataExtractor;
}

class DataExtractor : public QDialog
{
    Q_OBJECT

public:
    explicit DataExtractor(bool iskorean = 0, QWidget *parent = 0);
    ~DataExtractor();

    bool korean;

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
    void nlloc_gen(STAFILE);
    void runNLLocBClicked();

    void saveBClicked();

signals:

};

#endif // DATAEXTRACTOR_H
