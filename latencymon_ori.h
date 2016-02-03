#ifndef LATENCYMON_H
#define LATENCYMON_H

#include <QFrame>

#include "util.h"

extern "C" {
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <math.h>
    #include <transport.h>
    #include <swap.h>
    #include <time_ew.h>
    #include <trace_buf.h>
    #include <earthworm.h>
    #include <libmseed.h>
    #include <sys/sem.h>
}

#define NLOGO 5
#define VERSION "2013-06-24 2.5.1"

#define TYPE_NOTUSED 254
#define MAX_LEN_STR_SCNL 30

/* Structure for storing channel information, last scnl time and last packet time */
typedef struct {
    char scnl[MAX_LEN_STR_SCNL];
    double last_scnl_time;
    double last_packet_time;
} SCNL_LAST_SCNL_PACKET_TIME;

/* Max number of channels to check */
#define MAX_NUM_SCNL 2000

/* Cast quality fields to unsigned char when you want convert to hexadecimal. */
#define CAST_QUALITY( q ) ( (unsigned char) q )

static unsigned char InstId;        /* local installation id              */

namespace Ui {
class LatencyMon;
}

class LatencyMon : public QFrame
{
    Q_OBJECT
    
public:
    explicit LatencyMon(QWidget *parent = 0);
    ~LatencyMon();
    
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

    //SHM_INFO        region;
    long            RingKey;         /* Key to the transport ring to read from */
    MSG_LOGO        getlogo[NLOGO], logo;
    long            gotsize;
    char            msg[MAX_TRACEBUF_SIZ];
    char           *getSta, *getComp, *getNet, *getLoc, *inRing;
    char            wildSta, wildComp, wildNet, wildLoc;
    unsigned char   Type_Mseed;
    unsigned char   Type_TraceBuf,Type_TraceBuf2;
    unsigned char   Type_TraceComp, Type_TraceComp2;
    unsigned char   InstWildcard, ModWildcard;
    short          *short_data;
    int            *long_data;
    TRACE2_HEADER  *trh;
    char            orig_datatype[3];
    char            stime[256];
    char            etime[256];
    int             dataflag;
    int             i;
    int             rc;
    int             verbose; /* if verbose =1 print name and number for logo ids */
    int             useLoc;
    int             nLogo;

    char            ModName[MAX_MOD_STR];
    char            *modid_name;
    unsigned char   sequence_number;
    int             statistics_flag;
    int             dataargument_position;
    time_t monitor_start_time;
    double start_time, end_time;
    int seconds_to_live;
    unsigned long packet_total;
    unsigned long packet_total_size;
    SCNL_LAST_SCNL_PACKET_TIME scnl_time[MAX_NUM_SCNL]; /* times of last scnl and packet */
    int n_scnl_time;                                  /* current number of scnl_time items */
    MSRecord *msr;        /* mseed record */

    QTimer *checkWAVE_RING;

    void setup();
    void reset();
    void print_data_feed_latencies(TRACE2_HEADER  *trh, SCNL_LAST_SCNL_PACKET_TIME *scnl_time, int *n_scnl_time);

public slots:
    void findLatency();

private:
    Ui::LatencyMon *LatencyMon_ui;
};

#endif // LATENCYMON_H
