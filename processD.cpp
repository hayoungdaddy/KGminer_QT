#include "processD.h"

ProcessDialog::ProcessDialog(int min, int max, QWidget *parent) :
    QProgressDialog(parent)
{
    setRange(min, max);
}

ProcessDialog::~ProcessDialog()
{
}

void ProcessDialog::setup(int x)
{
    setValue(x);
}
