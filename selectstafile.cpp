#include "selectstafile.h"
#include "ui_selectstafile.h"

SelectStaFile::SelectStaFile(bool _event, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectStaFile)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean=false;
    event = _event;

    ui->listWidget->clear();
    ui->listWidgetDesc->clear();
    QDir dir;
    dir.setPath(PARAMSDIR + "/staInfo");
    QStringList files;
    QString fileName = "*.sta";
    files = dir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    ui->listWidget->addItems(files);

    QFile file;
    for(int i=0;i<files.count();i++)
    {
        file.setFileName(PARAMSDIR + "/staInfo/"+files.at(i));

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

void SelectStaFile::setup()
{
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
        cmd = "cp " + PARAMSDIR + "/staInfo/" + staFileName + " " + EVENTDIR + "/TMP/sta.info";
        system(cmd.toLatin1().data());
        emit sendSignaltoDataExtractor();
    }
    else
    {
        cmd = "cp " + PARAMSDIR + "/staInfo/" + staFileName + " " + PARAMSDIR + "/sta.info";
        system(cmd.toLatin1().data());
    }

    if(!event)
    {
    QFile file;
    STAFILE stafile;
    QString minLatforBinder, maxLatforBinder, minLonforBinder, maxLonforBinder;
    QString avgLatforNLLoc, avgLonforNLLoc;
    double minlat, maxlat, minlon, maxlon, avglat, avglon;
    int scnCount;

    file.setFileName(PARAMSDIR + "/sta.info");
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

            stafile.latM << temp + " " + temp2 + "N";
            stafile.lonM << temp3 + " " + temp4 + "E";
            stafile.elevM << stafile.elevKm[i].section('.',1,1);

            //qDebug() << stafile.staName[i] << " " << stafile.latD[i] << " " << stafile.lonD[i] << " " << stafile.elevKm[i];
        }

        avglat = (minlat + maxlat) / 2;
        avglon = (minlon + maxlon) / 2;
        //qDebug() << minlat << maxlat << minlon << maxlon << avglat << " " << avglon;
        avgLatforNLLoc = avgLatforNLLoc.setNum(avglat, 'f', 6);
        avgLonforNLLoc = avgLonforNLLoc.setNum(avglon, 'f', 6);
        minlat = minlat - 1; maxlat = maxlat + 2; minlon = minlon - 1; maxlon = maxlon + 2;
        minLatforBinder = minLatforBinder.setNum(minlat, 'f', 0);
        maxLatforBinder = maxLatforBinder.setNum(maxlat, 'f', 0);
        minLonforBinder = minLonforBinder.setNum(minlon, 'f', 0);
        maxLonforBinder = maxLonforBinder.setNum(maxlon, 'f', 0);
    }

    FileGenerator *gen = new FileGenerator;
    gen->pick_ew_gen(stafile);
    gen->pick_FP_gen(stafile);
    gen->hinv_gen(stafile);
    gen->tanklist_gen(stafile);
    gen->binder_gen(minLatforBinder, maxLatforBinder, minLonforBinder, maxLonforBinder);
    gen->ew2mseed_gen(stafile);
    gen->nlloc_gen(stafile, avgLatforNLLoc, avgLonforNLLoc);

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Staions Infomation loaded.");
    else msgBox.setText(codec->toUnicode("관측소 정보 로딩 완료."));
    msgBox.exec();
    }

    accept();
}
