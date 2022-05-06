#ifndef DEFININGSCREENS_H
#define DEFININGSCREENS_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDateTime>
#include <QCloseEvent>
//#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>


namespace Ui {
class DefiningScreens;
}

class DefiningScreens : public QDialog
{
    Q_OBJECT

public:
    explicit DefiningScreens(QWidget *parent = 0);
    ~DefiningScreens();

    int databaseindex;
    QString databaseconnection,databasename;
    QStandardItemModel valuesmodel;
    QStandardItemModel selectedvaluesmodel;
    void fill();
    void displaydisablethings();
public slots:
    void LogM(QString);
    void LogMR(QString);

signals:

     void logmessagesR(QString);
     void logmessages(QString);
     void dplot(QList<QVariant>);
     void doreport(QList <QList<QVariant> >);
private slots:

    void selectall(QModelIndex);
    void erasefromlist(QModelIndex);
    void eraseallselectedlist();
    void fillselectedsinglevalues(QModelIndex);
    void fillselectedsinglevaluesbyclick();
    void showhelp();
    void storescreens();
    QString resetviewports(QString ,double,double,double,double);
    void sendplots();
    void writereport();
private:
    Ui::DefiningScreens *ui;
    void fillsinglevalues();
    void startselectedsinglevalues();

};

#endif // DEFININGSCREENS_H
