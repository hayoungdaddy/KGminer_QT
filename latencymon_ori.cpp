#include "latencymon.h"
#include "ui_latencymon.h"

int search_scnl_time(SCNL_LAST_SCNL_PACKET_TIME *scnl_time, int *n_scnl_time, char *sta, char *chan, char *net, char *loc)
{
    char str_SCNL[MAX_LEN_STR_SCNL];
    int i = 0;
    int found = 0;

    /* initialize a SCNL string from current packet */
    snprintf(str_SCNL, MAX_LEN_STR_SCNL, "%s.%s.%s.%s", sta, chan, net, loc);

    /* search for SCNL string in scnl array */
    i = 0;
    while(i < *n_scnl_time &&  !found)
    {
        if(strcmp(str_SCNL, scnl_time[i].scnl) == 0)
        {
            found = 1;
        }
        else
        {
            i++;
        }
    }

    if(!found)
    {
        /* just add if space is available */
        if(*n_scnl_time < MAX_NUM_SCNL-1)
        {
            strncpy(scnl_time[*n_scnl_time].scnl, str_SCNL, MAX_LEN_STR_SCNL);
            scnl_time[*n_scnl_time].last_scnl_time = -1.0;
            scnl_time[*n_scnl_time].last_packet_time = -1.0;
            (*n_scnl_time)++;
        }
        else
        {
            /* TODO error message */
            i = -1;
            fprintf(stderr, "sniffwave: error: number of channels for gaps and latencies exceeded MAX_NUM_SCNL (%d).\n                  SCNL %s will not be monitored.",
                    MAX_NUM_SCNL, str_SCNL);
        }
    }

    return i;
}

int  check_for_gap_overlap_and_output_message(TRACE2_HEADER  *trh, SCNL_LAST_SCNL_PACKET_TIME *scnl_time, int *n_scnl_time)
{
    int ret = 0;
    int i = 0;
    char st1[256], st2[256];
    double diff_time;
    double SCNL_TIME_DIFF_TOLLERANCE;
    double t1, t2;

    /* Get index for SCNL */
    i = search_scnl_time(scnl_time, n_scnl_time, trh->sta, trh->chan, trh->net, trh->loc);

    if(i < 0)
    {
        /* Error message has been printed by search_scnl_time() */
        ret = i;
    }
    else
    {
        /* Skips the check the first time for the current scnl */
        if(scnl_time[i].last_scnl_time > 0.0)
        {
            t1 = scnl_time[i].last_scnl_time;
            t2 = trh->starttime;

            /* compare to last scnl time */
            SCNL_TIME_DIFF_TOLLERANCE = 2.0 * (1.0 / (double) trh->samprate);
            diff_time =  t2 - t1;

            if(fabs(diff_time) > SCNL_TIME_DIFF_TOLLERANCE)
            {
                /* check for a gap or an overlap */
                if(diff_time > 0.0)
                {
                        /* it is a gap */
                }
                else
                {
                    t1 = trh->starttime;
                    /* it is an overlap */
                    if(scnl_time[i].last_scnl_time > (trh->endtime + (1.0 / (double) trh->samprate))) {
                        t2 = trh->endtime + (1.0 / (double) trh->samprate);
                    } else {
                        t2 = scnl_time[i].last_scnl_time;
                    }
                    diff_time = t1 - t2;
                }

                /* Convert double epochs to string */
                datestr23 (t1, st1, 256);
                datestr23 (t2, st2, 256);

                /* output message for gap or overlap */
                fprintf(stdout, "%15s %8s of %6.2fs        %s (%.4f) %s (%.4f)\n",
                        scnl_time[i].scnl, (diff_time > 0.0) ? "gap" : "overlap", fabs(diff_time), st1, t1, st2, t2);
            }
        }

        /* Save last time for current scnl */
        scnl_time[i].last_scnl_time = trh->endtime + (1.0 / (double) trh->samprate);
    }

    return ret;
}

LatencyMon::LatencyMon(QWidget *parent) :
    QFrame(parent),
    LatencyMon_ui(new Ui::LatencyMon)
{
    LatencyMon_ui->setupUi(this);

    /* Process Status Setup */
    checkWAVE_RING = new QTimer(this);

    /* Process Timer Connection */
    connect(checkWAVE_RING, SIGNAL(timeout()), this, SLOT(findLatency()));

    /* set Table */
    LatencyMon_ui->LatencyTable->horizontalHeader()->setStyleSheet("QHeaderView { font-size: 8pt; }");
    LatencyMon_ui->LatencyTable->setColumnWidth(0, 90);
    LatencyMon_ui->LatencyTable->setColumnWidth(1, 85);
    LatencyMon_ui->LatencyTable->setColumnWidth(2, 85);
    LatencyMon_ui->LatencyTable->setColumnWidth(3, 78);

    QFile file("/opt/KGminer/params/sta.info");
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
                staName << _line.section(':', 0, 0);
            }
        }
        file.close();
    }
}

LatencyMon::~LatencyMon()
{
    delete LatencyMon_ui;
}

void LatencyMon::reset()
{
    qWarning("==================");
    //tport_detach( &region );
}

void LatencyMon::setup()
{
    SHM_INFO        region;
    verbose = 0;
    useLoc = 0;
    nLogo = NLOGO;
    sequence_number = 0;

    dataargument_position=5;

    packet_total=0;
    packet_total_size=0;

    n_scnl_time = 0;
    msr = NULL;
    seconds_to_live = 0;

    trh  = (TRACE2_HEADER *) msg;
    long_data  =  (int *)( msg + sizeof(TRACE2_HEADER) );
    short_data = (short *)( msg + sizeof(TRACE2_HEADER) );

    useLoc=wildSta=wildComp=wildNet=wildLoc=1;
    inRing = "WAVE_RING";

    dataflag = 0;

    if ((RingKey = GetKey( inRing )) == -1 )
    {
        fprintf( stderr, "Invalid RingName; exiting!\n" );
        exit( -1 );
    }

    qWarning("RingKey %d\n", RingKey);
    //qWarning("mid = %d, sid = %d key = %d\n", region.mid, region.sid, region.key);
    //region.mid = 0;
    //region.sid = 0;
    tport_attach( &region, RingKey );

    GetLocalInst( &InstId );

    if ( GetType( "TYPE_MSEED", &Type_Mseed ) != 0 )
    {
        fprintf(stderr, "%s: WARNING: Invalid message type <TYPE_MSEED>! Missing from earthworm.d or earthworm_global.d\n", "sniffwave" );
        Type_Mseed = TYPE_NOTUSED;
    }
    if ( GetType( "TYPE_TRACEBUF", &Type_TraceBuf ) != 0 )
    {
        fprintf(stderr, "%s: Invalid message type <TYPE_TRACEBUF>! Missing from earthworm.d or earthworm_global.d\n", "sniffwave" );
        exit( -1 );
    }
    if ( GetType( "TYPE_TRACE_COMP_UA", &Type_TraceComp ) != 0 )
    {
        fprintf(stderr, "%s: Invalid message type <TYPE_TRACE_COMP_UA>! Missing from earthworm.d or earthworm_global.d\n", "sniffwave" );
        exit( -1 );
    }
    if ( GetType( "TYPE_TRACEBUF2", &Type_TraceBuf2 ) != 0 )
    {
        fprintf(stderr, "%s: Invalid message type <TYPE_TRACEBUF2>! Missing from earthworm.d or earthworm_global.d\n", "sniffwave" );
        exit( -1 );
    }
    if ( GetType( "TYPE_TRACE2_COMP_UA", &Type_TraceComp2 ) != 0 )
    {
        fprintf(stderr,"%s: Invalid message type <TYPE_TRACE2_COMP_UA>! Missing from earthworm.d or earthworm_global.d\n", "sniffwave" );
        exit( -1 );
    }
    if ( GetModId( "MOD_WILDCARD", &ModWildcard ) != 0 )
    {
        fprintf(stderr, "%s: Invalid moduleid <MOD_WILDCARD>! Missing from earthworm.d or earthworm_global.d\n", "sniffwave" );
        exit( -1 );
    }
    if ( GetInst( "INST_WILDCARD", &InstWildcard ) != 0 )
    {
        fprintf(stderr, "%s: Invalid instid <INST_WILDCARD>! Missing from earthworm.d or earthworm_global.d\n", "sniffwave" );
        exit( -1 );
    }

    for( i=0; i<nLogo; i++ )
    {
        getlogo[i].instid = InstWildcard;
        getlogo[i].mod    = ModWildcard;
    }
    getlogo[0].type = Type_Mseed;
    getlogo[1].type = Type_TraceBuf;
    getlogo[2].type = Type_TraceComp;
    if (useLoc)
    {
        getlogo[3].type = Type_TraceBuf2;
        getlogo[4].type = Type_TraceComp2;
    }

    while ( tport_getmsg( &region, getlogo, nLogo, &logo, &gotsize, (char *)&msg, MAX_TRACEBUF_SIZ ) != GET_NONE )
    {
        packet_total++;
        packet_total_size+=gotsize;
    }
    //fprintf( stderr, "sniffwave: inRing flushed %ld packets of %ld bytes total.\n", packet_total, packet_total_size);

    if (seconds_to_live > 0)
    {
        monitor_start_time = time(0);
        packet_total = packet_total_size = 0;
        start_time = 0.0;
    }

    tport_detach( &region );

    //checkWAVE_RING->start( 1 );
}

void LatencyMon::findLatency()
{
    //qWarning("mid = %d, sid = %d key = %d\n", region.mid, region.sid, region.key);
    /*
    sta = "";
    scnl = "";
    nsamp = "";
    pinno = "";
    samprate = "";

    rc = tport_copyfrom( &region, getlogo, nLogo, &logo, &gotsize, msg, MAX_TRACEBUF_SIZ, &sequence_number );

    if ( rc == GET_NONE )
    {
        sleep_ew( 200 );
        return;
    }

    if ( rc == GET_TOOBIG )
    {
        fprintf( stderr, "sniffwave: retrieved message too big (%ld) for msg\n", gotsize );
        return;
    }

    if ( rc == GET_NOTRACK )
        fprintf( stderr, "sniffwave: Tracking error.\n");

    if ( rc == GET_MISS_LAPPED )
        fprintf( stderr, "sniffwave: Got lapped on the ring.\n");

    if ( rc == GET_MISS_SEQGAP )
        fprintf( stderr, "sniffwave: Gap in sequence numbers\n");

    if ( rc == GET_MISS )
        fprintf( stderr, "sniffwave: Missed messages\n");

    if (Type_Mseed != TYPE_NOTUSED && logo.type == Type_Mseed)
    {
        if ( msr_unpack (msg, gotsize, &msr, dataflag|statistics_flag, verbose) != MS_NOERROR)
        {
            fprintf (stderr, "Error parsing mseed record\n");
            return;
        }

        msr_print (msr, verbose);

        msr_free (&msr);
        return;

    }

    if ( (wildSta  || (strcmp(getSta,trh->sta)  ==0)) && (wildComp || (strcmp(getComp,trh->chan)==0)) &&
             (wildNet  || (strcmp(getNet,trh->net)  ==0)) && (((logo.type == Type_TraceBuf2 || logo.type == Type_TraceComp2) &&
             (wildLoc  || (strcmp(getLoc,trh->loc) == 0))) || ( (logo.type == Type_TraceBuf || logo.type == Type_TraceComp))))
    {
        strcpy(orig_datatype, trh->datatype);
        if(WaveMsg2MakeLocal( trh ) < 0)
        {
            char scnl[20], dt[3];
            scnl[0] = 0;
            strcat( scnl, trh->sta);
            strcat( scnl, ".");
            strcat( scnl, trh->chan);
            strcat( scnl, ".");
            strcat( scnl, trh->net);
            strcat( scnl, ".");
            strcat( scnl, trh->loc);

            for ( i=0; i<15; i++ )
            {
                if ( !isalnum(scnl[i]) && !ispunct(scnl[i]))
                scnl[i] = ' ';
            }
            strncpy( dt, trh->datatype, 2 );
            for ( i=0; i<2; i++ )
            {
                if ( !isalnum(dt[i]) && !ispunct(dt[i]))
                    dt[i] = ' ';
            }
            dt[i] = 0;

            return;
        }

        if (seconds_to_live > 0)
        {
            if (start_time == 0.0)
                start_time = trh->starttime;
            end_time = trh->endtime;
            packet_total++;
            packet_total_size += gotsize;
        }

        check_for_gap_overlap_and_output_message(trh, scnl_time, &n_scnl_time);

        datestr23 (trh->starttime, stime, 256);
        datestr23 (trh->endtime, etime, 256);

        sta = QString::fromStdString(trh->sta);
        nsamp = nsamp.setNum(trh->nsamp, 10);

        if( logo.type == Type_TraceBuf2  || logo.type == Type_TraceComp2 )
        {
            scnl = QString::fromStdString(trh->sta)  + "." + QString::fromStdString(trh->chan) + "." +
                    QString::fromStdString(trh->net) + "." + QString::fromStdString(trh->loc);
        }
        else
        {
            scnl = QString::fromStdString(trh->sta)  + "." + QString::fromStdString(trh->chan) + "." +
                    QString::fromStdString(trh->net);
        }

        pinno = pinno.setNum(trh->pinno, 10);
        samprate = samprate.setNum(trh->samprate, 'f', 0);
        starttime = QString::fromStdString(stime);
        endtime = QString::fromStdString(etime);
        dtype = QString::fromStdString(trh->datatype);

        print_data_feed_latencies(trh, scnl_time, &n_scnl_time);

    }
    */
}

void LatencyMon::print_data_feed_latencies(TRACE2_HEADER  *trh, SCNL_LAST_SCNL_PACKET_TIME *scnl_time, int *n_scnl_time)
{
    double d_now;
    double d_latency_second;
    double d_packet_latency_second = 0.0;
    int i = 0;

    /* Get current time */
    hrtime_ew( &d_now );

    /* Compute latency */
    d_latency_second = d_now - trh->endtime;

    /* Get index for SCNL */
    i = search_scnl_time(scnl_time, n_scnl_time, trh->sta, trh->chan, trh->net, trh->loc);

    if(i < 0)
    {
        /* Error message has been printed by search_scnl_time() */
    }
    else
    {
        if(scnl_time[i].last_packet_time > 0.0)
        {
            /* Compute packet latency */
            d_packet_latency_second = d_now - scnl_time[i].last_packet_time;
        }
        scnl_time[i].last_packet_time = d_now;
    }

    /* Print Data and Feed latencies */
    latencySecond = "";
    packetLatencySecond = "";

    latencySecond = latencySecond.setNum(d_latency_second, 'f', 1);
    packetLatencySecond = packetLatencySecond.setNum(d_packet_latency_second, 'f', 1);

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
                //LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 1, new QTableWidgetItem(nsamp));
                //LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 2, new QTableWidgetItem(pinno));
                //LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 3, new QTableWidgetItem(samprate));
                LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 1, new QTableWidgetItem(starttime));
                LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 2, new QTableWidgetItem(endtime));
                //LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 6, new QTableWidgetItem(dtype));
                LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 3, new QTableWidgetItem(latencySecond));
                //LatencyMon_ui->LatencyTable->setItem(LatencyMon_ui->LatencyTable->rowCount()-1, 4, new QTableWidgetItem(packetLatencySecond));
            }
            else
            {
                LatencyMon_ui->LatencyTable->setItem(whererow, 0, new QTableWidgetItem(scnl));
                //LatencyMon_ui->LatencyTable->setItem(whererow, 1, new QTableWidgetItem(nsamp));
                //LatencyMon_ui->LatencyTable->setItem(whererow, 2, new QTableWidgetItem(pinno));
                //LatencyMon_ui->LatencyTable->setItem(whererow, 3, new QTableWidgetItem(samprate));
                LatencyMon_ui->LatencyTable->setItem(whererow, 1, new QTableWidgetItem(starttime));
                LatencyMon_ui->LatencyTable->setItem(whererow, 2, new QTableWidgetItem(endtime));
                //LatencyMon_ui->LatencyTable->setItem(whererow, 6, new QTableWidgetItem(dtype));
                LatencyMon_ui->LatencyTable->setItem(whererow, 3, new QTableWidgetItem(latencySecond));
                //LatencyMon_ui->LatencyTable->setItem(whererow, 4, new QTableWidgetItem(packetLatencySecond));
            }
            break;
        }
    }

    for(int i=0;i<LatencyMon_ui->LatencyTable->rowCount();i++)
    {
        for(int j=0;j<4;j++)
        {
            LatencyMon_ui->LatencyTable->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}



