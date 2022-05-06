#ifndef PLOTDATA_H
#define PLOTDATA_H
#include "plotproprieties.h"
#include "plotting.h"

class plotdata: public QObject
{
    Q_OBJECT
public:
    explicit plotdata(QObject *parent = nullptr);
    ~plotdata();
   // plotproprieties p;
    xyzdata xyz;
    QVector<histodata *> histo;
    QVector<xydata *> xy;
    QString databaseconnection;
    long long timei,timef;
    long long mtimei,mtimef;
    bool movie;
    QStringList tnames, cnames;
    QString tname;
    QString Bins;
    void getxyplotdata(bool);
    void getxyzplotdata(bool);
    void gethistoplotdata(bool);
public slots:
    void LogMessages(QString mess);
    void LogMessagesR(QString mess);
signals:
    void logmessages(QString);
    void logmessagesR(QString);
private:



};

#endif // PLOTDATA_H
