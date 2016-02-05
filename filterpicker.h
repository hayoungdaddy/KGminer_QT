#ifndef FILTERPICKER_H
#define FILTERPICKER_H

#include <QDialog>
#include <QLineEdit>
#include <QList>
#include <QVBoxLayout>

#include "common.h"

namespace Ui {
    class FilterPicker;
}

class FilterPicker : public QDialog
{
    Q_OBJECT

public:
    explicit FilterPicker(QWidget *parent = 0);
    ~FilterPicker();

private:
    QStringList staName;
    QStringList chanName;
    QStringList netName;
    QStringList locName;
    QStringList str;
    QStringList fw;
    QStringList lw;
    QStringList th1;
    QStringList th2;
    QStringList te;

    QVBoxLayout *layout;
    QList<QLineEdit*> qlist;

    void addLineEdit(int);

private slots:
    void genButtonClicked();
    void lineedit_textChanged(QString text);

signals:

};

#endif // FILTERPICKER_H
