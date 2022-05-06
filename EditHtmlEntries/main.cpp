#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString folder="/home/cloud-daq/cloud/data/";
    QStringList      param;
    for (int i = 1; i <= argc; i++) {
        param.clear();
        param = QString(argv[i]).split("=");
        if (param[0].contains("folder")) {
            folder = param[1];
        }

    }
    if (!QDir(folder).exists()){
        QMessageBox msgBox;
        msgBox.setText(folder +" does not exists");
        msgBox.exec();
         return 1;
    }
    MainWindow w;
    w.folder=folder;
    w.setextfolder();

    w.show();

    return a.exec();
}
