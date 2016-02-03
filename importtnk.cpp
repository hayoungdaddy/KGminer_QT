#include "importtnk.h"
#include "ui_importtnk.h"

ImportTnk::ImportTnk(QWidget *parent) :
    QDialog(parent),
    ImportTnk_ui(new Ui::ImportTnk)
{
    ImportTnk_ui->setupUi(this);
    //codec = QTextCodec::codecForName( "utf8" );

    connect(ImportTnk_ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ImportTnk_ui->replayButton, SIGNAL(clicked()), this, SLOT(replayButtonClicked()));
    connect(ImportTnk_ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(selectFiles(int)));
    connect(ImportTnk_ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(checkOption(bool)));
}

ImportTnk::~ImportTnk()
{
    delete ImportTnk_ui;
}

void ImportTnk::setup()
{
         QDir dir;
         dir.setPath("/opt/KGminer/params/TNK");
         QStringList files;
         QString fileName = "*.tnk";
         files = dir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

         ImportTnk_ui->listWidget->addItems(files);
}

void ImportTnk::checkOption(bool b)
{
    if(b==true)
        ImportTnk_ui->secLE->setEnabled(true);
    else
        ImportTnk_ui->secLE->setEnabled(false);
}

void ImportTnk::selectFiles(int row)
{
    ImportTnk_ui->tankfileLE->setText(ImportTnk_ui->listWidget->item(row)->text());
}

void ImportTnk::replayButtonClicked()
{
    /* gen tankplayer.d file */
    QFile file;
    /* generate binder_ew.d file. */
    file.setFileName("/opt/KGminer/params/tankplayer.d");

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
        stream << "StartUpDelay  10               # seconds to wait before playing 1st file" << "\n";
        stream << "ScreenMsg     1                # (optional) if non-zero, informational messages will be" << "\n";
        stream << "                               #   written to the screen as data is played" << "\n";

        if(ImportTnk_ui->checkBox->isChecked())
        {
            stream << "SendLate      " << ImportTnk_ui->secLE->text() << "\n";
        }
        else
        {
            stream << "InterMessageDelayMillisecs 1" << "\n";
        }
        stream << "Debug         1                # for verbosity" << "\n";
        stream << "# List of files to play (up to 50 allowed):" << "\n";
        stream << "WaveFile        TNK/" << ImportTnk_ui->tankfileLE->text() << "\n";
        stream << "IgnoreTBVersionNumbers 0" << "\n";

        file.close();
    }

    QString cmd = "";

    file.setFileName("/opt/KGminer/script/pid/tankplayer.pid");

    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        line = stream.readLine();

        cmd = "/opt/KGminer/bin/restart " + line;
        system(cmd.toLatin1().data());

        file.close();
    }

    accept();
}
