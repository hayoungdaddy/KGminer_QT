#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    QPixmap pix("/opt/KGminer/params/CI.jpg");
    ui->label->setPixmap(pix);
    ui->label->setScaledContents(true);
}

About::~About()
{
	delete ui;
}

void About::on_quitButton_clicked()
{
    accept();
}
