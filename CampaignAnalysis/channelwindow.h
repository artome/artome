#ifndef CHANNELWINDOW_H
#define CHANNELWINDOW_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDateTime>
#include <QCloseEvent>
// #include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>

namespace Ui {
class channelWindow;
}

class channelWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit channelWindow(QWidget *parent = 0);
    ~channelWindow();



     int databaseindex;
     QString databaseconnection,databasename;
     QStandardItemModel valuesmodel;
     QStandardItemModel selectedvaluesmodel;
     void fillsinglevalues();
     void startselectedsinglevalues();

     void displaydisablethings();



 public slots:

    void send_DoIt(int );
    void fill();
    void getdatabaseinfo( QList<QVariant> , int) ;
    void logm(QString);
    void logmR(QString);

 signals:

      void CallTime();
      void DoIt(QStringList, QStringList,QStringList, QList<QVariant>, int);
      void logmessagesR(QString);
      void logmessages(QString);


private slots:

   void clearselected();
    void updateselectednumber();
    void selectall(QModelIndex);
    void erasefromlist(QModelIndex);
    void eraseallselectedlist();
    void fillselectedsinglevalues(QModelIndex);
    void fillselectedsinglevaluesbyclick();
    void showhelp();
    void timepressed();
    void makeatyplot();
    void makeasurfaceplot();
    void builtatable();
    void savetofile();
    void makeanhistogram();


private:
    Ui::channelWindow *ui;
    void sortNonCaseSensitive(QStringList&);
    QDateTime Itimes;
    QDateTime Ltimes;
    bool databasechanged;
    QString sepf;
    QString sepc;
    QString atf;

};

#endif // CHANNELWINDOW_H
