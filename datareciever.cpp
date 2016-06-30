#include "datareciever.h"
#include "ui_datareciever.h"

DataReciever::DataReciever(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataReciever)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    c = cfg;

    QDate today = QDate::currentDate();
    today = today.addDays(-1);

    QDateTime now;
    now.setTimeSpec(Qt::UTC);
    now.setDate(today);

    ui->startTime->setDateTime(now);
    today = today.addDays(+1);
    now.setDate(today);
    ui->endTime->setDateTime(now);

    ui->viewwaveform->setEnabled(false);

    connect(ui->extractButton, SIGNAL(clicked()), this, SLOT(extracButtonClicked()));
    connect(ui->simulButton, SIGNAL(clicked()), this ,SLOT(simulButtonClicked()));
    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(ui->viewwaveform, SIGNAL(clicked()), this, SLOT(viewWaveForm()));
}

DataReciever::~DataReciever()
{
    delete ui;
}

void DataReciever::setLanguageEn()
{
    setWindowTitle("Extract data");
    ui->stLB->setText("StartTime (GMT)");
    ui->etLB->setText("EndTime (GMT)");
    ui->extractButton->setText("Extract data");
    ui->simulButton->setText("Select for a simulation");
    ui->viewwaveform->setText("View waveform");
    ui->resetButton->setText("Reset time");
    ui->quitButton->setText("Quit");
}

void DataReciever::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("데이터 추출"));
    ui->stLB->setText(codec->toUnicode("시작시간 (GMT)"));
    ui->etLB->setText(codec->toUnicode("종료시간 (GMT)"));
    ui->extractButton->setText(codec->toUnicode("데이터 추출"));
    ui->simulButton->setText(codec->toUnicode("실시간 시뮬레이션"));
    ui->viewwaveform->setText(codec->toUnicode("파형보기"));
    ui->resetButton->setText(codec->toUnicode("시간리셋"));
    ui->quitButton->setText(codec->toUnicode("종료"));
}

void DataReciever::extracButtonClicked()
{
    /* gen tankplayer.d file */
    QFile file;
    file.setFileName(c.PARAMSDIR + "/tankplayer.d");

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        stream << "# tankplayer config file for playing waveform files" << "\n";
        stream << "RingName      SCN_RING        # play waveforms into this ring" << "\n";
        stream << "MyModuleId    MOD_ADSEND     # as this module id" << "\n";
        stream << "PlayMsgType   TYPE_TRACEBUF    # msg type to read from file" << "\n";
        stream << "LogFile       1                # 0=no log; 1=keep log file" << "\n";
        stream << "HeartBeatInt  30               # seconds between heartbeats" << "\n";
        stream << "Pause         10               # seconds to pause between wavefiles" << "\n";
        stream << "StartUpDelay  5               # seconds to wait before playing 1st file" << "\n";
        stream << "ScreenMsg     1                # (optional) if non-zero, informational messages will be" << "\n";
        stream << "                               #   written to the screen as data is played" << "\n";
        stream << "InterMessageDelayMillisecs 1" << "\n";
        stream << "Debug         1                # for verbosity" << "\n";
        stream << "# List of files to play (up to 50 allowed):" << "\n";
        stream << "WaveFile        DATA/TANK" << "\n";
        stream << "IgnoreTBVersionNumbers 0" << "\n";

        file.close();
    }

    extract();
}

void DataReciever::simulButtonClicked()
{
    /* gen tankplayer.d file */
    QFile file;
    /* generate binder_ew.d file. */
    file.setFileName(c.PARAMSDIR + "/tankplayer.d");

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        stream << "# tankplayer config file for playing waveform files" << "\n";
        stream << "RingName      SCN_RING        # play waveforms into this ring" << "\n";
        stream << "MyModuleId    MOD_ADSEND     # as this module id" << "\n";
        stream << "PlayMsgType   TYPE_TRACEBUF    # msg type to read from file" << "\n";
        stream << "LogFile       1                # 0=no log; 1=keep log file" << "\n";
        stream << "HeartBeatInt  30               # seconds between heartbeats" << "\n";
        stream << "Pause         10               # seconds to pause between wavefiles" << "\n";
        stream << "StartUpDelay  5               # seconds to wait before playing 1st file" << "\n";
        stream << "ScreenMsg     1                # (optional) if non-zero, informational messages will be" << "\n";
        stream << "                               #   written to the screen as data is played" << "\n";
        stream << "SendLate      5" << "\n";
        stream << "Debug         1                # for verbosity" << "\n";
        stream << "# List of files to play (up to 50 allowed):" << "\n";
        stream << "WaveFile        DATA/TANK" << "\n";
        stream << "IgnoreTBVersionNumbers 0" << "\n";

        file.close();
    }

    extract();
}

void DataReciever::reset()
{
    QDate today = QDate::currentDate();
    today = today.addDays(-1);

    QDateTime now;
    now.setTimeSpec(Qt::UTC);
    now.setDate(today);

    ui->startTime->setDateTime(now);
    today = today.addDays(+1);
    now.setDate(today);
    ui->endTime->setDateTime(now);

    ui->viewwaveform->setEnabled(false);
}

void DataReciever::extract()
{
    QDateTime st( ui->startTime->dateTime() ) ;
    st.setTimeSpec(Qt::UTC);
    QDateTime et( ui->endTime->dateTime() ) ;
    et.setTimeSpec(Qt::UTC);

    int diff = 0;
    diff = (st.secsTo( et) );

    QString scn;
    QString year = st.toString("yyyy");
    QString startString = st.toString("yyyy/MM/dd,hh:mm:ss");

    //qDebug() << QString::number(start) + " " + QString::number(end) + " " + QString::number(diff);

    QFile file;
    file.setFileName(c.PARAMSDIR + "/sta.info");
    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        while(!stream.atEnd())
        {
            line = stream.readLine();

            if(line.startsWith("#") || line.isNull() || line.startsWith("[") || line.startsWith("Desc") || line.startsWith("File"))
                continue;
            else
            {
                // /2015/TS/G01/HGZ.D
                scn = c.MSEEDDIR + "/" + year + "/" + line.section(' ', 3, 3) + "/" + line.section(' ', 0, 0) + "/" + line.section(' ', 1, 1) + ".D/*";
                QString cmd;
                cmd = "qmerge -T -f " + startString + " -s " + QString::number(diff) + " -b 512 -o " + c.PARAMSDIR + "/DATA/" + line.section(' ', 0, 0) + ".mseed" + " " + scn;
                //qDebug() << cmd;
                system(cmd.toLatin1().data());

                cmd = "ms2tank " + c.PARAMSDIR + "/DATA/" + line.section(' ', 0, 0) + ".mseed >> " + c.PARAMSDIR + "/DATA/tbuf";
                system(cmd.toLatin1().data());
            }
        }

        file.close();     
    }

    QString cmd;
    cmd = "remux_tbuf " + c.PARAMSDIR + "/DATA/tbuf " + c.PARAMSDIR + "/DATA/TANK";
    system(cmd.toLatin1().data());
    cmd = "rm " + c.PARAMSDIR + "/DATA/tbuf";
    system(cmd.toLatin1().data());

    /* reporting */
    file.setFileName(c.PARAMSDIR + "/DATA/TANK");
    if(file.open(QIODevice::ReadOnly))
    {
        if(file.size() == 0)
        {
            if(!korean) QMessageBox::warning( this, "Information", "There is no data.\nPlease check raw data.");
            else QMessageBox::warning( this, "Information", codec->toUnicode("데이터가 없습니다.\n원시데이터를 확인하여 주시기 바랍니다."));
            return;             
        }
        else
        {
            QMessageBox msgBox;
            if(!korean) msgBox.setText("The data is ready.\nPlease check this data using <View waveform> button.");
            else msgBox.setText("데이터가 준비되었습니다.\n<파형보기> 버튼을 이용하여 데이터를 확인할 수 있습니다.");
            msgBox.exec();
            ui->viewwaveform->setEnabled(true);
        }
        file.close();
    }
}

void DataReciever::viewWaveForm()
{
    QString cmd = c.SCRIPTDIR + "/runGeo.sh >> /dev/null 2>&1 &";
    system(cmd.toLatin1().data());
}
