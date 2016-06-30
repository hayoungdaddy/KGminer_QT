#ifndef UTIL_H
#define UTIL_H

#include <QObject>

#include "common.h"

class Util : public QObject
{
    Q_OBJECT

public:
    Util();
    ~Util();

    CFG readCfg();
};


#endif // UTIL_H
