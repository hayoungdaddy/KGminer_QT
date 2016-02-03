#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QProgressDialog>

class ProcessDialog : public QProgressDialog
{
    Q_OBJECT
    
public:
    explicit ProcessDialog(int min = 0, int max = 0, QWidget *parent = 0);
    ~ProcessDialog();
    
    void setup(int);
private:
};

#endif // PROCESSDIALOG_H
