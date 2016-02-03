
#include "grid.h"
#include "ui_grid.h"

Grid::Grid(STAINFO *stainfo, QWidget *parent) :
    QDialog(parent),
    Grid_ui(new Ui::Grid)
{
    Grid_ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );

    connect(Grid_ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(Grid_ui->genButton, SIGNAL(clicked()), this, SLOT(genButtonClicked()));
}

Grid::~Grid()
{
    delete Grid_ui;
}

void Grid::setup(STAINFO *stainfo)
{
    QFile file("/opt/KGminer/params/nll_mgr/grid/run/grid_p.in");
    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            if(_line.startsWith("#") || _line.isNull() || _line.startsWith("[") )
                continue;
            else
            {
                if(_line.startsWith("TRANS"))
                {
                    latOrig = _line.section(' ', 3, 3);
                    lonOrig = _line.section(' ', 4, 4);
                    fsp = _line.section(' ', 5, 5);
                    ssp = _line.section(' ', 6, 6);
                }
                else if(_line.startsWith("VGGRID"))
                {
                    ngx = _line.section(' ', 1, 1);
                    ngy = _line.section(' ', 2, 2);
                    ngz = _line.section(' ', 3, 3);
                    ogx = _line.section(' ', 4, 4);
                    ogy = _line.section(' ', 5, 5);
                    ogz = _line.section(' ', 6, 6);
                    gsx = _line.section(' ', 7, 7);
                    gsy = _line.section(' ', 8, 8);
                    gsz = _line.section(' ', 9, 9);
                    break;
                }
            }
        }

        file.close();

        if(language.startsWith("ko"))
        {
            setWindowTitle(codec->toUnicode("그리드 생성"));
        }
        else if(language.startsWith("en"))
        {
            setWindowTitle("Making Grid");
        }
    }

    // GTSRCE   KS01     LATLON    37.4766       127.8940    0     0.185
    QString source1 = ""; QString source2 = ""; QString source3 = ""; QString source4 = ""; QString source5 = "";
    if(stainfo[0].staName != NULL)
        source1 = "GTSRCE   " + stainfo[0].staName + "     LATLON    " + stainfo[0].lat + "       " + stainfo[0].lon + "    0     " + stainfo[0].elev;
    if(stainfo[1].staName != NULL)
        source2 = "GTSRCE   " + stainfo[1].staName + "     LATLON    " + stainfo[1].lat + "       " + stainfo[1].lon + "    0     " + stainfo[1].elev;
    if(stainfo[2].staName != NULL)
        source3 = "GTSRCE   " + stainfo[2].staName + "     LATLON    " + stainfo[2].lat + "       " + stainfo[2].lon + "    0     " + stainfo[2].elev;
    if(stainfo[3].staName != NULL)
        source4 = "GTSRCE   " + stainfo[3].staName + "     LATLON    " + stainfo[3].lat + "       " + stainfo[3].lon + "    0     " + stainfo[3].elev;
    if(stainfo[4].staName != NULL)
        source5 = "GTSRCE   " + stainfo[4].staName + "     LATLON    " + stainfo[4].lat + "       " + stainfo[4].lon + "    0     " + stainfo[4].elev;

    Grid_ui->latOrigLE->setText(latOrig);
    Grid_ui->lonOrigLE->setText(lonOrig);
    Grid_ui->fspLE->setText(fsp);
    Grid_ui->sspLE->setText(ssp);
    Grid_ui->ngxLE->setText(ngx);
    Grid_ui->ngyLE->setText(ngy);
    Grid_ui->ngzLE->setText(ngz);
    Grid_ui->ogxLE->setText(ogx);
    Grid_ui->ogyLE->setText(ogy);
    Grid_ui->ogzLE->setText(ogz);
    Grid_ui->gsxLE->setText(gsx);
    Grid_ui->gsyLE->setText(gsy);
    Grid_ui->gszLE->setText(gsz);
    Grid_ui->source1LE->setText(source1);
    Grid_ui->source2LE->setText(source2);
    Grid_ui->source3LE->setText(source3);
    Grid_ui->source4LE->setText(source4);
    Grid_ui->source5LE->setText(source5);
}

void Grid::genButtonClicked()
{
    /*
CONTROL 1 54321
TRANS  LAMBERT  WGS-84  37.25 128.00  36.0 38.0  0.0
VGOUT  ./model/layer
VGTYPE S
VGGRID  91 111 22  -45.0 -55.0 -1.0  1.0 1.0 1.0  SLOW_LEN
LAYER  -2.0  5.61 0.00    3.40  0.00  2.7 0.0
LAYER   7.0  6.19 0.00    3.60  0.00  2.7 0.0
LAYER  21.0  6.59 0.00    3.70  0.00  2.7 0.0
LAYER  31.0  7.78 0.00    4.45  0.00  2.7 0.0
GTFILES  ./model/layer  ./time/layer S
GTMODE GRID3D ANGLES_YES
GTSRCE   KS01     LATLON    37.4766       127.8940    0     0.185
GTSRCE   KS02     LATLON    37.4953       127.9032    0     0.233
GTSRCE   KS03     LATLON    37.4783       127.9177    0     0.155
GTSRCE   KS04     LATLON    37.4566       127.9207    0     0.123
GTSRCE   KS05     LATLON    37.4574       127.8924    0     0.139
GT_PLFD  1.0e-3  0
*/
    QFile file;
    QString staName[STANUM];
    staName[0] = Grid_ui->source1LE->text().simplified().section(' ', 1, 1);
    staName[1] = Grid_ui->source2LE->text().simplified().section(' ', 1, 1);
    staName[2] = Grid_ui->source3LE->text().simplified().section(' ', 1, 1);
    staName[3] = Grid_ui->source4LE->text().simplified().section(' ', 1, 1);
    staName[4] = Grid_ui->source5LE->text().simplified().section(' ', 1, 1);

    /* generate grid_p.in file. */
    file.setFileName("/opt/KGminer/params/nll_mgr/grid/run/grid_p.in");

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        stream << "CONTROL 1 54321" << "\n";
        stream << "TRANS  LAMBERT  WGS-84  " << Grid_ui->latOrigLE->text() << " " <<
                  Grid_ui->lonOrigLE->text() << "  " << Grid_ui->fspLE->text() << " " <<
                  Grid_ui->sspLE->text() << "  0.0" << "\n";
        stream << "VGOUT  /opt/KGminer/params/nll_mgr/grid/model/layer" << "\n";
        stream << "VGTYPE P" << "\n";
        stream << "VGGRID  " << Grid_ui->ngxLE->text() << " " << Grid_ui->ngyLE->text() << " " <<
                  Grid_ui->ngzLE->text() << " " << Grid_ui->ogxLE->text() << " " << Grid_ui->ogyLE->text() <<
                  " " << Grid_ui->ogzLE->text() << " " << Grid_ui->gsxLE->text() << " " <<
                  Grid_ui->gsyLE->text() << " " << Grid_ui->gszLE->text() << "  SLOW_LEN" << "\n";
        stream << "LAYER  -2.0  5.61 0.00    3.40  0.00  2.7 0.0" << "\n";
        stream << "LAYER   7.0  6.19 0.00    3.60  0.00  2.7 0.0" << "\n";
        stream << "LAYER  21.0  6.59 0.00    3.70  0.00  2.7 0.0" << "\n";
        stream << "LAYER  31.0  7.78 0.00    4.45  0.00  2.7 0.0" << "\n";
        stream << "GTFILES  /opt/KGminer/params/nll_mgr/grid/model/layer  /opt/KGminer/params/nll_mgr/grid/time/layer P" << "\n";
        stream << "GTMODE GRID3D ANGLES_YES" << "\n";
        stream << Grid_ui->source1LE->text() << "\n";
        stream << Grid_ui->source2LE->text() << "\n";
        stream << Grid_ui->source3LE->text() << "\n";
        stream << Grid_ui->source4LE->text() << "\n";
        stream << Grid_ui->source5LE->text() << "\n";
        stream << "GT_PLFD  1.0e-3  0" << "\n";

        file.close() ;
    }

    /* generate grid_s.in file. */
    file.setFileName("/opt/KGminer/params/nll_mgr/grid/run/grid_s.in");

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        stream << "CONTROL 1 54321" << "\n";
        stream << "TRANS  LAMBERT  WGS-84  " << Grid_ui->latOrigLE->text() << " " <<
                  Grid_ui->lonOrigLE->text() << "  " << Grid_ui->fspLE->text() << " " <<
                  Grid_ui->sspLE->text() << "  0.0" << "\n";
        stream << "VGOUT  /opt/KGminer/params/nll_mgr/grid/model/layer" << "\n";
        stream << "VGTYPE S" << "\n";
        stream << "VGGRID  " << Grid_ui->ngxLE->text() << " " << Grid_ui->ngyLE->text() << " " <<
                  Grid_ui->ngzLE->text() << " " << Grid_ui->ogxLE->text() << " " << Grid_ui->ogyLE->text() <<
                  " " << Grid_ui->ogzLE->text() << " " << Grid_ui->gsxLE->text() << " " <<
                  Grid_ui->gsyLE->text() << " " << Grid_ui->gszLE->text() << "  SLOW_LEN" << "\n";
        stream << "LAYER  -2.0  5.61 0.00    3.40  0.00  2.7 0.0" << "\n";
        stream << "LAYER   7.0  6.19 0.00    3.60  0.00  2.7 0.0" << "\n";
        stream << "LAYER  21.0  6.59 0.00    3.70  0.00  2.7 0.0" << "\n";
        stream << "LAYER  31.0  7.78 0.00    4.45  0.00  2.7 0.0" << "\n";
        stream << "GTFILES  /opt/KGminer/params/nll_mgr/grid/model/layer  /opt/KGminer/params/nll_mgr/grid/time/layer S" << "\n";
        stream << "GTMODE GRID3D ANGLES_YES" << "\n";
        stream << Grid_ui->source1LE->text() << "\n";
        stream << Grid_ui->source2LE->text() << "\n";
        stream << Grid_ui->source3LE->text() << "\n";
        stream << Grid_ui->source4LE->text() << "\n";
        stream << Grid_ui->source5LE->text() << "\n";
        stream << "GT_PLFD  1.0e-3  0" << "\n";

        file.close() ;
    }

    QString cmd = "Vel2Grid /opt/KGminer/params/nll_mgr/grid/run/grid_p.in";
    system(cmd.toLatin1().data());
    cmd = "Vel2Grid /opt/KGminer/params/nll_mgr/grid/run/grid_s.in";
    system(cmd.toLatin1().data());
    cmd = "Grid2Time /opt/KGminer/params/nll_mgr/grid/run/grid_p.in";
    system(cmd.toLatin1().data());
    cmd = "Grid2Time /opt/KGminer/params/nll_mgr/grid/run/grid_s.in";
    system(cmd.toLatin1().data());

    cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/model/layer.P.mod /opt/KGminer/params/nll_mgr/grid/gmt/ V G 1 0 1 " + Grid_ui->ngyLE->text() + "";
    system(cmd.toLatin1().data());
    cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/model/layer.S.mod /opt/KGminer/params/nll_mgr/grid/gmt/ V G 1 0 1 " + Grid_ui->ngyLE->text() + "";
    system(cmd.toLatin1().data());

    if(!staName[0].startsWith(" "))
    {
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[0] + ".time /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[0] + ".angle /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
    }
    if(!staName[1].startsWith(" "))
    {
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[1] + ".time /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[1] + ".angle /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
    }
    if(!staName[2].startsWith(" "))
    {
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[2] + ".time /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[2] + ".angle /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
    }
    if(!staName[3].startsWith(" "))
    {
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[3] + ".time /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[3] + ".angle /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
    }
    if(!staName[4].startsWith(" "))
    {
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[4] + ".time /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
        cmd = "Grid2GMT /opt/KGminer/params/nll_mgr/grid/run/grid_p.in /opt/KGminer/params/nll_mgr/grid/time/layer.P." + staName[4] + ".angle /opt/KGminer/params/nll_mgr/grid/gmt/ V G 0 0 0 " + Grid_ui->ngyLE->text() + "";
        system(cmd.toLatin1().data());
    }

    QMessageBox::warning( this, "Information", "Succeed!\n");
}
