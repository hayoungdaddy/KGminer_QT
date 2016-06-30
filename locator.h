#ifndef LOCATOR_H
#define LOCATOR_H

#include <QFile>
#include <QDebug>
#include <QDir>
#include <QProcess>

static void changeEVIDORID(QString dir, QString fn, int evid, int orid)
{
    QByteArray fileData;
    QFile file;
    file.setFileName(dir + "/" + fn);

    if(file.open(QIODevice::ReadWrite))
    {
        fileData = file.readAll();

        QString text = fileData.data();

        text.replace("EVID", QString::number(evid));
        text.replace("ORID", QString::number(orid));

        file.seek(0);
        file.write(text.toUtf8()); // write the new text back to the file

        file.close(); // close the file handle.
    }
}

static void rechangeEVIDORID(QString dir, QString fn, int evid, int orid)
{
    QByteArray fileData;
    QFile file;
    file.setFileName(dir + "/" + fn);

    if(file.open(QIODevice::ReadWrite))
    {
        fileData = file.readAll();

        QString text = fileData.data();

        text.replace(QString::number(orid) + "/NLLOC", "NEWORIGIN/NLLOC");
        text.replace(QString::number(orid) + "/picklist", "NEWORIGIN/picklist");
        text.replace(QString::number(orid) + "/LOC", "NEWORIGIN/LOC");

        file.seek(0);
        file.write(text.toUtf8()); // write the new text back to the file

        file.close(); // close the file handle.
    }
}

static void runNLLoc(QString eventDir, int evid, int orid, QString paramsDir, QString type, float minVel, float maxVel)
{
    QString resultDir, logDir, cmd;

    if(paramsDir.startsWith("NEWEVENT"))
    {
        resultDir = eventDir + "/NEWEVENT/NEWORIGIN/LOC";
        logDir = eventDir + "/NEWEVENT/NEWORIGIN/NLLOC";
    }
    else if(paramsDir.startsWith("NEWORIGIN"))
    {
        resultDir = eventDir + "/" + QString::number(evid) + "/NEWORIGIN/LOC";
        logDir = eventDir + "/" + QString::number(evid) + "/NEWORIGIN/NLLOC";
    }
    else
    {
        resultDir = eventDir + "/" + QString::number(evid) + "/" + QString::number(orid) + "/LOC";
        logDir = eventDir + "/" + QString::number(evid) + "/" + QString::number(orid) + "/NLLOC";
    }

    QDir dir;
    dir.setPath(resultDir);
    if(!dir.exists())
        dir.mkpath(".");
    dir.setPath(logDir);
    if(!dir.exists())
        dir.mkpath(".");

    if(!paramsDir.startsWith("NEWORIGIN") || !paramsDir.startsWith("NEWEVENT"))
    {
        cmd = "cp -R " + paramsDir + "/* " + logDir;
        system(cmd.toLatin1().data());
    }


    if(type.startsWith("SVM"))
    {
        if(!paramsDir.startsWith("NEWORIGIN") || !paramsDir.startsWith("NEWEVENT"))
        {
            changeEVIDORID(logDir, "grid_p.in", evid, orid);
            changeEVIDORID(logDir, "nlloc.in", evid, orid);
        }

        cmd = "Vel2Grid " + logDir + "/grid_p.in >> " + logDir + "/Vel2Grid.log";
        system(cmd.toLatin1().data());
        cmd = "Grid2Time " + logDir + "/grid_p.in >> " + logDir + "/Grid2Time.log";
        system(cmd.toLatin1().data());
        cmd = "NLLoc " + logDir + "/nlloc.in >> " + logDir + "/NLLoc.log";
        system(cmd.toLatin1().data());
        cmd = "grep GEOGRAPHIC " + resultDir + "/NLLOC*20*hyp >" + resultDir + "/origin";
        system(cmd.toLatin1().data());
    }
    else if(type.startsWith("MVM"))
    {
        for(float i=minVel;i<=maxVel;i=i+0.1)
        {
            if(!paramsDir.startsWith("NEWORIGIN") || !paramsDir.startsWith("NEWEVENT"))
            {
                changeEVIDORID(logDir + "/" + QString::number(i, 'f', 1), "grid_p.in", evid, orid);
                changeEVIDORID(logDir + "/" + QString::number(i, 'f', 1), "nlloc.in", evid, orid);
            }

            QString cmd;
            cmd = "Vel2Grid " + logDir + "/" + QString::number(i, 'f', 1) + "/grid_p.in >> " + logDir + "/" + QString::number(i, 'f', 1) + "/Vel2Grid.log";
            system(cmd.toLatin1().data());
            cmd = "Grid2Time " + logDir + "/" + QString::number(i, 'f', 1) + "/grid_p.in >> " + logDir + "/" + QString::number(i, 'f', 1) + "/Grid2Time.log";
            system(cmd.toLatin1().data());
            cmd = "NLLoc " + logDir + "/" + QString::number(i, 'f', 1) + "/nlloc.in >> " + logDir + "/" + QString::number(i, 'f', 1) + "/NLLoc.log";
            system(cmd.toLatin1().data());
        }

        double rmssum, rmsavg; rmssum = 0;
        double latsum, latavg, lonsum, lonavg, depsum, depavg; latsum=0; lonsum=0; depsum=0;
        int count = 0;
        QDir dir;
        dir.setPath( resultDir );
        QStringList files;
        QString filterName = "NLLOC*20*hyp";
        files = dir.entryList(QStringList(filterName), QDir::Files | QDir::NoSymLinks);

        QFile file;
        for(int j=0;j<files.count();j++)
        {
            file.setFileName(resultDir + "/" + files[j]);

            if(file.open(QIODevice::ReadOnly))
            {
                QTextStream stream(&file);
                QString line, _line;

                cmd = "grep QUALIT " + resultDir + "/" + files[j] + " >>" + resultDir + "/rms";
                system(cmd.toLatin1().data());

                while(!stream.atEnd())
                {
                    line = stream.readLine();
                    _line = line.simplified();

                    if(line.startsWith("QUALITY"))
                    {
                        rmssum = rmssum + _line.section(' ', 8, 8).toDouble();
                        continue;
                    }
                }
                file.close();
            }
        }

        rmsavg = rmssum / files.count();
        //qDebug() << QString::number(rmsavg) + " " + QString::number(rmssum) + " " + QString::number(files.count());

        for(int j=0;j<files.count();j++)
        {
            file.setFileName(resultDir + "/" + files[j]);

            if(file.open(QIODevice::ReadOnly))
            {
                QTextStream stream(&file);
                QString line, _line;

                while(!stream.atEnd())
                {
                    line = file.readLine();
                    _line = line.simplified();
                    double lat, lon, depth;
                    if(_line.startsWith("GEOGRAPHIC"))
                    {
                        lat = _line.section(' ', 9, 9).toDouble();
                        lon = _line.section(' ', 11, 11).toDouble();
                        depth = _line.section(' ', 13, 13).toDouble();
                    }
                    if(_line.startsWith("QUALITY"))
                    {
                        if( _line.section(' ', 8, 8).toDouble() >= rmsavg )
                        {
                            cmd = "grep GEOGRAPHIC " + file.fileName() + " >>" + resultDir + "/origins";
                            system(cmd.toLatin1().data());

                            latsum = latsum + lat;
                            lonsum = lonsum + lon;
                            depsum = depsum + depth;
                            count++;
                        }
                        continue;
                    }
                }
                file.close();
            }
        }

        latavg = latsum / count;
        lonavg = lonsum / count;
        depavg = depsum / count;

        cmd = "echo `head -1 " + resultDir + "/origins | awk '{print $1 \" \" $2 \" \" $3 \" \" $4 \" \" $5 \" \" $6 \" \" $7 \" \" $8 \" \"}'` Lat " + QString::number(latavg,'f',6)
                + " Long " + QString::number(lonavg,'f',6) + " Depth " + QString::number(depavg,'f',6) + " > " + resultDir + "/origin";
        system(cmd.toLatin1().data());
        //GEOGRAPHIC  OT 2015 12 01  05 03   40.5928  Lat 38.304769 Long 127.307725 Depth -0.468438
    }
}

#endif // LOCATOR_H
