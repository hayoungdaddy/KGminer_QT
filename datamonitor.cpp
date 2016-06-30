#include "datamonitor.h"
#include "ui_datamonitor.h"

DataMonitor::DataMonitor(CFG cfg, bool _korean, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataMonitor)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName( "utf8" );
    korean = _korean;

    c = cfg;

    ui->tableWidget->setRowCount(1);

    for(int i=0;i<24;i++)
        ui->tableWidget->setItem(0, i, new QTableWidgetItem(""));

    ui->pb->setRange(0, 100);
    ui->pb->setValue(0);

    if(korean)
        setLanguageKo();
    else
        setLanguageEn();

    connect(ui->calendarWidget, SIGNAL(currentPageChanged(int,int)), this, SLOT(changePage(int, int)));
    connect(ui->calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(clickCalendar(QDate)));
    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
}

DataMonitor::~DataMonitor()
{
    delete ui;
}

void DataMonitor::setLanguageEn()
{
    setWindowTitle("Data availability monitor");
    ui->groupBox->setTitle("Calendar");
    ui->groupBox2->setTitle("Data availability (each hour)");
    ui->quitButton->setText("Quit");
}

void DataMonitor::setLanguageKo()
{
    setWindowTitle(codec->toUnicode("지진 데이터 가용성 모니터링"));
    ui->groupBox->setTitle(codec->toUnicode("달력"));
    ui->groupBox2->setTitle(codec->toUnicode("데이터 가용성 (매 시)"));
    ui->quitButton->setText(codec->toUnicode("종료"));
}

void DataMonitor::changePage(int year, int month)
{
    QString yearS = QString::number(year);
    QString monS = QString::number(month);

    if(month < 10)
        monS = "0" + monS;

    QDate firstDayofMonth = QDate::fromString(yearS + monS + "01", "yyyyMMdd");
    int lastday = firstDayofMonth.daysInMonth();
    QDate lastDayofMonth = QDate::fromString(yearS + monS + QString::number(lastday), "yyyyMMdd");

    int firstjday, lastjday;
    firstjday = firstDayofMonth.dayOfYear();
    lastjday = lastDayofMonth.dayOfYear();

    int day = 1;

    for(int i=firstjday;i<=lastjday;i++)
    {
        QDate date;
        date.setDate(firstDayofMonth.year(), firstDayofMonth.month(), day);
        day++;

        QString cmd;
        QString temp;
        temp = QString::number(i);
        if(temp.count() == 1) temp = "00" + temp; else if(temp.count() == 2) temp = "0" + temp;
        cmd = "find " + c.MSEEDDIR + " | grep " + QString::number(firstDayofMonth.year()) + "." + temp + " > " + c.TMPDIR + "/findFull";
        system(cmd.toLatin1().data());

        QFile file;
        file.setFileName(c.TMPDIR + "/findFull");

        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            QString line;

            while(!stream.atEnd())
            {
                line = stream.readLine();
                line = line.right(3);

                if(line == temp)
                {
                    QBrush brush;
                    brush.setColor( Qt::green );
                    QTextCharFormat cf = ui->calendarWidget->dateTextFormat( date );
                    cf.setBackground( brush );
                    ui->calendarWidget->setDateTextFormat( date, cf );

                    break;
                }
            }
            file.close();
        }
    }
}

void DataMonitor::clickCalendar(QDate date)
{
    ui->pb->setRange(0, 5);
    ui->pb->setValue(0);

    for(int i=0;i<24;i++)
    {
        ui->tableWidget->setColumnWidth(i, 50);
    }

    sleep(0.5);

    ui->pb->setValue(1);

    QString cmd;
    cmd = "rm " + c.TMPDIR + "/msview.rt";
    system(cmd.toLatin1().data());

    for(int i=0;i<24;i++)
        ui->tableWidget->setItem(0, i, new QTableWidgetItem(""));

    QDate selDate;
    selDate = date;
    QString selYear = QString::number(selDate.year());

    int seljday = selDate.dayOfYear();

    ui->pb->setValue(2);

    QString temp;
    temp = QString::number(seljday);
    if(temp.count() == 1) temp = "00" + temp; else if(temp.count() == 2) temp = "0" + temp;

    cmd = c.SCRIPTDIR + "/findDataAbility.sh " + selYear + " " + temp;
    system(cmd.toLatin1().data());

    ui->pb->setValue(3);

    QFile file;
    file.setFileName(c.TMPDIR + "/msview.rt");
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line, _line;

        sleep(1);
        ui->pb->setValue(4);

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();
            int hour = _line.section(' ', 1, 1).toInt();
            QString sta = _line.section(' ', 0, 0);
            //qDebug() << sta + " " + QString::number(hour);

            QString temp = ui->tableWidget->item(0, hour)->text();

            temp = sta + "\n" + temp;

            ui->tableWidget->setItem(0, hour, new QTableWidgetItem(temp));
            ui->tableWidget->item(0, hour)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setColumnWidth(hour, 50);
            //ui->tableWidget->item(0, hour)->setSelected(true);
        }
        file.close();
    }

    ui->pb->setValue(5);
}
