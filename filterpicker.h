#ifndef FILTERPICKER_H
#define FILTERPICKER_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QList>
#include <QVBoxLayout>

#include "common.h"

class FilterPicker : public QDialog
{
    Q_OBJECT

public:
    explicit FilterPicker(CFG cfg, bool _korean = 0, QWidget *parent = 0);
    ~FilterPicker();

    bool korean;
    CFG c;

private:
    QTextCodec *codec;

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
