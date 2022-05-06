#include <QMessageBox>
#include <QDateTime>
#include <stdio.h>
#include <cmath>
#include <QFile>
#include "channelwindow.h"
#include "ui_channelwindow.h"
#include "QDebug"
#include "dbapi.h"
#include "buildsurfaceplot.h"
#include "buildhistogram.h"
#include "help.h"


channelWindow::channelWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::channelWindow)
{
    ui->setupUi(this);

    valuesmodel.clear();
    selectedvaluesmodel.clear();
    databasechanged=true;
    databaseindex=-1;
    sepf=";;";
    sepc=",,";
    atf="@@";

    connect(ui->timebutton, &QAbstractButton::pressed,
            this, &channelWindow::timepressed);

    connect(ui->Untick, &QAbstractButton::pressed, this, &channelWindow::clearselected);
    connect(ui->treeView, &QAbstractItemView::clicked,
            this, &channelWindow::updateselectednumber);
    connect(ui->treeView, &QAbstractItemView::doubleClicked,
            this, &channelWindow::selectall);
    connect(ui->all, &QAbstractButton::pressed,
            this, &channelWindow::fillselectedsinglevaluesbyclick);
    connect(ui->takeback, &QAbstractButton::pressed,
            this, &channelWindow::eraseallselectedlist);
    connect(ui->listView, &QAbstractItemView::doubleClicked,
            this, &channelWindow::erasefromlist);
    connect(ui->help, &QAbstractButton::pressed, this, &channelWindow::showhelp);

    connect(ui->cancel,&QAbstractButton::pressed,this, &QWidget::close);

    connect(ui->makeatyplot,&QAbstractButton::pressed,this, &channelWindow::makeatyplot);
    connect(ui->makeasurfaceplot,&QAbstractButton::pressed,this, &channelWindow::makeasurfaceplot);
    connect(ui->makeanhistogram,&QAbstractButton::pressed,this, &channelWindow::makeanhistogram);
    connect(ui->builtatable, &QAbstractButton::pressed,this, &channelWindow::builtatable);
    connect(ui->savetofile,&QAbstractButton::pressed,this, &channelWindow::savetofile);



}



channelWindow::~channelWindow()
{

    delete ui;


}

void channelWindow::makeatyplot(){
    send_DoIt(0);
}
void channelWindow::makeanhistogram()
{
 BuildHistogram dialog;

 int result;
 QList<QVariant> list;

 QStringList tablename,columnname,aliasnames;
 QList<QVariant> ctype;
 QString table;
 tablename.clear(); columnname.clear(); ctype.clear();

 for (int i = 0; i < selectedvaluesmodel.invisibleRootItem()->rowCount(); i++) {
      if(selectedvaluesmodel.invisibleRootItem()->child(i)->checkState()){
          list = selectedvaluesmodel.invisibleRootItem()->child(i)->data().toList();
          if( list[4]==QVariant::String) continue;
          tablename <<list[2].toString();
          columnname<< list[3].toString();
          ctype <<list[4];

      }
     }

if(!tablename.count()){
 QMessageBox msgBox;
 msgBox.setText("You need to select some channels to perform the plot");
 msgBox.exec();
 return;
}

  bool teste=false;
  table=tablename.at(0);
  for (int i=1;i< tablename.count(); i++)
  {
     if(tablename.at(i)!=table){teste=true; break;}
  }
  if(teste){
      QMessageBox msgBox;
      msgBox.setText("Unlike the time versus y values plots histograma plots  must be in the same table. Please narrow your sellected channels ");
      msgBox.exec();
      return;

  }

  dialog.setnofbin(columnname.count());
  result = dialog.exec();
   if(result){
       if(dialog.bins.count()){
           ctype.clear();
           columnname=dialog.bins;
           for(int i=0; i< dialog.bins.count(); i++)ctype <<"float";
       }
       tablename.clear();
       tablename << table;
       tablename << dialog.binlabels;
      emit DoIt(tablename,columnname,aliasnames, ctype, 5);
   }
}

void channelWindow::makeasurfaceplot(){

    buildsurfaceplot dialog;
    int result;
    QList<QVariant> list;

    QStringList tablename,columnname,aliasnames;
    QList<QVariant> ctype;
    QString table;
    tablename.clear(); columnname.clear(); ctype.clear();

    for (int i = 0; i < selectedvaluesmodel.invisibleRootItem()->rowCount(); i++) {
         if(selectedvaluesmodel.invisibleRootItem()->child(i)->checkState()){
             list = selectedvaluesmodel.invisibleRootItem()->child(i)->data().toList();
             if( list[4]==QVariant::String) continue;
             tablename <<list[2].toString();
             columnname<< list[3].toString();
             ctype <<list[4];

         }
        }
   if(!tablename.count()){
    QMessageBox msgBox;
    msgBox.setText("You need to select some channels to perform the plot");
    msgBox.exec();
    return;
   }
     bool teste=false;
     table=tablename.at(0);
     for (int i=1;i< tablename.count(); i++)
     {
        if(tablename.at(i)!=table){teste=true; break;}
     }
     if(teste){
         QMessageBox msgBox;
         msgBox.setText("Unlike the time versus y values plots surface plots (banana plots) must be in the same table. Please narrow your sellected channels ");
         msgBox.exec();
         return;

     }
     dialog.ndiameters=columnname.count();

     QString lixo;
     lixo=" Defining the diameters for ";
     lixo.append(QString::number(columnname.count()));
     lixo.append(" channels  from table ");
     lixo.append(table);
     lixo.append("\n Please fill the diameters values string.");
     dialog.writeinformation(lixo);
     result = dialog.exec();
      if(result){
          tablename.clear();
          tablename << table;
          tablename << dialog.channelstring;
          int type;
          if(dialog.yscale.contains("log")) type=-4; else type=4;
          if(dialog.itens.count()){
            columnname=dialog.itens.split(",");
          }

         emit DoIt(tablename,columnname,aliasnames, ctype, type);
      }
}

void channelWindow::builtatable(){
    send_DoIt(1);
}

void channelWindow::savetofile(){
 send_DoIt(3);
}


void channelWindow::fill()
{

   valuesmodel.clear();
   selectedvaluesmodel.clear();
   fillsinglevalues();
//sortmodel.setSortCaseSensitivity(Qt::CaseInsensitive);
//sortmodel.setSourceModel(&valuesmodel);
//ui->treeView->setModel(&sortmodel);
ui->treeView->setModel(&valuesmodel);
//ui->treeView->setSortingEnabled(true);



ui->treeView->show();
ui->listView->setModel(&selectedvaluesmodel);
ui->listView->show();

}
void  channelWindow::sortNonCaseSensitive(QStringList& sList)
{
    ///  Sorts the passed sList non-case-sensitively.
    ///  (Preserves the cases! Just doesn't use them
    ///  while sorting.)
    QMap<QString, QString> strMap;
    foreach(QString str, sList) {
        strMap.insert(str.toLower(), str);
    }
    sList = strMap.values();
}

void channelWindow::clearselected()
{

    for (int j = 0; j < valuesmodel.invisibleRootItem()->rowCount(); j++) {
        for (int i = 0; i < valuesmodel.invisibleRootItem()->child(j)->rowCount(); i++) {
            if (valuesmodel.invisibleRootItem()->child(j)->child(i)->checkState()) {
                valuesmodel.invisibleRootItem()->child(j)->child(i)->setCheckState(Qt::Unchecked);
            }
        }
    }


    ui->Untick->setText("Untick (0)");
}



void channelWindow::selectall(QModelIndex index)
{
    QList<QVariant> list;
     list = valuesmodel.itemFromIndex(index)->data().toList();
   QList<QVariant>  itemdata;
   dbapi table;
   connect(&table, &dbapi::logmessages, this,
          &channelWindow::logm);
   connect(&table, &dbapi::logmessagesR, this,
          &channelWindow::logmR);
   table.connectionName=databaseconnection;


    if (list.at(0).toInt() <= 0) {
         QStringList cnames,ctypes;
         cnames.clear();ctypes.clear();
         if(!valuesmodel.itemFromIndex(index)->rowCount()){
               QString instname,Fields_p,itemname;
             QStringList itens, properties,indv;
             if(list.at(0).toInt() == 0){
             table.getallcolumnsnameandtype(list.at(1).toString(), &cnames,&ctypes);
             instname=list.at(1).toString();instname=instname.replace("_data","");
             for(int j=0;j<cnames.count();j++){
                 itemname=cnames.at(j);
                 if(itemname.simplified()=="time") continue;
                 table.fechastringcell("Itensproprieties",  "NAMEID", instname, "description", &Fields_p );

                  itens=Fields_p.split(sepf);

                     indv.clear();
                     for(int i=0; i<itens.count();i++){
                         properties=itens.at(i).split(atf);
                         if(properties.count() <2){
                            continue;
                         }
                         if(!properties.at(1).compare(itemname)){
                            indv=properties.at(0).split(sepc);
                            break;
                         }
                     }
                     if(indv.count() ==4) {
                        if(indv.at(0).count()) itemname=indv.at(0);
                     }


                 QStandardItem *item = new QStandardItem(itemname);
                 item->setEditable(false);
                 itemdata.clear();
                 itemdata <<1<<itemname<<list.at(1).toString()<< cnames.at(j) << ctypes.at(j);
                 if(ctypes.at(j).contains("char") ||ctypes.at(j).contains("text") ) item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/document9.svg")));
                 else item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/education20.svg")));
                 item->setData(itemdata);
                 item->setCheckable(true);
                 item->setToolTip("a pensar");

                  valuesmodel.itemFromIndex(index)->appendRow(item);
             }

             }else if(list.at(0).toInt() == -1){
                 QList<std::vector<QString> * >  dipchannels;
                         QStringList dipcolumns;
                         dipcolumns.clear();
                         dipcolumns <<"channel" <<"channel_id" <<"channel_type" << "type_table";
                         for(int j=0; j<dipcolumns.count(); j++) dipchannels << new ( std::vector<QString>);
                         table.fechnotimecolumns_string_orderby(list.at(1).toString(),dipcolumns,"channel",&dipchannels);
                         Fields_p.clear();
                         table.fechastringcell("Itensproprieties",  "NAMEID", instname, "description", &Fields_p );
                         itens=Fields_p.split(sepf);
                         for(int j=0;j<(int )dipchannels.at(0)->size(); j++){
                             indv.clear();

                         itemname= dipchannels.at(0)->at(j);
                         itemdata.clear();
                         itemdata <<1<<itemname<<list.at(1).toString()<<dipchannels.at(0)->at(j)  << dipchannels.at(3)->at(j);

                         for(int k=0; k<itens.count();k++){
                             properties=itens.at(k).split(atf);
                             if(properties.count() <2){
                                continue;
                             }
                             if(!properties.at(1).compare(itemname)){
                                indv=properties.at(0).split(sepc);
                                break;
                             }
                         }
                         if(indv.count() ==4) {
                            if(indv.at(0).count()) itemname=indv.at(0);
                         }


                 instname=list.at(1).toString();instname=instname.replace("_channels","");


                     QStandardItem *item = new QStandardItem(itemname);
                     item->setEditable(false);
                     if(dipchannels.at(3)->at(j).contains("STRING")) item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/document9.svg")));
                     else item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/education20.svg")));
                     item->setData(itemdata);
                     item->setCheckable(true);
                     item->setToolTip("a pensar");

                      valuesmodel.itemFromIndex(index)->appendRow(item);
                 }
          }

        }else{
        //aqui é que vou selectionar tudo

        for (int i = 0; i < valuesmodel.itemFromIndex(index)->rowCount(); i++) {
            list = valuesmodel.itemFromIndex(index)->child(i)->data().toList();
             {
               valuesmodel.itemFromIndex(index)->child(i)->setCheckState(Qt::Checked);
            }
        }

        updateselectednumber();
           }
    }else{
        fillselectedsinglevalues(index);
    }
}
void channelWindow::erasefromlist(QModelIndex index)
{
     selectedvaluesmodel.removeRow(index.row());
     ui->listView->setModel(&selectedvaluesmodel);
}
void channelWindow::eraseallselectedlist()
{
     selectedvaluesmodel.clear();
     ui->listView->setModel(&selectedvaluesmodel);
}

void channelWindow::updateselectednumber()
{
    QString texto;
    int     n;

    n     = 0;
    texto = "Untick (";
    for (int j = 0; j < valuesmodel.invisibleRootItem()->rowCount(); j++) {
        for (int i = 0; i < valuesmodel.invisibleRootItem()->child(j)->rowCount(); i++) {
            if (valuesmodel.invisibleRootItem()->child(j)->child(i)->checkState()) {
                n++;
            }
        }
    }
    texto.append(QString::number(n));
    texto.append(")");
    ui->Untick->setText(texto);
}
void channelWindow::startselectedsinglevalues()
{
  //  QStandardItem *parentItem = selectedvaluesmodel.invisibleRootItem();
    QStringList crootData;
    crootData.clear();
    crootData << "selected value Channels";
    selectedvaluesmodel.setHorizontalHeaderLabels(crootData);
    ui->listView->setModel(&selectedvaluesmodel);

}

void channelWindow::getdatabaseinfo( QList<QVariant> dinfo, int index) {

       if( databaseindex==index) databasechanged=false; else databasechanged=true;
        Itimes.setTimeSpec(Qt::UTC);
        Ltimes.setTimeSpec(Qt::UTC);

        Itimes=Itimes.fromSecsSinceEpoch(dinfo.at(2).toLongLong());
        Ltimes=Ltimes.fromSecsSinceEpoch(dinfo.at(3).toLongLong());
        Itimes.setTimeSpec(Qt::UTC);
        Ltimes.setTimeSpec(Qt::UTC);

        databaseconnection=dinfo.at(0).toString();
        databasename=dinfo.at(1).toString();
        databaseindex=index;
        QString Timelabel;
        Timelabel=Itimes.toString("yyyy/MM/dd hh:mm:ss  -----> ");
        Timelabel.append(Ltimes.toString("yyyy/MM/dd hh:mm:ss"));
        ui->timelabel->setText(Timelabel);
        ui->databasename->setText(databasename);

       if(databasechanged) fill();

 }
void channelWindow::fillselectedsinglevalues(QModelIndex index)
{
    QStandardItem *item = new QStandardItem();
    item=valuesmodel.itemFromIndex(index)->clone();
    item->setCheckState(Qt::Checked);
    item->setDragEnabled(true);
    item->setDropEnabled(true);

      selectedvaluesmodel.invisibleRootItem()->appendRow(item);
      ui->listView->setModel(&selectedvaluesmodel);

}

void channelWindow::fillselectedsinglevaluesbyclick()
{

    for (int j = 0; j < valuesmodel.invisibleRootItem()->rowCount(); j++) {
        for (int i = 0; i <valuesmodel.invisibleRootItem()->child(j)->rowCount(); i++) {
            if (valuesmodel.invisibleRootItem()->child(j)->child(i)->checkState()) {
                QStandardItem *item = new QStandardItem();
                 item= valuesmodel.invisibleRootItem()->child(j)->child(i)->clone();
 //                item->setCheckState(Qt::Checked); //porque já est+a checked
                  item->setDragEnabled(true);
                  item->setDropEnabled(true);
                 selectedvaluesmodel.invisibleRootItem()->appendRow(item);

            }
        }
    }
       ui->listView->setModel(&selectedvaluesmodel);

}
void  channelWindow::timepressed(){
  emit CallTime();
}

void  channelWindow::send_DoIt(int type)
{


        QStringList tablename,columnname,namealias;
        QList<QVariant> ctype;
        tablename.clear(); columnname.clear(); ctype.clear();
 //       if(ui->makeaplot->isChecked()) type=0; else if(ui->builtatable->isChecked()) type=1;
 //       else if(ui->savetofile->isChecked()) type=2;
        QList<QVariant> list;

        for (int i = 0; i < selectedvaluesmodel.invisibleRootItem()->rowCount(); i++) {
             if(selectedvaluesmodel.invisibleRootItem()->child(i)->checkState()){
                 list = selectedvaluesmodel.invisibleRootItem()->child(i)->data().toList();
                 if(!type && list.at(4).toString().contains("char",Qt::CaseInsensitive)) continue;
                 tablename <<list[2].toString();
                 columnname<< list[3].toString();
                 namealias<< list[1].toString();
                 ctype <<list[4];

             }
            }

        emit DoIt(tablename,columnname,namealias, ctype, type);


}


void channelWindow::fillsinglevalues()
{


     QStandardItem *cparentItem = valuesmodel.invisibleRootItem();
     QStringList            hosts;

    QList<QVariant> itemdata;
    QStringList crootData;

    crootData.clear();
    crootData << "channels";
    hosts.clear();
    valuesmodel.setHorizontalHeaderLabels(crootData);
    //

    QSqlDatabase db = QSqlDatabase::database(databaseconnection);
    hosts=db.tables(QSql::AllTables);

    QBrush green;
    green.setColor(Qt::darkGreen);
    dbapi table;
    connect(&table, &dbapi::logmessages, this,
           &channelWindow::logm);
    connect(&table, &dbapi::logmessagesR, this,
           &channelWindow::logmR);
    table.connectionName=databaseconnection;
    QString instname;

    // special case for runtable
    QStandardItem *pitem = new QStandardItem("RunTable");
    itemdata.clear();
    itemdata<< 0 ;
     pitem->setData(itemdata);
     pitem->setEditable(false);
     QStandardItem *item = new QStandardItem("start of Run");
     item->setEditable(false);
     itemdata.clear();
     itemdata <<1<< "StartTime"<<"Runtable"<< "StartTime" << "numeric";
     item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/education20.svg")));
     item->setData(itemdata);
     item->setCheckable(true);
     pitem->appendRow(item);

     QStandardItem *item1 = new QStandardItem("end of Run");
     item1->setEditable(false);
     itemdata.clear();
     itemdata <<1<< "EndTime"<<"Runtable"<< "EndTime" << "numeric";
     item1->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/education20.svg")));
     item1->setData(itemdata);
     item1->setCheckable(true);
     pitem->appendRow(item1);

     QStandardItem *item2 = new QStandardItem("Type");
     item2->setEditable(false);
     itemdata.clear();
     itemdata <<1<< "Type"<<"Runtable"<< "Type" << "char";
     item2->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/document9.svg")));
     item2->setData(itemdata);
     item2->setCheckable(true);
     pitem->appendRow(item2);

     QStandardItem *item3 = new QStandardItem("Description");
     item3->setEditable(false);
     itemdata.clear();
     itemdata <<1<< "Description"<<"Runtable"<< "Description" << "char";
     item3->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/document9.svg")));
     item3->setData(itemdata);
     item3->setCheckable(true);
     pitem->appendRow(item3);

     QStandardItem *item4 = new QStandardItem("comments");
     item4->setEditable(false);
     itemdata.clear();
     itemdata <<1<< "comments"<<"Runtable"<< "comments" << "char";
     item4->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/document9.svg")));
     item4->setData(itemdata);
     item4->setCheckable(true);
     pitem->appendRow(item4);

     cparentItem->appendRow(pitem);

for (int i = 0; i < hosts.size(); i++) {

        if(!hosts.at(i).contains("_data") && !hosts.at(i).contains("_channels"))continue;
        instname=hosts.at(i);instname=instname.replace("_data","");instname=instname.replace("_channels","");
        QStandardItem *pitem = new QStandardItem(instname);
        itemdata.clear();
        if(hosts[i].contains("_data") ) {itemdata <<0 << hosts.at(i);}else {itemdata <<-1 << hosts.at(i);}
        pitem->setData(itemdata);
        pitem->setEditable(false);
        QString Comments_p;
        table.fechastringcell("TablesforDSV",  "NAMEID", instname, "NAME", &Comments_p );
        pitem->setToolTip(Comments_p);
        cparentItem->appendRow(pitem);


}
for (int i = 0; i < hosts.size(); i++) {

        if(!hosts.at(i).contains("_pdata"))continue;
        instname=hosts.at(i);instname=instname.replace("_pdata","");
        QStandardItem *pitem = new QStandardItem(instname);
        pitem->setForeground(green);
        itemdata.clear();
        itemdata <<0 << hosts.at(i);
        pitem->setData(itemdata);
        pitem->setEditable(false);
        QString Comments_p;
        table.fechastringcell("TableforDSV",  "NAMEID", instname, "NAME", &Comments_p );
        pitem->setToolTip(Comments_p);
        cparentItem->appendRow(pitem);


}

}



void channelWindow::showhelp(){
    QString data;
    QString fileName(":/help/helps/chwindows.html");

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
     data="No Help file found";
    }
    else
    {
        data = file.readAll();
    }

    file.close();


     Help dialog;
     dialog.fill(data);
     dialog.exec();



}
void channelWindow::displaydisablethings()
{
 /*   ui->savetofile->setHidden(true);
    ui->builtatable->setHidden(true);
    ui->makeaplot->setEnabled(false);
    ui->timebutton->setHidden(true);
    ui->timelabel->setHidden(true);
  */
}
void channelWindow::logm(QString mess){
    emit logmessages(mess);
}
void channelWindow::logmR(QString mess){
    emit logmessagesR(mess);
}
