#include "plotdata.h"
#include "dbapi.h"
#include "exprtk.hpp"
#include <algorithm>

plotdata::plotdata(QObject *parent) : QObject(parent)
{
    movie=false;
}
plotdata::~plotdata()
  {
for (int i = 0; i < xy.count(); i++) delete xy[i];
for (int i = 0; i < histo.count(); i++) delete histo[i];

  }


void plotdata::getxyzplotdata(bool firsttime){
    dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, &dbapi::logmessages, this,
            &plotdata::LogMessages);
    connect(&table, &dbapi::logmessagesR, this,
            &plotdata::LogMessagesR);
    long long ttimei;
    ttimei=timei;

    std::vector<long long> time;
    QVector<std::vector<double> *> c;

    if(firsttime) xyz.z.resize(cnames.count());
    else{
        if(!movie){
            for (int kk = 0; kk < cnames.count(); kk++) {
                xyz.z[kk].clear();
            }
            xyz.x.clear();
        }else {
            if(timei > mtimef ){
                for (int kk = 0; kk < cnames.count(); kk++) {
                    xyz.z[kk].clear();
                }
                xyz.x.clear();
            }else{
                int n=0;
             //    ttimei=mtimef;
                for (int j=0; j< (int) xyz.x.size(); j++ ){
                    if(1000*xyz.x.at(j) < timei) n++;
                    else break;
                }
                if((int) xyz.x.size()==0)ttimei=timei;else ttimei=std::min((long long)(1000*xyz.x.back()+1),mtimef);
                for (int kk = 0; kk < cnames.count(); kk++) {
                    xyz.z[kk].erase(xyz.z[kk].begin(),xyz.z[kk].begin()+n);
                }
                xyz.x.erase(xyz.x.begin(),xyz.x.begin()+n);
              //  LogMessagesR("erased "+QString::number(n));
            }

        }
    }
    for(int i=0; i<cnames.count(); i++) c << new ( std::vector<double>);
    table.fechvariouscolumns_timeanddoubles(tname, cnames, ttimei,timef, &time,&c);
    //LogMessages("fetch "+QString::number(time.size()));

    for (int j=0; j<(int)time.size(); j++){
        xyz.x.push_back(time.at(j)/1000.);
        for (int kk = 0; kk < cnames.count(); kk++) {
            xyz.z[kk].push_back(c.at(kk)->at(j)); //factor de escala aqui.
        }
    }

    for(int i=0;i<c.count();i++){
        delete c.at(i);
    }

}
void plotdata::gethistoplotdata(bool firsttime){
    //infelizmente não há  como implemenmtar um download incrementado
    dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, &dbapi::logmessages, this,
            &plotdata::LogMessages);
    connect(&table, &dbapi::logmessagesR, this,
            &plotdata::LogMessagesR);
    QStringList nhisto,nbins;
    nhisto=  Bins.split(";");

    if(firsttime){
        for (int i = 0; i < nhisto.count(); i++) {
            histo << new histodata;
            nbins=nhisto.at(i).split(",");
            for(int j=0; j<nbins.count(); j++) histo[i]->x.push_back(nbins.at(j));
        }}else{
        for (int i = 0; i < nhisto.count(); i++) {
            histo[i]->y.clear();
        }
    }
    std::vector<long long> time;
    QVector<std::vector<double> *> c;
    for(int i=0; i<cnames.count(); i++) c << new ( std::vector<double>);
    table.fechvariouscolumns_timeanddoubles(tname, cnames, timei,timef, &time,&c);

    int m=-1;
    for (int i = 0; i < nhisto.count(); i++) {
        nbins=nhisto.at(i).split(",");
    for(int k=0; k<nbins.count(); k++){
        m++;
            double sum=0.;
            int npoints=0;
        for (int j=0; j<(int)time.size(); j++){
            sum=sum+c.at(m)->at(j);
            npoints++;
        }

        histo[i]->y.push_back(sum/npoints);
        }
    }

    for(int i=0;i<c.count();i++){
        delete c.at(i);
    }


}
void plotdata::getxyplotdata(bool firsttime)
{
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>   expression_t;
    typedef exprtk::parser<double>       parser_t;
     QString mathexpT;
     double y;
     symbol_table_t symbol_table;
     symbol_table.add_variable("y",y);
     symbol_table.add_constants();
     expression_t expression;
     expression.register_symbol_table(symbol_table);

    dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, &dbapi::logmessages, this,
            &plotdata::LogMessages);
    connect(&table, &dbapi::logmessagesR, this,
            &plotdata::LogMessagesR);
    std::vector<double> vec;
    std::vector<long long> time;
    QList<long long> ttimei;
   for (int i = 0; i < tnames.count(); i++) {
    ttimei<<timei;
   }
    if(firsttime){
                 //for (int i = 0; i < tnames.count(); i++) {
                         //  xy << new xydata; passou para o plotcanvas para permitr passar o factor
                         //  xy[i]->p.cname=cnames.at(i);
                         //  xy[i]->p.tname=tnames.at(i);

                 //    }
}else{
        if(!movie){
        for (int i = 0; i < tnames.count(); i++) {
            xy[i]->x.clear();
            xy[i]->y.clear();
   }
        }else {
            if(timei > mtimef ){
                 for (int i = 0; i < tnames.count(); i++) {
                xy[i]->x.clear();
                xy[i]->y.clear();
                 }
            }else{
                 for (int i = 0; i < tnames.count(); i++) {
                     int n=0;

                     for (int j=0; j< (int) xy[i]->x.size(); j++ ){
                         if(1000*xy[i]->x.at(j) < timei) n++;
                         else break;
                     }
                     if((int) xy[i]->x.size()==0) ttimei[i]=timei;else{
                         ttimei[i]=std::min((long long)(1000*xy[i]->x.back()+1),mtimef);
                     }
                     xy[i]->x.erase(xy[i]->x.begin(), xy[i]->x.begin()+n);
                     xy[i]->y.erase(xy[i]->y.begin(), xy[i]->y.begin()+n);
                   //  LogMessagesR("erased "+QString::number(n));
                 }
            }
        }

    }
    for (int i = 0; i < tnames.count(); i++) {
    //    qDebug()<<timei << "   " <<ttimei.at(i);

        vec.clear();
        time.clear();
        if(!tnames.at(i).contains("_channel")){
            if(tnames.at(i)!="Runtable") {

        table.fech2columns_timeanddouble(tnames.at(i), cnames.at(i), ttimei.at(i), timef,
                                             &time, &vec);
        //LogMessages("fetch "+QString::number(vec.size()));
            }else{
                QList<std::vector<QString> * >  runrows;
                QStringList cnamesL;
                cnamesL.clear();
                    double rownamesF;
                    cnamesL << "RunNumber" <<cnames[i];

                    for(int j=0; j<cnamesL.count(); j++) runrows << new ( std::vector<QString>);

                    table.fechnotimecolumns_string_orderby("Runtable",cnamesL,"RunNumber",&runrows);

                    for(int j=0;j<(int )runrows.at(0)->size(); j++){
                        if(runrows.at(1)->at(j).toLongLong() <timei ||
                            runrows.at(1)->at(j).toLongLong() >timef) continue;
                        rownamesF=runrows.at(0)->at(j).toDouble();
                        vec.push_back(rownamesF);
                        time.push_back(runrows.at(1)->at(j).toLongLong());

                    }
                    for(int j=0; j<cnamesL.count(); j++) delete  runrows[j];
                    runrows.clear();
            }
        }else {
            QStringList cols;
            cols.clear();
            cols<<"channel_id" <<"type_table";
            QList <std::vector<QString> * > cc;
            for(int i=0; i<cols.count(); i++) cc << new ( std::vector<QString>);
            table.fechnotimecolumns_string_filterstring(tnames.at(i),cols,"channel",cnames.at(i),&cc);
            table.fech2columns_timeanddoublefilter(cc.at(1)->at(0), "value", "channel_id",cc.at(0)->at(0),
                                                   ttimei.at(i), timef, &time,&vec);
            for(int j=0; j<cols.count(); j++) delete cc[j];
            cc.clear();
        }

        unsigned long size = time.size();
        bool goodexpression=false;
        parser_t parser;
        if(xy.at(i)->p.factor.contains("y")){
         mathexpT=  xy.at(i)->p.factor;
        goodexpression=parser.compile(mathexpT.toStdString(),expression);
        }
        for (unsigned long j = 0; j < size; j++) {

            if(!xy.at(i)->p.factor.contains("y")) {
                xy[i]->x.push_back(time.at(j) / 1000.);
                xy[i]->y.push_back(vec.at(j)*xy.at(i)->p.factor.toDouble());
             }else{
              double vvalue;
              QString svalue;
              if (goodexpression)
              {
                  y=vec.at(j);
                  vvalue = expression.value();
                  svalue=QString::number(vvalue);
                  if(svalue.contains("n")) continue;
                   xy[i]->x.push_back(time.at(j) / 1000.);
                   xy[i]->y.push_back(vvalue);
              }else {
                  continue;
              }

            }
        }

    }

}
void plotdata::LogMessages(QString mess)
{
    emit logmessages(mess);
}
void plotdata::LogMessagesR(QString text)
{

    emit logmessagesR(text);


}
