#include "derivedplot.h"
#include "ui_derivedplot.h"
#include "dbapi.h"

DerivedPlot::DerivedPlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DerivedPlot)
{
    ui->setupUi(this);
    sc="|>";
    sv=":=";
    ss="|;";
    sepf=";;";
    sepc=",,";
    atf="@@";

    connect(ui->treeViewItem, &QAbstractItemView::doubleClicked,
                    this, &DerivedPlot::select);
    connect(ui->treeView_plot, &QAbstractItemView::doubleClicked,
            this, &DerivedPlot::erasefromlist);
    connect(ui->plot,&QAbstractButton::pressed,this,&DerivedPlot::dplot);
    connect(ui->additem,&QAbstractButton::pressed,this,&DerivedPlot::additem);

}

DerivedPlot::~DerivedPlot()
{
    delete ui;
}
void DerivedPlot::additem()
{
    QStringList            hosts;
    QList<QVariant> itemdata;
    QStringList crootData;
    QString instname;

    QSqlDatabase db = QSqlDatabase::database(databaseconnection);
    hosts=db.tables(QSql::AllTables);
    QStandardItem *pitem = new QStandardItem("RunTable");
    itemdata.clear();
    itemdata<< -3 ;
     pitem->setData(itemdata);
     pitem->setEditable(false);
     QStandardItem *item = new QStandardItem("start of Run");
     item->setEditable(false);
     itemdata.clear();
     itemdata <<-2<< "StartTime"<<"Runtable";
     item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/education20.svg")));
     item->setData(itemdata);
 //     item->setCheckable(true);
     pitem->appendRow(item);

     QStandardItem *item1 = new QStandardItem("end of Run");
     item1->setEditable(false);
     itemdata.clear();
     itemdata <<-2<< "EndTime"<<"Runtable";
     item1->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/education20.svg")));
     item1->setData(itemdata);
    // item1->setCheckable(true);
     pitem->appendRow(item1);
     itemmodel.invisibleRootItem()->appendRow(pitem);
     QBrush green;
     green.setColor(Qt::darkGreen);

     dbapi table;
     for (int i = 0; i < hosts.size(); i++) {

             if(!hosts.at(i).contains("_data") && !hosts.at(i).contains("_channels"))continue;
             instname=hosts.at(i);instname=instname.replace("_data","");instname=instname.replace("_channels","");
             QStandardItem *pitem = new QStandardItem(instname);
             itemdata.clear();
             if(hosts[i].contains("_data") ) {itemdata <<-3 << hosts.at(i);}else {itemdata <<-4 << hosts.at(i);}
             pitem->setData(itemdata);
             pitem->setEditable(false);
             QString Comments_p;
             table.fechastringcell("TablesforDSV",  "NAMEID", instname, "NAME", &Comments_p );
             pitem->setToolTip(Comments_p);
             itemmodel.invisibleRootItem()->appendRow(pitem);

     }
     for (int i = 0; i < hosts.size(); i++) {

             if(!hosts.at(i).contains("_pdata"))continue;
             instname=hosts.at(i);instname=instname.replace("_pdata","");
             QStandardItem *pitem = new QStandardItem(instname);
             pitem->setForeground(green);
             itemdata.clear();
             itemdata <<-3 << hosts.at(i);
             pitem->setData(itemdata);
             pitem->setEditable(false);
             QString Comments_p;
             table.fechastringcell("TableforDSV",  "NAMEID", instname, "NAME", &Comments_p );
             pitem->setToolTip(Comments_p);
             itemmodel.invisibleRootItem()->appendRow(pitem);


     }

   ui->additem->setEnabled(false);
}
void DerivedPlot::fillcurves()
{
    itemmodel.clear();


    QStandardItem *parentItem = itemmodel.invisibleRootItem();
    QList<QVariant> grafid;
    itemplot   = new QStandardItem("plot curves");
     // parentItem->setColumnCount(3); o 1 que aparecia era a coluna
    QStringList rootData;
    QString     ttip;
    long long   iparent;
    iparent = -1;
    rootData << "curves available to plot";
    itemmodel.setHorizontalHeaderLabels(rootData);

    grafid.clear();
    grafid <<  iparent;
    itemplot->setData(grafid);
    ttip.clear();
    ttip.append("curves in the original plot");
    itemplot->setToolTip(ttip);
    parentItem->appendRow(itemplot);





    QList<QVariant> itemdata;
    QStringList curves,cvalues;
    curves=plot.at(3).split(sc);
    QString tname,cname;
    for(int i=0; i<curves.count(); i++){
       cvalues=curves.at(i).split(ss);
       for(int j=0; j< cvalues.count(); j++){
           if (cvalues.at(j).contains("tname:=")) {tname=cvalues.at(j); tname=tname.remove("tname:=");}
           if (cvalues.at(j).contains("cname:=")) {cname=cvalues.at(j); cname=cname.remove("cname:=");}

       }
       itemdata.clear();
       grafid.clear();
       QString lixo;
       lixo=cname;
       lixo.append("@");
       lixo.append(tname);
       QStandardItem *item = new QStandardItem(lixo);
       grafid << i <<lixo;
       item->setData(grafid);
       item->setEditable(false);
       itemplot->appendRow(item);
 }

    ui->treeViewItem->setModel(&itemmodel);
    ui->treeViewItem->show();

    itemmodeltoplot.clear();
    rootData.clear();
    rootData << "curves to plot";
    itemmodeltoplot.setHorizontalHeaderLabels(rootData);


}
void DerivedPlot::select(QModelIndex( index))
{
    QList<QVariant> list;
     list = itemmodel.itemFromIndex(index)->data().toList();
    if(list.at(0).toInt() == -1) return;
    QList<QVariant>  itemdata;
    dbapi table;
    table.connectionName=databaseconnection;

    if(list.at(0).toInt() == -3 || list.at(0).toInt() == -4){
        QStringList cnames,ctypes;
        cnames.clear();ctypes.clear();
        if(!itemmodel.itemFromIndex(index)->rowCount()){
              QString instname,Fields_p,itemname;
            QStringList itens, properties,indv;
            if(list.at(0).toInt() == -3){
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

                if(ctypes.at(j).contains("char") ||ctypes.at(j).contains("text") ) continue;
                QStandardItem *item = new QStandardItem(itemname);
                item->setEditable(false);
                itemdata.clear();
                itemdata <<-2<<list.at(1).toString()<< cnames.at(j) ;
                item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/education20.svg")));
                item->setData(itemdata);
                 item->setToolTip("a pensar");

                 itemmodel.itemFromIndex(index)->appendRow(item);
            }

            }else if(list.at(0).toInt() == -4){
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
                        itemdata <<-2<<list.at(1).toString()<<dipchannels.at(0)->at(j) ;

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

                    if(dipchannels.at(3)->at(j).contains("STRING")) continue;
                    QStandardItem *item = new QStandardItem(itemname);
                    item->setEditable(false);
                    item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/education20.svg")));
                    item->setData(itemdata);
               //      item->setCheckable(true);
                    item->setToolTip("a pensar");

                     itemmodel.itemFromIndex(index)->appendRow(item);
                }
         }

       }
         ui->treeViewItem->setModel(&itemmodel);
        ui->treeViewItem->show();

    }else{
    if(list.at(0).toInt() == -2){
        QString name;
        name=list.at(2).toString();
        name.append("@");
        name.append(list.at(1).toString());
        QStandardItem *item = new QStandardItem(name);
        item->setEditable(false);
        itemdata.clear();
        itemdata <<-1<<list.at(2).toString()<<list.at(1).toString();
        item->setDragEnabled(true);
        item->setDropEnabled(true);
        item->setData(itemdata);
        itemmodeltoplot.invisibleRootItem()->appendRow(item);

    }else {
    QStandardItem *item = new QStandardItem(list.at(1).toString());
    item->setEditable(false);
    itemdata.clear();
    itemdata <<list.at(0).toInt();
    item->setDragEnabled(true);
    item->setDropEnabled(true);
    item->setData(itemdata);
    itemmodeltoplot.invisibleRootItem()->appendRow(item);
    }
    }
    ui->treeView_plot->setModel(&itemmodeltoplot);

    ui->treeView_plot->show();



}
void DerivedPlot::erasefromlist(QModelIndex index){
    itemmodeltoplot.removeRow(index.row());
    ui->treeView_plot->setModel(&itemmodeltoplot);
    ui->treeView_plot->show();
}
void DerivedPlot::dplot(){
    QList<QVariant> list;
    QList<QVariant> dlist;
    dlist.clear();
    dlist <<1;
    dlist<<plot.at(0);
    QStringList curves,scolumns,stables,ct;
    QString curvesp,columns,tables,coltables;
    ct=plot.at(1).split(";");
    scolumns=ct.at(0).split(",");
    stables=ct.at(1).split(",");
    curvesp.clear(),columns.clear(),tables.clear(),coltables.clear();
    curves=plot.at(3).split(sc);
    for (int i = 0; i < itemmodeltoplot.invisibleRootItem()->rowCount(); i++) {

             list = itemmodeltoplot.invisibleRootItem()->child(i)->data().toList();            
             if(curvesp.count()) curvesp.append(sc);
             if(columns.count()) columns.append(",");
             if(tables.count()) tables.append(",");
             if(list.at(0).toInt() >=0){
           curvesp.append(curves.at(list.at(0).toInt()));
           columns.append(scolumns.at(list.at(0).toInt()));
           tables.append(stables.at(list.at(0).toInt()));
             }else{
                 curvesp.append("yaxisLabel:="+list.at(1).toString()+ss);
                 curvesp.append("tname:="+list.at(2).toString()+ss);
                 curvesp.append("cname:="+list.at(1).toString());
                 columns.append(list.at(1).toString());
                 tables.append(list.at(2).toString());
             }
        }
    coltables=columns;
    coltables.append(";");
    coltables.append(tables);
    dlist <<coltables;
    dlist <<plot.at(2);
    dlist << curvesp;

    emit plots(dlist,plot.at(0));

}
