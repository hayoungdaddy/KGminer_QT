#include "changelocator.h"
#include "ui_changelocator.h"

ChangeLocator::ChangeLocator(QString loc1, QString loc2, QString loc3, QWidget *parent) :
    QDialog(parent),
    ChangeLocator_ui(new Ui::ChangeLocator)
{
    ChangeLocator_ui->setupUi(this);
    //codec = QTextCodec::codecForName( "utf8" );

    int i1, i2, i3;
    if(loc1.startsWith("N")) i1 = 0; else i1 = 1;
    if(loc2.startsWith("N")) i2 = 0; else i2 = 1;
    if(loc3.startsWith("N")) i3 = 0; else i3 = 1;

    ChangeLocator_ui->loc1->setCurrentIndex(i1);
    ChangeLocator_ui->loc2->setCurrentIndex(i2);
    ChangeLocator_ui->loc3->setCurrentIndex(i3);

    connect(ChangeLocator_ui->genButton, SIGNAL(clicked()), this, SLOT(genButtonClicked()));
    connect(ChangeLocator_ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
}

ChangeLocator::~ChangeLocator()
{
    delete ChangeLocator_ui;
}

void ChangeLocator::genButtonClicked()
{
    int i1, i2, i3;
    i1 = ChangeLocator_ui->loc1->currentIndex();
    i2 = ChangeLocator_ui->loc2->currentIndex();
    i3 = ChangeLocator_ui->loc3->currentIndex();
    emit changeloc(i1, i2, i3);
    accept();
}
