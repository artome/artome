#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMouseEvent>
#include <QItemSelectionModel>
#include <QInputDialog>
#include <QDebug>
#include "help.h"


#include "tablecanvas.h"
#include "ui_tablecanvas.h"
#include "dbapi.h"
#include "savetable.h"
#include "plot2table.h"

TableCanvas::TableCanvas(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TableCanvas)
{
    ui->setupUi(this);
    timestring="yyyy/MM/dd hh:mm:ss.zzz";
    menutableview=NULL;
    ui->tableView->viewport()->installEventFilter(this);
    connect(ui->actionSave_into_local_drive, &QAction::triggered,this,&TableCanvas::savedata);
    Itime.setTimeSpec(Qt::UTC);
    Ltime.setTimeSpec(Qt::UTC);
    superuser=true;
    timestringformat="dd/MM/yyyy hh:mm:ss.zzz";
    filltimestrings();
  //  connect(ui->Applydisplayformat, SIGNAL(pressed()),this,SLOT(on_Applydisplayformat_clicked()));
    connect(ui->sometexttimeformats, &QComboBox::activated, this,&TableCanvas::settimestrings);
    connect(ui->actionSurface_Plot, &QAction::triggered,this,&TableCanvas::Surface_Plot_triggered);
    connect(ui->actionMake_a_plot, &QAction::triggered,this,&TableCanvas::Make_a_plot_triggered);
    connect(ui->actionChoose_columns_to_plot, &QAction::triggered,this,&TableCanvas::Choose_columns_to_plot_triggered);
    connect(ui->actionColumn_Statistics, &QAction::triggered,this,&TableCanvas::Column_Statistics_triggered);
    connect(ui->actionLocal_Polynomial_fit, &QAction::triggered,this,&TableCanvas::Local_Polynomial_fit_triggered);
    connect(ui->actionSave_into_database, &QAction::triggered,this,&TableCanvas::Save_into_database_triggered);
    connect(ui->Applydisplayformat, &QAbstractButton::clicked,this,&TableCanvas::Applydisplayformat_clicked);


}

TableCanvas::~TableCanvas()
{
    delete ui;

}

/*void TableCanvas::timechanged(long long x, long long y){

}
*/

void TableCanvas::costumize()
{
    if(TableType.contains("FromLocalFileTobanana")){
        ui->tabWidget->hide();
        ui->actionSurface_Plot->setEnabled(true);
        ui->actionChoose_columns_to_plot->setVisible(false);
        ui->actionMake_a_plot->setVisible(false);
        if(superuser){
        ui->actionSave_into_database->setVisible(true);
        ui->actionSave_into_database->setEnabled(true);
        } else{
            ui->actionSave_into_database->setVisible(false);
            ui->actionSave_into_database->setEnabled(false);

        }
    }
}

void TableCanvas::getdatabaseinfo( QList<QVariant> dinfo, int index) {
    if(index!=databaseindex){
        return;
    }else{

        Itime=Itime.fromSecsSinceEpoch(dinfo.at(2).toLongLong()).toUTC();
        Ltime=Ltime.fromSecsSinceEpoch(dinfo.at(3).toLongLong()).toUTC();
     }
 }
void TableCanvas::filltablefromplot(QList<int> subpag, QList<int> element)
{

   int size;
   for(int s=0;s<subpag.count();s++){
       if(p->at(subpag.at(s))->plottype ==0){

           size=(int) data->at(subpag.at(s))->xy.at(element.at(s))->x.size();
       if(size){

       xymodel.headernames <<    "time";
       xymodel.ctype <<0;
       xymodel.headernames <<  data->at(subpag.at(s))->xy.at(element.at(s))->p.yaxisLabel;
       xymodel.ctype <<1;

           std::vector<double>* dataVec = new std::vector<double>(size);
           for (int ii = 0; ii < size; ii++) {
               dataVec->at(ii)= 1000*data->at(subpag.at(s))->xy.at(element.at(s))->x.at(ii);
           }
         if(size)  xymodel.addcoldoublearray(0,&dataVec->at(0),size);
         xymodel.addcoldoublearray(1,&data->at(subpag.at(s))->xy.at(element.at(s))->y[0],size);

  }

   } else  if(p->at(subpag.at(s))->plottype ==1){
       size=data->at(subpag.at(s))->xyz.x.size();
   if(size){
       TableType="bananafromtable";
       xymodel.rownames.clear();
       for(int jj=0; jj< (int) data->at(subpag.at(s))->xyz.y.size();jj++)
       {
           xymodel.addcolempty(1,size);
           xymodel.headernames << QString::number(data->at(subpag.at(s))->xyz.y.at(jj),'g',6);
           bananadiameter<<data->at(subpag.at(s))->xyz.y.at(jj);
           xymodel.ctype <<1;
           for (int ii = 0; ii < size; ii++) {
            if(jj==0){
                xymodel.rownames <<  QDateTime::fromSecsSinceEpoch((long long)data->at(subpag.at(s))->xyz.x.at(ii)).toUTC().toString(timestring);
            bananatime <<data->at(subpag.at(s))->xyz.x.at(ii);
            }
            xymodel.fillemptycol(1,ii,data->at(subpag.at(s))->xyz.z.at(jj).at(ii));
         }
        }
   }


   }    else  if(p->at(subpag.at(s))->plottype ==2){
   QString lixo;

       TableType="histoplot";
        xymodel.headernames << "binnames";
        xymodel.ctype <<2;
        xymodel.headernames << "values";
        xymodel.ctype <<1;

       size=(int) data->at(subpag.at(s))->histo.at(element.at(s))->x.size();
       xymodel.addcolempty(2,size);
       xymodel.addcolempty(1,size);
       for(int ii=0; ii<size; ii++){
           xymodel.fillStringemptycol(2,ii,data->at(subpag.at(s))->histo.at(element.at(s))->x.at(ii).toLatin1().constData());
           xymodel.fillemptycol(1,ii,data->at(subpag.at(s))->histo.at(element.at(s))->y.at(ii));
       }

   }
   }



   ui->tableView->setModel( &xymodel );
   //tablefit();
   ui->tableView->show();


}

void  TableCanvas::filltable()
{


    std::vector<double>   c;
    std::vector<QString>   sc;

    std::vector<long long> time;
    QString realname,instname;

    dbapi table;
    table.connectionName=databaseconnection;
    connect (&table,&dbapi::logmessages,this,&TableCanvas::LogMessage);
    connect (&table,&dbapi::logmessagesR,this,&TableCanvas::LogMessage);
    for(int i=0; i<cnames.size(); i++) {

        xymodel.headernames <<    "time";
      c.clear();time.clear(),sc.clear();
     if(tnames.at(i).compare("Runtable")){


       xymodel.headernames <<  aliasname.at(i);
      if(!tnames.at(i).contains("_channels")){
      if(!ctypes.at(i).toString().contains("char")){
          table.fech2columns_timeanddouble(tnames.at(i), cnames.at(i), (long long) Itime.toUTC().toMSecsSinceEpoch(),(long long)Ltime.toMSecsSinceEpoch(), &time,&c);
        xymodel.ctype <<0;
        xymodel.addcollonglongv(0,&time);
        xymodel.ctype <<1;
        xymodel.addcolv(1,&c);

      }else{
          table.fech2columns_timeandstring(tnames.at(i), cnames.at(i),(long long) Itime.toUTC().toMSecsSinceEpoch(),(long long)Ltime.toMSecsSinceEpoch(), &time,&sc);
          xymodel.ctype <<0;
          xymodel.addcollonglongv(0,&time);
          xymodel.ctype <<2;
          xymodel.addcolstringv(2,&sc);

      }        

         }else{ //caso type2
           QStringList cols;
           cols.clear();
           cols<<"channel_id" <<"type_table";
           QList <std::vector<QString> * > cc;
           for(int i=0; i<cols.count(); i++) cc << new ( std::vector<QString>);
           table.fechnotimecolumns_string_filterstring(tnames.at(i),cols,"channel",cnames.at(i),&cc);
       //    table.fechlineofstrings(tnames.at(i),"channel",cnames.at(i),cols,&lcols);

           if(!cc.at(1)->at(0).contains("_string")){
               table.fech2columns_timeanddoublefilter(cc.at(1)->at(0), "value", "channel_id",cc.at(0)->at(0),(long long) Itime.toUTC().toMSecsSinceEpoch(),(long long)Ltime.toMSecsSinceEpoch(), &time,&c);
               xymodel.ctype <<0;
               xymodel.addcollonglongv(0,&time);
               xymodel.ctype <<1;
               xymodel.addcolv(1,&c);

             }else{
                 table.fech2columns_timeandstringfilter(cc.at(1)->at(0), "value", "channel_id",cc.at(0)->at(0),(long long) Itime.toUTC().toMSecsSinceEpoch(),(long long)Ltime.toMSecsSinceEpoch(), &time,&sc);
                 xymodel.ctype <<0;
                 xymodel.addcollonglongv(0,&time);
                 xymodel.ctype <<2;
                 xymodel.addcolstringv(2,&sc);

             }
             for(int i=0; i<cols.count(); i++) delete cc[i];
             cc.clear();

      }
     }else{
            //começa aqui o caso do run
               QList<std::vector<QString> * >  runrows;

                QStringList cnamesL;
                cnamesL.clear();
                if(cnames[i].contains("Start")|| cnames[i].contains("End")){
                    double rownamesF;
                    cnamesL << "RunNumber" <<cnames[i];

                    for(int j=0; j<cnamesL.count(); j++) runrows << new ( std::vector<QString>);

                  //  table.fechnotimecolumns_string("Runtable",cnamesL,&runrows);
                    table.fechnotimecolumns_string_orderby("Runtable",cnamesL,"RunNumber",&runrows);

                    for(int j=0;j<(int )runrows.at(0)->size(); j++){
                        if(runrows.at(1)->at(j).toLongLong() <(long long) Itime.toUTC().toMSecsSinceEpoch() ||
                            runrows.at(1)->at(j).toLongLong() >(long long) Ltime.toUTC().toMSecsSinceEpoch() ) continue;
                        rownamesF=runrows.at(0)->at(j).toDouble();
                        c.push_back(rownamesF);
                        time.push_back(runrows.at(1)->at(j).toLongLong());


                      }
                      for(int j=0; j<cnamesL.count(); j++) delete  runrows[j];
                      runrows.clear();

                    xymodel.ctype <<0;
                    xymodel.addcollonglongv(0,&time);
                    xymodel.ctype <<1;
                    xymodel.addcolv(1,&c);
                    xymodel.headernames <<    "RunNumber";


                }else{
                    cnamesL << "StartTime" <<cnames[i];

                    for(int j=0; j<cnamesL.count(); j++) runrows << new ( std::vector<QString>);

                    //table.fechnotimecolumns_string("Runtable",cnamesL,&runrows);
                    table.fechnotimecolumns_string_orderby("Runtable",cnamesL,"RunNumber",&runrows);

                    for(int j=0;j<(int) runrows.at(0)->size(); j++){
                        if(runrows.at(0)->at(j).toLongLong() <(long long) Itime.toUTC().toMSecsSinceEpoch() ||
                            runrows.at(0)->at(j).toLongLong() >(long long) Ltime.toUTC().toMSecsSinceEpoch() ) continue;

                        sc.push_back(runrows.at(1)->at(j));
                        time.push_back(runrows.at(0)->at(j).toLongLong());


                      }
                      for(int j=0; j<cnamesL.count(); j++) delete  runrows[j];
                      runrows.clear();


                    xymodel.ctype <<0;
                    xymodel.addcollonglongv(0,&time);
                    xymodel.ctype <<2;
                    xymodel.addcolstringv(2,&sc);
                     xymodel.headernames <<    cnames[i];

                }
        // acaba aqui o run
    }


}
    ui->tableView->setModel( &xymodel );
    //tablefit();
    ui->tableView->show();

}
void TableCanvas::savedata()
{
    SaveTable dialog;
    QString timestring,separator,sep,filedataName;
    int result;
    dialog.suggestname=TableTitle;
    if(TableType.contains("runtable") ||TableType.contains("noTimetable") ) dialog.hidetimeoptions();
    result = dialog.exec();
    if(result){
        filedataName=dialog.fileName;
        timestring=dialog.timeString;
        separator=dialog.separator;
        if(separator.contains("comma")) sep=","; else if (separator.contains("tab")) sep="\t";
        else if (separator.contains("space")) sep=" ";
 //
        QFile file(filedataName);

        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Cannot write file %1:\n%2.")
                                    .arg(filedataName)
                                    .arg(file.errorString()));
            emit LogMessage("couldn't open de file to save data");
            return;
        }

        QTextStream out(&file);

        out.setRealNumberNotation(QTextStream::SmartNotation);
        out.setRealNumberPrecision(10);

if(TableType.contains("time_versus_axisdata")){

            QString lixo;
            lixo.clear();
            for (int j = 0; j < xymodel.columnCount(); j++) {

                lixo.append(xymodel.headernames[j]);
                if (j < xymodel.columnCount() - 1) lixo.append(sep);
            }
    //        emit LogMessage("columns"+QString::number(xymodel.columnCount()));
            out << lixo.toLatin1().constData();
            out << Qt::endl;

            for (int i = 0; i < xymodel.rowCount(); i++) {
                  lixo.clear();
                for (int j = 0; j < xymodel.columnCount(); j++) {

                    if(xymodel.ctype[j]==1){
                       if((int)xymodel.m_datareal.at(xymodel.cindex[j])->size() >i) lixo.append(QString::number(xymodel.m_datareal.at(xymodel.cindex[j])->at(i)));
                       else lixo.append("");
                    }else if (xymodel.ctype[j]==2){
                          if((int)xymodel.m_datastring.at(xymodel.cindex[j])->size() >i)
                              lixo.append(xymodel.m_datastring.at(xymodel.cindex[j])->at(i).c_str());
                          else lixo.append("");
                       }else{
                         if((int)xymodel.m_datatime.at(xymodel.cindex[j])->size() >i){
                        if (timestring.contains("unixtime")) {
                             lixo.append(QString::number((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)/1000.,'f',0));
                        } else if (timestring.contains("igor")) {
                           lixo.append(QString::number((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)/1000.+ 2082844800,'f',0));
                        }  else if (timestring.contains("unix_mili_time"))lixo.append(QString::number((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i),'f',0));
                        else if (timestring.contains("matlabtime"))lixo.append(QString::number((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)/86400000.+719529,'f',0));
 else {
                            lixo.append(QDateTime::fromMSecsSinceEpoch((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)).toUTC().toString(timestring));
                        }
                       }  else lixo.append("");
                    }
                    if (j < xymodel.columnCount() - 1) lixo.append(sep);


                    }
                 out << lixo.toLatin1().constData();
                 out << Qt::endl;
                }

            }else if(TableType.contains("Banana")){

    QString lixo;
    lixo.clear();
    lixo.append("time\\diameters");
    lixo.append(sep);
    for (int j = 0; j < xymodel.columnCount(); j++) {
        lixo.append(QString::number(bananadiameter[j]));
        if (j < xymodel.columnCount() - 1) lixo.append(sep);
    }

    out << lixo.toLatin1().constData();
    out << Qt::endl;

    for (int i = 0; i < xymodel.rowCount(); i++) {
          lixo.clear();
           if (timestring.contains("unixtime")) {
             lixo.append(QString::number((long long)bananatime[i], 'f',0));
             } else if (timestring.contains("igor")) {
              lixo.append(QString::number((long long)bananatime[i]+ 2082844800,'f',0));
            } else {
            lixo.append((QDateTime::fromSecsSinceEpoch((long long)bananatime[i]).toUTC().toString(timestring)));
            }

        lixo.append(sep);
        for (int j = 0; j < xymodel.columnCount(); j++) {
               if((int)xymodel.m_datareal.at(xymodel.cindex[j])->size() >i) lixo.append(QString::number(xymodel.m_datareal.at(xymodel.cindex[j])->at(i)));
               else lixo.append("");

            if (j < xymodel.columnCount() - 1) lixo.append(sep);


            }
         out << lixo.toLatin1().constData();
         out << Qt::endl;
        }

}else if(TableType.contains("histoplot")){


    QString lixo;
    lixo.clear();
    for (int j = 0; j < xymodel.columnCount(); j++) {

        lixo.append(xymodel.headernames[j]);
        if (j < xymodel.columnCount() - 1) lixo.append(sep);
    }
//        emit LogMessage("columns"+QString::number(xymodel.columnCount()));
    out << lixo.toLatin1().constData();
    out << Qt::endl;

    for (int i = 0; i < xymodel.rowCount(); i++) {
          lixo.clear();
        for (int j = 0; j < xymodel.columnCount(); j++) {

            if(xymodel.ctype[j]==1){
               if((int)xymodel.m_datareal.at(xymodel.cindex[j])->size() >i) lixo.append(QString::number(xymodel.m_datareal.at(xymodel.cindex[j])->at(i)));
               else lixo.append("");
            }else if (xymodel.ctype[j]==2){
                  if((int)xymodel.m_datastring.at(xymodel.cindex[j])->size() >i)
                      lixo.append(xymodel.m_datastring.at(xymodel.cindex[j])->at(i).c_str());
                  else lixo.append("");
               }else{
                 if((int)xymodel.m_datatime.at(xymodel.cindex[j])->size() >i){
                if (timestring.contains("unixtime")) {
                     lixo.append(QString::number((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)/1000.,'f',0));
                } else if (timestring.contains("igor")) {
                   lixo.append(QString::number((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)/1000.+ 2082844800,'f',0));
                } else {
                    lixo.append(QDateTime::fromMSecsSinceEpoch((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)).toUTC().toString(timestring));
                }
               }  else lixo.append("");
            }
            if (j < xymodel.columnCount() - 1) lixo.append(sep);


            }
         out << lixo.toLatin1().constData();
         out << Qt::endl;
        }

}else{




    QString lixo;
    lixo.clear();
    for (int j = 0; j < xymodel.columnCount(); j++) {

        lixo.append(xymodel.headernames[j]);
        if (j < xymodel.columnCount() - 1) lixo.append(sep);
    }
//        emit LogMessage("columns"+QString::number(xymodel.columnCount()));
    out << lixo.toLatin1().constData();
    out << Qt::endl;


    for (int i = 0; i < xymodel.rowCount(); i++) {
          lixo.clear();
        for (int j = 0; j < xymodel.columnCount(); j++) {

            if(xymodel.ctype[j]==1){
               if((int)xymodel.m_datareal.at(xymodel.cindex[j])->size() >i) lixo.append(QString::number(xymodel.m_datareal.at(xymodel.cindex[j])->at(i)));
               else lixo.append("");
            }else if (xymodel.ctype[j]==2){
                  if((int)xymodel.m_datastring.at(xymodel.cindex[j])->size() >i)
                      lixo.append(xymodel.m_datastring.at(xymodel.cindex[j])->at(i).c_str());
                  else lixo.append("");
               }else{
                 if((int)xymodel.m_datatime.at(xymodel.cindex[j])->size() >i){
                if (timestring.contains("unixtime")) {
                     lixo.append(QString::number((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)/1000.,'f',0));
                } else if (timestring.contains("igor")) {
                   lixo.append(QString::number((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)/1000.+ 2082844800,'f',0));
                } else {
                    lixo.append(QDateTime::fromMSecsSinceEpoch((long long)xymodel.m_datatime.at(xymodel.cindex[j])->at(i)).toUTC().toString(timestring));
                }
               }  else lixo.append("");
            }
            if (j < xymodel.columnCount() - 1) lixo.append(sep);


            }
         out << lixo.toLatin1().constData();
         out << Qt::endl;
        }

}


file.close();

    }
}

void TableCanvas::settimetitle()
{
setWindowTitle(Itime.toString("dd/MM/yyyy hh:mm:ss  ")+TableTitle+Ltime.toString("   dd/MM/yyyy hh:mm:ss"));
}

void TableCanvas::showhelp(){

       Help dialog;
       dialog.Text( "<html><head/><body><p>By default the t<span style=\" color:#0000ff;\">able windows</span> have an area for the <span style=\" color:#008000;\">table values</span> itself and a <span style=\" color:#008000;\">log area </span>to display information whenever there are mathematical operations performed in its values. By its nature some <span style=\" color:#0000ff;\">tables windows </span>will not have displayed the <span style=\" color:#008000;\">log area</span>. Others table windows can enlarge or shrink the <span style=\" color:#008000;\">log area</span>, through mouse actions, or close/open it using the entry logwindow at the menu area.</p><p><img src=\":/ScreenShots/Icons/tablemenuline.png\"/></p><p align=\"justify\">The<span style=\" color:#0000ff;\"> table window</span> has a task bar. The taskbar can be moved or closed to take the most of the available space. It can be removed/added by using the menu entry<span style=\" font-weight:600; font-style:italic;\"> ToolBar </span>or by click the right mouse butto"
                    "n anywhere in the menu area or menu bar. All the actions (icons) in the tool bar are also sub-entries in the menu area items. At the moment all of them but the help are in<span style=\" font-weight:600; font-style:italic;\"> menu </span>entry.</p><p align=\"justify\">It is also in our roadmap to include a lot of mouse operations, but for now and only for tables with time curves (may them be obtained from plots or from the <span style=\" color:#0000ff;\">numerical channel dialog</span>) whenever one clicks in a cell the <span style=\" font-style:italic;\">mean, maximum</span>, <span style=\" font-style:italic;\">minimum</span> and <span style=\" font-style:italic;\">variance</span> of the column data will be displayed at the <span style=\" color:#008000;\">log window area</span>. </p><p>The actions on the taskbar can be much less depending of the table nature. However the save action and help action will always be present.</p><p>The actions at taskbar <img src=\":/ScreenShots/Icons/tabletaskbar.png\"/> are by t"
                    "he order of apperance:</p><p><br/></p><p><img src=\":/Icons/Icons/table-save.png\"/> this will save in a local ascii file the table. Depending of the table nature one will have the option to choose between several time formats to save the columns of time. </p><p><img src=\":/Icons/Icons/insert-chart-curve.png\"/> this will build a mutiaxis plot with the items sellected. Are only considered items in database so the plot will also be visible for the sync time mechanism. </p><p><img src=\":/Icons/Icons/tools-sum.png\"/> this will allow to choose a set of matematical operations to perform on data. At the time this help was written <span style=\" color:#ff00ff;\">no operations were yet implemented</span>. However all of them will have their own help, if justified. Basic statics values for each column (max, min, variance and mean values) are displayed in the<span style=\" color:#008000;\"> log window area</span> whenever one mouse clicks in a table cell.</p><p><img src=\":/Icons/Icons/tool-animator.png\"/> this will"
                    " <span style=\" font-style:italic;\">rebuild </span>de table using the time interval of the<span style=\" color:#0000ff;\"> mainwindow interface</span>. It will only work on items stored in database.</p><p><img src=\":/Icons/Icons/link-break.png\"/> this will only set a flag! So if checked <img src=\":/ScreenShots/Icons/syncchecked.png\"/> the <span style=\" color:#0000ff;\">table window</span> time interval will be syncronized, and the table rebuilt, if needed, whenever one press the <img src=\":/ScreenShots/Icons/synclinkplotsbutton.png\"/> button at the <span style=\" color:#0000ff;\">main interface</span>. This is especially useful when one wants to to time syncronize, simultaneously, a set of windows. It only works with items stored in database.</p><p><img src=\":/Icons/Icons/plotNosync.png\"/> this will build a mutiaxis plot with columns of the table, regardeless of them beeing on database or not. So the plot will not be visible for the sync time mechanism. <span style=\" color:#ff0000;\">(not implemente"
                    "d yet when this short help was written) </span></p><p><img src=\":/Icons/Icons/clock-edit.png\"/> this will allow the oportunity to change only the time interval of the<span style=\" color:#0000ff;\"> table window</span>, without the need to use the time sync mechanism with the <span style=\" color:#0000ff;\">main interface</span> time interval. It only works with table of items stored in dtabase.</p><p><img src=\":/Icons/Icons/help-contents-5.png\"/> You are reading this so you know what this action does.<br/></p><p><br/></p><p><br/></p></body></html>"
                   );
       int result;
        result = dialog.exec();
        if(result){

        }


}
void TableCanvas::tablefromlocalformatedfile()
{
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("open file to import"),
                             tr("Cannot open file to read "));
        return;
    }
    QString sep, line;
    QStringList  listofvalues;
    QDateTime   time;
    time.setTimeSpec(Qt::UTC);

    if(separator.contains("comma")) sep=","; else if (separator.contains("tab")) sep="\t";
    else if (separator.contains("space")) sep=" ";else if (separator.contains("semicolon")) sep=";";
    QTextStream in(&file);

    for (int j = 0; j < igl; j++) {
        in.readLine();
    }


       long long t;
       line = in.readLine();
       QStringList columnstitle;
       columnstitle=line.split(QRegularExpression(sep));
       xymodel.headernames <<    "time";
       xymodel.ctype <<0;
       QList<std::vector<double> * > m_datareal;
       QList<std::vector<double> * > m_datatime;
       m_datatime <<  new std::vector<double>;

       for (int i=1; i< columnstitle.count();i++)
       {
           xymodel.headernames <<   columnstitle.at(i);
           xymodel.ctype <<1;
           m_datareal <<  new std::vector<double>;
       }
       colstime.clear();
       colstime <<0;
       gtimeformat=timestring;

    do {
        line = in.readLine();
        if(line.isNull()) break;
        listofvalues = line.split(QRegularExpression(sep));
        t=gettime(&listofvalues);
        if(t <0) continue;
        if(listofvalues.count() <columnstitle.count() )continue;

        m_datatime.at(0)->push_back(((double) t));
        for (int i=1; i< columnstitle.count();i++)
        {

            m_datareal.at(i-1)->push_back( listofvalues.at(i).toDouble());
        }

    } while (!line.isNull());

       int size=m_datatime.at(0)->size();
       if(size){
       xymodel.addcoldoublearray(0,&m_datatime.at(0)->at(0),size);
       for (int i=0; i< columnstitle.count()-1;i++)
       {
          xymodel.addcoldoublearray(1,&m_datareal.at(i)->at(0),size);

       }

      }

    ui->tableView->setModel( &xymodel );
    //tablefit();
    ui->tableView->show();

}


void TableCanvas::tablefromlocalfile(QString columnstoinclude,QString timecolumns)
{
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("open file to import"),
                             tr("Cannot open file to read "));
        return;
    }
    QString sep, line;
    QStringList  listofvalues;
    QDateTime   time;
    time.setTimeSpec(Qt::UTC);

    if(separator.contains("comma")) sep=","; else if (separator.contains("tab")) sep="\t";
    else if (separator.contains("space")) sep=" ";else if (separator.contains("semicolon")) sep=";";
    QTextStream in(&file);
    colstime.clear();
    colstimeformat.clear();

    for (int j = 0; j < igl; j++) {
        in.readLine();
    }

     QStringList channeltimes;

     datevalues = timecolumns.split(QRegularExpression(";"));
     for (int j=0; j<datevalues.count(); j++)
        {
           channeltimes=datevalues.at(j).split(QRegularExpression("@"));
           colstime << channeltimes.at(1).toInt();
           colstimeformat << channeltimes.at(0);
     }
     for (int j=0; j<colstime.count(); j++)
        {
           if(colstimeformat.at(j)=="UNIXTIME"){
              gtimeformat= "UNIXTIME";
              int i;
              i=colstime.at(j);
              colstime.clear();
              colstime<<i;
              break;
           }else if(datevalues.at(j)=="IGORTIME"){
               gtimeformat= "IGORTIME";
               int i;
               i=colstime.at(j);
               colstime.clear();
               colstime<<i;
               break;
           }else if(datevalues.at(j)=="UNIXTIMIMILI"){
               gtimeformat= "UNIXTIMIMILI";
               int i;
               i=colstime.at(j);
               colstime.clear();
               colstime<<i;
               break;
           }
       gtimeformat.append(colstimeformat[j].simplified());
       if(j<colstime.count()-1) gtimeformat.append(":");

     }
       long long t;
       QStringList columnsstring,channellcolumns;
       columnsstring = columnstoinclude.split(QRegularExpression(";"));
       QList<int> collumnstoadd;
       QStringList columnstitle;
       collumnstoadd.clear();columnstitle.clear();
       for (int j=0; j<columnsstring.count(); j++)
          {
             channellcolumns=columnsstring.at(j).split(QRegularExpression("@"));
             collumnstoadd << channellcolumns.at(1).toInt();
             columnstitle << channellcolumns.at(0);
       }
       xymodel.headernames <<    "time";
       xymodel.ctype <<0;
       QList<std::vector<double> * > m_datareal;
       QList<std::vector<double> * > m_datatime;
       m_datatime <<  new std::vector<double>;

       for (int i=0; i< columnstitle.count();i++)
       {
           xymodel.headernames <<   columnstitle.at(i);
           xymodel.ctype <<1;
           m_datareal <<  new std::vector<double>;
       }




    do {
        line = in.readLine();
        if(line.isNull()) break;
        listofvalues = line.split(QRegularExpression(sep));
        t=gettime(&listofvalues);
        if(t <0) continue;
        if(listofvalues.count() <columnsstring.count() )continue;

        m_datatime.at(0)->push_back(((double) t));
        for (int i=0; i< columnstitle.count();i++)
        {
            if(collumnstoadd.at(i)<listofvalues.count())
            m_datareal.at(i)->push_back( listofvalues.at(collumnstoadd.at(i)).toDouble());
        }

    } while (!line.isNull());

       int size=m_datatime.at(0)->size();
       if(size){
       xymodel.addcoldoublearray(0,&m_datatime.at(0)->at(0),size);
       for (int i=0; i< columnstitle.count();i++)
       {
          xymodel.addcoldoublearray(1,&m_datareal.at(i)->at(0),size);

       }

      }

    ui->tableView->setModel( &xymodel );
    //tablefit();
    ui->tableView->show();

}
void TableCanvas::tablefromlocalfilefornanana(){
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("open file to import"),
                             tr("Cannot open file to read "));
        return;
    }
    QString sep, line;
    QStringList  listofvalues;
    QDateTime   time;
    time.setTimeSpec(Qt::UTC);

    if(separator.contains("comma")) sep=","; else if (separator.contains("tab")) sep="\t";
    else if (separator.contains("space")) sep=" ";else if (separator.contains("semicolon")) sep=";";
    QTextStream in(&file);

    for (int j = 0; j < igl; j++) {
        in.readLine();
    }


       long long t;
       QStringList channellcolumns;
       xymodel.headernames <<    "time";
       xymodel.ctype <<0;
       QList<std::vector<double> * > m_datareal;
       QList<std::vector<double> * > m_datatime;
       m_datatime <<  new std::vector<double>;

       line = in.readLine();
       channellcolumns = line.split(QRegularExpression(sep));

       for (int i=1; i< channellcolumns.count();i++)
       {
           xymodel.headernames <<   channellcolumns.at(i);
           xymodel.ctype <<1;
           m_datareal <<  new std::vector<double>;
       }

    do {
        line = in.readLine();
        if(line.isNull()) break;
        listofvalues = line.split(QRegularExpression(sep));

        if(timestring.contains("UNIXTIMEMILI")) t=listofvalues.at(0).toLongLong();
        else if(timestring.contains("UNIXTIME")) t=1000.*listofvalues.at(0).toLongLong();
        else if(timestring.contains("IGORTIME")) t=1000.*(listofvalues.at(0).toLongLong()-2082844800);
        else if(timestring.contains("MATLABTIME")) t=1000.*((listofvalues.at(0).toLongLong()-719529)*86400);
        else t=time.fromString(listofvalues.at(0),timestring).toMSecsSinceEpoch();
        if(t <0) continue;
        if(listofvalues.count() <channellcolumns.count() )continue;

        m_datatime.at(0)->push_back(((double) t));
        for (int i=0; i< listofvalues.count()-1;i++)
        {
            m_datareal.at(i)->push_back( listofvalues.at(i+1).toDouble());
        }

    } while (!line.isNull());

       int size=m_datatime.at(0)->size();
       if(size){
       xymodel.addcoldoublearray(0,&m_datatime.at(0)->at(0),size);
       for (int i=0; i< channellcolumns.count()-1;i++)
       {
          xymodel.addcoldoublearray(1,&m_datareal.at(i)->at(0),size);

       }

      }

    ui->tableView->setModel( &xymodel );
    //tablefit();
    ui->tableView->show();

}

long long TableCanvas::gettime(QStringList * channelvalues)

{
  time.setTimeSpec(Qt::UTC);
  if(gtimeformat=="UNIXTIME"){
   return (long long)(1000*channelvalues->at(colstime.at(0)).toLongLong());
  }else if(gtimeformat=="IGORTIME"){
      return (long long)(1000*(channelvalues->at(colstime.at(0)).toLongLong()-2082844800));
     }else if(gtimeformat=="UNIXTIMEMILI"){
      return (long long)(channelvalues->at(colstime.at(0)).toLongLong());
     }

  time.setTimeSpec(Qt::UTC);
  gtimestring.clear();
  for(int i=0;i<colstime.count();i++){
      if(colstime[i]> channelvalues->count()-1){
         return -1;
      }
   gtimestring.append(channelvalues->at(colstime[i]).simplified());
   if(i<colstime.count()-1)gtimestring.append(":");
}

  time = time.fromString(gtimestring, gtimeformat);
  time.setTimeSpec(Qt::UTC);
  if (time.date().year() < 2000 && time.date().year() >= 1900) {
      time = time.addYears(100);
  }
  return (long long)(time.toMSecsSinceEpoch());
}
void TableCanvas::LogMessage(QString mess)
{
  emit  logmessages(mess);
}
void TableCanvas::LogMessageR(QString mess)
{
  emit  logmessagesR(mess);
}

void TableCanvas::Surface_Plot_triggered()
{
   emit addtable(this,1);
}

bool TableCanvas::eventFilter( QObject * watched, QEvent * event )
{

    QWidget *canvas = qobject_cast<QWidget *>(watched);
    if ( canvas && event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent *mouseevent = (QMouseEvent *)event;
       if(mouseevent->button()==Qt::LeftButton){

       } if(mouseevent->button()==Qt::RightButton){
            if (!menutableview) {
                menutableview=new QMenu(this);
              menutableview->addAction(ui->actionSurface_Plot);
              menutableview->addAction(ui->actionMake_a_plot);
              menutableview->addAction(ui->actionChoose_columns_to_plot);
               menutableview->addAction(ui->actionSave_into_local_drive);
              menutableview->addAction(ui->actionColumn_Statistics);
              menutableview->addAction(ui->actionLocal_Polynomial_fit);
  //            menutableview->addAction(ui->actionLinked_with_time_changes);
  //            menutableview->addAction(ui->actionMake_a_Table);
  //            menutableview->addAction(ui->actionSavedata);
  //            menutableview->addAction(ui->actionSync_this_plot_with_the_period);
            }
              menutableview->exec(ui->tableView->mapToGlobal(mouseevent->pos()));
       }

    }else if ( canvas && event->type() == QEvent::MouseButtonRelease)
    {

    }

    return QObject::eventFilter(watched, event);
}

void TableCanvas::Make_a_plot_triggered()
{
    //choose columns to plot;
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedColumns();
    columnstoplot.clear();
    int ncolumnun=0;
    foreach (QModelIndex index, indexList) {
        if(xymodel.ctype.at(index.column()) ==1)ncolumnun++;
        columnstoplot << index.column();
    }

  if(!ncolumnun){
      QMessageBox::warning(this, tr("ERROR"),
                           tr("YOU NEED TO SELECT AT LEAST ONE COLUMN other than a timecolumn"));
      return;

  }
    emit addtable(this,2);
}

void TableCanvas::Choose_columns_to_plot_triggered()
{
    Plot2Table dialog;

    dialog.xymodel=&xymodel;
    dialog.filltablecolumns();
    int result;
    result = dialog.exec();
    if(result){
        columnstoplot.clear();
        for(int i=0; i<dialog.listofcolumns.count();i++)
        {
          columnstoplot << dialog.listofcolumns.at(i);
        }
        if(!columnstoplot.count()){
            QMessageBox::warning(this, tr("ERROR"),
                                 tr("YOU NEED TO SELECT AT LEAST ONE  COLUMN"));
            return;

        }
  emit addtable(this,2);
}
}

void TableCanvas::Column_Statistics_triggered()

{
    //choose columns to plot;
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedColumns();
    columnstoplot.clear();
    int ncolumnun=0;
    foreach (QModelIndex index, indexList) {
        if(xymodel.ctype.at(index.column()) ==1)ncolumnun++;
        columnstoplot << index.column();
    }
  if(!ncolumnun){
      QMessageBox::warning(this, tr("ERROR"),
                           tr("YOU NEED TO SELECT AT LEAST ONE COLUMN other than a timecolumn"));
      return;

  }
  emit addtable(this,3);
}

void TableCanvas::Local_Polynomial_fit_triggered()
{
    //choose columns to plot;
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedColumns();
    columnstoplot.clear();
    int ncolumnun=0;
    foreach (QModelIndex index, indexList) {
        if(xymodel.ctype.at(index.column()) ==1)ncolumnun++;
        columnstoplot << index.column();
    }
  if(!ncolumnun){
      QMessageBox::warning(this, tr("ERROR"),
                           tr("YOU NEED TO SELECT AT LEAST ONE COLUMN other than a timecolumn"));
      return;

  }
    emit addtable(this,4);
}

void TableCanvas::Save_into_database_triggered()
{
    dbapi table;
    table.connectionName=databaseconnection;
    connect (&table,&dbapi::logmessages,this,&TableCanvas::LogMessage);
    connect (&table,&dbapi::logmessagesR,this,&TableCanvas::LogMessageR);

    if(TableType.contains("FromLocalFileTobanana")){
        QString tablename;
        bool ok;
            tablename = QInputDialog::getText(this, tr("Name  of the table"),
                                                 tr("Name of the table?"), QLineEdit::Normal,
                                                 "newtable", &ok);
       if  (table.checkiftableixist(tablename)){
           QMessageBox::warning(this, tr("ERROR"),
                                tr("Tablename already exists. Try again with a different name"));
           return;

       }

       QStringList cnames, ctypes;
       QString lixo;
       cnames.clear(); ctypes.clear();
       cnames << "time"; ctypes <<"BIGINT";
       for(int i=1; i<xymodel.headernames.count(); i++)
       {
         lixo= xymodel.headernames.at(i) ;
         lixo.replace(".","_");
         lixo.prepend("d");
         cnames << lixo;
         ctypes << "FLOAT";
       }
       table.createtable_onekey(tablename,"time",cnames,ctypes);
       QString message;
       message="INSERT IGNORE INTO Tablesdescription (Tablenames,TableDis) VALUES ('";
       message.append(tablename);message.append("','");
       message.append("Table upload from aeroplot resulting from the local file: ");
       message.append(fileName);
       message.append( "')");
       table.doaquery(message);
       int nrows=0;

       for(int i=0;i<xymodel.rowCount(); i++){
           nrows++;
           if(nrows==1){
               message="INSERT IGNORE INTO ";
               message.append(tablename);
               message.append(" (time,");
               for (int kk = 1; kk < cnames.count(); kk++) {
                  message.append(cnames.at(kk));
                  if(kk<cnames.count()-1)message.append(",");
                  else message.append(") VALUES ");
               }

           }else message.append(",");
           message.append(" ('");
           message.append(QString::number((long long) xymodel.m_datatime.at(0)->at(i)));
           message.append("','");
           for (int kk = 0; kk < xymodel.columnCount()-1; kk++) {
              message.append(QString::number(xymodel.m_datareal.at(kk)->at(i)));
              if(kk<xymodel.columnCount()-2)message.append("','");
              else message.append("')");
           }
           if(nrows ==10){
               message.append(";");
               table.doaquery(message);
               nrows=0;
           }


    }
       if(nrows ){
           message.append(";");
           table.doaquery(message);
       }

        message="INSERT IGNORE INTO Surfaceplots (plotname,title,tname,ylegend,zlimits, zlegendtype,yaxistype,palette,diametersnames,diametersvalues) VALUES ('";
        message.append("Bananafor"+tablename);message.append("',");
        message.append("'");message.append("Default Banana plot from "+tablename);message.append("','");
        message.append(tablename); message.append("',");
        message.append("'");message.append("diameters");message.append("','");
        message.append("0;0','");
        message.append("rightcolorscale;linear");message.append("','");
        message.append("log");message.append("','");
        message.append("rainbow_cloud#300");message.append("','");
        QString Diametervalues;
        Diametervalues.clear();
        lixo.clear();
        for(int i=1; i<cnames.count(); i++){
          if(i>1) lixo.append(",");
          lixo.append(cnames.at(i));
          Diametervalues.append(xymodel.headernames.at(i));
          if(i<cnames.count()-1)Diametervalues.append(",");
        }
        message.append(lixo);message.append("',");
        message.append("'");message.append(Diametervalues);message.append("')\n");
        table.doaquery(message);

}
    }

void TableCanvas::readvalues()
{


    if(ui->Igortime->isChecked()) timeString="igor"; else if (ui->unixtime->isChecked())timeString="unixtime";
    else if (ui->TimeString->isChecked())timeString=ui->datef->text();


}

void TableCanvas::filltimestrings()
{
    ui->sometexttimeformats->addItem("yyyy/MM/dd hh:mm:ss.zzz", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyy/MM/dd hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyy-MM-dd hh:mm:ss.zzz", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyy-MM-dd hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yy/MM/dd hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yy-MM-dd hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("MM/dd/yyyy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("MM-dd-yyyy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("MM/dd/yy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("MM-dd-yy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("dd/MM/yyyy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("dd-MM-yyyy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("dd/MM/yy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("dd-MM-yy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyMMddhh", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyMMddhhmm", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyMMddhhmmss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyyMMddhh", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyyMMddhhmm", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyyMMddhhmmss", Qt::DisplayRole);

}

void TableCanvas::settimestrings(int index)
{
    ui->datef->setText(ui->sometexttimeformats->itemText(index));
}

void TableCanvas::Applydisplayformat_clicked()
{
    readvalues();
    xymodel.timestringformat=timeString;
    ui->tableView->setModel( &xymodel );
    //tablefit();
    ui->tableView->show();


}
