#include "picklist.h"
#include "ui_picklist.h"

PickList::PickList(QWidget *parent) :
    QFrame(parent),
    PickList_ui(new Ui::PickList)
{
    PickList_ui->setupUi(this);

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
    delete PickList_ui;
}

void PickList::clear()
{
    PickList_ui->textEdit->clear();
}

void PickList::checkdate()
{
    QDate date = QDate::currentDate();
    if(date.dayOfYear() != today.dayOfYear())
    {
        today = date;
        setup();
        //clear();
    }
}

void PickList::setup()
{
    QString year,month,day;
    year = today.toString("yyyy");
    month = today.toString("MM");
    day = today.toString("dd");
    QString pickfile = DATADIR + "/pick/picks." + year + month + day;
    QString program = "tail";
    QStringList arguments;
    arguments << "-f" << pickfile;

    pickProcess->start(program, arguments);
}

void PickList::loadTextFile()
{
    QString line = pickProcess->readAllStandardOutput();
    line = line.simplified();
    QStringList splitS = line.split(" ");
    PickList_ui->textEdit->append(splitS[4] + " " + splitS[5] + " " + splitS[6].left(4) + "/" + splitS[6].mid(4,2) + "/" + splitS[6].mid(6,2) + " " +
            splitS[6].mid(8,2) + ":" + splitS[6].mid(10,2) + ":" + splitS[6].mid(12,6));
}

