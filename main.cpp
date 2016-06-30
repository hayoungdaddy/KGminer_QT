#include <QApplication>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString tmpDir = QDir::tempPath();
    QLockFile lockFile(tmpDir + "/KGminer.lock");

    if(!lockFile.tryLock(100))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("You already have this app running."
                        "\r\nOnly one instance is allowed.");
        msgBox.exec();
        return 1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
