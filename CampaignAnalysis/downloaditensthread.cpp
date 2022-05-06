#include "downloaditensthread.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QTemporaryFile>
#include "dbapi.h"

DownloadItensThread::DownloadItensThread(QObject *parent) :
    QThread(parent)
{
    filename="download.csv";
    sepf=";;";
    sepc=",,";
    atf="@@";
    databaseconnection="downloaditenAero";

 }
void DownloadItensThread::run()
{

    emit LogMessage("doing the downloading");
    QString sep;
     QString lixo,line ;
     QString realname,instname;
    long row;
    QList <QTemporaryFile *> tempfiles;
    QList <QTextStream *> tempstreams;
        if(separator.contains("comma")) sep=","; else if (separator.contains("tab")) sep="\t";
        else if (separator.contains("space")) sep=" ";
 //
        QFile file(filename);

        if (!file.open(QFile::WriteOnly | QFile::Text)) {
        emit LogMessage("couldn't open  file to save data");
            return;
        }
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL",databaseconnection);
        db.setHostName(server);
        db.setDatabaseName(database);
        db.setUserName(user);
        db.setPassword(password);
        if(server.contains("127.0.0.1"))db.setPort(localport);else  db.setPort(port);
        db.setConnectOptions("MYSQL_OPT_RECONNECT=TRUE");

        if (db.open()){

        dbapi table;
        table.connectionName=databaseconnection;
        connect (&table,&dbapi::logmessages,this,&DownloadItensThread::LogMessage);
        connect (&table,&dbapi::logmessagesR,this,&DownloadItensThread::LogMessage);

        lixo.clear();
        for (int j = 0; j < cnames.count() ; j++) {
            lixo.append("time");
            lixo.append(sep);
            lixo.append(aliasname.at(j));
            if(j<cnames.count()-1)lixo.append(sep);
            tempfiles << new QTemporaryFile ;
            tempstreams << new QTextStream ;
        }
        QTextStream out(&file);
        out << lixo << Qt::endl;
        out.setRealNumberNotation(QTextStream::SmartNotation);
        out.setRealNumberPrecision(10);

        QString columnt;
    std::vector<double>   c;
    std::vector<QString>   sc;

    std::vector<long long> time;
    row=0;
    for(int i=0; i<cnames.size(); i++) {
        tempfiles[i]->open();
        tempstreams[i]->setDevice(tempfiles[i]);
        tempstreams[i]->setRealNumberNotation(QTextStream::SmartNotation);
        tempstreams[i]->setRealNumberPrecision(10);

       c.clear();time.clear();sc.clear();
     if(tnames.at(i).compare("Runtable")){
      if(!ctypes.at(i).toString().contains("char",Qt::CaseInsensitive) ){
          if(!tnames.at(i).contains("_channels"))
          table.fech2columns_timeanddouble(tnames.at(i), cnames.at(i), (long long) itime*1000,(long long) ltime*1000, &time,&c);
          else{
              QStringList cols,lcols;
              cols.clear();lcols.clear();
              cols<<"channel_id" <<"type_table";
               QList <std::vector<QString> * > cc;
               for(int j=0; j<cols.count(); j++) cc << new ( std::vector<QString>);
               table.fechnotimecolumns_string_filterstring(tnames.at(i),cols,"channel",cnames.at(i),&cc);
               table.fech2columns_timeanddoublefilter(cc.at(1)->at(0), "value", "channel_id",cc.at(0)->at(0),(long long) itime*1000,(long long) ltime*1000,  &time,&c);
               for(int j=0; j<cols.count(); j++) delete cc[j];
               cc.clear();
                 }
          for(int j=0;j<(int )time.size();j++){
              columnt.clear();line.clear();
              if(timestring.contains("unixtime"))columnt.append(QString::number(time.at(j)/1000,'f',0));
              else if (timestring.contains("igor"))columnt.append(QString::number(time.at(j)/1000+2082844800,'f',0));
              else if (timestring.contains("unix_mili_time"))columnt.append(QString::number(time.at(j),'f',0));
              else if (timestring.contains("matlabtime"))columnt.append(QString::number(time.at(j)/86400000.+719529,'f',0));
              else  columnt.append(QDateTime::fromMSecsSinceEpoch(time.at(j)).toTimeSpec(Qt::UTC).toString(timestring));

              line.append(columnt);
              line.append(sep);
              line.append(QString::number(c.at(j),'g',12));
              *tempstreams[i] << line << Qt::endl;
       }

      }else{
          if(!tnames.at(i).contains("_channels") )
          table.fech2columns_timeandstring(tnames.at(i), cnames.at(i),(long long) itime*1000,(long long) ltime*1000, &time,&sc);
           else{
                QStringList cols,lcols;
                cols.clear();lcols.clear();
                cols<<"channel_id" <<"type_table";
                QList <std::vector<QString> * > cc;
                for(int j=0; j<cols.count(); j++) cc << new ( std::vector<QString>);
                table.fechnotimecolumns_string_filterstring(tnames.at(i),cols,"channel",cnames.at(i),&cc);
                table.fech2columns_timeandstringfilter(cc.at(1)->at(0), "value", "channel_id",lcols.at(0),(long long) itime*1000,(long long) ltime*1000, &time,&sc);
                for(int j=0; j<cols.count(); j++) delete cc[i];
                cc.clear();

            }
          for(int j=0;j<(int )time.size();j++){
              columnt.clear();line.clear();
              if(timestring.contains("unixtime"))columnt.append(QString::number(time.at(j)/1000,'f',0));
              else if (timestring.contains("igor"))columnt.append(QString::number(time.at(j)/1000+2082844800,'f',0));
              else if (timestring.contains("unix_mili_time"))columnt.append(QString::number(time.at(j),'f',0));
              else if (timestring.contains("matlabtime"))columnt.append(QString::number(time.at(j)/86400000.+719529,'f',0));
              else  columnt.append(QDateTime::fromMSecsSinceEpoch(time.at(j)).toTimeSpec(Qt::UTC).toString(timestring));

              line.append(columnt);
              line.append(sep);
              line.append(sc.at((j)));
              *tempstreams[i] << line << Qt::endl;
       }

      }}else{
            //começa aqui o caso do run
               QList<std::vector<QString> * >  runrows;

                QStringList cnamesL;
                cnamesL.clear();
                if(cnames.at(i).contains("Start")|| cnames.at(i).contains("End")){
                    double rownamesF;
                    cnamesL << "RunNumber"<<cnames.at(i);

                    for(int j=0; j<cnamesL.count(); j++) runrows << new ( std::vector<QString>);

                   // table.fechnotimecolumns_string("runlist",cnamesL,&runrows);
                    table.fechnotimecolumns_string_orderby("Runtable",cnamesL,"RunNumber",&runrows);

                    for(int j=0;j<(int )runrows.at(0)->size(); j++){
                        if(runrows.at(1)->at(j).toLongLong()<itime*1000 ||runrows.at(1)->at(j).toLongLong()>ltime*1000 ) continue;
                        rownamesF=runrows.at(0)->at(j).toDouble();
                        c.push_back(rownamesF);
                        time.push_back(runrows.at(1)->at(j).toLongLong());


                      }
                      for(int j=0; j<cnamesL.count(); j++) delete  runrows[j];
                      runrows.clear();


                    for(int j=0;j<(int )time.size();j++){
                        columnt.clear();line.clear();
                        if(timestring.contains("unixtime"))columnt.append(QString::number(time.at(j)/1000,'f',0));
                        else if (timestring.contains("igor"))columnt.append(QString::number(time.at(j)/1000+2082844800,'f',0));
                        else if (timestring.contains("unix_mili_time"))columnt.append(QString::number(time.at(j),'f',0));
                        else if (timestring.contains("matlabtime"))columnt.append(QString::number(time.at(j)/86400000.+719529,'f',0));
                        else  columnt.append(QDateTime::fromMSecsSinceEpoch(time.at(j)).toTimeSpec(Qt::UTC).toString(timestring));

                        line.append(columnt);
                        line.append(sep);
                        line.append(QString::number(c.at(j),'g',12));
                        *tempstreams[i] << line << Qt::endl;
                 }



                }else{
                    cnamesL << "StartTime" <<cnames.at(i);

                    for(int j=0; j<cnamesL.count(); j++) runrows << new ( std::vector<QString>);

                  //  table.fechnotimecolumns_string("runlist",cnamesL,&runrows);
                    table.fechnotimecolumns_string_orderby("Runtable",cnamesL,"RunNumber",&runrows);

                    for(int j=0;j<(int )runrows.at(0)->size(); j++){
                        if(runrows.at(0)->at(j).toLongLong()<itime*1000 ||runrows.at(0)->at(j).toLongLong()>ltime*1000 ) continue;
                        sc.push_back(runrows.at(1)->at(j));
                        time.push_back(runrows.at(0)->at(j).toLongLong());


                      }
                      for(int j=0; j<cnamesL.count(); j++) delete  runrows[j];
                      runrows.clear();

                    for(int j=0;j<(int )time.size();j++){
                        columnt.clear();line.clear();
                        if(timestring.contains("unixtime"))columnt.append(QString::number(time.at(j)/1000,'f',0));
                        else if (timestring.contains("igor"))columnt.append(QString::number(time.at(j)/1000+2082844800,'f',0));
                        else if (timestring.contains("unix_mili_time"))columnt.append(QString::number(time.at(j),'f',0));
                        else if (timestring.contains("matlabtime"))columnt.append(QString::number(time.at(j)/86400000.+719529,'f',0));
                        else  columnt.append(QDateTime::fromMSecsSinceEpoch(time.at(j)).toTimeSpec(Qt::UTC).toString(timestring));

                        line.append(columnt);
                        line.append(sep);
                        line.append(sc.at((j)));
                        *tempstreams[i] << line << Qt::endl;
                 }

                }
        // acaba aqui o run
    }

row=std::max(row,(long) time.size());
}

            for (int i=0; i< cnames.count(); i++){
                tempstreams[i]->seek(0);
            }
            QString lineout;
            for(int j=0; j< row; j++){
                lineout.clear();
                for (int i=0; i< cnames.count(); i++){
                    if(i!=0)  lineout.append(sep);
                   line=tempstreams[i]->readLine();
                   if(line.isNull())lineout.append(sep); else  lineout.append(line);
            }

                out << lineout << Qt::endl;
            }

        for (int i=0; i< cnames.count(); i++){
                delete tempstreams[i];
                tempfiles[i]->close();
            }

        file.close();
        }else{
            emit LogMessage("Unable to open database for downloading");

        }
        QSqlDatabase::removeDatabase(databaseconnection);
        emit LogMessage("ending the downloading");
        emit Endwork();


}
