#ifndef COMMON_H
#define COMMON_H

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>
#include <QDir>
#include <QProcess>

static QString HOMEDIR="/opt/KGminer/V1.0";
static QString BINDIR="/opt/KGminer/V1.0/bin";
static QString PARAMSDIR="/opt/KGminer/V1.0/params";
static QString SCRIPTDIR="/opt/KGminer/V1.0/script";
static QString MSEEDDIR="/data/mseed";
static QString DATADIR="/data";
static QString DB="/data/db/kgminer.db";
static QString TMPDIR="/data/tmp";
static QString EVENTDIR="/data/event";
static int LATENCYSOCKNUM = 9908;

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
