#include "selectstafile.h"
#include "ui_selectstafile.h"

// _event == true   :  using making new event mode
// _event == false  :  normal mode
SelectStaFile::SelectStaFile(CFG cfg, bool _korean, bool _event, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectStaFile)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;
    event = _event;
    c = cfg;

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    ui->listWidget->clear();
    ui->listWidgetDesc->clear();
    QDir dir;
    dir.setPath(c.PARAMSDIR + "/staInfo");
    QStringList files;
    QString fileName = "*.sta";
    files = dir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    ui->listWidget->addItems(files);

    QFile file;
    for(int i=0;i<files.count();i++)
    {
        file.setFileName(c.PARAMSDIR + "/staInfo/"+files.at(i));

        if( file.open( QIODevice::ReadOnly ) )
        {
            QTextStream stream(&file);
            QString line;

            line = stream.readLine();
            line = stream.readLine();
            line = line.section(':', 1, 1);

            ui->listWidgetDesc->addItem(line);

            file.close();
        }
    }

    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->selectButton, SIGNAL(clicked()), this, SLOT(selectButtonClicked()));
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(selectFiles(int)));
}

SelectStaFile::~SelectStaFile()
{
    delete ui;
}

void SelectStaFile::setLanguageEn()
{
    setWindowTitle("Select a Station File");
    ui->selectButton->setText("Select");
    ui->quitButton->setText("Quit");
    ui->lb1->setText("File Name");
    ui->lb2->setText("Description");
}

void SelectStaFile::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("관측소 정보 파일 선택"));
    ui->selectButton->setText(codec->toUnicode("확인"));
    ui->quitButton->setText(codec->toUnicode("종료"));
    ui->lb1->setText(codec->toUnicode("파일명"));
    ui->lb2->setText(codec->toUnicode("설명"));
}

void SelectStaFile::selectFiles(int row)
{
    ui->stafileLE->setText(ui->listWidget->item(row)->text());
}

void SelectStaFile::selectButtonClicked()
{
    QString staFileName = ui->stafileLE->text();
    QString cmd;
    if(event)
    {
        cmd = "cp " + c.PARAMSDIR + "/staInfo/" + staFileName + " " + c.EVENTDIR + "/NEWEVENT/sta.info";
        system(cmd.toLatin1().data());
        emit sendSignaltoDataExtractor();
    }
    else
    {
        cmd = "cp " + c.PARAMSDIR + "/staInfo/" + staFileName + " " + c.PARAMSDIR + "/sta.info";
        system(cmd.toLatin1().data());
    }

    STAFILE stafile;

    FileGenerator *gen = new FileGenerator;

    if(!event)
    {
        stafile = gen->makeSTAFILE(c, false);
        gen->pick_ew_gen(c, stafile);
        gen->pick_FP_gen(c, stafile);
        gen->hinv_gen(c, stafile);
        gen->tanklist_gen(c, stafile);
        gen->binder_gen(c, stafile);
        gen->ew2mseed_gen(c, stafile);
        gen->nlloc_gen(false, c, stafile);
    }
    else
    {
        stafile = gen->makeSTAFILE(c, true);
        gen->nlloc_gen(true, c, stafile);
    }

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Staions Infomation loaded.");
    else msgBox.setText(codec->toUnicode("관측소 정보 로딩 완료."));
    msgBox.exec();
    accept();
}
