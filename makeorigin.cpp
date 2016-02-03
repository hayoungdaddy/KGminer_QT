#include "makeorigin.h"
#include "ui_makeorigin.h"

MakeOrigin::MakeOrigin(QString evid, QString orid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MakeOrigin)
{
    ui->setupUi(this);
    //codec = QTextCodec::codecForName( "utf8" );
}

MakeOrigin::~MakeOrigin()
{
    delete ui;
}

void MakeOrigin::setup(QString evid, QString orid)
{
    ui->evidLE->setText(evid);
    ui->oridLE->setText(orid);

    ui->latLE->clear();
    ui->lonLE->clear();
    ui->depthLE->clear();
    ui->timeLE->clear();
    ui->alLE->clear();

    EVID = evid;
    ORID = orid;
}

void MakeOrigin::closeEvent(QCloseEvent *event)
{
    QString cmd = "pkill geotool";
    system(cmd.toLatin1().data());

    cmd = "rm -rf " + EVENTDIR + "/" + EVID + "/" + ORID;
    system(cmd.toLatin1().data());
}

void MakeOrigin::on_quitButton_clicked()
{
    if( !QMessageBox::question( this,
                                "Warning",
                "Are you sure?",
                "Yes",
                "No",
                QString::null, 1, 1 ) )
    {
        QString cmd = "pkill geotool";
        system(cmd.toLatin1().data());

        cmd = "rm -rf " + EVENTDIR + "/" + EVID + "/" + ORID;
        system(cmd.toLatin1().data());

        accept();
    }
    else
        return;
}

void MakeOrigin::on_showButton_clicked()
{
    QString cmd = SCRIPTDIR + "/viewWave.sh " + EVID + " " + ORID + " >> /dev/null 2>&1 &";
    system(cmd.toLatin1().data());
}

void MakeOrigin::on_saveButton_clicked()
{
    QString cmd = "cp " + EVENTDIR + "/" + EVID + "/" + ORID + "/data/css/css.arrival " + EVENTDIR + "/" + EVID + "/" + ORID + "/data/css/.css.arrival";
    system(cmd.toLatin1().data());

    cmd = SCRIPTDIR + "/makePickforNLLoc.sh " + EVID + " " + ORID;
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    msgBox.setText("Saved picklists.");
    msgBox.exec();
}

void MakeOrigin::on_editButton_clicked()
{
    NLLoc *nlloc = new NLLoc(EVID, ORID, "1", this);
    nlloc->setup();
    nlloc->show();
}

void MakeOrigin::on_nllocButton_clicked()
{
    ui->nllocButton->setEnabled(false);
    QFile file;
    file.setFileName(EVENTDIR + "/" + EVID + "/" + ORID + "/NLLOC/type");
    QString minv,maxv;
    if(file.open( QIODevice::ReadOnly ))
    {
        QTextStream stream(&file);
        QString line;

        line = stream.readLine();
        TYPE = line.section(":", 0, 0);

        if(TYPE == "MVM")
        {
            minv = line.section(":", 1, 1);
            maxv = line.section(":", 2, 2);
        }

        file.close();
    }

    QString cmd = "cp " + EVENTDIR + "/" + EVID + "/" + ORID + "/data/css/css.arrival " + EVENTDIR + "/" + EVID + "/" + ORID + "/data/css/.css.arrival";
    system(cmd.toLatin1().data());

    cmd = SCRIPTDIR + "/makePickforNLLoc.sh " + EVID + " " + ORID;
    system(cmd.toLatin1().data());

    if(TYPE == "SVM")
    {
        cmd = SCRIPTDIR + "/runNLLocSVM.sh " + EVID + " " + ORID + " 0";
    }
    else if(TYPE == "MVM")
    {
        cmd = SCRIPTDIR + "/runNLLocMVM.sh " + EVID + " " + ORID + " 0 " + minv + " " + maxv;
    }
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    msgBox.setText("Finished NLLoc.");
    msgBox.exec();

    ui->nllocButton->setEnabled(true);

    file.setFileName(EVENTDIR + "/" + EVID + "/" + ORID + "/LOC/NLLOC.origin");

    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            if(_line.startsWith("#") || _line.isNull() || _line.startsWith("[") )
                continue;
            else
            {
                if(_line.startsWith("GEOGRAPHIC"))
                {
                    ui->latLE->setText(_line.section(' ', 9, 9));
                    ui->lonLE->setText(_line.section(' ', 11, 11));
                    ui->depthLE->setText(_line.section(' ', 13, 13));
                    ui->timeLE->setText(_line.section(' ', 2, 2) + "-" + _line.section(' ', 3, 3) + "-"
                                        + _line.section(' ', 4, 4) + " " + _line.section(' ', 5, 5) + ":"
                                        + _line.section(' ', 6, 6) + ":" + _line.section(' ', 7, 7).section('.', 0, 0));
                    if(TYPE == "SVM")
                        ui->alLE->setText("NLLoc_SVM");
                    else if(TYPE == "MVM")
                        ui->alLE->setText("NLLoc_MVM");
                }
            }
        }
        file.close();
    }
    ui->inputDBButton->setEnabled(true);
}

void MakeOrigin::on_inputDBButton_clicked()
{
    QString cmd;
    cmd = SCRIPTDIR + "/inputDatatoDB.sh " + EVID + " " + ORID + " Manual " + TYPE;
    qDebug() << cmd;
    system(cmd.toLatin1().data());

    cmd = "pkill geotool";
    system(cmd.toLatin1().data());

    QMessageBox msgBox;
    msgBox.setText("Added new origin.");
    msgBox.exec();

    emit resetTable();

    accept();
}
