#include "binder.h"
#include "ui_binder.h"

Binder::Binder(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Binder)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    c = cfg;

    setup();

    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->genButton, SIGNAL(clicked()), this, SLOT(genButtonClicked()));
    connect(ui->defaultButton, SIGNAL(clicked()), this, SLOT(defaultButtonClicked()));
}

Binder::~Binder()
{
    delete ui;
}

void Binder::setup()
{
    QFile file( c.PARAMSDIR + "/binder_ew.d");
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
                if(_line.startsWith("dspace"))
                    dspace = _line.section(' ', 1, 1);
                else if(_line.startsWith("grdlat"))
                {
                    grdlat1 = _line.section(' ', 1, 1);
                    grdlat2 = _line.section(' ', 2, 2);
                }
                else if(_line.startsWith("grdlon"))
                {
                    grdlon1 = _line.section(' ', 1, 1);
                    grdlon2 = _line.section(' ', 2, 2);
                }
                else if(_line.startsWith("grdz"))
                {
                    grdz1 = _line.section(' ', 1, 1);
                    grdz2 = _line.section(' ', 2, 2);
                }
                else if(_line.startsWith("rstack"))
                    rstack = _line.section(' ', 1, 1);
                else if(_line.startsWith("tstack"))
                    tstack = _line.section(' ', 1, 1);
                else if(_line.startsWith("stack"))
                    stack = _line.section(' ', 1, 1);
                else if(_line.startsWith("thresh"))
                    thresh = _line.section(' ', 1, 1);
                else if(_line.startsWith("focus"))
                {
                    focus = _line.section(' ', 1, 1);
                    break;
                }
            }
        }

        file.close();
    }

    ui->dspaceLE->setText(dspace);
    ui->grdlat1LE->setText(grdlat1); ui->grdlat2LE->setText(grdlat2);
    ui->grdlon1LE->setText(grdlon1); ui->grdlon2LE->setText(grdlon2);
    ui->grdz1LE->setText(grdz1); ui->grdz2LE->setText(grdz2);
    ui->rstackLE->setText(rstack);
    ui->tstackLE->setText(tstack);
    ui->stackLE->setText(stack);
    ui->threshLE->setText(thresh);
    ui->focusLE->setText(focus);
}

void Binder::setLanguageEn()
{
    setWindowTitle("Config Binder Parameters");
    ui->genButton->setText("Generate");
    ui->defaultButton->setText("Default");
    ui->quitButton->setText("Quit");
    ui->lb1->setText("Parameter Name");
    ui->lb2->setText("Name");
}

void Binder::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("Binder 설정 값 변경"));
    ui->genButton->setText(codec->toUnicode("변경"));
    ui->defaultButton->setText(codec->toUnicode("기본값"));
    ui->quitButton->setText(codec->toUnicode("취소"));
    ui->lb1->setText(codec->toUnicode("설정"));
    ui->lb2->setText(codec->toUnicode("값"));
}

void Binder::defaultButtonClicked()
{
    ui->dspaceLE->setText("2");
    /*
    ui->grdlat1LE->setText("37");
    ui->grdlat2LE->setText("40");
    ui->grdlon1LE->setText("126");
    ui->grdlon2LE->setText("129");
    */
    ui->grdz1LE->setText("0.0");
    ui->grdz2LE->setText("10.0");
    ui->rstackLE->setText("100");
    ui->tstackLE->setText("3.0");
    ui->stackLE->setText("20");
    ui->threshLE->setText("4");
    ui->focusLE->setText("100");
}

void Binder::genButtonClicked()
{
    QFile file;
    /* generate binder_ew.d file. */
    file.setFileName( c.PARAMSDIR + "/binder_ew.d");

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
        stream << "lay   -2.0    5.61" << "\n";
        stream << "lay    0.0    6.08" << "\n";
        stream << "lay   20.0    6.50" << "\n";
        stream << "lay   32.0    7.80" << "\n\n";
        stream << "# Set FIFO lengths" << "\n";
        stream << "#-----------------" << "\n";
        stream << "pick_fifo_length  1000   # optional: default=1000" << "\n";
        stream << "quake_fifo_length  100   # optional: default=100" << "\n\n";
        stream << "# Define association grid, set stacking parameters." << "\n";
        stream << "#--------------------------------------------------" << "\n";
        stream << "dspace   " << ui->dspaceLE->text() << "\n";
        stream << "grdlat   " << ui->grdlat1LE->text() << "    " << ui->grdlat2LE->text() << "\n";
        stream << "grdlon   " << ui->grdlon1LE->text() << "   " << ui->grdlon2LE->text() << "\n";
        stream << "grdz     " << ui->grdz1LE->text() << "    " << ui->grdz2LE->text() << "\n\n";
        stream << "rstack   " << ui->rstackLE->text() << "\n";
        stream << "tstack   " << ui->tstackLE->text() << "\n";
        stream << "stack    " << ui->stackLE->text() << "\n";
        stream << "thresh   " << ui->threshLE->text() << "\n";
        stream << "focus    " << ui->focusLE->text() << "\n";
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

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Completed file generation.");
    else msgBox.setText(codec->toUnicode("설정 값 변경 완료."));
    msgBox.exec();

    accept();
}
