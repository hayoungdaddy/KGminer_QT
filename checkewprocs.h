#ifndef CHECKEWPROCS_H
#define CHECKEWPROCS_H

#include <QObject>
#include <QTimer>
#include <QProcess>

#include "common.h"

class CheckEWProcs : public QObject
{
    Q_OBJECT

public:
    CheckEWProcs(CFG cfg);
    ~CheckEWProcs();

    CFG c;

private slots:
    void checkEWProcess();
    EWMODULEINFO readEwModuleInfo();

signals:
    void sendEWModuleListToMainwindow(EWMODULEINFO);
};

#endif // CHECKEWPROCS_H
