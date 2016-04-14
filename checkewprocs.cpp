#include "checkewprocs.h"

CheckEWProcs::CheckEWProcs( CFG cfg )
{
    QTimer* timer = new QTimer(this);
    timer->start( 3000 );

    c = cfg;

    connect(timer, SIGNAL(timeout()), this, SLOT(checkEWProcess()));
}

CheckEWProcs::~CheckEWProcs()
{
}

void CheckEWProcs::checkEWProcess()
{
    QString program = c.BINDIR + "/status";
    QProcess *process = new QProcess();
    process->setStandardOutputFile(c.PARAMSDIR + "/ewmodulelist", QIODevice::Truncate);
    process->start( program );
    if(!process->waitForStarted())
        return;
    if(!process->waitForFinished())
        return;

    EWMODULEINFO ewmoduleinfo;
    ewmoduleinfo = readEwModuleInfo();

    qRegisterMetaType<EWMODULEINFO>("EWMODULEINFO");

    //qDebug() << ewmoduleinfo.prName;

    emit sendEWModuleListToMainwindow(ewmoduleinfo);
}

EWMODULEINFO CheckEWProcs::readEwModuleInfo()
{
    EWMODULEINFO ewmoduleinfo;
    QFile file;
    file.setFileName(c.PARAMSDIR + "/ewmodulelist");

    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();
            QChar c = _line[0];

            //qDebug() << _line;

            if(c.isUpper() || _line.isNull() || _line.startsWith("-----") || _line.isEmpty() || _line.startsWith("type"))
                continue;
            else
            {
                ewmoduleinfo.prName << _line.section(' ', 0, 0);
                ewmoduleinfo.prID << _line.section(' ', 1, 1);
                ewmoduleinfo.status << _line.section(' ', 2, 2);
                ewmoduleinfo.argument << _line.section(' ', 6, 6);
                continue;
            }
        }
        file.close();
    }
    return ewmoduleinfo;
}
