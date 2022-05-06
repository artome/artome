#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include "server.h"
#include "dbapi.h"
#include "timefromruns.h"
#include <QSettings>
#include <QProcess>
#include <QRegularExpression>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    refreshtimer=new QTimer(this);
    refreshtimer->start(300000); //5 min
    Monitoringdatabase="campaignMonitoring";
    connected=false;
    connect(ui->Rtimeselect,SIGNAL(pressed()),this,SLOT(timefromruns()));
    connect(ui->sendtime,SIGNAL(pressed()),this,SLOT(sendtime()));
    connect(ui->pclose,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->Last2h,SIGNAL(pressed()),this,SLOT(last2h()));
    connect(ui->Last4h,SIGNAL(pressed()),this,SLOT(last4h()));
    connect(ui->last8h,SIGNAL(pressed()),this,SLOT(last8h()));
    connect(ui->unsync,SIGNAL(pressed()),this,SLOT(unsynk()));
    connect(ui->killall,SIGNAL(pressed()),this,SLOT(killall()));
    connect(ui->refresh,SIGNAL(pressed()),this,SLOT(refreshlists()));

    connect(ui->SetRunLayouts,SIGNAL(pressed()),this,SLOT(layoutdialog()));

    connect(ui->slavepath,SIGNAL(textEdited(QString)),this,SLOT(changeslavepath(QString)));

    current_time.setTimeSpec(Qt::UTC);
    firsttime.setTimeSpec(Qt::UTC);
    current_time=current_time.currentDateTime().toTimeSpec(Qt::UTC);
    firsttime=current_time.toTimeSpec(Qt::UTC).addSecs(-30*60);
    ui->IdateEdit->setDate(firsttime.date());
    ui->LdateEdit->setDate(current_time.date());
    ui->ItimeEdit->setTime(firsttime.time());
    ui->LtimeEdit->setTime(current_time.time());
    QSettings settings("HDaxSystem", "Monitoring");
    slavepath=settings.value("Path/path").toString();
    ui->slavepath->setText(slavepath);

    connect(ui->bytype, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(callslave(QModelIndex)));

    connect(ui->byinstrument, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(callslaveI(QModelIndex)));

    connect(ui->trunlayouts, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(callslaveR(QModelIndex)));


   runlayouts=nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::refreshlists(){
    plotsfilltree();
    plotsfilltreeInstr();
    runfilltree();
}
void MainWindow::ConnectDatabase() {
  Server dialog;

  int result;
  int control;
  control=0;
  dialog.connectionname = Monitoringdatabase;
  QString lixo;
  lixo = "<html><head/><body><p><span style=\" font-weight:600; "
                 "font-style:italic;\">CloudMonitoring</span></p><p><span "
                 "style=\" font-weight:600; font-style:italic;\">connection "
                 "dialog</span></p></body></html>";
  dialog.labeltext(lixo);
  do {
      control++;
      result = dialog.exec();
  if (result) {
    database = dialog.cdatabase;
    user = dialog.cuser;
    password = dialog.cpassword;
    server = dialog.cserver;
    setWindowTitle("Cloud Monitoring --- Database:" + database);
    ui->actionConnect->setChecked(true);
    connected=true;
    plotsfilltree();
    plotsfilltreeInstr();
    runfilltree();
    connect(refreshtimer, &QTimer::timeout, this, &MainWindow::refreshlists);


  }else{
      if(control>2){
          QMessageBox msgBox;
          msgBox.setText("You are not been able to connect");
          msgBox.setInformativeText("do you want to try again?");
          msgBox.setIcon(QMessageBox::Question);

          msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

          int ret = msgBox.exec();
          switch (ret) {
          case QMessageBox::Yes:

            break;
          case QMessageBox::No:

               return;
            break;
          default:
            // should never be reached
            break;
          }
      }

  }
  } while (!result);

}

void MainWindow::plotsfilltree()
{
    listplotmodel.clear();
    QStandardItem *parentItem = listplotmodel.invisibleRootItem();
    QList<QVariant> grafid;
    QStandardItem   *Graphitem   = new QStandardItem("timevalues-plots");
    QStandardItem   *Bananaitem  = new QStandardItem("Surface-plots");
    QStandardItem   *Screens   = new QStandardItem("Aggregated plots");
    QStandardItem   *Histoitem   = new QStandardItem("Histo-plots");
    //   QStandardItem   *Scatteritem = new QStandardItem("Scatter-plots");

    // parentItem->setColumnCount(3); o 1 que aparecia era a coluna
    QStringList rootData;
    QString     ttip;
    long long   iparent;
    iparent = -1;
    rootData << "Plots";
    listplotmodel.setHorizontalHeaderLabels(rootData);

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

    grafid.clear();
    grafid << iparent;
    Screens->setData(grafid);
    ttip.clear();
    ttip.append("Aggregated,  in a single page, plots stored in database");
    Histoitem->setToolTip(ttip);
    parentItem->appendRow(Screens);

    dbapi table;
    table.connectionName=Monitoringdatabase;
    connect (&table,SIGNAL(logmessages(QString)),this,SLOT(LogMessage(QString)));
    connect (&table,SIGNAL(logmessagesR(QString)),this,SLOT(LogMessageR(QString)));
    QStringList cnames;



    QList<std::vector<QString> * >  bananaplots;

    cnames.clear();
    cnames <<"plotname" << "columnsandtables";
    for(int i=0; i<cnames.count(); i++) bananaplots << new ( std::vector<QString>);
     table.fechnotimecolumns_string_orderby("Surfaceplots",cnames,"plotname",&bananaplots);

    for(unsigned int i=0;i<bananaplots.at(0)->size(); i++){
        QStandardItem *item = new QStandardItem(bananaplots.at(0)->at(i));
        grafid.clear();
        item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/banana4.svg")));
        grafid <<2 <<bananaplots.at(0)->at(i) << bananaplots.at(1)->at(i) ;
        QStringList lixo;
        lixo=bananaplots.at(1)->at(i).split(";");
        item->setData(grafid);
        item->setEditable(false);
        ttip.clear();
        ttip.append("Banana from table ");
        ttip.append(lixo.at(0));
        ttip.append(" entitled :\n");
        ttip.append(bananaplots.at(0)->at(i));

        item->setToolTip(ttip);
        Bananaitem->appendRow(item);

        grafid.clear();



    }
    for(int i=0;i<bananaplots.count();i++) delete bananaplots.at(i);

    QList<std::vector<QString> * >  typlots;


    cnames.clear();
    cnames <<"plotname" << "columnsandtables";
    for(int i=0; i<cnames.count(); i++) typlots << new ( std::vector<QString>);

    table.fechnotimecolumns_string_orderby("typlots",cnames,"plotname",&typlots);

    for(unsigned int i=0;i< typlots.at(0)->size(); i++){
        QStandardItem *item = new QStandardItem(typlots.at(0)->at(i));
        grafid.clear();
        item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/educational15.svg")));
        grafid <<1 <<typlots.at(0)->at(i)<<typlots.at(1)->at(i);
        item->setData(grafid);
        item->setEditable(false);

        Graphitem->appendRow(item);

        grafid.clear();
    }

    for(int i=0;i<typlots.count();i++) delete typlots.at(i);




    QList<std::vector<QString> * >  histoplots;

    cnames.clear();
    cnames <<"plotname" << "columnsandtables";
    for(int i=0; i<cnames.count(); i++) histoplots << new ( std::vector<QString>);
    table.fechnotimecolumns_string_orderby("Histoplots",cnames,"plotname",&histoplots);

    for(unsigned int i=0;i< histoplots.at(0)->size(); i++){
        QStandardItem *item = new QStandardItem(histoplots.at(0)->at(i));
        grafid.clear();
        item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/chart15.svg")));
        grafid <<3 <<histoplots.at(0)->at(i) << histoplots.at(1)->at(i) ;

        item->setData(grafid);
        item->setEditable(false);
        ttip.clear();
        QStringList lixo;
        lixo=histoplots.at(1)->at(i).split(";");
        ttip.append("Histo from table");
        ttip.append(lixo.at(0));
        ttip.append(" entitled :\n");
        ttip.append(histoplots.at(1)->at(i));

        item->setToolTip(ttip);
        Histoitem->appendRow(item);

        grafid.clear();
    }


    for(int i=0;i<histoplots.count();i++) delete histoplots.at(i);


    QList<std::vector<QString> * >  screenplots;

    cnames.clear();
    cnames <<"screenname";
    for(int i=0; i<cnames.count(); i++) screenplots << new ( std::vector<QString>);
    //     table.fechnotimecolumns_string("Screens",cnames,&screenplots);
    table.fechnotimecolumns_string_filterstring_orderby("Screens",cnames,"plotorder","0","screenname",&screenplots);
 //   table.fechnotimecolumns_string_orderby("Screens",cnames,"screenname",&screenplots);
    for(unsigned int i=0;i< screenplots.at(0)->size(); i++){
        QStandardItem *item = new QStandardItem(screenplots.at(0)->at(i));
        grafid.clear();
        item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/screens.svg")));
        grafid <<10 <<screenplots.at(0)->at(i) ;
        item->setData(grafid);
        item->setEditable(false);
        Screens->appendRow(item);

        grafid.clear();
    }
    for(int i=0;i<screenplots.count();i++) delete screenplots.at(i);

    ui->bytype->setModel(&listplotmodel);
    ui->bytype->show();

}

void MainWindow::plotsfilltreeInstr()
{
    listplotmodelInst.clear();
    QStandardItem *parentItem = listplotmodelInst.invisibleRootItem();
    QList<QVariant> grafid;

    // parentItem->setColumnCount(3); o 1 que aparecia era a coluna
    QStringList rootData;
    QString     ttip;
    rootData << "Plots by Instrument";
    listplotmodelInst.setHorizontalHeaderLabels(rootData);


    dbapi table;
    table.connectionName=Monitoringdatabase;
    connect (&table,SIGNAL(logmessages(QString)),this,SLOT(LogMessage(QString)));
    connect (&table,SIGNAL(logmessagesR(QString)),this,SLOT(LogMessageR(QString)));

    QStringList cnames,instrumentstable;
    QList<std::vector<QString> * >  Instruments;
    cnames.clear();
    instrumentstable.clear();
    plostInst.clear();

    cnames <<"NAMEID" << "NAME"  ;
    for(int i=0; i<cnames.count(); i++) Instruments << new ( std::vector<QString>);
     table.fechnotimecolumns_string_orderby("TablesforDSV",cnames,"NAME", &Instruments);

     for(int i=0;i<Instruments.at(0)->size(); i++){
     QStandardItem *item =new QStandardItem(Instruments.at(1)->at(i));
     grafid.clear();
     grafid <<  "-1";
     item->setData(grafid);
     ttip.clear();
     ttip.append("Plots stored in database using data from the instrument ");
     ttip.append(Instruments.at(0)->at(i));
     item->setToolTip(ttip);
     parentItem->appendRow(item);
     plostInst    << item;
     instrumentstable << Instruments.at(0)->at(i);
    }
    for(int i=0; i< Instruments.count(); i++) delete Instruments.at(i);



    QList<std::vector<QString> * >  bananaplots;

    cnames.clear();
    cnames <<"plotname" << "columnsandtables";
    for(int i=0; i<cnames.count(); i++) bananaplots << new ( std::vector<QString>);
     table.fechnotimecolumns_string_orderby("Surfaceplots",cnames,"plotname",&bananaplots);

    for(unsigned int i=0;i<bananaplots.at(0)->size(); i++){
        QStandardItem *item = new QStandardItem(bananaplots.at(0)->at(i));
        grafid.clear();
        item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/banana4.svg")));
        grafid <<2 <<bananaplots.at(0)->at(i) << bananaplots.at(1)->at(i) ;
        QStringList lixo;
        lixo=bananaplots.at(1)->at(i).split(";");
        item->setData(grafid);
        item->setEditable(false);
        ttip.clear();
        ttip.append("Banana from table ");
        ttip.append(lixo.at(0));
        ttip.append(" entitled :\n");
        ttip.append(bananaplots.at(0)->at(i));

        item->setToolTip(ttip);
        bool already=false;
       for(int j=0; j<plostInst.size(); j++ ){
           {
               if(bananaplots.at(1)->at(i).contains(instrumentstable.at(j))){
                        if (!already) plostInst.at(j)->appendRow(item);else plostInst.at(j)->appendRow(item->clone());
                        already=true;
                    }
              }
        }



        grafid.clear();



    }
    for(int i=0;i<bananaplots.count();i++) delete bananaplots.at(i);

    QList<std::vector<QString> * >  typlots;


    cnames.clear();
    cnames <<"plotname" << "columnsandtables";
    for(int i=0; i<cnames.count(); i++) typlots << new ( std::vector<QString>);

    table.fechnotimecolumns_string_orderby("typlots",cnames,"plotname",&typlots);

    for(unsigned int i=0;i< typlots.at(0)->size(); i++){
        QStandardItem *item = new QStandardItem(typlots.at(0)->at(i));
        grafid.clear();
        item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/educational15.svg")));
        grafid <<1 <<typlots.at(0)->at(i)<<typlots.at(1)->at(i);
        item->setData(grafid);
        item->setEditable(false);

        bool already=false;
        for(int j=0; j<plostInst.size(); j++ ){
              if(typlots.at(1)->at(i).contains(instrumentstable.at(j))){
                  if (!already) plostInst.at(j)->appendRow(item);else plostInst.at(j)->appendRow(item->clone());
                  already=true;
              }
        }


        grafid.clear();
    }

    for(int i=0;i<typlots.count();i++) delete typlots.at(i);




    QList<std::vector<QString> * >  histoplots;

    cnames.clear();
    cnames <<"plotname" << "columnsandtables";
    for(int i=0; i<cnames.count(); i++) histoplots << new ( std::vector<QString>);
    table.fechnotimecolumns_string_orderby("Histoplots",cnames,"plotname",&histoplots);

    for(unsigned int i=0;i< histoplots.at(0)->size(); i++){
        QStandardItem *item = new QStandardItem(histoplots.at(0)->at(i));
        grafid.clear();
        item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/chart15.svg")));
        grafid <<3 <<histoplots.at(0)->at(i) << histoplots.at(1)->at(i) ;

        item->setData(grafid);
        item->setEditable(false);
        ttip.clear();
        QStringList lixo;
        lixo=histoplots.at(1)->at(i).split(";");
        ttip.append("Histo from table");
        ttip.append(lixo.at(0));
        ttip.append(" entitled :\n");
        ttip.append(histoplots.at(1)->at(i));

        item->setToolTip(ttip);
        for(int j=0; j<plostInst.size(); j++ ){
              if(histoplots.at(1)->at(i).contains(instrumentstable.at(j)))plostInst.at(j)->appendRow(item);
        }

        grafid.clear();
    }


    for(int i=0;i<histoplots.count();i++) delete histoplots.at(i);


    ui->byinstrument->setModel(&listplotmodelInst);
    ui->byinstrument->show();

}

void MainWindow::closeEvent(QCloseEvent *event) {

  QMessageBox msgBox;
  msgBox.setText("This will close the Program");
  msgBox.setInformativeText("are you sure?");
  msgBox.setIcon(QMessageBox::Question);

  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

  int ret = msgBox.exec();
  switch (ret) {
  case QMessageBox::Yes:
    event->accept();
    break;
  case QMessageBox::No:
    // Don't Save was clicked
    event->ignore();
    break;
  default:
    // should never be reached
    break;
  }
}
void MainWindow::timefromruns()
{
        TimeFromRuns dialog;
       dbapi table;

       table.connectionName=Monitoringdatabase;
  //     connect (&table,SIGNAL(logmessages(QString)),this,SLOT(Logmessages(QString)));
  //     connect (&table,SIGNAL(logmessagesR(QString)),this,SLOT(LogmessagesR(QString)));
       QStringList cnames;
       QList<std::vector<QString> * >  runrows;


        cnames.clear();
           cnames <<"RunNumber" <<"StartTime" <<"EndTime"<<"Type" <<"Description"  <<"Comments" ;
        for(int i=0; i<cnames.count(); i++) runrows << new ( std::vector<QString>);
        table.fechnotimecolumns_string_orderby("Runtable",cnames,"RunNumber",&runrows);
        model.m_rowCount=runrows.at(0)->size();
         model.rownames.clear();
         model.startrun.clear();
        model.endrun.clear();
        model.description.clear();
        model.commentaries.clear();
         model.type .clear();
          model.startc.clear();
          model.endc.clear();
        for(int i=0;i<(int )runrows.at(0)->size(); i++){
         model.rownames <<  runrows.at(0)->at(i);
         model.startrun << QDateTime::fromMSecsSinceEpoch(runrows.at(1)->at(i).toLongLong()).toUTC().toSecsSinceEpoch();
         model.endrun << QDateTime::fromMSecsSinceEpoch(runrows.at(2)->at(i).toLongLong()).toUTC().toSecsSinceEpoch();
         model.description <<  runrows.at(4)->at(i);

          model.commentaries <<  runrows.at(5)->at(i);
         model.type <<  runrows.at(3)->at(i);
         model.startc << 0;
         model.endc << 0;

    }

       dialog.model=&model;
       connect(&model,SIGNAL(checkchanged(int,int,bool)), &dialog,SLOT(check(int,int,bool)));
       dialog.fill();
       int result;
       result = dialog.exec();
       if(result){
        model.vscrollpos=dialog.Getvscrollpos();
        IItime=dialog.Itime;
        LLtime=dialog.Ltime;

        writeinterface();
        //Este comando ser chamado antes das duas linhas seguintes.
       // porque vai mudar o valor no widgets do tempo e por conseguinte mudaria os labels para manual setted.

        Itimeslabel=dialog.Sstart;
        Ltimeslabel=dialog.Send;

        ui->startrun->setText(Itimeslabel);
        ui->endrun->setText(Ltimeslabel);

         }

    }
void MainWindow::unsynk()
{

    QFile localpipe("clovisoutsidetime.pro");
    QTextStream out (&localpipe);
    if(localpipe.open(QFile::WriteOnly)){
        out << "-1" <<"\t" <<"-1" << "\n";
        QFile::remove("clovisoutsidetime.txt");
        localpipe.close();
        localpipe.rename("clovisoutsidetime.txt");
        QTimer::singleShot(20000, this, SLOT(last2h()));

    }else{
        QMessageBox msgBox;
        msgBox.setText("Not possible to open file to write unsync instrunstions");
        msgBox.exec();
    }
}
void MainWindow::last2h()
{

    current_time=current_time.currentDateTime().toTimeSpec(Qt::UTC);
    firsttime=current_time.toTimeSpec(Qt::UTC).addSecs(-2*60*60);
    long long itime,ltime;
    itime=(long long)firsttime.toUTC().toSecsSinceEpoch();
    ltime=(long long)current_time.toUTC().toSecsSinceEpoch();

    QFile localpipe("clovisoutsidetime.pro");
    QTextStream out (&localpipe);
    if(localpipe.open(QFile::WriteOnly)){
        out << QString::number(itime) <<"\t" <<QString::number(ltime) << "\n";
        QFile::remove("clovisoutsidetime.txt");
        localpipe.close();
        localpipe.rename("clovisoutsidetime.txt");
    }else{
        QMessageBox msgBox;
        msgBox.setText("Not possible to open file to write time");
        msgBox.exec();
    }
}
void MainWindow::last4h()
{

    current_time=current_time.currentDateTime().toTimeSpec(Qt::UTC);
    firsttime=current_time.toTimeSpec(Qt::UTC).addSecs(-4*60*60);
    long long itime,ltime;
    itime=(long long)firsttime.toUTC().toSecsSinceEpoch();
    ltime=(long long)current_time.toUTC().toSecsSinceEpoch();
    ui->IdateEdit->setDate(firsttime.date());
    ui->LdateEdit->setDate(current_time.date());
    ui->ItimeEdit->setTime(firsttime.time());
    ui->LtimeEdit->setTime(current_time.time());

    QFile localpipe("clovisoutsidetime.pro");
    QTextStream out (&localpipe);
    if(localpipe.open(QFile::WriteOnly)){
        out << QString::number(itime) <<"\t" <<QString::number(ltime) << "\n";
        QFile::remove("clovisoutsidetime.txt");
        localpipe.close();
        localpipe.rename("clovisoutsidetime.txt");
    }else{
        QMessageBox msgBox;
        msgBox.setText("Not possible to open file to write time");
        msgBox.exec();
    }
}
void MainWindow::last8h()
{

    current_time=current_time.currentDateTime().toTimeSpec(Qt::UTC);
    firsttime=current_time.toTimeSpec(Qt::UTC).addSecs(-8*60*60);
    long long itime,ltime;
    itime=(long long)firsttime.toUTC().toSecsSinceEpoch();
    ltime=(long long)current_time.toUTC().toSecsSinceEpoch();
    ui->IdateEdit->setDate(firsttime.date());
    ui->LdateEdit->setDate(current_time.date());
    ui->ItimeEdit->setTime(firsttime.time());
    ui->LtimeEdit->setTime(current_time.time());

    QFile localpipe("clovisoutsidetime.pro");
    QTextStream out (&localpipe);
    if(localpipe.open(QFile::WriteOnly)){
        out << QString::number(itime) <<"\t" <<QString::number(ltime) << "\n";
        QFile::remove("clovisoutsidetime.txt");
        localpipe.close();
        localpipe.rename("clovisoutsidetime.txt");
    }else{
        QMessageBox msgBox;
        msgBox.setText("Not possible to open file to write time");
        msgBox.exec();
    }
}



void  MainWindow::writeinterface()
{
    QString savedi,savedf;
    savedi=Itimeslabel; savedf=Ltimeslabel;
    ui->IdateEdit->setDate(IItime.date());
    ui->LdateEdit->setDate(LLtime.date());
    ui->ItimeEdit->setTime(IItime.time());
    ui->LtimeEdit->setTime(LLtime.time());
    Itimeslabel=savedi; Ltimeslabel=savedf;
    ui->startrun->setText(Itimeslabel);
    ui->endrun->setText(Ltimeslabel);

}

void MainWindow::sendtime()
{

    Itime.setDate(ui->IdateEdit->date());
    Ltime.setDate(ui->LdateEdit->date());
    Itime.setTime(ui->ItimeEdit->time());
    Ltime.setTime(ui->LtimeEdit->time());
    long long itime,ltime;
    itime=(long long)Itime.toUTC().toSecsSinceEpoch();
    ltime=(long long)Ltime.toUTC().toSecsSinceEpoch();
    QFile localpipe("clovisoutsidetime.pro");
    QTextStream out (&localpipe);
    if(localpipe.open(QFile::WriteOnly)){
        out << QString::number(itime) <<"\t" <<QString::number(ltime) << "\n";
        QFile::remove("clovisoutsidetime.txt");
        localpipe.close();
        localpipe.rename("clovisoutsidetime.txt");
    }else{
        QMessageBox msgBox;
        msgBox.setText("Not possible to open file to write time");
        msgBox.exec();
    }


}
void MainWindow::changeslavepath(QString text){
QSettings settings("HDaxSystem", "Monitoring");
slavepath=text;
settings.setValue("Path/path", text);
}
void MainWindow::callslave(QModelIndex index)
{
    QList<QVariant> list;
    list = listplotmodel.itemFromIndex(index)->data().toList();
    if (list[0].toInt() == -1) {
        return;
    }
 QString table,plot;
 if(list.at(0).toInt()==1){
    plot=list.at(1).toString();
    table="typlots";
 }else  if(list.at(0).toInt()==2){
     plot=list.at(1).toString();
     table="Surfaceplots";
  }else  if(list.at(0).toInt()==3){
     plot=list.at(1).toString();
     table="Histoplots";
  }else  if(list.at(0).toInt()==10){
     plot=list.at(1).toString();
     table="Screens";
  }
 QString command, barg;
 QStringList arguments;
 arguments.clear();
 barg="database=";
 barg.append(database);
 arguments<< barg;
 barg="user=";
 barg.append(user);
 arguments<< barg;
 barg="password=";
 barg.append(password);
 arguments<< barg;
 barg="server=";
 barg.append(server);
 arguments<< barg;
 barg="plot=";
 barg.append(plot);
 barg.append("@");
 barg.append(table);
 arguments<< barg;

 barg="hours=";
 barg.append(ui->horas->text());
 arguments<< barg;

 barg="minutes=";
 barg.append(ui->minutos->text());
 arguments<< barg;

 command=ui->slavepath->text();
 command.append("MonitoringSlave");


QProcess::startDetached(command,arguments);
}
void MainWindow::callslaveI(QModelIndex index)
{
    QList<QVariant> list;
    list = listplotmodelInst.itemFromIndex(index)->data().toList();
    if (list[0].toInt() == -1) {
        return;
    }
 QString table,plot;
 if(list.at(0).toInt()==1){
    plot=list.at(1).toString();
    table="typlots";
 }else  if(list.at(0).toInt()==2){
     plot=list.at(1).toString();
     table="Surfaceplots";
  }else  if(list.at(0).toInt()==3){
     plot=list.at(1).toString();
     table="Histoplots";
  }else  if(list.at(0).toInt()==10){
     plot=list.at(1).toString();
     table="Screens";
  }
 QString command, barg;
 QStringList arguments;
 arguments.clear();
 barg="database=";
 barg.append(database);
 arguments<< barg;
 barg="user=";
 barg.append(user);
 arguments<< barg;
 barg="password=";
 barg.append(password);
 arguments<< barg;
 barg="server=";
 barg.append(server);
 arguments<< barg;
 barg="plot=";
 barg.append(plot);
 barg.append("@");
 barg.append(table);
 arguments<< barg;

 barg="hours=";
 barg.append(ui->horas->text());
 arguments<< barg;

 barg="minutes=";
 barg.append(ui->minutos->text());
 arguments<< barg;

 command=ui->slavepath->text();
 command.append("MonitoringSlave");


QProcess::startDetached(command,arguments);
}
void MainWindow::killall()
{
    QMessageBox msgBox;
    msgBox.setText("This will close all the monitoring windows");
    msgBox.setInformativeText("are you sure?");
    msgBox.setIcon(QMessageBox::Question);

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:

      break;
    case QMessageBox::No:

         return;
      break;
    default:
      // should never be reached
      break;
    }
    QString command;
    QStringList arguments;
    arguments.clear();
    arguments <<"-x" << "MonitoringSlave";
    command="pkill";
    QProcess::startDetached(command,arguments);
}
void MainWindow::layoutdialog()
{
    if(!runlayouts){
        runlayouts=new DefineRunLayouts(this);
        runlayouts->setWindowFlags(Qt::Window);
        runlayouts->filllayouts();
}
     runlayouts->show();
     runlayouts->raise();
     runlayouts-> activateWindow();
}
void MainWindow::LogMessage(QString mess)
{
    ui->textBrowser->append(mess);
}
void MainWindow::LogMessageR(QString text)
{
    ui->textBrowser->setTextColor("red");
    ui->textBrowser->append(text);
    ui->textBrowser->setTextColor("black");
}

void MainWindow::runfilltree()
{
    struct RunsL{
        QString name;
        QString command;
    };
    listrunmodel.clear();
    QStandardItem *parentItem = listrunmodel.invisibleRootItem();
    QList<QVariant> grafid;
    QStringList rootData;
    rootData << "Run Layouts";
    listrunmodel.setHorizontalHeaderLabels(rootData);

    QList<RunsL> runs;
    runs.clear();
    RunsL run;

    QSettings settings("CampaignMonitoring","runlayouts");
    int size = settings.beginReadArray("layouts");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        run.name = settings.value("name").toString();
        run.command = settings.value("command").toString();
        runs.append(run);
}
    settings.endArray();

    for(int i=0; i<runs.count();i++){
       QStandardItem *item = new QStandardItem(runs.at(i).name);
       grafid.clear();
       item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/responsive12.svg")));
       grafid <<1 <<runs.at(i).name << runs.at(i).command;
       item->setData(grafid);
       item->setEditable(false);
       item->setCheckable(false);
       parentItem->appendRow(item);

    }

    ui->trunlayouts->setModel(&listrunmodel);
    ui->trunlayouts->show();

}

void MainWindow::callslaveR(QModelIndex index)
{
    QList<QVariant> list;
    list = listrunmodel.itemFromIndex(index)->data().toList();
    if (list[0].toInt() == -1) {
        return;
    }
QStringList instances;
QString command, barg;
QStringList arguments,ninstances;
command=ui->slavepath->text();
command.append("MonitoringSlave");

instances=list.at(2).toString().split(";");

for(int i=0; i<instances.count();i++){
    ninstances=instances.at(i).split(QRegularExpression("\\s+"));
    arguments.clear();
   barg="database=";
   barg.append(database);
   arguments<< barg;
   barg="user=";
   barg.append(user);
   arguments<< barg;
   barg="password=";
   barg.append(password);
   arguments<< barg;
   barg="server=";
   barg.append(server);
   arguments<< barg;
   for(int j=0;j<ninstances.count();j++){
       arguments <<ninstances.at(j);
   }
           QProcess::startDetached(command,arguments);
 }



}
