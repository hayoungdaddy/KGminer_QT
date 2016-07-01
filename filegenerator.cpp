#include "filegenerator.h"

FileGenerator::FileGenerator(QWidget *parent) :
    QDialog(parent)
{
}

FileGenerator::~FileGenerator()
{
}

STAFILE FileGenerator::makeSTAFILE(CFG c, bool event)
{
    STAFILE stafile;
    QFile file;

    double minlat, maxlat, minlon, maxlon, avglat, avglon;
    int scnCount;

    if(!event) file.setFileName(c.PARAMSDIR + "/sta.info");
    else if(event) file.setFileName(c.EVENTDIR + "/NEWEVENT/sta.info");

    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        while(!stream.atEnd())
        {
            line = stream.readLine();

            if(line.startsWith("#") || line.isNull() || line.startsWith("["))
                continue;
            else if(line.startsWith("Filename"))
            {
                stafile.filename = line.section(":",1,1);
                continue;
            }
            else if(line.startsWith("Description"))
            {
                stafile.description = line.section(":",1,1);
                continue;
            }
            else
            {
                stafile.staName << line.section(" ", 0, 0);
                stafile.chanName << line.section(" ", 1, 1);
                stafile.locName << line.section(" ", 2, 2);
                stafile.netName << line.section(" ", 3, 3);
                stafile.latD << line.section(" ", 4, 4);
                stafile.lonD << line.section(" ", 5, 5);
                stafile.elevKm << line.section(" ", 6, 6);
            }
        }

        file.close();

        scnCount = stafile.staName.count();

        minlat = 999; maxlat = 0; minlon = 999; maxlon = 0;

        for(int i=0;i<scnCount;i++)
        {
            double fx, fy, latd, lond;
            int x, y;

            fx = stafile.latD[i].toDouble();
            fy = stafile.lonD[i].toDouble();
            if(fx < minlat) minlat = fx;
            if(fx > maxlat) maxlat = fx;
            if(fy < minlon) minlon = fy;
            if(fy > maxlon) maxlon = fy;

            x = stafile.latD[i].section('.',0,0).toInt();
            y = stafile.lonD[i].section('.',0,0).toInt();
            latd = 60 * (fx - x);
            lond = 60 * (fy - y);

            QString temp, temp2, temp3, temp4;
            temp = temp.setNum(x, 10);
            temp2 = temp2.setNum(latd, 'f', 4);
            temp3 = temp3.setNum(y, 10);
            temp4 = temp4.setNum(lond, 'f', 4);

            if(temp2.toFloat() < 10)
                stafile.latM << temp + "  " + temp2 + "N";
            else
                stafile.latM << temp + " " + temp2 + "N";

            if(temp4.toFloat() < 10)
                stafile.lonM << temp3 + "  " + temp4 + "E";
            else
                stafile.lonM << temp3 + " " + temp4 + "E";
            stafile.elevM << stafile.elevKm[i].section('.',1,1);
        }

        avglat = (minlat + maxlat) / 2;
        avglon = (minlon + maxlon) / 2;
        stafile.avgLatforNLLoc = stafile.avgLatforNLLoc.setNum(avglat, 'f', 6);
        stafile.avgLonforNLLoc = stafile.avgLonforNLLoc.setNum(avglon, 'f', 6);
        minlat = minlat - 1; maxlat = maxlat + 2; minlon = minlon - 1; maxlon = maxlon + 2;
        stafile.minLatforBinder = stafile.minLatforBinder.setNum(minlat, 'f', 0);
        stafile.maxLatforBinder = stafile.maxLatforBinder.setNum(maxlat, 'f', 0);
        stafile.minLonforBinder = stafile.minLonforBinder.setNum(minlon, 'f', 0);
        stafile.maxLonforBinder = stafile.maxLonforBinder.setNum(maxlon, 'f', 0);
    }

    return stafile;
}

void FileGenerator::pick_ew_gen(CFG c, STAFILE stafile)
{
    /* generate pick_ew.KGminer file. */
    QFile file;
    file.setFileName(c.PARAMSDIR + "/pick_ew.KGminer");

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

void FileGenerator::pick_FP_gen(CFG c, STAFILE stafile)
{
    /* generate pick_FP file. */
    QFile file;
    file.setFileName(c.PARAMSDIR + "/pick_FP.KGminer");

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

void FileGenerator::hinv_gen(CFG c, STAFILE stafile)
{
    /* generate KGminer.hinv file. */
    // GOS1  KS ZHGZ  33 18.0180N126 12.6000E 0.0      A 0.00  0.00  0.00  0.00 1  0.00-- 1.000
    // SSSSS NN cCCC LLL LL.LLLLNSSS SS.SSSSE EEEEEEEE A 0.00
    QFile file;
    file.setFileName(c.PARAMSDIR + "/KGminer.hinv");

    int scnCount = stafile.staName.count();

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        for(int i=0;i<scnCount;i++)
        {
            QString sta, net, chan, lat, lon, elev;

            if(stafile.staName[i].length() == 1) sta = stafile.staName[i] + "   ";
            else if(stafile.staName[i].length() == 2) sta = stafile.staName[i] + "   ";
            else if(stafile.staName[i].length() == 3) sta = stafile.staName[i] + "  ";
            else if(stafile.staName[i].length() == 4) sta = stafile.staName[i] + " ";
            else if(stafile.staName[i].length() == 5) sta = stafile.staName[i];

            //qDebug() << QString::number(stafile.staName[i].length());
            if(stafile.netName[i].length() == 2) net = " " + stafile.netName[i];
            else if(stafile.netName[i].length() == 3) net = stafile.netName[i];

            chan = stafile.chanName[i].right(1) + stafile.chanName[i];
            elev = stafile.elevM[i] + ".0";

            if(elev.length() == 2) elev = "0.0     ";
            else if(elev.length() == 3) elev = elev + "     ";
            else if(elev.length() == 4) elev = elev + "    ";
            else if(elev.length() == 5) elev = elev + "   ";
            else if(elev.length() == 6) elev = elev + "  ";
            else if(elev.length() == 7) elev = elev + " ";

            stream << sta << net << " " << chan << "  " << stafile.latM[i] << stafile.lonM[i] << " " << elev << " "
                      << "A 0.00  0.00  0.00  0.00 1  0.00-- 1.000" << "\n";
        }

        file.close() ;
    }
}

void FileGenerator::tanklist_gen(CFG c, STAFILE stafile)
{
    /* generate KGminer.tank_list file. */
    QFile file;
    file.setFileName(c.PARAMSDIR + "/KGminer.tank_list");

    int scnCount = stafile.staName.count();

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        for(int i=0;i<scnCount;i++)
        {
            stream << "Tank " << stafile.staName[i] << " " << stafile.chanName[i] <<
                      " " << stafile.netName[i] << " " << stafile.locName[i] << " 2948 INST_UNKNOWN MOD_WILDCARD 100 10000	" + c.TNKDIR + "/" <<
                      stafile.staName[i] << "." << stafile.chanName[i] << "." << stafile.netName[i] << "." << stafile.locName[i] << ".tnk" << "\n";
        }

        file.close() ;
    }
}

void FileGenerator::binder_gen(CFG c, STAFILE stafile)
{
    /* generate binder_ew.d file. */
    QFile file;
    file.setFileName(c.PARAMSDIR + "/binder_ew.d");
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
        stream << "grdlat   " << stafile.minLatforBinder << "    " << stafile.maxLatforBinder << "\n";
        stream << "grdlon   " << stafile.minLonforBinder << "    " << stafile.maxLonforBinder << "\n";
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

void FileGenerator::ew2mseed_gen(CFG c, STAFILE stafile)
{
    /* generate ew2mseed.d file. */
    QFile file;
    file.setFileName(c.PARAMSDIR + "/ew2mseed.d");
    int scnCount = stafile.staName.count();

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        stream << "MseedDir " + c.MSEEDDIR << "\n";
        stream << "LogFile 2" << "\n";
        stream << "Verbosity 4" << "\n";
        stream << "LockFile " + c.MSEEDDIR +"/ew2mseed_1.lock" << "\n";
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

// event == true : c.EVENTDIR + "/NEWEVENT/NEWORIGIN/NLLOC/" is basic dir
// event == false : c.PARAMSDIR + "/NLLOC/1~3/" is basic dir
// nlloc_gen function is making files for SVM mode only
void FileGenerator::nlloc_gen(bool event, CFG c, STAFILE stafile)
{
    QString mainDir;

    if(event)
        mainDir = c.EVENTDIR + "/NEWEVENT/NEWORIGIN/NLLOC";
    else
        mainDir = c.PARAMSDIR + "/NLLOC";

    double fst, lst;
    fst = stafile.avgLatforNLLoc.toDouble()-2;
    lst = stafile.avgLatforNLLoc.toDouble()+2;

    int scnCount = stafile.staName.count();

    if(event)
    {
        gen_type(mainDir + "/type");
        gen_gridP(mainDir + "/grid_p.in", stafile, fst, lst, scnCount, c.EVENTDIR + "/NEWEVENT/NEWORIGIN/NLLOC");
        gen_nllocIn(mainDir + "/nlloc.in", stafile.avgLatforNLLoc, stafile.avgLonforNLLoc, fst, lst, c.EVENTDIR + "/NEWEVENT/NEWORIGIN/picklist" ,
                    c.EVENTDIR + "/NEWEVENT/NEWORIGIN/NLLOC", c.EVENTDIR + "/NEWEVENT/NEWORIGIN/LOC/NLLOC");
    }
    else
    {
        for(int j=1;j<=3;j++)
        {
            gen_type(mainDir + "/" + QString::number(j) + "/type");
            gen_gridP(mainDir + "/" + QString::number(j) + "/grid_p.in", stafile, fst, lst, scnCount,
                      c.EVENTDIR + "/EVID/ORID/NLLOC");
            gen_nllocIn(mainDir + "/" + QString::number(j) + "/nlloc.in", stafile.avgLatforNLLoc, stafile.avgLonforNLLoc, fst, lst,
                        c.EVENTDIR + "/EVID/ORID/picklist" , c.EVENTDIR + "/EVID/ORID/NLLOC",
                        c.EVENTDIR + "/EVID/ORID/LOC/NLLOC");
        }
    }
}

void FileGenerator::gen_type(QString filename)
{
    QFile file;
    file.setFileName(filename);
    if( file.open( QIODevice::WriteOnly ))
    {
        QTextStream stream( &file ) ;
        stream << "SVM" << "\n";
        file.close();
    }
}

void FileGenerator::gen_gridP(QString filename, STAFILE stafile, double fst, double lst, int scnCount, QString path)
{
    QFile file;
    file.setFileName(filename);
    if( file.open( QIODevice::WriteOnly ))
    {
        QTextStream stream( &file ) ;
        stream << "CONTROL 1 54321" << "\n";
        stream << "TRANS LAMBERT WGS-84 " << stafile.avgLatforNLLoc << " " << stafile.avgLonforNLLoc << " " << QString::number(fst) << " " << QString::number(lst) << " 0.0" << "\n";
        stream << "VGOUT " + path + "/model/layer" << "\n";
        stream << "VGTYPE P" << "\n";
        stream << "VGGRID  6 6 2 -2.5 -2.5 -0.5 1 1 1  SLOW_LEN" << "\n";
        stream << "LAYER 0.0 4.0 0.0" << "\n";
        stream << "GTFILES  " + path + "/model/layer " +
                  path + "/time/layer P" << "\n";
        stream << "GTMODE GRID3D ANGLES_NO" << "\n";
        for(int i=0;i<scnCount;i++)
            stream << "GTSRCE " << stafile.staName[i] << " LATLON " << stafile.latD[i] << " " << stafile.lonD[i] << " 0 " << stafile.elevKm[i] << "\n";
        stream << "GT_PLFD  1.0e-3  0" << "\n";

        file.close();
    }
}

void FileGenerator::gen_nllocIn(QString filename, QString avgLat, QString avgLon, double fst, double lst, QString path1, QString path2, QString path3)
{
    QFile file;
    file.setFileName(filename);
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
        stream << "LOCGRID  321 321 101 -1.6 -1.6 -0.5 0.01 0.01 0.01  PROB_DENSITY SAVE" << "\n";
        stream << "LOCPHSTAT 9999.0 -1 9999.0 1.0 1.0 9999.9 -9999.9 9999.9" << "\n";
        stream << "LOCANGLES ANGLES_NO 5" << "\n";
        stream << "LOCMAG  ML_HB 1.0 1.110 0.00189" << "\n";
        stream << "LOCELEVCORR 1 5.8 3.46" << "\n";
        stream << "LOCSTAWT 0 -1.0" << "\n";
        stream << "LOCFILES " + path1 + " HYPOINVERSE_Y2000_ARC " + path2 +
                  "/time/layer " + path3 << "\n";
        stream << "LOCHYPOUT SAVE_HYPOINVERSE_Y2000_ARC" << "\n";

        file.close();
    }
}
