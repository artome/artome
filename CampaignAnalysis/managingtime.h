#ifndef MANAGINGTIME_H
#define MANAGINGTIME_H

#include <QDialog>
#include <QVariant>
#include <QDateTime>
#include "timerunsmodel.h"

namespace Ui {
class ManagingTime;
}

class ManagingTime : public QDialog
{
    Q_OBJECT

public:
    explicit ManagingTime(QWidget *parent = 0);
    ~ManagingTime();
     QDateTime Itime,Ltime,IItime,LLtime;
     QDateTime current_time,firsttime;

     QString Itimeslabel;
     QString Ltimeslabel;

     void setTimeWidgets(QDate, QTime, QDate, QTime );
     int databaseindex;
     QList<QVariant> databaseinfo;
     TimeRunsModel  model;
public slots:

     void getdatabaseinfo( QList<QVariant> , int) ;
     void Logmessages(QString);
     void LogmessagesR(QString);

signals:
    void senddatabaseinfo(QList<QVariant>  , int) ;
    void logmessages(QString);
    void logmessagesR(QString);


private  slots:
   void timefromruns();
   void timefromevents();
   void manualchangeLtime();
   void manualchangeItime();
   void readvalues();
   void writeinterface();
   void setlast20();
   void setlast40();
   void setlast60();

private:
   Ui::ManagingTime *ui;



};

#endif // MANAGINGTIME_H
