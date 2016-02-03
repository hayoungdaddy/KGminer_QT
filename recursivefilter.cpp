#include "recursivefilter.h"
#include "ui_recursivefilter.h"

RecursiveFilter::RecursiveFilter(QWidget *parent) :
    QDialog(parent),
    RecursiveFilter_ui(new Ui::RecursiveFilter)
{
    RecursiveFilter_ui->setupUi(this);
    //codec = QTextCodec::codecForName( "utf8" );

    connect(RecursiveFilter_ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(RecursiveFilter_ui->genButton, SIGNAL(clicked()), this, SLOT(genButtonClicked()));
}

RecursiveFilter::~RecursiveFilter()
{
    delete RecursiveFilter_ui;
}

/*
para_q_for_accel  0.98
para_q_for_veloA  0.98
para_q_for_veloV  0.98
para_q_for_disp   0.98
para_lambda       1
para_q_for_snr    0.98
para_t_signal     0.05
para_tau_k 	  3
para_t_noise      30.0
gain		  32196833414.7
*/
void RecursiveFilter::setup()
{
    QFile file("/opt/KGminer/params/recursivefilter.d");
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
                if(_line.startsWith("para_q_for_accel"))
                    accel = _line.section(' ', 1, 1);
                else if(_line.startsWith("para_q_for_veloA"))
                    veloA = _line.section(' ', 1, 1);
                else if(_line.startsWith("para_q_for_veloV"))
                    veloV = _line.section(' ', 1, 1);
                else if(_line.startsWith("para_q_for_disp"))
                    disp = _line.section(' ', 1, 1);
                else if(_line.startsWith("para_lambda"))
                    lambda = _line.section(' ', 1, 1);
                else if(_line.startsWith("para_q_for_snr"))
                    snr = _line.section(' ', 1, 1);
                else if(_line.startsWith("para_t_signal"))
                    signal = _line.section(' ', 1, 1);
                else if(_line.startsWith("para_tau_k"))
                    tauk = _line.section(' ', 1, 1);
                else if(_line.startsWith("para_t_noise"))
                    noise = _line.section(' ', 1, 1);
                else if(_line.startsWith("gain"))
                {
                    gain = _line.section(' ', 1, 1);
                    break;
                }
            }
        }

        file.close();
    }

    RecursiveFilter_ui->accel->setText(accel);
    RecursiveFilter_ui->veloA->setText(veloA);
    RecursiveFilter_ui->veloV->setText(veloV);
    RecursiveFilter_ui->disp->setText(disp);
    RecursiveFilter_ui->lambda->setText(lambda);
    RecursiveFilter_ui->snr->setText(snr);
    RecursiveFilter_ui->signal->setText(signal);
    RecursiveFilter_ui->tauk->setText(tauk);
    RecursiveFilter_ui->noise->setText(noise);
    RecursiveFilter_ui->gain->setText(gain);
}

void RecursiveFilter::genButtonClicked()
{
    QFile file;
    /* generate recursivefilter.d file. */
    file.setFileName("/opt/KGminer/params/recursivefilter.d");

    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file ) ;

        stream << "#" << "\n";
        stream << "#                     Recursive Filter's Configuration File" << "\n";
        stream << "#" << "\n";
        stream << "MyModId       MOD_RECURSIVEFILTER      # This instance of Recursive Filter" << "\n";
        stream << "InRing           WAVE_RING     # Transport ring to find waveform data on," << "\n";
        stream << "OutRing          PHYSIC_RING     # Transport ring to write output to," << "\n";
        stream << "HeartbeatInt            10     # Heartbeat interval, in seconds," << "\n";
        stream << "Debug                    0     # If 1, print debugging message" << "\n";
        stream << "" << "\n";
        stream << "para_q_for_accel  " << RecursiveFilter_ui->accel->text() << "\n";
        stream << "para_q_for_veloA  " << RecursiveFilter_ui->veloA->text() << "\n";
        stream << "para_q_for_veloV  " << RecursiveFilter_ui->veloV->text() << "\n";
        stream << "para_q_for_disp  " << RecursiveFilter_ui->disp->text() << "\n";
        stream << "para_lambda       " << RecursiveFilter_ui->lambda->text() << "\n";
        stream << "para_q_for_snr    " << RecursiveFilter_ui->snr->text() << "\n";
        stream << "para_t_signal     " << RecursiveFilter_ui->signal->text() << "\n";
        stream << "para_tau_k 	  " << RecursiveFilter_ui->tauk->text() << "\n";
        stream << "para_t_noise      " << RecursiveFilter_ui->noise->text() << "\n";
        stream << "gain		  " << RecursiveFilter_ui->gain->text() << "\n";
        stream << "" << "\n";
        stream << "" << "\n";
        stream << "# Specify which messages to look at with Getlogo commands." << "\n";
        stream << "#   GetLogo <installation_id> <module_id> <message_type>" << "\n";
        stream << "# The message_type must be either TYPE_TRACEBUF or TYPE_TRACEBUF2." << "\n";
        stream << "# Use as many GetLogo commands as you need." << "\n";
        stream << "# If no GetLogo commands are given, Recursive Filter will look at all" << "\n";
        stream << "# TYPE_TRACEBUF and TYPE_TRACEBUF2 messages in InRing." << "\n";
        stream << "#-----------------------------------------------------------------" << "\n";
        stream << "GetLogo  INST_WILDCARD  MOD_WILDCARD  TYPE_TRACEBUF2" << "\n";

        file.close() ;
    }

    QMessageBox::warning( this, "Information", "Generation OK.\n");
}
