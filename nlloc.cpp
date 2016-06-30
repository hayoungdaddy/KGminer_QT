#include "nlloc.h"
#include "ui_nlloc.h"

NLLoc::NLLoc(CFG cfg, bool _korean, QString evid, QString orid, QString num, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NLLoc)
{
    ui->setupUi(this);

    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    c = cfg;

    EVID = evid;
    ORID = orid;
    NUMBER = num;

    setup();

    connect(ui->svmr, SIGNAL(clicked()), this, SLOT(svmbClicked()));
    connect(ui->mvmr, SIGNAL(clicked()), this, SLOT(mvmbClicked()));
    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->genButton, SIGNAL(clicked()), this, SLOT(genButtonClicked()));
    connect(ui->defaultButton, SIGNAL(clicked()), this, SLOT(defaultButtonClicked()));
}

NLLoc::~NLLoc()
{
    delete ui;
}

void NLLoc::setLanguageEn()
{
    setWindowTitle("Config NLLoc Parameters");
    ui->genButton->setText("Generate");
    ui->defaultButton->setText("Default");
    ui->quitButton->setText("Quit");
}

void NLLoc::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("NLLoc 설정 값 변경"));
    ui->genButton->setText(codec->toUnicode("변경"));
    ui->defaultButton->setText(codec->toUnicode("기본값"));
    ui->quitButton->setText(codec->toUnicode("취소"));
}

void NLLoc::svmbClicked()
{
    ui->svmf->setEnabled(true);
    ui->mvmf->setEnabled(false);
}

void NLLoc::mvmbClicked()
{
    ui->mvmf->setEnabled(true);
    ui->svmf->setEnabled(false);
}

void NLLoc::setup()
{
    // EVID == "0" is normal change mode
    // EVID == num is make new origin mode
    // EVID == "NEWEVENT" is make new event mode

    if(EVID == "0")
        MAINDIR = c.PARAMSDIR + "/NLLOC/" + NUMBER;
    else
        MAINDIR = c.EVENTDIR + "/" + EVID + "/" + ORID + "/NLLOC";

    // find NLLoc type
    QFile file;
    file.setFileName(MAINDIR + "/type");
    if( file.open( QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line;
        line = stream.readLine();
        TYPE = line.section(":", 0, 0);
        if(TYPE == "MVM")
        {
            MINV = line.section(":", 1, 1);
            MAXV = line.section(":", 2, 2);
        }

        file.close();
    }

    if(TYPE == "SVM")
    {
        file.setFileName(MAINDIR + "/grid_p.in");
        ui->svmr->setChecked(true);
        ui->svmf->setEnabled(true);
        ui->mvmf->setEnabled(false);
        ui->fpv->setText("3.5");
        ui->lpv->setText("5.5");
    }
    else if(TYPE == "MVM")
    {
        file.setFileName(MAINDIR + "/" + MINV + "/grid_p.in" );
        ui->mvmr->setChecked(true);
        ui->svmf->setEnabled(false);
        ui->mvmf->setEnabled(true);
        ui->textEdit->setText("LAYER 0.0 4.0 0.0");
    }

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
                if(_line.startsWith("TRANS"))
                {
                    ui->latOrigLE->setText(_line.section(' ', 3, 3));
                    ui->lonOrigLE->setText(_line.section(' ', 4, 4));
                    ui->fspLE->setText(_line.section(' ', 5, 5));
                    ui->sspLE->setText(_line.section(' ', 6, 6));
                    continue;
                }

                if(_line.startsWith("VGGRID"))
                {
                    ui->vngxLE->setText(_line.section(' ', 1, 1));
                    ui->vngyLE->setText(_line.section(' ', 2, 2));
                    ui->vngzLE->setText(_line.section(' ', 3, 3));
                    ui->vogxLE->setText(_line.section(' ', 4, 4));
                    ui->vogyLE->setText(_line.section(' ', 5, 5));
                    ui->vogzLE->setText(_line.section(' ', 6, 6));
                    ui->vgsxLE->setText(_line.section(' ', 7, 7));
                    ui->vgsyLE->setText(_line.section(' ', 8, 8));
                    ui->vgszLE->setText(_line.section(' ', 9, 9));
                }

                if(_line.startsWith("LAYER") && TYPE == "SVM")
                {
                    ui->textEdit->append(_line);
                    continue;
                }

                if(_line.startsWith("GTSRCE"))
                {
                    ui->listWidget->addItem(_line);
                    continue;
                }
            }
        }
        file.close();
    }

    if(TYPE == "SVM")
        file.setFileName(MAINDIR + "/nlloc.in");
    else if(TYPE == "MVM")
        file.setFileName(MAINDIR + "/" + MINV + "/nlloc.in" );

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
                if(_line.startsWith("LOCGRID"))
                {
                    ui->ngxLE->setText(_line.section(' ', 1, 1));
                    ui->ngyLE->setText(_line.section(' ', 2, 2));
                    ui->ngzLE->setText(_line.section(' ', 3, 3));
                    ui->ogxLE->setText(_line.section(' ', 4, 4));
                    ui->ogyLE->setText(_line.section(' ', 5, 5));
                    ui->ogzLE->setText(_line.section(' ', 6, 6));
                    ui->gsxLE->setText(_line.section(' ', 7, 7));
                    ui->gsyLE->setText(_line.section(' ', 8, 8));
                    ui->gszLE->setText(_line.section(' ', 9, 9));
                }
            }
        }
    }
}

void NLLoc::defaultButtonClicked()
{
    ui->vngxLE->setText("6"); ui->vngyLE->setText("6"); ui->vngzLE->setText("2");
    ui->vogxLE->setText("-2.5"); ui->vogyLE->setText("-2.5"); ui->vogzLE->setText("-0.5");
    ui->vgsxLE->setText("1"); ui->vgsyLE->setText("1"); ui->vgszLE->setText("1");
    ui->ngxLE->setText("321"); ui->ngyLE->setText("321"); ui->ngzLE->setText("101");
    ui->ogxLE->setText("-1.6"); ui->ogyLE->setText("-1.6"); ui->ogzLE->setText("-0.5");
    ui->gsxLE->setText("0.01"); ui->gsyLE->setText("0.01"); ui->gszLE->setText("0.01");
}

void NLLoc::genButtonClicked()
{
    QFile file;

    if(ui->svmr->isChecked())
        TYPE = "SVM";
    else if(ui->mvmr->isChecked())
    {
        TYPE = "MVM";
        MINV = ui->fpv->text();
        MAXV = ui->lpv->text();
    }

    if(EVID == "0")
        MAINDIR = c.PARAMSDIR + "/NLLOC/" + NUMBER;
    else
        MAINDIR = c.EVENTDIR + "/" + EVID + "/" + ORID + "/NLLOC";

    if(TYPE == "SVM")
    {
        QString cmd = "echo SVM > " + MAINDIR + "/type"; system(cmd.toLatin1().data());
    }
    else if(TYPE == "MVM")
    {
        QString cmd = "echo MVM:" + MINV + ":" + MAXV + " > " + MAINDIR + "/type"; system(cmd.toLatin1().data());
    }

    if(TYPE == "SVM")
    {
        QDir d;
        d.setPath(MAINDIR + "/model");
        if(!d.exists())
            d.mkpath(".");
        d.setPath(MAINDIR + "/time");
        if(!d.exists())
            d.mkpath(".");

        file.setFileName(MAINDIR + "/grid_p.in");
        if( file.open( QIODevice::WriteOnly ) )
        {
            QTextStream stream( &file ) ;
            stream << "CONTROL 1 54321" << "\n";
            stream << "TRANS  LAMBERT  WGS-84  " << ui->latOrigLE->text() << " " <<
                      ui->lonOrigLE->text() << "  " << ui->fspLE->text() << " " <<
                      ui->sspLE->text() << "  0.0" << "\n";

            if(EVID == "0")
                stream << "VGOUT " + c.EVENTDIR + "/EVID/ORID/NLLOC/model/layer" << "\n";
            else
                stream << "VGOUT " + MAINDIR + "/model/layer" << "\n";

            stream << "VGTYPE P" << "\n";
            stream << "VGGRID  " << ui->vngxLE->text() << " " << ui->vngyLE->text() << " " <<
                      ui->vngzLE->text() << " " << ui->vogxLE->text() << " " << ui->vogyLE->text() <<
                      " " << ui->vogzLE->text() << " " << ui->vgsxLE->text() << " " <<
                      ui->vgsyLE->text() << " " << ui->vgszLE->text() << "  SLOW_LEN" << "\n";
            stream << ui->textEdit->toPlainText() << "\n";

            if(EVID == "0")
            {
                stream << "GTFILES  " + c.EVENTDIR + "/EVID/ORID/NLLOC/model/layer " + c.EVENTDIR +
                          "/EVID/ORID/NLLOC/time/layer P" << "\n";
            }
            else
            {
                stream << "GTFILES  " + MAINDIR + "/model/layer " + MAINDIR + "/time/layer P" << "\n";
            }

            stream << "GTMODE GRID3D ANGLES_NO" << "\n";
            int count = ui->listWidget->count();
            for(int i=0;i<count;i++)
            {
                stream << ui->listWidget->item(i)->text() << "\n";
            }

            stream << "GT_PLFD  1.0e-3  0" << "\n";

            file.close();
        }

        file.setFileName(MAINDIR + "/nlloc.in");
        if( file.open( QIODevice::WriteOnly ) )
        {
            QTextStream stream( &file ) ;

            stream << "CONTROL 1 54321" << "\n";
            stream << "TRANS  LAMBERT  WGS-84  " << ui->latOrigLE->text() << " " <<
                      ui->lonOrigLE->text() << "  " << ui->fspLE->text() << " " <<
                      ui->sspLE->text() << "  0.0" << "\n";
            stream << "LOCSIG  KGminer" << "\n";
            stream << "LOCCOM KGminer" << "\n";
            stream << "LOCMETH  EDT_OT_WT 1.0e6 2 25  -1 -1.80 6 -1.0 0" << "\n";
            stream << "LOCHYPOUT  SAVE_NLLOC_ALL SAVE_HYPOINVERSE_Y2000_ARC" << "\n";
            stream << "LOCGAU 0.5 0.0" << "\n";
            stream << "LOCGAU2 0.02 0.1 2.0" << "\n";
            stream << "LOCPHASEID  P   P p G PN Pn P* Pg PG Pb PB" << "\n";
            stream << "LOCPHASEID  S   S s G SN Sn S* Sg SG Sb SB" << "\n";
            stream << "LOCQUAL2ERR 0.1 0.2 0.5 1.0 99999.9" << "\n";
            stream << "LOCSEARCH  OCT 10 10 4 0.01 10000 5000 0 1" << "\n";
            stream << "LOCGRID  " << ui->ngxLE->text() << " " << ui->ngyLE->text() << " " <<
                      ui->ngzLE->text() << " " << ui->ogxLE->text() << " " << ui->ogyLE->text() <<
                      " " << ui->ogzLE->text() << " " << ui->gsxLE->text() << " " <<
                      ui->gsyLE->text() << " " << ui->gszLE->text() << "  PROB_DENSITY SAVE" << "\n";
            stream << "LOCPHSTAT 9999.0 -1 9999.0 1.0 1.0 9999.9 -9999.9 9999.9" << "\n";
            stream << "LOCANGLES ANGLES_NO 5" << "\n";
            stream << "LOCMAG  ML_HB 1.0 1.110 0.00189" << "\n";
            stream << "LOCELEVCORR 1 5.8 3.46" << "\n";
            stream << "LOCSTAWT 0 -1.0" << "\n";
            if(EVID == "0")
            {
                stream << "LOCFILES " << c.EVENTDIR << "/EVID/ORID/picklist HYPOINVERSE_Y2000_ARC " << c.EVENTDIR << "/EVID/ORID/NLLOC/time/layer "
                      << c.EVENTDIR << "/EVID/ORID/LOC/NLLOC" << "\n";
            }
            else
            {
                stream << "LOCFILES " << c.EVENTDIR << "/" << EVID << "/" << ORID << "/picklist HYPOINVERSE_Y2000_ARC " << MAINDIR << "/time/layer "
                      << c.EVENTDIR << "/" << EVID << "/" << ORID << "/LOC/NLLOC" << "\n";
            }
            stream << "LOCHYPOUT SAVE_HYPOINVERSE_Y2000_ARC" << "\n";

            file.close();
        }
    }
    else if(TYPE == "MVM")
    {
        float minv = MINV.toFloat(); float maxv = MAXV.toFloat();
        for(float f=minv;f<=maxv;f=f+0.1)
        {
            QString VEL;
            VEL = VEL.setNum(f, 'f', 1);

            QDir d;
            d.setPath(MAINDIR + "/" + VEL + "/model");
            if(!d.exists())
                d.mkpath(".");
            d.setPath(MAINDIR + "/" + VEL + "/time");
            if(!d.exists())
                d.mkpath(".");

            file.setFileName(MAINDIR + "/" + VEL + "/grid_p.in");
            if( file.open( QIODevice::WriteOnly ) )
            {
                QTextStream stream( &file ) ;
                stream << "CONTROL 1 54321" << "\n";
                stream << "TRANS  LAMBERT  WGS-84  " << ui->latOrigLE->text() << " " <<
                          ui->lonOrigLE->text() << "  " << ui->fspLE->text() << " " <<
                          ui->sspLE->text() << "  0.0" << "\n";

                if(EVID == "0")
                    stream << "VGOUT " + c.EVENTDIR + "/EVID/ORID/NLLOC/" + VEL + "/model/layer" << "\n";
                else
                    stream << "VGOUT " + MAINDIR + "/" + VEL + "/model/layer" << "\n";

                stream << "VGTYPE P" << "\n";
                stream << "VGGRID  " << ui->vngxLE->text() << " " << ui->vngyLE->text() << " " <<
                          ui->vngzLE->text() << " " << ui->vogxLE->text() << " " << ui->vogyLE->text() <<
                          " " << ui->vogzLE->text() << " " << ui->vgsxLE->text() << " " <<
                          ui->vgsyLE->text() << " " << ui->vgszLE->text() << "  SLOW_LEN" << "\n";
                stream << "LAYER 0.0 " << VEL << " 0.0" << "\n";

                if(EVID == "0")
                {
                    stream << "GTFILES  " + c.EVENTDIR + "/EVID/ORID/NLLOC/" + VEL + "/model/layer " + c.EVENTDIR +
                              "/EVID/ORID/NLLOC/" + VEL + "/time/layer P" << "\n";
                }
                else
                {
                    stream << "GTFILES  " + MAINDIR + "/" + VEL + "/model/layer " + MAINDIR + "/" + VEL + "/time/layer P" << "\n";
                }

                stream << "GTMODE GRID3D ANGLES_NO" << "\n";
                int count = ui->listWidget->count();
                for(int i=0;i<count;i++)
                {
                    stream << ui->listWidget->item(i)->text() << "\n";
                }

                stream << "GT_PLFD  1.0e-3  0" << "\n";

                file.close();
            }
            file.setFileName(MAINDIR + "/" + VEL + "/nlloc.in");
            if( file.open( QIODevice::WriteOnly ) )
            {
                QTextStream stream( &file ) ;

                stream << "CONTROL 1 54321" << "\n";
                stream << "TRANS  LAMBERT  WGS-84  " << ui->latOrigLE->text() << " " <<
                          ui->lonOrigLE->text() << "  " << ui->fspLE->text() << " " <<
                          ui->sspLE->text() << "  0.0" << "\n";
                stream << "LOCSIG  KGminer" << "\n";
                stream << "LOCCOM KGminer" << "\n";
                stream << "LOCMETH  EDT_OT_WT 1.0e6 2 25  -1 -1.80 6 -1.0 0" << "\n";
                stream << "LOCHYPOUT  SAVE_NLLOC_ALL SAVE_HYPOINVERSE_Y2000_ARC" << "\n";
                stream << "LOCGAU 0.5 0.0" << "\n";
                stream << "LOCGAU2 0.02 0.1 2.0" << "\n";
                stream << "LOCPHASEID  P   P p G PN Pn P* Pg PG Pb PB" << "\n";
                stream << "LOCPHASEID  S   S s G SN Sn S* Sg SG Sb SB" << "\n";
                stream << "LOCQUAL2ERR 0.1 0.2 0.5 1.0 99999.9" << "\n";
                stream << "LOCSEARCH  OCT 10 10 4 0.01 10000 5000 0 1" << "\n";
                stream << "LOCGRID  " << ui->ngxLE->text() << " " << ui->ngyLE->text() << " " <<
                          ui->ngzLE->text() << " " << ui->ogxLE->text() << " " << ui->ogyLE->text() <<
                          " " << ui->ogzLE->text() << " " << ui->gsxLE->text() << " " <<
                          ui->gsyLE->text() << " " << ui->gszLE->text() << "  PROB_DENSITY SAVE" << "\n";
                stream << "LOCPHSTAT 9999.0 -1 9999.0 1.0 1.0 9999.9 -9999.9 9999.9" << "\n";
                stream << "LOCANGLES ANGLES_NO 5" << "\n";
                stream << "LOCMAG  ML_HB 1.0 1.110 0.00189" << "\n";
                stream << "LOCELEVCORR 1 5.8 3.46" << "\n";
                stream << "LOCSTAWT 0 -1.0" << "\n";

                if(EVID == "0")
                {
                    stream << "LOCFILES " + c.EVENTDIR + "/EVID/ORID/picklist HYPOINVERSE_Y2000_ARC " + c.EVENTDIR + "/EVID/ORID/NLLOC/"
                              + VEL + "/time/layer " + c.EVENTDIR + "/EVID/ORID/LOC/NLLOC_" + VEL << "\n";
                }
                else
                {
                    stream << "LOCFILES " + c.EVENTDIR + "/" + EVID + "/" + ORID + "/picklist HYPOINVERSE_Y2000_ARC " + MAINDIR + "/" + VEL +
                           "/time/layer " + c.EVENTDIR + "/" + EVID + "/" + ORID + "/LOC/NLLOC_" + VEL << "\n";
                }
                stream << "LOCHYPOUT SAVE_HYPOINVERSE_Y2000_ARC" << "\n";

                file.close();
            }
        }
    }

    QMessageBox msgBox;
    if(!korean) msgBox.setText("Completed file generation.");
    else msgBox.setText(codec->toUnicode("설정 값 변경 완료."));
    msgBox.exec();

    accept();
}
