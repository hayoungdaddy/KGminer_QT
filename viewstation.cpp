#include "viewstation.h"

ViewStation::ViewStation(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent)
{
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;

    c = cfg;

    count = 0;

    QFile file;
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
                filename = line.section(":",1,1);
                continue;
            }
            else if(line.startsWith("Description"))
            {
                description = line.section(":",1,1);
                continue;
            }
            else
            {
                count++;
                sta.resize(count); chan.resize(count); loc.resize(count); net.resize(count); lat.resize(count); lon.resize(count); elev.resize(count);
                sta[count-1] = line.section(" ", 0, 0);
                chan[count-1] = line.section(" ", 1, 1);
                loc[count-1] = line.section(" ", 2, 2);
                net[count-1] = line.section(" ", 3, 3);
                lat[count-1] = line.section(" ", 4, 4);
                lon[count-1] = line.section(" ", 5, 5);
                elev[count-1] = line.section(" ", 6, 6);
            }
        }
        file.close();
    }

    mainlayout = new QVBoxLayout(this);
    toplayout = new QHBoxLayout;
    toplayout2 = new QHBoxLayout;
    labellayout = new QHBoxLayout;
    middlelayout = new QVBoxLayout;
    buttomlayout = new QHBoxLayout;

    addbutton = new QPushButton;  
    genbutton = new QPushButton;   
    viewstabutton = new QPushButton;   
    quitbutton = new QPushButton;

    la1 = new QLabel; 
    la1->setAlignment(Qt::AlignCenter);
    filenameLE = new QLineEdit;
    filenameLE->setAlignment(Qt::AlignCenter);
    filenameLE->setText(filename);
    filenameLE->setEnabled(false);
    toplayout->addWidget(la1);
    toplayout->addWidget(filenameLE);

    la2 = new QLabel;   
    la2->setAlignment(Qt::AlignCenter);
    descLE = new QLineEdit;
    descLE->setAlignment(Qt::AlignCenter);
    descLE->setText(description);
    descLE->setEnabled(false);
    toplayout2->addWidget(la2);
    toplayout2->addWidget(descLE);

    if(!korean)
    {
        setWindowTitle("Config Stations Information");
        addbutton->setText("Add new line");
        genbutton->setText("Generate");
        viewstabutton->setText("View stations location");
        quitbutton->setText("Cancel");
        la1->setText("File Name");
        la2->setText("Description");
    }
    else
    {
        setWindowTitle(codec->toUnicode("관측소 정보 설정"));
        addbutton->setText(codec->toUnicode("관측소 입력 라인 추가"));
        genbutton->setText(codec->toUnicode("설정 값 변경"));
        viewstabutton->setText(codec->toUnicode("관측소 위치 보기"));
        quitbutton->setText(codec->toUnicode("종료"));
        la1->setText(codec->toUnicode("파일명"));
        la2->setText(codec->toUnicode("설명"));
    }

    QLabel* label = new QLabel[7];
    label[0].setText("Station Code");
    label[1].setText("Channel");
    label[2].setText("Location Code");
    label[3].setText("Network Code");
    label[4].setText("Latitude (DD)");
    label[5].setText("Longitude (DD)");
    label[6].setText("Elevation (Km)");

    for(int i=0;i<7;i++)
    {
        label[i].setAlignment(Qt::AlignCenter);
        labellayout->addWidget(&label[i]);
    }

    for(int i=0;i<sta.size();i++)
    {
        addbutton_clicked();
    }

    buttomlayout->addWidget(addbutton);
    buttomlayout->addWidget(genbutton);
    buttomlayout->addWidget(viewstabutton);
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
    connect(viewstabutton, SIGNAL(clicked()), SLOT(viewstabuttonClicked()));
}

ViewStation::~ViewStation()
{
}

void ViewStation::viewstabuttonClicked()
{
    QString cmd;
    cmd = "grep -v File " + c.PARAMSDIR + "/sta.info | grep -v Desc > /usr/local/tomcat/webapps/viewstaloc/sta.info";
    system(cmd.toLatin1().data());
    cmd = "firefox 127.0.0.1:8080/viewstaloc &";
    system(cmd.toLatin1().data());
}

void ViewStation::addbutton_clicked()
{
    QHBoxLayout *hlayout =new QHBoxLayout;
    QLineEdit *stale = new QLineEdit;
    QLineEdit *chanle = new QLineEdit;
    QLineEdit *locle = new QLineEdit;
    QLineEdit *netle = new QLineEdit;
    QLineEdit *latle = new QLineEdit;
    QLineEdit *lonle = new QLineEdit;
    QLineEdit *elevle = new QLineEdit;

    qlist.push_back(stale);

    if(qlist.count()<=count)
    {
        stale->setText(sta[qlist.count()-1]);
        chanle->setText(chan[qlist.count()-1]);
        locle->setText(loc[qlist.count()-1]);
        netle->setText(net[qlist.count()-1]);
        latle->setText(lat[qlist.count()-1]);
        lonle->setText(lon[qlist.count()-1]);
        elevle->setText(elev[qlist.count()-1]);
    }
    else
    {
        sta.resize(qlist.count()); chan.resize(qlist.count()); loc.resize(qlist.count()); net.resize(qlist.count());
        lat.resize(qlist.count()); lon.resize(qlist.count()); elev.resize(qlist.count());
    }

    int i=0;
    stale->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    chanle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    locle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    netle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    latle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    lonle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) ); i++;
    elevle->setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) );

    stale->setAlignment(Qt::AlignCenter);
    chanle->setAlignment(Qt::AlignCenter);
    locle->setAlignment(Qt::AlignCenter);
    netle->setAlignment(Qt::AlignCenter);
    latle->setAlignment(Qt::AlignCenter);
    lonle->setAlignment(Qt::AlignCenter);
    elevle->setAlignment(Qt::AlignCenter);

    hlayout->addWidget(stale);
    hlayout->addWidget(chanle);
    hlayout->addWidget(locle);
    hlayout->addWidget(netle);
    hlayout->addWidget(latle);
    hlayout->addWidget(lonle);
    hlayout->addWidget(elevle);

    middlelayout->addLayout(hlayout);

    connect(stale, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(chanle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(locle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(netle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(latle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(lonle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    connect(elevle, SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
}

void ViewStation::lineedit_textChanged(QString text)
{
    QObject *obj = QObject::sender();
    QString senderobjName = obj->objectName();

    int isTextchanged = senderobjName.indexOf("lineedit") != -1;

    if(isTextchanged)
    {
        int i = senderobjName.section('_', 1, 1).toInt() - 1;
        int j = senderobjName.section('_', 2, 2).toInt() - 1;

        if(j == 0) sta[i] = text;
        else if(j == 1) chan[i] = text;
        else if(j == 2) loc[i] = text;
        else if(j == 3) net[i] = text;
        else if(j == 4) lat[i] = text;
        else if(j == 5) lon[i] = text;
        else if(j == 6) elev[i] = text;
    }
}

void ViewStation::genButtonClicked()
{
    int count = 0;

    for(int i=0;i<sta.size();i++)
    {
        if(sta[count] == "")
            break;
        count++;
    }

    QFile file;

    /* generate station file. */
    QString staFileName = c.PARAMSDIR + "/staInfo/" + filenameLE->text() + ".sta";
    file.setFileName( staFileName );

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;
        stream <<  "Filename:" << filenameLE->text() << "\n";
        stream <<  "Description:" << descLE->text() << "\n" ;

        for(int i=0;i<count;i++)
        {
            stream << sta[i] << " " << chan[i] << " " << loc[i] << " " << net[i] << " " << lat[i] << " " << lon[i] << " " << elev[i] << "\n";
        }
        file.close() ;
    }

    QString cmd = "cp " + staFileName + " " + c.PARAMSDIR + "/sta.info";
    system(cmd.toLatin1().data());

    STAFILE stafile;

    FileGenerator *gen = new FileGenerator;
    stafile = gen->makeSTAFILE(c, false);
    gen->pick_ew_gen(c, stafile);
    gen->pick_FP_gen(c, stafile);
    gen->hinv_gen(c, stafile);
    gen->tanklist_gen(c, stafile);
    gen->binder_gen(c, stafile);
    gen->ew2mseed_gen(c, stafile);
    gen->nlloc_gen(false, c, stafile);

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Staions Info. file generated and loaded.");
    else msgBox.setText(codec->toUnicode("관측소 정보 파일 수정 완료."));
    msgBox.exec();
    accept();
}
