#include <QWidget>

#include "util.h"

Util::Util( QWidget *parent ) :
    QWidget( parent )
{
}

Util::~Util()
{
}


void Util::readCfg()
{
    emit sendCFGtoMainWindow(cfg);
}

void Util::readStationSet()
{
    emit sendStationSettoMainWindow(cfg);
}
