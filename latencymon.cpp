#include "latencymon.h"
#include "ui_latencymon.h"

LatencyMon::LatencyMon(QWidget *parent) :
    QFrame(parent),
    LatencyMon_ui(new Ui::LatencyMon)
{
    LatencyMon_ui->setupUi(this);

    rvMsgSock_port = LATENCYSOCKNUM ;

    rvMsgSock = new QUdpSocket(this);
    localhost.setAddress( "127.0.0.1" ) ;

    if( rvMsgSock->bind( localhost, rvMsgSock_port ) )
    {
        connect(rvMsgSock, SIGNAL(readyRead()), this, SLOT(rvMsg()));
    }
    else
        qWarning("rv Latency Sock bind Failure");

    /* set Table */    
    LatencyMon_ui->LatencyTable->clear();
    LatencyMon_ui->LatencyTable->horizontalHeader()->setStyleSheet("QHeaderView { font-size:8pt; }");
    LatencyMon_ui->LatencyTable->setHorizontalHeaderItem(0, new QTableWidgetItem("SCNL"));
    LatencyMon_ui->LatencyTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Start Time"));
    LatencyMon_ui->LatencyTable->setHorizontalHeaderItem(2, new QTableWidgetItem("End Time"));
    //LatencyMon_ui->LatencyTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Latency"));
    LatencyMon_ui->LatencyTable->setRowCount(0);
    LatencyMon_ui->LatencyTable->setColumnWidth(0, 100);
    LatencyMon_ui->LatencyTable->setColumnWidth(1, 130);
    LatencyMon_ui->LatencyTable->setColumnWidth(2, 130);
    //LatencyMon_ui->LatencyTable->setColumnWidth(3, 80);

    QFile file(PARAMSDIR + "/sta.info");
    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&file);
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            if(_line.startsWith("#") || _line.isNull() || _line.startsWith("[") || _line.startsWith("Desc") || _line.startsWith("File"))
                continue;
            else
            {
                staName << _line.section(' ', 0, 0);
            }
        }
        file.close();
    }
}

LatencyMon::~LatencyMon()
{
    delete LatencyMon_ui;
    rvMsgSock->close();
}

void LatencyMon::setup()
{
    /* set Table */
    LatencyMon_ui->LatencyTable->clear();
    LatencyMon_ui->LatencyTable->horizontalHeader()->setStyleSheet("QHeaderView { font-size:8pt; }");
    LatencyMon_ui->LatencyTable->setHorizontalHeaderItem(0, new QTableWidgetItem("SCNL"));
    LatencyMon_ui->LatencyTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Start Time"));
    LatencyMon_ui->LatencyTable->setHorizontalHeaderItem(2, new QTableWidgetItem("End Time"));
    //LatencyMon_ui->LatencyTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Latency"));
    LatencyMon_ui->LatencyTable->setRowCount(0);
    LatencyMon_ui->LatencyTable->setColumnWidth(0, 100);
    LatencyMon_ui->LatencyTable->setColumnWidth(1, 130);
    LatencyMon_ui->LatencyTable->setColumnWidth(2, 130);
    //LatencyMon_ui->LatencyTable->setColumnWidth(3, 80);
}

void LatencyMon::rvMsg()
{
     while (rvMsgSock->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(rvMsgSock->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        rvMsgSock->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);
        processTheDatagram(datagram);
    }
}

void LatencyMon::processTheDatagram(QByteArray datagram)
{
    TRACE2_HEADER_TEMP data;
    memcpy( &data, datagram.data(), datagram.size() );

    sta = QString::fromStdString(data.sta);
    scnl = QString::fromStdString(data.sta)  + "." + QString::fromStdString(data.chan) + "." +
            QString::fromStdString(data.net) + "." + QString::fromStdString(data.loc);

    QDateTime dts, dte;
    dts.setTime_t( uint( data.starttime) );
    dts = dts.toUTC();
    dte.setTime_t( uint (data.endtime) );
    dte = dte.toUTC();

    starttime = dts.toString("yyyy/MM/dd hh:mm:ss");
    endtime = dte.toString("yyyy/MM/dd hh:mm:ss");

    for(int i=0;i<staName.length();i++)
    {
        if(sta.startsWith(staName[i]))
        {
            bool founded = false;
            int whererow = 0;

            for(int j=0;j<LatencyMon_ui->LatencyTable->rowCount();j++)
            {
                if(LatencyMon_ui->LatencyTable->item(j, 0)->text().startsWith(staName[i]))
                {
                    founded = true;
                    whererow = j;
                    break;
                }
            }

            if(!founded)  // didn't find
            {
                LatencyMon_ui->LatencyTable->setRowCount(LatencyMon_ui->LatencyTable->rowCount()+1);
                LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 0, new QTableWidgetItem(scnl));
                LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 1, new QTableWidgetItem(starttime));
                LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 2, new QTableWidgetItem(endtime));
                //atencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 3, new QTableWidgetItem(latencySecond));
            }
            else
            {
                LatencyMon_ui->LatencyTable->setItem(whererow, 0, new QTableWidgetItem(scnl));
                LatencyMon_ui->LatencyTable->setItem(whererow, 1, new QTableWidgetItem(starttime));
                LatencyMon_ui->LatencyTable->setItem(whererow, 2, new QTableWidgetItem(endtime));
                //LatencyMon_ui->LatencyTable->setItem(whererow, 3, new QTableWidgetItem(latencySecond));
            }
            for(int i=0;i<LatencyMon_ui->LatencyTable->rowCount();i++)
            {
                for(int j=0;j<LatencyMon_ui->LatencyTable->columnCount();j++)
                {
                    LatencyMon_ui->LatencyTable->item(i,j)->setTextAlignment(Qt::AlignCenter);
                }
            }
            //findLatency();
            break;
        }
    }
}

void LatencyMon::findLatency()
{    
    /*
    QDateTime now = QDateTime::currentDateTime();
    now = now.toUTC();

    for(int i=0;i<LatencyMon_ui->LatencyTable->rowCount();i++)
    {
        QDateTime date;
        QString d = LatencyMon_ui->LatencyTable->item(i, 2)->text();
        date = QDateTime::fromString(d,"yyyy/MM/dd HH:mm:ss");
        //int diff = now.time().secsTo(date.time());
        int diff = now.toMSecsSinceEpoch() - date.toMSecsSinceEpoch();

        latencySecond = latencySecond.setNum(diff, 10);
        LatencyMon_ui->LatencyTable->setItem(i, 3, new QTableWidgetItem(latencySecond));
    }

    */
}




