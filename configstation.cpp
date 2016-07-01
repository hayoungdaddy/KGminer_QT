#include "configstation.h"

ConfigStation::ConfigStation(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent)
{
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;

    c = cfg;

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
    genbutton = new QPushButton;
    quitbutton = new QPushButton;
    la1 = new QLabel;
    la1->setAlignment(Qt::AlignCenter);
    filenameLE = new QLineEdit;
    filenameLE->setAlignment(Qt::AlignCenter);
    toplayout->addWidget(la1);
    toplayout->addWidget(filenameLE);
    la2 = new QLabel;
    la2->setAlignment(Qt::AlignCenter);
    descLE = new QLineEdit;
    descLE->setAlignment(Qt::AlignCenter);
    toplayout2->addWidget(la2);
    toplayout2->addWidget(descLE);

    if(!korean)
    {
        addbutton->setText("Add new line");
        genbutton->setText("Generate");
        quitbutton->setText("Cancel");
        la1->setText("File Name");
        la2->setText("Description");
    }
    else
    {
        addbutton->setText(codec->toUnicode("관측소 입력 라인 추가"));
        genbutton->setText(codec->toUnicode("설정 값 변경"));
        quitbutton->setText(codec->toUnicode("종료"));
        la1->setText(codec->toUnicode("파일명(영문, 숫자만 사용)"));
        la2->setText(codec->toUnicode("설명(영문, 숫자만 사용)"));
    }

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
    }
}

void ConfigStation::genButtonClicked()
{
    QDir dir;
    dir.setPath( c.PARAMSDIR + "/staInfo");
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
    QString staFileName = c.PARAMSDIR + "/staInfo/" + filenameLE->text() + ".sta";
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
    else msgBox.setText(codec->toUnicode("관측소 정보 파일 생성 완료."));
    msgBox.exec();
    accept();
}
