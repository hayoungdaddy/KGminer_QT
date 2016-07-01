#ifndef FILEGENERATOR_H
#define FILEGENERATOR_H

#include "common.h"

namespace Ui {
    class FileGenerator;
}

class FileGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit FileGenerator(QWidget *parent = 0);
    ~FileGenerator();

    STAFILE makeSTAFILE(CFG, bool);
    void pick_ew_gen(CFG, STAFILE);
    void pick_FP_gen(CFG, STAFILE);
    void binder_gen(CFG, STAFILE);
    void hinv_gen(CFG, STAFILE);
    void tanklist_gen(CFG, STAFILE);
    void ew2mseed_gen(CFG, STAFILE);
    void nlloc_gen(bool, CFG, STAFILE);
    void gen_type(QString);
    void gen_gridP(QString, STAFILE, double, double, int, QString);
    void gen_nllocIn(QString, QString, QString, double, double, QString, QString, QString);
};

#endif // FILEGENERATOR_H
