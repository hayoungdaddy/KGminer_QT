#include "maketnk.h"
#include "ui_maketnk.h"

MakeTnk::MakeTnk(QWidget *parent) :
    QDialog(parent),
    MakeTnk_ui(new Ui::MakeTnk)
{
    MakeTnk_ui->setupUi(this);
    //codec = QTextCodec::codecForName( "utf8" );

    connect(MakeTnk_ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(MakeTnk_ui->genButton, SIGNAL(clicked()), this, SLOT(genButtonClicked()));
    connect(MakeTnk_ui->fileBrowButton, SIGNAL(clicked()), this, SLOT(openFileBrowser()));
    //connect(MakeTnk_ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(selectFiles(int)));
    //connect(MakeTnk_ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(checkOption(bool)));
}

MakeTnk::~MakeTnk()
{
    delete MakeTnk_ui;
}

void MakeTnk::setup()
{
         QDir dir;
         dir.setPath("/opt/KGminer/params/TNK");
         QStringList files;
         QString fileName = "*.tnk";
         files = dir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

         //MakeTnk_ui->listWidget->addItems(files);
}

void MakeTnk::openFileBrowser()
{
    MakeTnk_ui->listWidget->clear();
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select data files", "/home", "*.*");
    MakeTnk_ui->listWidget->addItems(fileNames);
}

void MakeTnk::genButtonClicked()
{
    if(MakeTnk_ui->listWidget->count() == 0 || MakeTnk_ui->tankFileNameLE->text() == "")
    {
        QMessageBox::warning( this, "Information", "Please input information\n");
        return;
    }
    int count = MakeTnk_ui->listWidget->count();
    QStringList fileNames;
    for(int i=0;i<count;i++)
    {
        fileNames << MakeTnk_ui->listWidget->item(i)->text();
    }
    //qDebug() << count << " " << fileNames;

    QString tankFileName = MakeTnk_ui->tankFileNameLE->text() + ".tnk";
    QDir dir;
    dir.setPath("/opt/KGminer/params/TNK");
    QStringList files;
    QString fileName = "*.tnk";
    files = dir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    for(int i=0;i<files.count();i++)
    {
        if(tankFileName == files[i])
        {
            QMessageBox::warning( this, "Information", "Duplicate TANK file name.\nPlease use other name.");
            return;
        }
    }

    QString cmd;
    QFile file;
    file.setFileName("/opt/KGminer/params/makeTnk.d");

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        for(int i=0;i<count;i++)
        {
            QString cmd;
            cmd = "scart -I " + fileNames[i] + " /data/mseed";
            system(cmd.toLatin1().data());
            stream << fileNames[i] << "\n";
        }
        file.close() ;
    }

    if(MakeTnk_ui->formatCB->currentText() == "sac")
    {
        cmd = "/opt/KGminer/script/makeTnk.sh sac " + tankFileName;
        qDebug() << cmd;
        system(cmd.toLatin1().data());
    }
    else
    {
        cmd = "/opt/KGminer/script/makeTnk.sh mseed " + tankFileName;
        system(cmd.toLatin1().data());
    }

    accept();
}
