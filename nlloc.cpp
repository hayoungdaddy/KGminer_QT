#include "nlloc.h"
#include "ui_nlloc.h"

NLLoc::NLLoc(QString evid, QString orid, QString num, QWidget *parent) :
    QDialog(parent),
    NLLoc_ui(new Ui::NLLoc)
{
    NLLoc_ui->setupUi(this);
    //codec = QTextCodec::codecForName( "utf8" );
    EVID = evid;
    ORID = orid;
    NUMBER = num;

    connect(NLLoc_ui->svmr, SIGNAL(clicked()), this, SLOT(svmbClicked()));
    connect(NLLoc_ui->mvmr, SIGNAL(clicked()), this, SLOT(mvmbClicked()));
    connect(NLLoc_ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(NLLoc_ui->genButton, SIGNAL(clicked()), this, SLOT(genButtonClicked()));
}

NLLoc::~NLLoc()
{
    delete NLLoc_ui;
}

void NLLoc::svmbClicked()
{
    NLLoc_ui->svmf->setEnabled(true);
    NLLoc_ui->mvmf->setEnabled(false);
}

void NLLoc::mvmbClicked()
{
    NLLoc_ui->mvmf->setEnabled(true);
    NLLoc_ui->svmf->setEnabled(false);
}

void NLLoc::setup()
{
    if(EVID != "0")
    {
        MAINDIR = EVENTDIR + "/" + EVID + "/" + ORID + "/NLLOC";
    }
    else
    {
        MAINDIR = PARAMSDIR + "/NLLOC/" + NUMBER;
    }
    qDebug() << MAINDIR;

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
        NLLoc_ui->svmr->setChecked(true);
        NLLoc_ui->svmf->setEnabled(true);
        NLLoc_ui->mvmf->setEnabled(false);
        NLLoc_ui->fpv->setText("3.5");
        NLLoc_ui->lpv->setText("5.5");
    }
    else if(TYPE == "MVM")
    {
        file.setFileName(MAINDIR + "/" + MINV + "/grid_p.in" );
        NLLoc_ui->mvmr->setChecked(true);
        NLLoc_ui->svmf->setEnabled(false);
        NLLoc_ui->mvmf->setEnabled(true);
        NLLoc_ui->textEdit->setText("LAYER 0.0 4.0 0.0");
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
                    NLLoc_ui->latOrigLE->setText(_line.section(' ', 3, 3));
                    NLLoc_ui->lonOrigLE->setText(_line.section(' ', 4, 4));
                    NLLoc_ui->fspLE->setText(_line.section(' ', 5, 5));
                    NLLoc_ui->sspLE->setText(_line.section(' ', 6, 6));
                    continue;
                }

                if(_line.startsWith("VGGRID"))
                {
                    NLLoc_ui->vngxLE->setText(_line.section(' ', 1, 1));
                    NLLoc_ui->vngyLE->setText(_line.section(' ', 2, 2));
                    NLLoc_ui->vngzLE->setText(_line.section(' ', 3, 3));
                    NLLoc_ui->vogxLE->setText(_line.section(' ', 4, 4));
                    NLLoc_ui->vogyLE->setText(_line.section(' ', 5, 5));
                    NLLoc_ui->vogzLE->setText(_line.section(' ', 6, 6));
                    NLLoc_ui->vgsxLE->setText(_line.section(' ', 7, 7));
                    NLLoc_ui->vgsyLE->setText(_line.section(' ', 8, 8));
                    NLLoc_ui->vgszLE->setText(_line.section(' ', 9, 9));
                }

                if(_line.startsWith("LAYER") && TYPE == "SVM")
                {
                    NLLoc_ui->textEdit->append(_line);
                    continue;
                }

                if(_line.startsWith("GTSRCE"))
                {
                    NLLoc_ui->listWidget->addItem(_line);
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
                    NLLoc_ui->ngxLE->setText(_line.section(' ', 1, 1));
                    NLLoc_ui->ngyLE->setText(_line.section(' ', 2, 2));
                    NLLoc_ui->ngzLE->setText(_line.section(' ', 3, 3));
                    NLLoc_ui->ogxLE->setText(_line.section(' ', 4, 4));
                    NLLoc_ui->ogyLE->setText(_line.section(' ', 5, 5));
                    NLLoc_ui->ogzLE->setText(_line.section(' ', 6, 6));
                    NLLoc_ui->gsxLE->setText(_line.section(' ', 7, 7));
                    NLLoc_ui->gsyLE->setText(_line.section(' ', 8, 8));
                    NLLoc_ui->gszLE->setText(_line.section(' ', 9, 9));
                }
            }
        }
    }
}

void NLLoc::genButtonClicked()
{
    QFile file;

    if(NLLoc_ui->svmr->isChecked())
        TYPE = "SVM";
    else if(NLLoc_ui->mvmr->isChecked())
    {
        TYPE = "MVM";
        MINV = NLLoc_ui->fpv->text();
        MAXV = NLLoc_ui->lpv->text();
    }

    if(EVID != "0")
    {
        MAINDIR = EVENTDIR + "/" + EVID + "/" + ORID + "/NLLOC";
    }
    else
    {
        MAINDIR = PARAMSDIR + "/NLLOC/" + NUMBER;
    }

    file.setFileName(MAINDIR + "/grid_p.in");

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
        QString cmd;
        cmd = "mkdir " + MAINDIR + "/model";
        system(cmd.toLatin1().data());
        cmd = "mkdir " + MAINDIR + "/time";
        system(cmd.toLatin1().data());

        file.setFileName(MAINDIR + "/grid_p.in");
        if( file.open( QIODevice::WriteOnly ) )
        {
            QTextStream stream( &file ) ;
            stream << "CONTROL 1 54321" << "\n";
            stream << "TRANS  LAMBERT  WGS-84  " << NLLoc_ui->latOrigLE->text() << " " <<
                      NLLoc_ui->lonOrigLE->text() << "  " << NLLoc_ui->fspLE->text() << " " <<
                      NLLoc_ui->sspLE->text() << "  0.0" << "\n";

            if(EVID != "0")
            {
                stream << "VGOUT " << MAINDIR << "/model/layer" << "\n";
            }
            else
            {
                stream << "VGOUT " + EVENTDIR + "/EVID/ORID/NLLOC/model/layer" << "\n";
            }
            stream << "VGTYPE P" << "\n";
            stream << "VGGRID  " << NLLoc_ui->vngxLE->text() << " " << NLLoc_ui->vngyLE->text() << " " <<
                      NLLoc_ui->vngzLE->text() << " " << NLLoc_ui->vogxLE->text() << " " << NLLoc_ui->vogyLE->text() <<
                      " " << NLLoc_ui->vogzLE->text() << " " << NLLoc_ui->vgsxLE->text() << " " <<
                      NLLoc_ui->vgsyLE->text() << " " << NLLoc_ui->vgszLE->text() << "  SLOW_LEN" << "\n";
            stream << NLLoc_ui->textEdit->toPlainText() << "\n";

            if(EVID != "0")
            {
                stream << "GTFILES  " << MAINDIR << "/model/layer " << MAINDIR << "/time/layer P" << "\n";
            }
            else
            {
                stream << "GTFILES  " + EVENTDIR + "/EVID/ORID/NLLOC/model/layer " + EVENTDIR + "/EVID/ORID/NLLOC/time/layer P" << "\n";
            }

            stream << "GTMODE GRID3D ANGLES_NO" << "\n";
            int count = NLLoc_ui->listWidget->count();
            for(int i=0;i<count;i++)
            {
                stream << NLLoc_ui->listWidget->item(i)->text() << "\n";
            }

            stream << "GT_PLFD  1.0e-3  0" << "\n";

            file.close();
        }
        file.setFileName(MAINDIR + "/nlloc.in");
        if( file.open( QIODevice::WriteOnly ) )
        {
            QTextStream stream( &file ) ;

            stream << "CONTROL 1 54321" << "\n";
            stream << "TRANS  LAMBERT  WGS-84  " << NLLoc_ui->latOrigLE->text() << " " <<
                      NLLoc_ui->lonOrigLE->text() << "  " << NLLoc_ui->fspLE->text() << " " <<
                      NLLoc_ui->sspLE->text() << "  0.0" << "\n";
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
            stream << "LOCGRID  " << NLLoc_ui->ngxLE->text() << " " << NLLoc_ui->ngyLE->text() << " " <<
                      NLLoc_ui->ngzLE->text() << " " << NLLoc_ui->ogxLE->text() << " " << NLLoc_ui->ogyLE->text() <<
                      " " << NLLoc_ui->ogzLE->text() << " " << NLLoc_ui->gsxLE->text() << " " <<
                      NLLoc_ui->gsyLE->text() << " " << NLLoc_ui->gszLE->text() << "  PROB_DENSITY SAVE" << "\n";
            stream << "LOCPHSTAT 9999.0 -1 9999.0 1.0 1.0 9999.9 -9999.9 9999.9" << "\n";
            stream << "LOCANGLES ANGLES_NO 5" << "\n";
            stream << "LOCMAG  ML_HB 1.0 1.110 0.00189" << "\n";
            stream << "LOCELEVCORR 1 5.8 3.46" << "\n";
            stream << "LOCSTAWT 0 -1.0" << "\n";
            if(EVID != "0")
            {
                stream << "LOCFILES " << EVENTDIR << "/" << EVID << "/" << ORID << "/picklist HYPOINVERSE_Y2000_ARC " << MAINDIR << "/time/layer "
                      << EVENTDIR << "/" << EVID << "/" << ORID << "/LOC/NLLOC" << "\n";
            }
            else
            {
                stream << "LOCFILES " << EVENTDIR << "/EVID/ORID/picklist HYPOINVERSE_Y2000_ARC " << EVENTDIR << "/EVID/ORID/NLLOC/time/layer "
                      << EVENTDIR << "/EVID/ORID/LOC/NLLOC" << "\n";
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
            QString cmd = "mkdir " + MAINDIR + "/" + VEL; system(cmd.toLatin1().data());
            cmd = "mkdir " + MAINDIR + "/" + VEL + "/model"; system(cmd.toLatin1().data());
            cmd = "mkdir " + MAINDIR + "/" + VEL + "/time"; system(cmd.toLatin1().data());
            file.setFileName(MAINDIR + "/" + VEL + "/grid_p.in");
            if( file.open( QIODevice::WriteOnly ) )
            {
                QTextStream stream( &file ) ;
                stream << "CONTROL 1 54321" << "\n";
                stream << "TRANS  LAMBERT  WGS-84  " << NLLoc_ui->latOrigLE->text() << " " <<
                          NLLoc_ui->lonOrigLE->text() << "  " << NLLoc_ui->fspLE->text() << " " <<
                          NLLoc_ui->sspLE->text() << "  0.0" << "\n";
                if(EVID != "0")
                {
                    stream << "VGOUT " << MAINDIR << "/" << VEL << "/model/layer" << "\n";
                }
                else
                {
                    stream << "VGOUT " << EVENTDIR + "/EVID/ORID/NLLOC/" << VEL << "/model/layer" << "\n";
                }
                stream << "VGTYPE P" << "\n";
                stream << "VGGRID  " << NLLoc_ui->vngxLE->text() << " " << NLLoc_ui->vngyLE->text() << " " <<
                          NLLoc_ui->vngzLE->text() << " " << NLLoc_ui->vogxLE->text() << " " << NLLoc_ui->vogyLE->text() <<
                          " " << NLLoc_ui->vogzLE->text() << " " << NLLoc_ui->vgsxLE->text() << " " <<
                          NLLoc_ui->vgsyLE->text() << " " << NLLoc_ui->vgszLE->text() << "  SLOW_LEN" << "\n";
                stream << "LAYER 0.0 " << VEL << " 0.0" << "\n";
                if(EVID != "0")
                {
                    stream << "GTFILES  " << MAINDIR << "/" << VEL << "/model/layer " << MAINDIR << "/" << VEL << "/time/layer P" << "\n";
                }
                else
                {
                    stream << "GTFILES  " << EVENTDIR << + "/EVID/ORID/NLLOC/" << VEL << "/model/layer " << EVENTDIR << "/EVID/ORID/NLLOC/" << VEL << "/time/layer P" << "\n";
                }
                stream << "GTMODE GRID3D ANGLES_NO" << "\n";
                int count = NLLoc_ui->listWidget->count();
                for(int i=0;i<count;i++)
                {
                    stream << NLLoc_ui->listWidget->item(i)->text() << "\n";
                }

                stream << "GT_PLFD  1.0e-3  0" << "\n";

                file.close();
            }
            file.setFileName(MAINDIR + "/" + VEL + "/nlloc.in");
            if( file.open( QIODevice::WriteOnly ) )
            {
                QTextStream stream( &file ) ;

                stream << "CONTROL 1 54321" << "\n";
                stream << "TRANS  LAMBERT  WGS-84  " << NLLoc_ui->latOrigLE->text() << " " <<
                          NLLoc_ui->lonOrigLE->text() << "  " << NLLoc_ui->fspLE->text() << " " <<
                          NLLoc_ui->sspLE->text() << "  0.0" << "\n";
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
                stream << "LOCGRID  " << NLLoc_ui->ngxLE->text() << " " << NLLoc_ui->ngyLE->text() << " " <<
                          NLLoc_ui->ngzLE->text() << " " << NLLoc_ui->ogxLE->text() << " " << NLLoc_ui->ogyLE->text() <<
                          " " << NLLoc_ui->ogzLE->text() << " " << NLLoc_ui->gsxLE->text() << " " <<
                          NLLoc_ui->gsyLE->text() << " " << NLLoc_ui->gszLE->text() << "  PROB_DENSITY SAVE" << "\n";
                stream << "LOCPHSTAT 9999.0 -1 9999.0 1.0 1.0 9999.9 -9999.9 9999.9" << "\n";
                stream << "LOCANGLES ANGLES_NO 5" << "\n";
                stream << "LOCMAG  ML_HB 1.0 1.110 0.00189" << "\n";
                stream << "LOCELEVCORR 1 5.8 3.46" << "\n";
                stream << "LOCSTAWT 0 -1.0" << "\n";
                if(EVID != "0")
                {
                    stream << "LOCFILES " << EVENTDIR << "/" << EVID << "/" << ORID << "/picklist HYPOINVERSE_Y2000_ARC " << MAINDIR << "/" << VEL << "/time/layer "
                       << EVENTDIR << "/" << EVID << "/" << ORID << "/LOC/NLLOC_" << VEL << "\n";
                }
                else
                {
                    stream << "LOCFILES " << EVENTDIR << "/EVID/ORID/picklist HYPOINVERSE_Y2000_ARC " << EVENTDIR << "/EVID/ORID/NLLOC/" << VEL << "/time/layer "
                       << EVENTDIR << "/EVID/ORID/LOC/NLLOC_" << VEL << "\n";
                }
                stream << "LOCHYPOUT SAVE_HYPOINVERSE_Y2000_ARC" << "\n";

                file.close();
            }
        }
    }

    QMessageBox msgBox;
    msgBox.setText("Completed file generation.");
    msgBox.exec();
    accept();
}
