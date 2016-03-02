#include "filegenerator.h"

FileGenerator::FileGenerator(QWidget *parent) :
    QDialog(parent)
{
}

FileGenerator::~FileGenerator()
{
}

void FileGenerator::pick_ew_gen(STAFILE stafile)
{
    /* generate pick_ew.KGminer file. */
    QFile file;
    file.setFileName(PARAMSDIR + "/pick_ew.KGminer");

    int scnCount = stafile.staName.count();

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;
        stream << "#" << "\n" ;
        stream << "#                              MinBigZC       RawDataFilt    LtaFilt         DeadSta          PreEvent" << "\n" ;
        stream << "# Pick  Pin    Station/   MinSmallZC   MaxMint           StaFilt       RmavFilt           AltCoda" << "\n" ;
        stream << "# Flag  Numb   Comp/Net   Itr1   MinPeakSize  i9  CharFuncFilt  EventThresh          CodaTerm         Erefs" << "\n" ;
        stream << "# ----  ----   --------   ----------------------------------------------------------------------------------" << "\n" ;

        for(int i=0;i<scnCount;i++)
        {
            stream << "    1     1  " << stafile.staName[i] << "  " << stafile.chanName[i] <<
                      " " << stafile.netName[i] << " " << stafile.locName[i] << " 3  40  3 132  500  2  .939  3.  .4  .015 5.  .9961  1200. 1081.1   .8  1.5 110000. 8388608" << "\n";
        }

        file.close() ;
    }
}

void FileGenerator::pick_FP_gen(STAFILE stafile)
{
    /* generate pick_FP file. */
    QFile file;
    file.setFileName(PARAMSDIR + "/pick_FP.KGminer");

    int scnCount = stafile.staName.count();

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;
        for(int i=0;i<scnCount;i++)
        {
            QString temp;
            temp = temp.setNum(i+1, 10);
            stream << "    1    " + temp + " " << stafile.staName[i] << "  " << stafile.chanName[i] << " " << stafile.netName[i] << " " << stafile.locName[i] << " 1.5 2.5 10 6 0.1" << "\n";
        }
        file.close() ;
    }
}

void FileGenerator::hinv_gen(STAFILE stafile)
{
    /* generate KGminer.hinv file. */
    QFile file;
    file.setFileName(PARAMSDIR + "/KGminer.hinv");

    int scnCount = stafile.staName.count();

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        for(int i=0;i<scnCount;i++)
        {
            QString size = stafile.netName[i];
            QString temp = stafile.chanName[i].right(1);
            if(size.length() == 2)
            {
                stream << stafile.staName[i] << "   " << stafile.netName[i] << " " << temp << stafile.chanName[i] << "  " <<
                          stafile.latM[i] << stafile.lonM[i] << " " << stafile.elevM[i] << "0.0   A 0.00  0.00  0.00  0.00 1  0.00-- 1.000" << "\n";
            }
            else if(size.length() == 3)
            {
                stream << stafile.staName[i] << "  " << stafile.netName[i] << " " << temp << stafile.chanName[i] << "  " <<
                          stafile.latM[i] << stafile.lonM[i] << " " << stafile.elevM[i] << "0.0   A 0.00  0.00  0.00  0.00 1  0.00-- 1.000" << "\n";
            }
            //qDebug() << size;
        }

        file.close() ;
    }
}

void FileGenerator::tanklist_gen(STAFILE stafile)
{
    /* generate KGminer.tank_list file. */
    QFile file;
    file.setFileName(PARAMSDIR + "/KGminer.tank_list");

    int scnCount = stafile.staName.count();

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        for(int i=0;i<scnCount;i++)
        {
            stream << "Tank " << stafile.staName[i] << " " << stafile.chanName[i] <<
                      " " << stafile.netName[i] << " " << stafile.locName[i] << " 2948 INST_UNKNOWN MOD_WILDCARD 100 10000	" + DATADIR + "/tnk/" <<
                      stafile.staName[i] << "." << stafile.chanName[i] << "." << stafile.netName[i] << "." << stafile.locName[i] << ".tnk" << "\n";
        }

        file.close() ;
    }
}

void FileGenerator::binder_gen(QString minLatforBinder, QString maxLatforBinder, QString minLonforBinder, QString maxLonforBinder)
{
    /* generate binder_ew.d file. */
    QFile file;
    file.setFileName(PARAMSDIR + "/binder_ew.d");
    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        stream << "# This is binder's parameter file!" << "\n";
        stream << "#---------------------------------" << "\n";
        stream << " MyModuleId   MOD_BINDER_EW # module id for this instance of binder" << "\n";
        stream << "RingName     PICK_RING     # transport ring for I/O" << "\n";
        stream << "# withers warning (20060517).  apparently if this param is configured then the" << "\n";
        stream << "# ring needs to be defined in earthworm.d but can not be defined in" << "\n";
        stream << "# startstop.d." << "\n";
        stream << "BufferRing   BINDER_RING     # private ring for buffering incoming picks" << "\n";
        stream << "                           #  (default=BINDER_RING)" << "\n";
        stream << "LogFile      1             # 0 = turn off logging" << "\n";
        stream << "                           # 1 = log to disk and stderr/stdout" << "\n";
        stream << "                           # 2 = log to disk but not to stderr/stdout" << "\n";
        stream << "HeartbeatInt 30            # Seconds between heartbeats" << "\n\n";
        stream << "# List the message logos to grab from transport ring" << "\n";
        stream << "#              Installation       Module      # Message Type" << "\n";
        stream << "#-------------------------------------------------------------" << "\n";
        stream << "GetPicksFrom  INST_WILDCARD    MOD_WILDCARD  # TYPE_PICK_SCNL" << "\n\n";
        stream << "# Set level of output for writing log file" << "\n";
        stream << "#-----------------------------------------" << "\n";
        stream << "# log_stack            # comment out to turn of grid-stack logging" << "\n";
        stream << "hypcode    7         # Sets a value to specify how much information should be" << "\n";
        stream << "                      # included in binder's log file after each event" << "\n";
        stream << "                      # relocation. Possible values 0-7." << "\n\n";
        stream << "# Load station list" << "\n";
        stream << "#------------------" << "\n";
        stream << "maxsite 100" << "\n";
        stream << "site_file \"KGminer.hinv\"" << "\n\n";
        stream << "# Load crustal model" << "\n";
        stream << "# Refer to file containing \"lay\" commands, or list them here" << "\n";
        stream << "#-----------------------------------------------------------" << "\n";
        stream << "#lay   -2.0    5.61" << "\n";
        stream << "lay    0.0    4.00" << "\n";
        stream << "#lay   20.0    6.50" << "\n";
        stream << "#lay   32.0    7.80" << "\n\n";
        stream << "# Set FIFO lengths" << "\n";
        stream << "#-----------------" << "\n";
        stream << "pick_fifo_length  1000   # optional: default=1000" << "\n";
        stream << "quake_fifo_length  100   # optional: default=100" << "\n\n";
        stream << "# Define association grid, set stacking parameters." << "\n";
        stream << "#--------------------------------------------------" << "\n";
        stream << "dspace   2" << "\n";
        stream << "grdlat   " << minLatforBinder << "    " << maxLatforBinder << "\n";
        stream << "grdlon   " << minLonforBinder << "    " << maxLonforBinder << "\n";
        stream << "grdz     0.0    10.0     " << "\n\n";
        stream << "rstack   100" << "\n";
        stream << "tstack   3.0" << "\n";
        stream << "stack    20" << "\n";
        stream << "thresh   4" << "\n";
        stream << "focus    100" << "\n";
        stream << "grid_wt 0  4" << "\n";
        stream << "grid_wt 1  4" << "\n";
        stream << "grid_wt 2  4" << "\n";
        stream << "grid_wt 3  4" << "\n\n";
        stream << "# Set parameters for associating picks with active hypocenters" << "\n";
        stream << "#-------------------------------------------------------------" << "\n";
        stream << "rAvg_Factor 5.0" << "\n";
        stream << "taper   0.0     1.0" << "\n";
        stream << "taper   50.0    1.0" << "\n";
        stream << "taper   200.0   2.5" << "\n";
        stream << "taper   400.0   4.5" << "\n";
        stream << "taper_OT 2.0    2.0" << "\n";
        stream << "t_dif   -1.0  120." << "\n\n";
        stream << "# Set parameters for locating events" << "\n";
        stream << "#-----------------------------------" << "\n";
        stream << "wt 0 1.0" << "\n";
        stream << "wt 1 0.5" << "\n";
        stream << "wt 2 0.25" << "\n";
        stream << "wt 3 0.125" << "\n";
        stream << "ph P  1.0" << "\n";
        stream << "ph Pn 0.5" << "\n";
        stream << "ph Pg 0.5" << "\n";
        stream << "r_max  400.0" << "\n";
        stream << "zrange 1.0 30.0" << "\n";
        stream << "MaxStep 25.0  5.0" << "\n";
        stream << "MinXYZstep 0.1" << "\n";
        stream << "MaxIter 3" << "\n";
        stream << "MaxDeltaRms 1.0001" << "\n";
        stream << "locate_eq  25   2" << "\n";
        stream << "locate_eq  50   4" << "\n";
        stream << "locate_eq  75   8" << "\n";
        stream << "locate_eq 100  16" << "\n\n";
        stream << "# Set parameters for assessing groups of picks" << "\n";
        stream << "# with Bill Ellsworth's resampling technique:" << "\n";
        stream << "#----------------------------------------------" << "\n";
        stream << "assess_pk  0  0" << "\n";
        stream << "maxtrial   500" << "\n";
        stream << "maxwt      3" << "\n";
        stream << "v_halfspace  5.0" << "\n";
        stream << "residual_cut  5.0  200.0" << "\n";
        stream << "#log_accepted" << "\n\n";
        stream << "# Load in the next valid quake sequence number" << "\n";
        stream << "#---------------------------------------------" << "\n";
        stream << "EventIdFile quake_id.d" << "\n\n";
        stream << "# THE END" << "\n";

        file.close() ;
    }
}

void FileGenerator::ew2mseed_gen(STAFILE stafile)
{
    /* generate ew2mseed.d file. */
    QFile file;
    file.setFileName(PARAMSDIR + "/ew2mseed.d");
    int scnCount = stafile.staName.count();

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        stream << "MseedDir " + MSEEDDIR << "\n";
        stream << "LogFile 2" << "\n";
        stream << "Verbosity 4" << "\n";
        stream << "LockFile " + DATADIR +"/ew2mseed_1.lock" << "\n";
        for(int i=0;i<scnCount;i++)
        {
            stream << "SCNLocSz " << stafile.staName[i] << " " << stafile.chanName[i] << " " << stafile.netName[i] << " " << stafile.locName[i] << " 512 STEIM2" << "\n";
        }
        stream << "WaveServer 127.0.0.1 16029" << "\n";
        stream << "TravelTimeout  20" << "\n";
        stream << "SocketReconnect  3000" << "\n";
        stream << "RecordsNumber 3" << "\n";

        file.close();
    }
}

void FileGenerator::nlloc_gen(STAFILE stafile, QString avgLat, QString avgLon)
{
    /* generate grid_p.in file */
    QString vgout, vggrid, gtfiles, locgrid, locfiles;
    QStringList layer;
    QFile file;

    double fst, lst;
    fst = avgLat.toDouble()-2;
    lst = avgLat.toDouble()+2;

    qDebug() << avgLat + " " + avgLon;

    int scnCount = stafile.staName.count();

    for(int j=1;j<=3;j++)
    {
        file.setFileName(PARAMSDIR + "/NLLOC/" + QString::number(j) + "/grid_p.in");
        if( file.open( QIODevice::ReadOnly ) )
        {
            QTextStream stream(&file);
            QString line;

            while(!stream.atEnd())
            {
                line = stream.readLine();

                if(line.startsWith("#") || line.isNull() || line.startsWith("["))
                    continue;
                else if(line.startsWith("VGOUT"))
                    vgout = line;
                else if(line.startsWith("VGGRID"))
                    vggrid = line;
                else if(line.startsWith("LAYER"))
                    layer << line;
                else if(line.startsWith("GTFILES"))
                    gtfiles = line;
            }
            file.close();
        }
        if( file.open( QIODevice::WriteOnly ))
        {
            QTextStream stream( &file ) ;
            // TRANS  LAMBERT  WGS-84  38.300629 127.320459  37 40  0.0
            stream << "CONTROL 1 54321" << "\n";
            stream << "TRANS LAMBERT WGS-84 " << avgLat << " " << avgLon << " " << QString::number(fst) << " " << QString::number(lst) << " 0.0" << "\n";
            stream << vgout << "\n";
            stream << "VGTYPE P" << "\n";
            stream << vggrid << "\n";
            for(int i=0;i<layer.count();i++)
                stream << layer[i] << "\n";
            stream << gtfiles << "\n";
            stream << "GTMODE GRID3D ANGLES_NO" << "\n";
            for(int i=0;i<scnCount;i++)
                stream << "GTSRCE " << stafile.staName[i] << " LATLON " << stafile.latD[i] << " " << stafile.lonD[i] << " 0 " << stafile.elevKm[i] << "\n";
            stream << "GT_PLFD  1.0e-3  0" << "\n";

            file.close();
        }

        file.setFileName(PARAMSDIR + "/NLLOC/" + QString::number(j) + "/nlloc.in");
        if( file.open( QIODevice::ReadOnly ) )
        {
            QTextStream stream(&file);
            QString line;

            while(!stream.atEnd())
            {
                line = stream.readLine();

                if(line.startsWith("#") || line.isNull() || line.startsWith("["))
                    continue;
                else if(line.startsWith("LOCGRID"))
                    locgrid = line;
                else if(line.startsWith("LOCFILES"))
                    locfiles = line;
            }
            file.close();
        }
        if( file.open( QIODevice::WriteOnly ))
        {
            QTextStream stream( &file ) ;
            stream << "CONTROL 1 54321" << "\n";
            stream << "TRANS LAMBERT WGS-84 " << avgLat << " " << avgLon << " " << QString::number(fst) << " " << QString::number(lst) << " 0.0" <<  "\n";
            stream << "LOCSIG  KGminer" << "\n";
            stream << "LOCCOM KGminer" << "\n";
            stream << "LOCMETH  EDT_OT_WT 1.0e6 2 25  -1 -1.80 6 -1.0 0" << "\n";
            stream << "LOCHYPOUT  SAVE_NLLOC_ALL SAVE_HYPOINVERSE_Y2000_ARC" << "\n";
            stream << "LOCGAU 0.5 0.0" << "\n";
            stream << "LOCGAU2 0.02 0.1 2.0" << "\n";
            stream << "LOCPHASEID  P   P p G PN Pn P* Pg PG Pb PB" << "\n";
            stream << "LOCPHASEID  S   S s G SN Sn S* Sg SG Sb SB" << "\n";
            stream << "LOCQUAL2ERR 0.1 0.2 0.5 1.0 99999.9" << "\n";
            stream << "LOCSEARCH  OCT 10 10 4 0.01 10000 5000 0 1" << "\n";
            stream << locgrid << "\n";
            stream << "LOCPHSTAT 9999.0 -1 9999.0 1.0 1.0 9999.9 -9999.9 9999.9" << "\n";
            stream << "LOCANGLES ANGLES_NO 5" << "\n";
            stream << "LOCMAG  ML_HB 1.0 1.110 0.00189" << "\n";
            stream << "LOCELEVCORR 1 5.8 3.46" << "\n";
            stream << "LOCSTAWT 0 -1.0" << "\n";
            stream << locfiles << "\n";
            stream << "LOCHYPOUT SAVE_HYPOINVERSE_Y2000_ARC" << "\n";

            file.close();
        }
    }
}
