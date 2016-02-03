#ifndef GRID_H
#define GRID_H

#include <QDialog>

#include "util.h"

namespace Ui {
    class Grid;
}

class Grid : public QDialog
{
    Q_OBJECT

public:
    explicit Grid(STAINFO *stainfo = 0, QWidget *parent = 0);
    ~Grid();

    void setup(STAINFO *stainfo);
    QString language;
    QTextCodec *codec;

private:
    Ui::Grid *Grid_ui;

    QString latOrig;
    QString lonOrig;
    QString fsp;
    QString ssp;
    QString ngx, ngy, ngz;
    QString ogx, ogy, ogz;
    QString gsx, gsy, gsz;

private slots:
    void genButtonClicked();

signals:

};

#endif // GRID_H
