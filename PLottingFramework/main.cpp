#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QMessageBox>

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "Plottingconnection");
    db.setHostName("cloud2.sim.ul.pt");
    db.setDatabaseName("cloud14a");
    db.setUserName("cloud");
    db.setPassword("Disibe21");
    if (!db.open()) {
        QMessageBox msgBox;
        msgBox.setText("Unable to open database");
        msgBox.exec();
        return 1;
    }

    MainWindow w;
    w.databaseconnection="Plottingconnection";
    w.show();
    return a.exec();
}
