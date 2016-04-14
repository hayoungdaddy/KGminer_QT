#include "viewlog.h"
#include "ui_viewlog.h"

ViewLog::ViewLog(CFG cfg, QWidget *parent) :
    QDialog(parent),
    ViewLog_ui(new Ui::ViewLog)
{
    ViewLog_ui->setupUi(this);
    //codec = QTextCodec::codecForName( "utf8" );

    c = cfg;

    connect(ViewLog_ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ViewLog_ui->viewButton, SIGNAL(clicked()), this, SLOT(viewLogButtonClicked()));
    connect(ViewLog_ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(selectFiles(int)));
}

ViewLog::~ViewLog()
{
    delete ViewLog_ui;
}

void ViewLog::setup(QString logName)
{
    ViewLog_ui->listWidget->clear();
    QDir dir;
    dir.setPath("/opt/KGminer/logs");
    QStringList files;
    QString fileName = logName + "*";
    files = dir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    ViewLog_ui->listWidget->addItems(files);
}

void ViewLog::selectFiles(int row)
{
    ViewLog_ui->lineEdit->setText(ViewLog_ui->listWidget->item(row)->text());
}

void ViewLog::viewLogButtonClicked()
{
    QString fn = "/opt/KGminer/logs/" + ViewLog_ui->lineEdit->text();
    QFile *logFile = new QFile( fn );
    if(logFile->open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(logFile);
        QString line;
        ViewLog_ui->textEdit->clear();

        while(!stream.atEnd())
        {
            line = stream.readLine();

            ViewLog_ui->textEdit->append(line);
        }
    }

    logFile->close();

}
