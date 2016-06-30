#include "picklist.h"
#include "ui_picklist.h"

PickList::PickList(CFG cfg, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PickList)
{
    ui->setupUi(this);
    c = cfg;

    today = QDate::currentDate();

    checkdatetimer = new QTimer(this);
    checkdatetimer->start( 10000 );
    connect(checkdatetimer, SIGNAL(timeout()), this, SLOT(checkdate()));

    /* pick process */
    pickProcess = new QProcess;
    connect(pickProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(loadTextFile()));
}

PickList::~PickList()
{
    delete ui;
}

void PickList::clear()
{
    ui->textEdit->clear();
}

void PickList::checkdate()
{
    QDate date = QDate::currentDate();
    if(date.dayOfYear() != today.dayOfYear())
    {
        today = date;
        setup();
    }
}

void PickList::setup()
{
    QString year,month,day;
    year = today.toString("yyyy");
    month = today.toString("MM");
    day = today.toString("dd");
    QString pickfile = c.PICKDIR + "/picks." + year + month + day;
    QString program = "tail";
    QStringList arguments;
    arguments << "-0f" << pickfile;

    pickProcess->start(program, arguments);
}

void PickList::loadTextFile()
{
    /*
    QString line = pickProcess->readAllStandardOutput();
    line = line.simplified();
    QStringList splitS = line.split(" ");
    ui->textEdit->append(splitS[4] + " " + splitS[5] + " " + splitS[6].left(4) + "-" + splitS[6].mid(4,2) + "-" + splitS[6].mid(6,2) + " " +
            splitS[6].mid(8,2) + ":" + splitS[6].mid(10,2) + ":" + splitS[6].mid(12,6));
            */
    QByteArray array = pickProcess->readAllStandardOutput();
    QStringList strLines = QString(array).split("\n");

    // 8 57 255 65960 SGP.HGZ.KS.-- ?1 20160414123551.860 10 0 0
    foreach (QString line, strLines)
    {
        line = line.simplified();
        line = line.section(' ', 4, 4) + " " + line.section(' ', 6, 6).left(4) + "-" + line.section(' ', 6, 6).mid(4,2)
                + "-" + line.section(' ', 6, 6).mid(6,2) + " " + line.section(' ', 6, 6).mid(8,2)
                + ":" + line.section(' ', 6, 6).mid(10,2) + ":" + line.section(' ', 6, 6).mid(12,6);
        if(!line.startsWith(" ")) ui->textEdit->append(line);
    }
}
