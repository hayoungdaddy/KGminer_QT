#ifndef CHANGELOCATOR_H
#define CHANGELOCATOR_H

#include <QDialog>
#include <QFileDialog>

#include "common.h"

namespace Ui {
    class ChangeLocator;
}

class ChangeLocator : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeLocator(QString loc1, QString loc2, QString loc3, QWidget *parent = 0);
    ~ChangeLocator();

    void setup();

private:
    Ui::ChangeLocator *ChangeLocator_ui;

public slots:

private slots:
    void genButtonClicked();

signals:
    void changeloc(int, int, int);

};

#endif // CHANGELOCATOR_H
