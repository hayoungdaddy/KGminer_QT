#include "filterpicker.h"
#include "ui_filterpicker.h"

FilterPicker::FilterPicker(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent)
{
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;

    c = cfg;

    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    QHBoxLayout *labellayout = new QHBoxLayout;
    layout = new QVBoxLayout;
    QHBoxLayout *buttonlayout = new QHBoxLayout;

    if(korean)
        setWindowTitle(codec->toUnicode("Filter Picker 설정 값 변경"));
    else
        setWindowTitle("Config filterPicker parameters.");

    QLabel *label = new QLabel[6];
    label[0].setText("S/C/N Name");
    label[1].setText("filterWindow");
    label[2].setText("longTermWindow");
    label[3].setText("threshold1");
    label[4].setText("threshold2");
    label[5].setText("tUpEvent");

    for(int i=0;i<6;i++)
    {
        label[i].setAlignment(Qt::AlignCenter);
        labellayout->addWidget(&label[i]);
    }

    QPushButton *okButton = new QPushButton;
    QPushButton *noButton = new QPushButton;

    if(korean)
    {
        okButton->setText(codec->toUnicode("변경"));
        noButton->setText(codec->toUnicode("취소"));
    }
    else
    {
        okButton->setText("Generate");
        noButton->setText("Cancel");
    }

    buttonlayout->addWidget(okButton);
    buttonlayout->addWidget(noButton);
    buttonlayout->setAlignment(Qt::AlignBottom);
    mainlayout->addLayout(labellayout);
    mainlayout->addLayout(layout);
    mainlayout->addLayout(buttonlayout);

    QFile file(c.PARAMSDIR + "/pick_FP.KGminer");
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
                staName << _line.section(' ', 2, 2);
                chanName << _line.section(' ', 3, 3);
                netName << _line.section(' ', 4, 4);
                locName << _line.section(' ', 5, 5);
                fw << _line.section(' ', 6, 6);
                lw << _line.section(' ', 7, 7);
                th1 << _line.section(' ', 8, 8);
                th2 << _line.section(' ', 9, 9);
                te << _line.section(' ', 10, 10);
            }
        }

        file.close();
    }

    int count = staName.count();
    for(int i=0;i<count;i++)
    {
        addLineEdit(i);
    }

    connect(okButton, SIGNAL(clicked()), this, SLOT(genButtonClicked()));
    connect(noButton, SIGNAL(clicked()), this, SLOT(accept()));
}

FilterPicker::~FilterPicker()
{
}

void FilterPicker::addLineEdit(int count)
{
    QHBoxLayout *hlayout =new QHBoxLayout;
    QLineEdit* le = new QLineEdit[6];
    qlist.push_back(&le[0]);

    for(int i=0;i<6;i++)
    {
        hlayout->addWidget(&le[i]);
        le[i].setObjectName("lineedit_" + QString::number(qlist.count()) + "_" + QString::number(i+1) );
        le[i].setAlignment(Qt::AlignCenter);
        connect(&le[i], SIGNAL(textChanged(QString)), SLOT(lineedit_textChanged(QString)));
    }
    le[0].setText(staName[count] + " " + chanName[count] + " " + netName[count] + " " + locName[count]);
    le[0].setEnabled(false);
    le[1].setText(fw[count]);
    le[2].setText(lw[count]);
    le[3].setText(th1[count]);
    le[4].setText(th2[count]);
    le[5].setText(te[count]);

    layout->addLayout(hlayout);
}

void FilterPicker::lineedit_textChanged(QString text)
{
    QObject *obj = QObject::sender();
    QString senderobjName = obj->objectName();

    int i = senderobjName.section('_', 1, 1).toInt() - 1;
    int j = senderobjName.section('_', 2, 2).toInt() - 1;

    if(j == 1) fw[i] = text;
    else if(j == 2) lw[i] = text;
    else if(j == 3) th1[i] = text;
    else if(j == 4) th2[i] = text;
    else if(j == 5) te[i] = text;
}

void FilterPicker::genButtonClicked()
{
    QFile file;
    file.setFileName(c.PARAMSDIR + "/pick_FP.KGminer");

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;
        for(int i=0;i<staName.count();i++)
        {
            stream << "1 " << QString::number(i+1) << " " << staName[i] << " " << chanName[i] << " " << netName[i] << " " << locName[i] << " " <<
                      fw[i] << " " << lw[i] << " " << th1[i] << " " << th2[i] << " " << te[i] << "\n";
        }

        file.close() ;
    }

    QMessageBox msgBox;
    if(korean)
        msgBox.setText(codec->toUnicode("설정 값 변경 완료"));
    else
        msgBox.setText("Completed a file generation.");
    msgBox.exec();

    accept();
}
