#ifndef COMMON_H
#define COMMON_H

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>
#include <QDir>
#include <QProcess>

typedef struct
{
    QString filename;
    QString description;
    QStringList staName;
    QStringList chanName;
    QStringList locName;
    QStringList netName;
    QStringList latD;
    QStringList lonD;
    QStringList elevKm;
    QStringList latM;
    QStringList lonM;
    QStringList elevM;
} STAFILE;

typedef struct
{
    QString BINDIR;
    QString PARAMSDIR;
    QString SCRIPTDIR;
    QString LOGDIR;
    QString MSEEDDIR;
    QString TNKDIR;
    QString EVENTDIR;
    QString DBDIR;
    QString DBNAME;
    QString PICKDIR;
    QString TMPDIR;
    QString LATENCYSOCKET;
} CFG;

typedef struct
{
    QString filename;
    QString description;
    QStringList staName;
    QStringList chanName;
    QStringList locName;
    QStringList netName;
    QStringList latD;
    QStringList lonD;
    QStringList elevKm;
    QStringList latM;
    QStringList lonM;
    QStringList elevM;
} STATIONSET;

typedef struct
{
    QStringList prName;
    QStringList prID;
    QStringList status;
    QStringList argument;
} EWMODULEINFO;

typedef struct
{
    QString evName;
    QStringList staName;
    QStringList chanName;
    QStringList locName;
    QStringList phase;
    QStringList dateTime;
    QStringList sec;
    QStringList msec;
} PICKINFO;

#endif // COMMON_H
