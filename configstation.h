#ifndef CONFIGSTATION_H
#define CONFIGSTATION_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "common.h"
#include "filegenerator.h"

namespace Ui {
    class ConfigStation;
}

class ConfigStation : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigStation(QWidget *parent = 0);
    ~ConfigStation();

    bool korean;
    void setLanguageEn();
    void setLanguageKo();
    void setup();

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

    QLabel *la1;
    QLabel *la2;

    QLineEdit *filenameLE;
    QLineEdit *descLE;

    std::vector<QString> sta;
    std::vector<QString> chan;
    std::vector<QString> comp;
    std::vector<QString> loc;
    std::vector<QString> net;
    std::vector<QString> lat;
    std::vector<QString> lon;
    std::vector<QString> elev;

private slots:
    void addbutton_clicked();
    void lineedit_textChanged(QString text);
    void genButtonClicked();

signals:

};

#endif // CONFIGSTATION_H
