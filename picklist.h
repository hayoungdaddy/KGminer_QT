#ifndef PICKLIST_H
#define PICKLIST_H

#include <QFrame>
#include <QProcess>
#include <QDate>
#include <QTimer>

#include "util.h"

namespace Ui {
class PickList;
}

class PickList : public QFrame
{
    Q_OBJECT
    
public:
    explicit PickList(QWidget *parent = 0);
    ~PickList();    

    void setup();
    void clear();

    QProcess *pickProcess;

    QDate today;

public slots:
    void loadTextFile();

private:
    Ui::PickList *PickList_ui;
    QTimer *checkdatetimer;

private slots:
    void checkdate();
};

#endif // PICKLIST_H
