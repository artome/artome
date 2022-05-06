#ifndef DOWNLOADITENSTHREAD_H
#define DOWNLOADITENSTHREAD_H

#include <QThread>
#include <QList>
#include <QStringList>
#include <QVariant>

class DownloadItensThread : public QThread
{
    Q_OBJECT
public:
    explicit DownloadItensThread(QObject *parent = nullptr);

   long long itime, ltime;
   QString filename,timestring,separator;
   QString databaseconnection;
   QStringList tnames,cnames;
   QStringList aliasname;
   QList<QVariant> ctypes;
   QString database,user,password,server;
   int localport,port;


signals:

   void LogMessage(QString);
   void Endwork();

public slots:

private:
    void run();
    QString sepf;
    QString sepc;
    QString atf;

};

#endif // DOWNLOADITENSTHREAD_H
