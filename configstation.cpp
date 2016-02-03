#include "configstation.h"

ConfigStation::ConfigStation(QWidget *parent) :
    QDialog(parent)
{
    codec = QTextCodec::codecForName( "utf8" );
    korean=false;

    if(!korean) setWindowTitle("Config Stations Information");
    else setWindowTitle(codec->toUnicode("관측소 정보 설정"));

    setMinimumWidth(1100);
    setMaximumWidth(1100);

    mainlayout = new QVBoxLayout(this);
    toplayout = new QHBoxLayout;
    toplayout2 = new QHBoxLayout;
    labellayout = new QHBoxLayout;
    middlelayout = new QVBoxLayout;
    buttomlayout = new QHBoxLayout;

    addbutton = new QPushButton;
    if(!korean) addbutton->setText("Add new line");
    else addbutton->setText(codec->toUnicode("관측소 입력 라인 추가"));
    genbutton = new QPushButton;
    if(!korean) genbutton->setText("Generate");
    else genbutton->setText(codec->toUnicode("설정 값 변경"));
    quitbutton = new QPushButton;
    if(!korean) quitbutton->setText("Cancel");
    else quitbutton->setText(codec->toUnicode("종료"));

    la1 = new QLabel;
    if(!korean) la1->setText("File Name");
    else la1->setText(codec->toUnicode("파일명(영문, 숫자만 사용)"));
    la1->setAlignment(Qt::AlignCenter);
    filenameLE = new QLineEdit;
    filenameLE->setAlignment(Qt::AlignCenter);
    toplayout->addWidget(la1);
    toplayout->addWidget(filenameLE);

    la2 = new QLabel;
    if(!korean) la2->setText("Description");
    else la2->setText(codec->toUnicode("설명(영문, 숫자만 사용)"));
    la2->setAlignment(Qt::AlignCenter);
    descLE = new QLineEdit;
    descLE->setAlignment(Qt::AlignCenter);
    toplayout2->addWidget(la2);
    toplayout2->addWidget(descLE);

    QLabel* label = new QLabel[8];
    label[0].setText("Station Code");
    label[1].setText("Channel");
    label[2].setText("Component");
    label[3].setText("Location Code");
    label[4].setText("Network Code");
    label[5].setText("Latitude (DD)");
    label[6].setText("Longitude (DD)");
    label[7].setText("Elevation (Km)");

    for(int i=0;i<8;i++)
    {
        label[i].setAlignment(Qt::AlignCenter);
        labellayout->addWidget(&label[i]);
    }

    for(int i=0;i<5;i++)
    {
        addbutton_clicked();
    }

    buttomlayout->addWidget(addbutton);
    buttomlayout->addWidget(genbutton);
    buttomlayout->addWidget(quitbutton);
    buttomlayout->setAlignment(Qt::AlignBottom);

    mainlayout->addLayout(toplayout);
    mainlayout->addLayout(toplayout2);
    mainlayout->addLayout(labellayout);
    mainlayout->addLayout(middlelayout);
    mainlayout->addLayout(buttomlayout);

    connect(addbutton, SIGNAL(clicked()), SLOT(addbutton_clicked()));
    connect(quitbutton, SIGNAL(clicked()), SLOT(accept()));
    connect(genbutton, SIGNAL(clicked()), SLOT(genButtonClicked()));
}

ConfigStation::~ConfigStation()
{
}

void ConfigStation::setLanguageEn()
{

    setWindowTitle("Config Stations Information");

    addbutton->setText("Add new line");
    genbutton->setText("Generate");
    quitbutton->setText("Cancel");
    la1->setText("File Name");
    la2->setText("Description");
}

void ConfigStation::setLanguageKo()
{

    setWindowTitle(codec->toUnicode("관측소 정보 설정"));
    addbutton->setText(codec->toUnicode("관측소 입력 라인 추가"));
    genbutton->setText(codec->toUnicode("설정 값 변경"));
    quitbutton->setText(codec->toUnicode("종료"));
    la1->setText(codec->toUnicode("파일명(영문, 숫자만 사용)"));
    la2->setText(codec->toUnicode("설명(영문, 숫자만 사용)"));
}

void ConfigStation::setup()
{

}

void ConfigStation::addbutton_clicked()
{
    QHBoxLayout *hlayout =new QHBoxLayout;
    QLineEdit *stale = new QLineEdit;
    QComboBox *chancb = new QComboBox;
    QComboBox *compcb = new QComboBox;
    QLineEdit *locle = new QLineEdit;
    locle->setEnabled(false);
    QLineEdit *netle = new QLineEdit;
    QLineEdit *latle = new QLineEdit;
    QLineEdit *lonle = new QLineEdit;
    QLineEdit *elevle = new QLineEdit;

    qlist.push_back(stale);

    sta.resize(qlist.count()); chan.resize(qlist.count()); comp.resize(qlist.count()); loc.resize(qlist.count()); net.resize(qlist.count());
    lat.resize(qlist.count()); lon.resize(qlist.count()); elev.resize(qlist.count());
    chan[qlist.count()-1] = "HG";
    comp[qlist.count()-1] = "Z";
    loc[qlist.count()-1] = "--";

    int i=0;
    stale->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    chancb->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    compcb->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    locle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    netle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    latle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    lonle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    elevle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) );

    stale->setAlignment(Qt::AlignCenter);
    locle->setAlignment(Qt::AlignCenter);
    netle->setAlignment(Qt::AlignCenter);
    latle->setAlignment(Qt::AlignCenter);
    lonle->setAlignment(Qt::AlignCenter);
    elevle->setAlignment(Qt::AlignCenter);

    locle->setText("--");
    QStringList chan;
    chan << "HG" << "BG" << "HH" << "BH" << "EL" << "SL";
    chancb->addItems(chan); chancb->setCurrentIndex(0);

    QStringList comp;
    comp << "Z" << "N" << "E" << "Z/N/E" << "N/E";
    compcb->addItems(comp); compcb->setCurrentIndex(0);

    //qDebug() << "index " << QString::number(qlist.count());

    hlayout->addWidget(stale);
    hlayout->addWidget(chancb);
    hlayout->addWidget(compcb);
    hlayout->addWidget(locle);
    hlayout->addWidget(netle);
    hlayout->addWidget(latle);
    hlayout->addWidget(lonle);
    hlayout->addWidget(elevle);

    chancb->setMaximumWidth(130);
    chancb->setMinimumWidth(130);
    compcb->setMaximumWidth(130);
    compcb->setMinimumWidth(130);

    middlelayout->addLayout(hlayout);

    connect(stale, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(chancb, SIGNAL(currentIndexChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(compcb, SIGNAL(currentIndexChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(locle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(netle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(latle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(lonle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(elevle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
}

void ConfigStation::lineedit_textChanged(QString text)
{
    QObject *obj = QObject::sender();
    QString senderobjName = obj->objectName();

    //qDebug() << senderobjName;

    int isTextchanged = senderobjName.indexOf("lineedit") != -1;

    if(isTextchanged)
    {
        int i = senderobjName.section('_', 1, 1).toInt() - 1;
        int j = senderobjName.section('_', 2, 2).toInt() - 1;

        if(j == 0) sta[i] = text;
        else if(j == 1) chan[i] = text;
        else if(j == 2) comp[i] = text;
        else if(j == 3) loc[i] = text;
        else if(j == 4) net[i] = text;
        else if(j == 5) lat[i] = text;
        else if(j == 6) lon[i] = text;
        else if(j == 7) elev[i] = text;

        //qDebug() << QString::number(i) << " " << QString::number(j) << " " << text;
    }
}

void ConfigStation::genButtonClicked()
{
    QDir dir;
    dir.setPath( PARAMSDIR + "/staInfo");
    QStringList files;
    QString fileName = "*.sta";
    files = dir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    for(int i=0;i<files.count();i++)
    {
        if(files[i] == filenameLE->text() + ".sta")
        {
            if(!korean) QMessageBox::warning( this, "Information", "Duplicate Station file name.\nPlease use other name.");
            else QMessageBox::warning( this, codec->toUnicode("정보"), codec->toUnicode("중복되는 파일명을 가진 파일이 존재합니다.\n다른 파일명을 사용하십시오."));
            return;
        }
    }

    if(filenameLE->text() == "")
    {
        if(!korean) QMessageBox::warning( this, "Information", "Please Input a file name.");
        else QMessageBox::warning( this, codec->toUnicode("정보"), codec->toUnicode("파일명을 입력하십시오.(영문, 숫자만 사용)") );
        return;
    }

    if(descLE->text() == "")
    {
        QMessageBox::warning( this, "Information", "Please Input a description.");

        if(!korean) QMessageBox::warning( this, "Information", "Please Input a description.");
        else QMessageBox::warning( this, codec->toUnicode("정보"), codec->toUnicode("관측소 설명란을 입력하십시오.(영문, 숫자만 사용)") );
        return;
    }

    int count = 0;

    for(int i=0;i<sta.size();i++)
    {
        if(sta[count] == "")
            break;
        count++;
    }

    QFile file;
    /* generate station file. */
    QString staFileName = PARAMSDIR + "/staInfo/" + filenameLE->text() + ".sta";
    file.setFileName( staFileName );

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;
        stream <<  "Filename:" << filenameLE->text() << "\n";
        stream <<  "Description:" << descLE->text() << "\n" ;

        for(int i=0;i<count;i++)
        {
            if(comp[i] == "Z/N/E")
            {
                stream << sta[i] << " " << chan[i] << "Z" << " " << loc[i] << " " << net[i] << " " << lat[i] << " " << lon[i] << " " << elev[i] << "\n";
                stream << sta[i] << " " << chan[i] << "N" << " " << loc[i] << " " << net[i] << " " << lat[i] << " " << lon[i] << " " << elev[i] << "\n";
                stream << sta[i] << " " << chan[i] << "E" << " " << loc[i] << " " << net[i] << " " << lat[i] << " " << lon[i] << " " << elev[i] << "\n";
            }
            else if(comp[i] == "Z")
                stream << sta[i] << " " << chan[i] << "Z" << " " << loc[i] << " " << net[i] << " " << lat[i] << " " << lon[i] << " " << elev[i] << "\n";
            else if(comp[i] == "N")
                stream << sta[i] << " " << chan[i] << "N" << " " << loc[i] << " " << net[i] << " " << lat[i] << " " << lon[i] << " " << elev[i] << "\n";
            else if(comp[i] == "E")
                stream << sta[i] << " " << chan[i] << "E" << " " << loc[i] << " " << net[i] << " " << lat[i] << " " << lon[i] << " " << elev[i] << "\n";
            else if(comp[i] == "N/E")
            {
                stream << sta[i] << " " << chan[i] << "N" << " " << loc[i] << " " << net[i] << " " << lat[i] << " " << lon[i] << " " << elev[i] << "\n";
                stream << sta[i] << " " << chan[i] << "E" << " " << loc[i] << " " << net[i] << " " << lat[i] << " " << lon[i] << " " << elev[i] << "\n";
            }
        }
        file.close() ;
    }

    QString cmd = "cp " + staFileName + " " + PARAMSDIR + "/sta.info";
    system(cmd.toLatin1().data());

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

        scnCount = stafile.staName.count();

        minlat = 999; maxlat = 0; minlon = 999; maxlon = 0;

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
    if(!korean) msgBox.setText("Staions Info. file generated and loaded.");
    else msgBox.setText(codec->toUnicode("관측소 정보 파일 생성 완료."));
    msgBox.exec();
    accept();
}
