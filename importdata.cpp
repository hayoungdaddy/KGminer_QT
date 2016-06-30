#include "importdata.h"
#include "ui_importdata.h"

ImportData::ImportData(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportData)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;

    c = cfg;

    ui->status->setText("idle...");
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnWidth(0, 360);
    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->setColumnWidth(2, 200);
    ui->tableWidget->setColumnWidth(3, 190);
    ui->tableWidget->setColumnWidth(4, 180);

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    ui->pb->setRange(0, 100);
    ui->pb->setValue(0);

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

void ImportData::setLanguageEn()
{
    setWindowTitle("Import miniseed data");
    ui->quitButton->setText("Quit");
    ui->genButton->setText("Import");
    ui->fileBrowButton->setText("File browser");
    ui->viewwaveformButton->setText("View waveform");
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("File Name"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Start Time"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("End Time"));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("Data Ability"));
    ui->tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem("View Waveform"));
}

void ImportData::setLanguageKo()
{  
    setWindowTitle(codec->toUnicode("miniseed 데이터 입력"));
    ui->quitButton->setText(codec->toUnicode("종료"));
    ui->genButton->setText(codec->toUnicode("자료 입력"));
    ui->fileBrowButton->setText(codec->toUnicode("파일 브라우져"));
    ui->viewwaveformButton->setText(codec->toUnicode("파형 보기"));
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(codec->toUnicode("파일명")));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(codec->toUnicode("시작시간")));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(codec->toUnicode("끝시간")));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(codec->toUnicode("데이터 가용")));
    ui->tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem(codec->toUnicode("파형보기")));
}

void ImportData::openFileBrowser()
{
    QStringList fileNames, filteredFilenames;


    if(!korean)
        fileNames = QFileDialog::getOpenFileNames(this, "Select data files", "/media/sf_KGminer", "*.*");
    else
        fileNames = QFileDialog::getOpenFileNames(this, codec->toUnicode("입력 데이터를 선택하십시오. (miniseed format only)"), "/media/sf_KGminer", "*.*");


    /*
    if(!korean)
        fileNames = QFileDialog::getOpenFileNames(this, "Select data files", "/home/yyw/data", "*.*");
    else
        fileNames = QFileDialog::getOpenFileNames(this, codec->toUnicode("입력 데이터를 선택하십시오. (miniseed format only)"), "/home/yyw/data", "*.*");
        */

    ui->tableWidget->setRowCount(0);

    int count = fileNames.count();

    if(count == 0)
        return;

    QRegExp rx("\\b(cpu|deg|hum|lce|lcq|vec|vep|vvb|vvx|C4|C8|C12|C16|ACE|LOG|VFP|VMU|VMV|VMW|LCQ|LCE|LHE|LHN|LHZ|LGE|LGN|LGZ|OCF|HAZ|HAN|HAE|BAZ|BAN|BAE)\\b");

    for(int i=0;i<count;i++)
    {
        QStringList temp = fileNames[i].split("/");
        int last = temp.count();
        QString fileName = temp[last-1];

        int x = rx.indexIn(fileName);

        if(x == -1)
        {
            //qDebug() << "not match";
            filteredFilenames << fileNames[i];
        }
        else
        {
            continue;
            //qDebug() << "match";
        }
    }

    if(filteredFilenames.count() == 0)
        return;
    else
    {
        count = filteredFilenames.count();

        ui->pb->setRange(0, count);
        ui->pb->setValue(0);

        scnl.clear();

        for(int i=0;i<count;i++)
        {
            ui->pb->setValue(i);
            ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(filteredFilenames[i]));
            //ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i, 4, new QTableWidgetItem("View"));
            ui->tableWidget->item(i, 4)->setTextAlignment(Qt::AlignCenter);

            readMseedFile(filteredFilenames[i], i);

        }
        ui->pb->setValue(count);
    }
}

void ImportData::viewWaveformClicked()
{
    QFile file;
    file.setFileName(c.TMPDIR + "/runGeoFull.file");
    if(file.open( QIODevice::WriteOnly ))
    {
        QTextStream stream(&file);

        for(int i=0;i<ui->tableWidget->rowCount();i++)
        {
            stream << ui->tableWidget->item(i, 0)->text() << "\n";
        }

        file.close();
    }

    QString program = c.SCRIPTDIR + "/runGeoFull.sh";
    QStringList arguments;
    arguments << file.fileName();
    QProcess *process = new QProcess;

    process->start(program, arguments);
}

void ImportData::viewWaveClicked(int row, int col)
{
    if(col == 4)
    {
        QString program = c.SCRIPTDIR + "/runGeoSingle.sh";
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

    ui->status->setText("Importing...");
    for(int i=0;i<count;i++)
    {

        ui->pb->setValue(i);

        if(ui->tableWidget->item(i,1)->text().startsWith("Unvalidated data"))
        {
            qDebug() << ui->tableWidget->item(i,0)->text();
        }
        else
        {
            QString cmd ;
            cmd = c.BINDIR +"/scart -s -I " + ui->tableWidget->item(i,0)->text() + " " + c.MSEEDDIR;
            system(cmd.toLatin1().data());
        }
    }

    ui->status->setText("Sorting...");
    scnl.removeDuplicates();

    /*
    for(int i=0;i<scnl.count();i++)
    {
        //qDebug() << scnl[i];
        QString sort = c.TMPDIR + "/unsort";
        QString cmd = c.BINDIR + "/MSFiles2Sorted1File " + scnl[i] + " " + sort;
        system(cmd.toLatin1().data());
        cmd = "mv " + sort + " " + scnl[i];
        system(cmd.toLatin1().data());
    }
    */

    ui->pb->setValue(count);

    ui->status->setText("Idle...");

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Completed data Importing.");
    else msgBox.setText(codec->toUnicode("데이터 입력을 완료하였습니다."));
    msgBox.exec();

}

void ImportData::readMseedFile(QString fileName, int i)
{
    MSTraceGroup *mstg = 0;
    MSRecord *msr = 0;
    int retcode;
    char *fn;
    QByteArray ba;
    int verbose = 0;
    int reclen = -1;
    int totalrecs = 0;
    double delta;

    mstg = mst_initgroup(mstg);

    ba = fileName.toLatin1();
    fn = ba.data();

    while( (retcode = ms_readmsr(&msr, fn, reclen, NULL, NULL, 1, 1, verbose-1)) == MS_NOERROR )
    {
        mst_addmsrtogroup(mstg, msr, 1, -1.0, -1.0);
        totalrecs++;
    }

    if(retcode != MS_ENDOFFILE)
    {
        //qDebug() << "Error reading";
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem( "Unvalidated data" ));
        ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem( "Unvalidated data" ));
        ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem( "Unvalidated data" ));
        ui->tableWidget->item(i, 3)->setTextAlignment(Qt::AlignCenter);
        return;
    }

    QDateTime start;
    QDateTime end;

    delta = 1 / mstg->traces->samprate;

    start.setTimeSpec(Qt::UTC);
    end.setTimeSpec(Qt::UTC);
    start.setTime_t( (double) MS_HPTIME2EPOCH(mstg->traces->starttime) );
    end.setTime_t( (double) MS_HPTIME2EPOCH(mstg->traces->endtime) + delta );

    QDate date;
    date = start.date();

    scnl << c.MSEEDDIR + "/" + start.toString("yyyy") + "/" + mstg->traces->network + "/" + mstg->traces->station + "/" + mstg->traces->channel + ".D/"
            + mstg->traces->network + "." + mstg->traces->station + "." + mstg->traces->location
            + "." + mstg->traces->channel + ".D." + start.toString("yyyy") + "." + QString::number(date.dayOfYear());

    /*
    qDebug() << c.MSEEDDIR + "/" + start.toString("yyyy") + "/" + mstg->traces->network + "/" + mstg->traces->station + "/" + mstg->traces->channel + ".D/"
                + mstg->traces->network + "." + mstg->traces->station + "." + mstg->traces->location
                + "." + mstg->traces->channel + ".D." + start.toString("yyyy") + "." + QString::number(date.dayOfYear());
                */

    //qDebug() << QString::number(mstg->traces->samplecnt);

    double diff = ((double) MS_HPTIME2EPOCH(mstg->traces->endtime) + delta) - (double) MS_HPTIME2EPOCH(mstg->traces->starttime);
    double totalsamples = diff * mstg->traces->samprate;

    QProgressBar *bar = new QProgressBar();
    bar->setRange(0, 100);
    double max = ( mstg->traces->samplecnt * 100) / totalsamples;
    bar->setValue(int(max));
    bar->setTextVisible(true);
    bar->setEnabled(false);
    ui->tableWidget->setCellWidget(i, 3, bar);

    ui->tableWidget->setItem(i, 1, new QTableWidgetItem( start.toString("yyyy-MM-dd hh:mm:ss") ));
    ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(i, 2, new QTableWidgetItem( end.toString("yyyy-MM-dd hh:mm:ss") ));
    ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignCenter);

    ms_readmsr(&msr, NULL, 0, NULL, NULL, 0, 0, 0);
    mst_freegroup(&mstg);
}

