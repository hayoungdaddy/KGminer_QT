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

    QFile file;
    STAFILE stafile;
    QString minLatforBinder, maxLatforBinder, minLonforBinder, maxLonforBinder;
    QString avgLatforNLLoc, avgLonforNLLoc;
    double minlat, maxlat, minlon, maxlon, avglat, avglon;
    int scnCount;

    if(event)
        file.setFileName(c.EVENTDIR + "/NEWEVENT/sta.info");
    else
        file.setFileName(c.PARAMSDIR + "/sta.info");

    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line;

        while(!stream.atEnd())
        {
            line = stream.readLine();

            if(line.startsWith("#") || line.isNull() || line.startsWith("["))
                continue;
            else if(line.startsWith("Filename"))
            {
                stafile.filename = line.section(":",1,1);
                continue;
            }
            else if(line.startsWith("Description"))
            {
                stafile.description = line.section(":",1,1);
                continue;
            }
            else
            {
                stafile.staName << line.section(" ", 0, 0);
                stafile.chanName << line.section(" ", 1, 1);
                stafile.locName << line.section(" ", 2, 2);
                stafile.netName << line.section(" ", 3, 3);
                stafile.latD << line.section(" ", 4, 4);
                stafile.lonD << line.section(" ", 5, 5);
                stafile.elevKm << line.section(" ", 6, 6);
            }
        }

        file.close();
        minlat = 999; maxlat = 0; minlon = 999; maxlon = 0;

        scnCount = stafile.staName.count();

        for(int i=0;i<scnCount;i++)
        {
            double fx, fy, latd, lond;
            int x, y;

            fx = stafile.latD[i].toDouble();
            fy = stafile.lonD[i].toDouble();
            if(fx < minlat) minlat = fx;
            if(fx > maxlat) maxlat = fx;
            if(fy < minlon) minlon = fy;
            if(fy > maxlon) maxlon = fy;

            x = stafile.latD[i].section('.',0,0).toInt();
            y = stafile.lonD[i].section('.',0,0).toInt();
            latd = 60 * (fx - x);
            lond = 60 * (fy - y);

            QString temp, temp2, temp3, temp4;
            temp = temp.setNum(x, 10);
            temp2 = temp2.setNum(latd, 'f', 4);
            temp3 = temp3.setNum(y, 10);
            temp4 = temp4.setNum(lond, 'f', 4);

            if(temp2.toFloat() < 10)
                stafile.latM << temp + "  " + temp2 + "N";
            else
                stafile.latM << temp + " " + temp2 + "N";

            if(temp4.toFloat() < 10)
                stafile.lonM << temp3 + "  " + temp4 + "E";
            else
                stafile.lonM << temp3 + " " + temp4 + "E";
            stafile.elevM << stafile.elevKm[i].section('.',1,1);
        }

        avglat = (minlat + maxlat) / 2;
        avglon = (minlon + maxlon) / 2;
        avgLatforNLLoc = avgLatforNLLoc.setNum(avglat, 'f', 6);
        avgLonforNLLoc = avgLonforNLLoc.setNum(avglon, 'f', 6);
        minlat = minlat - 1; maxlat = maxlat + 2; minlon = minlon - 1; maxlon = maxlon + 2;
        minLatforBinder = minLatforBinder.setNum(minlat, 'f', 0);
        maxLatforBinder = maxLatforBinder.setNum(maxlat, 'f', 0);
        minLonforBinder = minLonforBinder.setNum(minlon, 'f', 0);
        maxLonforBinder = maxLonforBinder.setNum(maxlon, 'f', 0);
    }

    FileGenerator *gen = new FileGenerator;
    if(!event)
    {
        gen->pick_ew_gen(c, stafile);
        gen->pick_FP_gen(c, stafile);
        gen->hinv_gen(c, stafile);
        gen->tanklist_gen(c, stafile);
        gen->binder_gen(c, minLatforBinder, maxLatforBinder, minLonforBinder, maxLonforBinder);
        gen->ew2mseed_gen(c, stafile);
        gen->nlloc_gen(false, c, stafile, avgLatforNLLoc, avgLonforNLLoc);
    }
    else
    {
        gen->nlloc_gen(true, c, stafile, avgLatforNLLoc, avgLonforNLLoc);
    }

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Staions Infomation loaded.");
    else msgBox.setText(codec->toUnicode("관측소 정보 로딩 완료."));
    msgBox.exec();
    accept();
}
