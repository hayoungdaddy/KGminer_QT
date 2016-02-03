#ifndef UTIL_H
#define UTIL_H

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>

static QString HOMEDIR="/opt/KGminer";
static QString BINDIR="/opt/KGminer/bin";
static QString PARAMSDIR="/opt/KGminer/params";
static QString SCRIPTDIR="/opt/KGminer/script";
static QString MSEEDDIR="/data/mseed";
static QString DATADIR="/data";
static QString DB="/data/db/kgminer.db";
static QString TMPDIR="/opt/KGminer/tmp";
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

#endif // UTIL_H
