#ifndef IMPORTTNK_H
#define IMPORTTNK_H

#include <QDialog>
#include <QFileDialog>

#include "common.h"

namespace Ui {
    class ImportTnk;
}

class ImportTnk : public QDialog
{
    Q_OBJECT

public:
    explicit ImportTnk(QWidget *parent = 0);
    ~ImportTnk();

    void setup();

private:
    Ui::ImportTnk *ImportTnk_ui;

public slots:
    void selectFiles(int);
    void checkOption(bool);

private slots:
    void replayButtonClicked();

signals:

};

#endif // IMPORTTNK_H
