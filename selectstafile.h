#ifndef SELECTSTAFILE_H
#define SELECTSTAFILE_H

#include <QDialog>
#include <QFileDialog>

#include "util.h"
#include "filegenerator.h"

namespace Ui {
    class SelectStaFile;
}

class SelectStaFile : public QDialog
{
    Q_OBJECT

public:
    explicit SelectStaFile(bool _event = 0, QWidget *parent = 0);
    ~SelectStaFile();

    bool korean;
    bool event;
    void setLanguageEn();
    void setLanguageKo();
    void setup();

private:
    Ui::SelectStaFile *ui;

    QTextCodec *codec;

public slots:
    void selectFiles(int);
    void selectButtonClicked();

private slots:    

signals:
    void sendSignaltoDataExtractor();

};

#endif // SELECTSTAFILE_H
