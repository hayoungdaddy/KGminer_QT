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

    void pick_ew_gen(STAFILE);
    void pick_FP_gen(STAFILE);
    void binder_gen(QString, QString, QString, QString);
    void hinv_gen(STAFILE);
    void tanklist_gen(STAFILE);
    void ew2mseed_gen(STAFILE);
    void nlloc_gen(STAFILE, QString, QString);
};

#endif // FILEGENERATOR_H
