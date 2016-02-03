#ifndef MAKETNK_H
#define MAKETNK_H

#include <QDialog>
#include <QFileDialog>

#include "util.h"

namespace Ui {
    class MakeTnk;
}

class MakeTnk : public QDialog
{
    Q_OBJECT

public:
    explicit MakeTnk(QWidget *parent = 0);
    ~MakeTnk();

    void setup();

private:
    Ui::MakeTnk *MakeTnk_ui;

public slots:

private slots:
    void openFileBrowser();
    void genButtonClicked();

signals:

};

#endif // MAKETNK_H
