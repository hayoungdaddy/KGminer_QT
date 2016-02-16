#ifndef LOCATOR_H
#define LOCATOR_H

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QProcess>

static void runNLLoc(QString eventDir, int evid, int orid, QString paramsDir, QString type, float minVel, float maxVel)
{
    QString resultDir = eventDir + "/" + QString::number(evid) + "/" + QString::number(orid) + "/LOC";
    QString logDir = eventDir + "/" + QString::number(evid) + "/" + QString::number(orid) + "/NLLOC";
    QDir dir;
    dir.setPath(resultDir);
    if(!dir.exists())
        dir.mkpath(".");
    dir.setPath(logDir);
    if(!dir.exists())
        dir.mkpath(".");

    QString cmd;
    cmd = "cp -R " + paramsDir + "/* " + logDir;
    system(cmd.toLatin1().data());

    if(type.startsWith("SVM"))
    {

    }
}

#endif // LOCATOR_H
