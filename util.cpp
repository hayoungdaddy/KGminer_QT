#include "util.h"

Util::Util()
{
}

Util::~Util()
{
}

CFG Util::readCfg()
{
    CFG cfg;
    QFile file;
    file.setFileName("/opt/KGminer/V1.0/params/KGminer.params");

    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        while(!stream.atEnd())
        {
            line = stream.readLine();

            if(line.startsWith("#") || line.isNull() || line.startsWith("["))
                continue;
            else if(line.startsWith("BINDIR"))
            {
                cfg.BINDIR = line.section("=",1,1);
                continue;
            }
            else if(line.startsWith("PARAMSDIR"))
            {
                cfg.PARAMSDIR = line.section("=",1,1);
                continue;
            }
            else if(line.startsWith("SCRIPTDIR"))
            {
                cfg.SCRIPTDIR = line.section("=",1,1);
                continue;
            }
            else if(line.startsWith("MSEEDDIR"))
            {
                cfg.MSEEDDIR = line.section("=",1,1);
                continue;
            }
            else if(line.startsWith("TNKDIR"))
            {
                cfg.TNKDIR = line.section("=",1,1);
                continue;
            }
            else if(line.startsWith("EVENTDIR"))
            {
                cfg.EVENTDIR = line.section("=",1,1);
                continue;
            }
            else if(line.startsWith("DBDIR"))
            {
                cfg.DBDIR = line.section("=",1,1);
                continue;
            }
            else if(line.startsWith("DBNAME"))
            {
                cfg.DBNAME = line.section(":",1,1);
                continue;
            }
            else if(line.startsWith("PICKDIR"))
            {
                cfg.PICKDIR = line.section(":",1,1);
                continue;
            }
            else if(line.startsWith("TMPDIR"))
            {
                cfg.TMPDIR = line.section(":",1,1);
                continue;
            }
            else if(line.startsWith("LATENCYSOCKET"))
            {
                cfg.LATENCYSOCKET = line.section(":",1,1);
                continue;
            }
        }

        file.close();
    }

    return cfg;
}

void Util::readStationSet()
{
    STATIONSET stationset;
    emit sendStationSettoMainWindow(stationset);
}
