#ifndef VIEWSTATION_H
#define VIEWSTATION_H

#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "common.h"
#include "filegenerator.h"

namespace Ui {
    class ViewStation;
}

class ViewStation : public QDialog
{
    Q_OBJECT

public:
    explicit ViewStation(QWidget *parent = 0);
    ~ViewStation();

    bool korean;
    void setLanguageEn();
    void setLanguageKo();
    void setup();

    int count;

private:
    QTextCodec *codec;

    QVBoxLayout *mainlayout;
    QHBoxLayout *toplayout;
    QHBoxLayout *toplayout2;
    QHBoxLayout *labellayout;
    QVBoxLayout *middlelayout;
    QHBoxLayout *buttomlayout;
    QList<QLineEdit*> qlist;

    QPushButton *addbutton;
    QPushButton *genbutton;
    QPushButton *quitbutton;
    QPushButton *viewstabutton;

    QLabel *la1;
    QLabel *la2;

    QLineEdit *filenameLE;
    QLineEdit *descLE;

    QString filename, description;

    std::vector<QString> sta;
    std::vector<QString> chan;
    std::vector<QString> loc;
    std::vector<QString> net;
    std::vector<QString> lat;
    std::vector<QString> lon;
    std::vector<QString> elev;

private slots:
    void viewstabuttonClicked();
    void addbutton_clicked();
    void lineedit_textChanged(QString text);
    void genButtonClicked();

signals:

};

#endif // VIEWSTATION_H
