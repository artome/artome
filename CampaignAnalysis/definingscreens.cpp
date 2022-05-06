#include "definingscreens.h"
#include "ui_definingscreens.h"
#include "dbapi.h"
#include <QMessageBox>
DefiningScreens::DefiningScreens(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DefiningScreens)
{
    ui->setupUi(this);
    connect(ui->treeView, &QAbstractItemView::doubleClicked,
            this, &DefiningScreens::selectall);
    connect(ui->put, &QAbstractButton::pressed,
            this, &DefiningScreens::fillselectedsinglevaluesbyclick);
    connect(ui->takeback, &QAbstractButton::pressed,
            this, &DefiningScreens::eraseallselectedlist);
    connect(ui->listView, &QAbstractItemView::doubleClicked,
            this, &DefiningScreens::erasefromlist);
    connect(ui->help, &QAbstractButton::pressed, this, &DefiningScreens::showhelp);

    connect(ui->cancel,&QAbstractButton::pressed,this, &QWidget::close);
    connect(ui->acceptbutton,&QAbstractButton::pressed,this, &DefiningScreens::storescreens);
    connect(ui->Plotall,&QAbstractButton::pressed,this, &DefiningScreens::sendplots);
    connect(ui->report,&QAbstractButton::pressed,this, &DefiningScreens::writereport);



}

DefiningScreens::~DefiningScreens()
{
    delete ui;
}
void DefiningScreens::fill()
{

   valuesmodel.clear();
   selectedvaluesmodel.clear();
   fillsinglevalues();

ui->treeView->setModel(&valuesmodel);
ui->treeView->show();
ui->listView->setModel(&selectedvaluesmodel);
ui->listView->show();


}
void DefiningScreens::sendplots()
{
     QList<QVariant> list;
    for (int i = 0; i < selectedvaluesmodel.invisibleRootItem()->rowCount(); i++) {
         if(selectedvaluesmodel.invisibleRootItem()->child(i)->checkState()){
             list = selectedvaluesmodel.invisibleRootItem()->child(i)->data().toList();
             emit  dplot(list);
         }
        }

}
void DefiningScreens::writereport()
{

    QList<QVariant> list,clist;
    QList <QList<QVariant> > slist;
    slist.clear();

    for (int i = 0; i < selectedvaluesmodel.invisibleRootItem()->rowCount(); i++) {
         if(selectedvaluesmodel.invisibleRootItem()->child(i)->checkState()){
             list = selectedvaluesmodel.invisibleRootItem()->child(i)->data().toList();
             clist.clear();
             clist<< list.at(0) << list.at(1) << list.at(2);
             slist<<clist;
         }
        }
       emit doreport( slist);
}

void DefiningScreens::storescreens()
{
    QList<QVariant> list;

    QString screenname,columnsandtables,proprieties,surface,curves,histos ;
    screenname=ui->screenname->text().simplified();
    if(!screenname.count()){
        QMessageBox::information(this, tr("Empty Screen name"),
                                 tr("The screen name cannot be empty. "));
        return;
    }
    QString message;
    message="INSERT  INTO Screens (screenname,plottype,plotorder,columnsandtables,proprieties,surface,curves,histos) VALUES ";
    message.append("(?,?,?,?,?,?,?,?);");
    QStringList binds;
    dbapi newtable;
    newtable.connectionName=databaseconnection;
    connect (&newtable,&dbapi::logmessages,this,&DefiningScreens::LogM);
    connect (&newtable,&dbapi::logmessagesR,this,&DefiningScreens::LogMR);
    newtable.Delrowsmachingacolumnvalue("Screens","screenname",screenname.simplified());
    QList <double> xvmin,xvmax,yvmin,yvmax;
    QStringList limitsx,plimites,limitsy;
    double min,max;
    bool start;
    double xl,yl;
    limitsx=ui->xvlimits->text().split(";");
    limitsy=ui->yvlimits->text().split(";");
    if(limitsx.count()!=limitsy.count()){
        QMessageBox::information(this, tr("The nunber of your y limites are not the same as the x  "),
                                 tr("you must set by order the limits of every plot in page "));
        return;
    }


    for(int i=0; i<limitsx.count();i++){
        plimites=limitsx.at(i).split(",");
        xl=0.8/((double)plimites.count());
        min=0.1; start=false;
          for(int j=0; j<plimites.count(); j++){
            if(!plimites.at(j).toInt() && !start){
                min=min+xl;
            }else {start=true;}
        }
        max=0.9; start=false;
         for(int j=plimites.count()-1; j>-1; j--){
            if(!plimites.at(j).toInt() && !start){
                max=max-xl;
            }else {start=true;}
        }
        xvmin<<min; xvmax<<max;
    }
    limitsy=ui->yvlimits->text().split(";");
    for(int i=0; i<limitsy.count();i++){
        plimites=limitsy.at(i).split(",");
        yl=0.8/((double)plimites.count());
        min=0.1; start=false;
        for(int j=0; j<plimites.count(); j++){
            if(!plimites.at(j).toInt() && !start){
                min=min+yl;
            }else {start=true;}
        }
        max=0.9; start=false;
            for(int j=plimites.count()-1; j>-1; j--){
            if(!plimites.at(j).toInt() && !start){
                max=max-yl;
            }else {start=true;}
        }
        yvmin<<min; yvmax<<max;
    }




    int order=-1;
    for (int i = 0; i < selectedvaluesmodel.invisibleRootItem()->rowCount(); i++) {
         if(selectedvaluesmodel.invisibleRootItem()->child(i)->checkState()){
             list = selectedvaluesmodel.invisibleRootItem()->child(i)->data().toList();
             surface="not relevant";curves="not relevant";histos="not relevant";
             binds.clear();
             order++;
             columnsandtables=list.at(2).toString();
             if(order <yvmin.count()){
                 proprieties=resetviewports(list.at(3).toString(),xvmin.at(order),xvmax.at(order),yvmin.at(order),yvmax.at(order));
             }else proprieties=list.at(3).toString();

             if(list.at(0).toInt()==1){
                curves =list.at(4).toString();
             }else if(list.at(0).toInt()==2){
                 surface =list.at(4).toString();
             }else if(list.at(0).toInt()==3){
                 histos =list.at(4).toString();
             }
             binds<<screenname<<QString::number(list.at(0).toInt()-1)<<QString::number(order)<<columnsandtables<<proprieties<<surface<<curves<<histos;
             newtable.preparebind(message,&binds);
         }
        }


/*    QString lixo;
    QString message;
    message="INSERT  INTO typlots (plotname,columnsandtables,proprieties,curves) VALUES ";
    message.append("(?,?,?,?) ON DUPLICATE KEY UPDATE columnsandtables=VALUES(columnsandtables), proprieties=VALUES(proprieties), curves=VALUES(curves);");
    QStringList binds;
    binds.clear();
    createstringstostorety(0);
    binds <<plotname <<closanstables<<plotp <<curvesp;
    table.preparebind(message,&binds);
*/
/*    query="INSERT IGNORE INTO Screens (screenname,title,format,plots) VALUES ('";
    query.append(ui->screenname->text());query.append("',");
    query.append("'");query.append(ui->screentitle->text());query.append("','");
    query.append(QString::number(ui->nl->value())+"x"+QString::number(ui->nc->value())); query.append("',");
    lixo=plotsname.at(0);lixo.append("@");lixo.append(tablename.at(0));
    for (int i=1; i<tablename.count();i++)
    {
       lixo.append(";");
       lixo.append(plotsname.at(i));lixo.append("@");lixo.append(tablename.at(i));
    }

    query.append("'");query.append(lixo);
    query.append("')\n");
    newtable.doaquery(query);
*/
 //   this->accept();

}

QString  DefiningScreens::resetviewports(QString p,double xi,double xl,double yi,double yl){
    QStringList fields, values;
    QString ss,sv;
    sv=":=";
    ss="|;";
    QString out;
    out.clear();
    fields=p.split(ss);
    bool x1,x2,y1,y2;
    x1=false;x2=false;y1=false;y2=false;
    bool start=false;
    for(int i=0; i< fields.count();i++){
        values=fields.at(i).split(sv);
        if(start) out.append(ss);
        if (values.at(0)=="xvmin"){
            out.append("xvmin"+sv+QString::number(xi));
            x1=true;
        }else if (values.at(0)=="xvmax"){
                out.append("xvmax"+sv+QString::number(xl));
                x2=true;
        }else if (values.at(0)=="yvmin"){
                out.append("yvmin"+sv+QString::number(yi));
                y1=true;
        }else if (values.at(0)=="yvmax"){
                    out.append("yvmax"+sv+QString::number(yl));
                    y2=true;
        } else {

            out.append(fields.at(i));
        }

        start=true;
    }
    if(!x1)
    out.prepend("xvmin"+sv+QString::number(xi)+ss);
    if(!x2)
    out.prepend("xvmax"+sv+QString::number(xl)+ss);
    if(!y1)
        out.prepend("yvmin"+sv+QString::number(yi)+ss);
    if(!y2)
        out.prepend("yvmax"+sv+QString::number(yl)+ss);

    return out;

}
void DefiningScreens::eraseallselectedlist()
{
     selectedvaluesmodel.clear();
     ui->listView->setModel(&selectedvaluesmodel);
}
void DefiningScreens::selectall(QModelIndex index)
{
    QList<QVariant> list;
    list = valuesmodel.itemFromIndex(index)->data().toList();
//    Message("cplot"+QString::number(list.size()));

    if (list[0].toInt() == -1) {
        //aqui é que vou selectionar tudo

        for (int i = 0; i < valuesmodel.itemFromIndex(index)->rowCount(); i++) {
            list = valuesmodel.itemFromIndex(index)->child(i)->data().toList();
            {
               valuesmodel.itemFromIndex(index)->child(i)->setCheckState(Qt::Checked);
            }
        }

    }else{
        fillselectedsinglevalues(index);
    }
}
void DefiningScreens::erasefromlist(QModelIndex index)
{
     selectedvaluesmodel.removeRow(index.row());
     ui->listView->setModel(&selectedvaluesmodel);
}
void DefiningScreens::startselectedsinglevalues()
{
  //  QStandardItem *parentItem = selectedvaluesmodel.invisibleRootItem();
    QStringList crootData;
    crootData.clear();
    crootData << "selected value Channels";
    selectedvaluesmodel.setHorizontalHeaderLabels(crootData);
    ui->listView->setModel(&selectedvaluesmodel);

}
void DefiningScreens::fillselectedsinglevalues(QModelIndex index)
{
    QStandardItem *item = new QStandardItem();
    item=valuesmodel.itemFromIndex(index)->clone();
    item->setCheckState(Qt::Checked);
    item->setDragEnabled(true);
    item->setDropEnabled(true);

      selectedvaluesmodel.invisibleRootItem()->appendRow(item);
      ui->listView->setModel(&selectedvaluesmodel);

}

void DefiningScreens::fillselectedsinglevaluesbyclick()
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
void DefiningScreens::fillsinglevalues()
{
    valuesmodel.clear();
    QStandardItem *parentItem = valuesmodel.invisibleRootItem();
    QList<QVariant> grafid;
    QStandardItem   *Graphitem   = new QStandardItem("timevalues-plots");
    QStandardItem   *Bananaitem  = new QStandardItem("Surface-plots");
    QStandardItem   *Histoitem   = new QStandardItem("Histo-plots");
  //  QStandardItem   *Scatteritem = new QStandardItem("Scatter-plots");

    // parentItem->setColumnCount(3); o 1 que aparecia era a coluna
    QStringList rootData;
    QString     ttip;
    long long   iparent;
    iparent = -1;
    rootData << "Plots";
    valuesmodel.setHorizontalHeaderLabels(rootData);

    grafid.clear();
    grafid <<  iparent;
    Graphitem->setData(grafid);
    ttip.clear();
    ttip.append("Plots stored in database. Multiaxis or single axis");
    Graphitem->setToolTip(ttip);
    parentItem->appendRow(Graphitem);

    grafid.clear();
    grafid  << iparent;
    Bananaitem->setData(grafid);
    ttip.clear();
    ttip.append("Surface  Plots (as bananas) stored in database");
    Bananaitem->setToolTip(ttip);
    parentItem->appendRow(Bananaitem);

    grafid.clear();
    grafid << iparent;
    Histoitem->setData(grafid);
    ttip.clear();
    ttip.append("Histogram plots stored in database");
    Histoitem->setToolTip(ttip);
    parentItem->appendRow(Histoitem);

    dbapi table;
    table.connectionName=databaseconnection;
    connect (&table,&dbapi::logmessages,this,&DefiningScreens::LogM);
    connect (&table,&dbapi::logmessagesR,this,&DefiningScreens::LogMR);
    QStringList cnames;


          QList<std::vector<QString> * >  bananaplots;

            cnames.clear();
            cnames <<"plotname"  <<"columnsandtables"<<"proprieties" <<"surface";
            for(int i=0; i<cnames.count(); i++) bananaplots << new ( std::vector<QString>);
            table.fechnotimecolumns_string("Surfaceplots",cnames,&bananaplots);

            for(int i=0;i<(int )bananaplots.at(0)->size(); i++){
            QStandardItem *item = new QStandardItem(bananaplots.at(0)->at(i));
            grafid.clear();
            item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/banana4.svg")));
            grafid <<2 <<bananaplots.at(0)->at(i)<<bananaplots.at(1)->at(i)<<bananaplots.at(2)->at(i)<<bananaplots.at(3)->at(i) ;

            item->setCheckable(true);
            item->setData(grafid);
            item->setEditable(false);
            Bananaitem->appendRow(item);

            grafid.clear();
      }
      for(int i=0; i<cnames.count(); i++) delete bananaplots[i];
           QList<std::vector<QString> * >  typlots;


            cnames.clear();
            cnames <<"plotname"<<"columnsandtables"<<"proprieties" <<"curves" ;
            for(int i=0; i<cnames.count(); i++) typlots << new ( std::vector<QString>);
            table.fechnotimecolumns_string("typlots",cnames,&typlots);

            for(int i=0;i<(int )typlots.at(0)->size(); i++){
            QStandardItem *item = new QStandardItem(typlots.at(0)->at(i));
            grafid.clear();
             item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/educational15.svg")));
            grafid <<1 <<typlots.at(0)->at(i)  <<typlots.at(1)->at(i) <<typlots.at(2)->at(i) <<typlots.at(3)->at(i);
            item->setData(grafid);
            item->setCheckable(true);
            item->setEditable(false);

            Graphitem->appendRow(item);

            grafid.clear();
      }
      for(int i=0; i<cnames.count(); i++) delete typlots[i];

            QList<std::vector<QString> * >  histoplots;

              cnames.clear();
              cnames <<"plotname"<<"columnsandtables"<<"proprieties" <<"histos";
              for(int i=0; i<cnames.count(); i++) histoplots << new ( std::vector<QString>);
              table.fechnotimecolumns_string("Histoplots",cnames,&histoplots);

              for(int i=0;i<(int )histoplots.at(0)->size(); i++){
              QStandardItem *item = new QStandardItem(histoplots.at(0)->at(i));
              grafid.clear();
     //          emit LogMessage(bananaplots.at(1)->at(i)+bananaplots.at(2)->at(i) +bananaplots.at(3)->at(i)+bananaplots.at(4)->at(i)) ;
              item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/chart15.svg")));
              grafid <<3 <<histoplots.at(0)->at(i)<<histoplots.at(1)->at(i)<<histoplots.at(2)->at(i)<<histoplots.at(3)->at(i) ;

              item->setCheckable(true);
              item->setData(grafid);
              item->setEditable(false);

              item->setToolTip(ttip);
              Histoitem->appendRow(item);

              grafid.clear();
        }

        for(int i=0; i<cnames.count(); i++) delete histoplots[i];
}


void DefiningScreens::showhelp(){

}
void DefiningScreens::LogM(QString text){
    emit logmessages(text);
}
void DefiningScreens::LogMR(QString text){
    emit logmessagesR(text);
}
