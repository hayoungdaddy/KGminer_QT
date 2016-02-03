#include "importdata.h"
#include "ui_importdata.h"

ImportData::ImportData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportData)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean=false;

    connect(ui->fileBrowButton, SIGNAL(clicked()), this, SLOT(openFileBrowser()));
    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->genButton, SIGNAL(clicked()), this, SLOT(genButtonClicked()));    
    connect(ui->viewwaveformButton, SIGNAL(clicked()), this, SLOT(viewWaveformClicked()));
    connect(ui->tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(viewWaveClicked(int, int)));
}

ImportData::~ImportData()
{
    delete ui;
}

void ImportData::setup()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnWidth(0, 360);
    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->setColumnWidth(2, 200);
    ui->tableWidget->setColumnWidth(3, 190);
    ui->tableWidget->setColumnWidth(4, 180);
    ui->pb->setRange(0, 100);
    ui->pb->setValue(0);
}

void ImportData::setLanguageEn()
{
    setWindowTitle("Import miniseed data");
    ui->quitButton->setText("Quit");
    ui->genButton->setText("Import");
    ui->fileBrowButton->setText("File browser");
    ui->viewwaveformButton->setText("View waveform");
}

void ImportData::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("miniseed 데이터 입력"));
    ui->quitButton->setText(codec->toUnicode("종료"));
    ui->genButton->setText(codec->toUnicode("자료 입력"));
    ui->fileBrowButton->setText(codec->toUnicode("파일 브라우져"));
    ui->viewwaveformButton->setText(codec->toUnicode("파형 보기"));
}

void ImportData::openFileBrowser()
{
    QStringList fileNames, filteredFilenames;
    if(!korean)
        fileNames = QFileDialog::getOpenFileNames(this, "Select data files", "/media/sf_KGminer", "*.*");
    else
        fileNames = QFileDialog::getOpenFileNames(this, codec->toUnicode("입력 데이터를 선택하십시오. (miniseed format only)"), "/home", "*.*");

    ui->tableWidget->setRowCount(0);

    int count = fileNames.count();

    if(count == 0)
        return;

    for(int i=0;i<count;i++)
    {
        QStringList temp = fileNames[i].split("/");
        int last = temp.count();
        QStringList temp2 = temp[last-1].split(".");
        int last2 = temp2.count();
        if(temp[last-1].section('.',2,2).startsWith("VM") || temp[last-1].section('.',2,2).startsWith("LH") ||
                temp[last-1].section('.',2,2).startsWith("VFP") || temp[last-1].section('.',2,2).startsWith("LC") ||
                temp[last-1].section('.',2,2).startsWith("OCF") || temp[last-1].section('.',2,2).startsWith("ACE") ||
                temp[last-1].section('.',2,2).startsWith("LOG") || temp[last-1].section('.',2,2).startsWith("LG") ||
                temp[last-1].section('.',2,2).startsWith("HA") || temp[last-1].section('.',2,2).startsWith("BA"))
        {
            continue;
        }
        else if(temp2[last2-1].startsWith("sac"))
            continue;
        else
        {
            filteredFilenames << fileNames[i];
        }
    }

    count = filteredFilenames.count();

    ui->pb->setRange(0, count);
    ui->pb->setValue(0);

    for(int i=0;i<count;i++)
    {
        ui->pb->setValue(i);
        ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(filteredFilenames[i]));
        ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem("View"));
        ui->tableWidget->item(i, 4)->setTextAlignment(Qt::AlignCenter);

        QString cmd;
        cmd = SCRIPTDIR + "/getDataStatus.sh " + filteredFilenames[i];
        system(cmd.toLatin1().data());

        QFile file;
        file.setFileName(TMPDIR + "/msview.tmp");

        if(file.open( QIODevice::ReadOnly ))
        {
            QTextStream stream(&file);
            QString line, _line;
            int x = 0;

            while(!stream.atEnd())
            {
                line = stream.readLine();
                if(x==0)
                    scnl << line;
                x++;
                _line = line.simplified();

                if(_line.startsWith("TIME"))
                {
                    // TIME-2015/12/02 08:00:00.000-2015/12/02 09:00:00.000
                    // GAP-720000-720000
                    ui->tableWidget->setItem(i, 1, new QTableWidgetItem(_line.section('-',1,1)));
                    ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget->setItem(i, 2, new QTableWidgetItem(_line.section('-',2,2)));
                    ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignCenter);
                }

                if(_line.startsWith("GAP"))
                {
                    QProgressBar *bar = new QProgressBar();
                    bar->setRange(0, 100);
                    double max = (_line.section('-',2,2).toDouble() * 100) / _line.section('-',1,1).section('.',0,0).toDouble();
                    bar->setValue(int(max));
                    bar->setTextVisible(true);
                    bar->setEnabled(false);
                    ui->tableWidget->setCellWidget(i, 3, bar);
                    break;
                }
            }
            file.close();
        }
    }
    ui->pb->setValue(count);
}

void ImportData::viewWaveformClicked()
{
    QFile file;
    file.setFileName(TMPDIR + "/runGeoFull.file");
    if(file.open( QIODevice::WriteOnly ))
    {
        QTextStream stream(&file);

        for(int i=0;i<ui->tableWidget->rowCount();i++)
        {
            stream << ui->tableWidget->item(i, 0)->text() << "\n";
        }

        file.close();
    }

    QString program = SCRIPTDIR + "/runGeoFull.sh";
    QStringList arguments;
    arguments << file.fileName();
    QProcess *process = new QProcess;

    process->start(program, arguments);
}

void ImportData::viewWaveClicked(int row, int col)
{
    if(col == 4)
    {
        QString program = SCRIPTDIR + "/runGeoSingle.sh";
        QStringList arguments;
        arguments << ui->tableWidget->item(row, 0)->text();
        QProcess *process = new QProcess;
        process->start(program, arguments);
    }
}

void ImportData::genButtonClicked()
{
    int count = ui->tableWidget->rowCount();

    if(count == 0)
    {
        if(!korean) QMessageBox::warning( this, "Information", "Choose data files using <File Browser> button.\n");
        else QMessageBox::warning( this, codec->toUnicode("정보"), codec->toUnicode("<파일 브라우져> 버튼을 이용하여 데이터를 선택하십시오.\n"));
        return;
    }

    ui->pb->setRange(0, count);
    ui->pb->setValue(0);

    for(int i=0;i<count;i++)
    {
        ui->pb->setValue(i);

        /*
        QString program = BINDIR + "/scart";
        QStringList arguments;
        arguments << "-I" << ui->tableWidget->item(i, 0)->text() << MSEEDDIR;
        QProcess *process = new QProcess;

        qDebug() << program ;
        qDebug() << arguments;

        process->start(program, arguments);
        */

        QString cmd ;
        cmd = BINDIR +"/scart -I " + ui->tableWidget->item(i,0)->text() + " " + MSEEDDIR;
        system(cmd.toLatin1().data());
    }

    //YYYY/STA/HGZ.D/TS.G01.00.HGZ.D.2016.015
    //cmd = BINDIR + "/MSFiles2Sorted1File " +
    for(int i=0;i<scnl.count();i++)
    {
        //TS_HSA_00_HGZ, 000000, D, 512, 602 samples, 100 Hz, 2016,019,22:00:00.000000
        QString net,sta,loc,chan,year,jdate;
        net = scnl[i].section(',',0,0).section('_',0,0);
        sta = scnl[i].section(',',0,0).section('_',1,1);
        loc = scnl[i].section(',',0,0).section('_',2,2);
        chan = scnl[i].section(',',0,0).section('_',3,3);
        year = scnl[i].section(' ',8,8).section(',',0,0);
        jdate = scnl[i].section(' ',8,8).section(',',1,1);

        QString unsort = MSEEDDIR + "/" + year + "/" + net + "/" + sta + "/" + chan + ".D/" + net + "." + sta + "." + loc + "." + chan + ".D." + year + "." + jdate;
        QString sort = TMPDIR + "/tempdata";
        QString cmd = BINDIR + "/MSFiles2Sorted1File " + unsort + " " + sort;
        system(cmd.toLatin1().data());
        cmd = "mv " + sort + " " + unsort;
        system(cmd.toLatin1().data());

        //qDebug() << cmd;
    }

    ui->pb->setValue(count);

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Completed data Importing.");
    else msgBox.setText(codec->toUnicode("데이터 입력을 완료하였습니다."));
    msgBox.exec();
}

