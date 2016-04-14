#ifndef LATENCYMON_H
#define LATENCYMON_H

#include <QFrame>
#include <QUdpSocket>
#include <QDateTime>

#include "common.h"

typedef struct {
        int     pinno;
        int     nsamp;
        double  starttime;
        double  endtime;
        double  samprate;
        char    sta[7];
        char    net[9];
        char    chan[4];
        char    loc[3];
        char    version[2];
        char    datatype[3];
        char    quality[2];
        char    pad[2];
} TRACE2_HEADER_TEMP;

namespace Ui {
class LatencyMon;
}

class LatencyMon : public QFrame
{
    Q_OBJECT
    
public:
    explicit LatencyMon(CFG cfg, QWidget *parent = 0);
    ~LatencyMon();    

    void setup();

    CFG c;

public slots:

private:
    Ui::LatencyMon *LatencyMon_ui;

    QString sta;
    QString scnl;
    QString nsamp;
    QString pinno;
    QString samprate;
    QString starttime, endtime;
    QString dtype;
    QString latencySecond;
    QString packetLatencySecond;
    QStringList staName;

    QUdpSocket *rvMsgSock;
    int rvMsgSock_port ;
    QHostAddress localhost ;
    void processTheDatagram(QByteArray);
    void findLatency();

private slots:
    void rvMsg();
};

#endif // LATENCYMON_H
