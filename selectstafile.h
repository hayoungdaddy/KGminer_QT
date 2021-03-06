#ifndef SELECTSTAFILE_H
#define SELECTSTAFILE_H

#include <QDialog>
#include <QFileDialog>

#include "common.h"
#include "filegenerator.h"

namespace Ui {
    class SelectStaFile;
}

class SelectStaFile : public QDialog
{
    Q_OBJECT

public:
    explicit SelectStaFile(CFG cfg, bool _korean = 0, bool _event = 0, QWidget *parent = 0);
    ~SelectStaFile();

    bool korean;
    CFG c;
    bool event;
    void setLanguageEn();
    void setLanguageKo();

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
