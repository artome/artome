#include "plotcanvas.h"

#include <QApplication>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QTimer>
#include "dbapi.h"
#include "qdatetime.h"
int main(int argc, char *argv[])
{
    QString server,plot,user,password,database;
    int hours,minutes;
    hours=0;minutes=0;
    QStringList      param;
    for (int i = 1; i <= argc; i++) {
        param.clear();
        param = QString(argv[i]).split("=");
        if (param[0].contains("server")) {
            server = param[1];
            }
        if (param[0].contains("plot")) {
            plot = param[1];
         }
        if (param[0].contains("user")) {
            user = param[1];
        }
        if (param[0].contains("password")) {
            password = param[1];
        }
        if (param[0].contains("database")) {
            database = param[1];
        }
        if (param[0].contains("hours")) {
            hours = param[1].toInt();
        }
        if (param[0].contains("minutes")) {
            minutes = param[1].toInt();
        }


    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","Monitoringconnection");
    db.setHostName(server);
    db.setDatabaseName(database);
    db.setUserName(user);
    db.setPassword(password);
    db.setConnectOptions("MYSQL_OPT_RECONNECT=TRUE");
     if (!db.open()){
       qDebug() <<"Unable to connect to database";
       qDebug() << db.lastError().text();
       qDebug() << "server   =" << server.toLatin1().constData() ;
       qDebug() << "password   =" << password.toLatin1().constData() ;
       qDebug() << "user   =" << user.toLatin1().constData() ;
       qDebug() << "database   =" << database.toLatin1().constData() ;

       return 1;
     }
     param=plot.split("@");
     if(param.count()!=2) return 1;



    QApplication a(argc, argv);
    plotcanvas w;
    w.databaseconnection="Monitoringconnection";
    w.Monitoring=true;

 //   w.timei=1574435090000;
 //   w.timef=1574435512000;

    w.timef=QDateTime::currentDateTime().toTimeSpec(Qt::UTC).toMSecsSinceEpoch();
    long long interv;
    interv=(hours*60 +minutes)*60000.;
     w.timei=w.timef-interv;
    w.setinterval(hours,minutes);
    if(param.at(1).contains("typlots")) {
        dbapi table;
               table.connectionName = "Monitoringconnection";
               QStringList cnames;
               cnames.clear();
               cnames  << "columnsandtables";
               QList <std::vector<QString> * > cp;
               for(int i=0; i<cnames.count(); i++) cp << new ( std::vector<QString>);

               table.fechnotimecolumns_string_filterstring("typlots", cnames,"plotname",param.at(0), &cp );
               QStringList coltable;
               coltable=cp.at(0)->at(0).split(";");
               w.cnames=coltable.at(0).split(",");
               w.tnames=coltable.at(1).split(",");;
               w.plottyfromlist(param.at(0));
               for(int s=0; s<cp.count(); s++) delete cp[s];
    }
    if(param.at(1).contains("Histoplots")) {

        dbapi table;
               table.connectionName = "Monitoringconnection";
               QStringList cnames;
               cnames.clear();
               cnames  << "columnsandtables";
               QList <std::vector<QString> * > cp;
               for(int i=0; i<cnames.count(); i++) cp << new ( std::vector<QString>);

               table.fechnotimecolumns_string_filterstring("Histoplots", cnames,"plotname",param.at(0), &cp );
               QStringList coltable;
               coltable=cp.at(0)->at(0).split(";");
               w.cnames=coltable.at(1).split(",");
               w.tnames<<coltable.at(0);
               w.plotHfromlist(param.at(0));
               for(int s=0; s<cp.count(); s++) delete cp[s];
}
    if(param.at(1).contains("Surfaceplots")) {
        dbapi table;
               table.connectionName = "Monitoringconnection";
               QStringList cnames;
               cnames.clear();
               cnames  << "columnsandtables";
               QList <std::vector<QString> * > cp;
               for(int i=0; i<cnames.count(); i++) cp << new ( std::vector<QString>);

               table.fechnotimecolumns_string_filterstring("Surfaceplots", cnames,"plotname",param.at(0), &cp );
               QStringList coltable;
               coltable=cp.at(0)->at(0).split(";");
               w.cnames=coltable.at(1).split(",");
               w.tnames<<coltable.at(0);
               w.plotSfromlist(param.at(0));
               for(int s=0; s<cp.count(); s++) delete cp[s];
     }
    if(param.at(1).contains("Screens")) {

        w.plotAfromlist(param.at(0));}

     w.choosemode();
     w.show();
     w.plotrefresh();
    QTimer::singleShot(100, &w, SLOT(fitintowidget()));

    return a.exec();
}
