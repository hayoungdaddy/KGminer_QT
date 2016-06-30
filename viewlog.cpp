#include "viewlog.h"
#include "ui_viewlog.h"

ViewLog::ViewLog(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewLog)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    c = cfg;

    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->viewButton, SIGNAL(clicked()), this, SLOT(viewLogButtonClicked()));
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(selectFiles(int)));
}

ViewLog::~ViewLog()
{
    delete ui;
}

void ViewLog::setup(QString logName)
{
    ui->listWidget->clear();
    QDir dir;
    dir.setPath(c.LOGDIR);
    QStringList files;
    QString fileName = logName + "*";
    files = dir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    ui->listWidget->addItems(files);
}

void ViewLog::setLanguageEn()
{
    setWindowTitle("Log File Viewer");
    ui->viewButton->setText("View");
    ui->quitButton->setText("Quit");
    ui->valueLB->setText("Log File Lists");
    ui->valueLB_2->setText("Selected a File");
}

void ViewLog::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("로그 파일 뷰어"));
    ui->viewButton->setText(codec->toUnicode("보기"));
    ui->quitButton->setText(codec->toUnicode("취소"));
    ui->valueLB->setText(codec->toUnicode("로그 파일 리스트"));
    ui->valueLB_2->setText(codec->toUnicode("선택한 파일"));
}

void ViewLog::selectFiles(int row)
{
    ui->lineEdit->setText(ui->listWidget->item(row)->text());
}

void ViewLog::viewLogButtonClicked()
{
    QString fn = "/opt/KGminer/logs/" + ui->lineEdit->text();
    QFile *logFile = new QFile( fn );
    if(logFile->open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(logFile);
        QString line;
        ui->textEdit->clear();

        while(!stream.atEnd())
        {
            line = stream.readLine();

            ui->textEdit->append(line);
        }
    }

    logFile->close();

}
