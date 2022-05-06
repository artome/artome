#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include "server.h"
#include "dbapi.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionconnect, SIGNAL(triggered(bool)),this, SLOT(ConnectDatabase()));
    databaseconnection="clouddbaseconn";
    serverheader="<html><head/><body><p><span style=\" font-weight:600; font-style:italic;\">Cloud database</span></p><p><span style=\" font-weight:600; font-style:italic;\">connection dialog</span></p></body></html>";
    listlabel="Elog Entries";
    listtable="LOGTABLE";
    ui->InstDesc->ext = ".elog";
    ui->InstDesc->extb = ".LOG";
    ui->InstDesc->sfolder = "oelog";
    ui->InstDesc->keyname="RunNumber";

    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(choosetable()));
    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(readEntry(QModelIndex)));
    ui->InstDesc->htmledit=false;

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::ConnectDatabase()
{
    Server dialog;

    int result;
    dialog.connectionname=databaseconnection;
    dialog.labeltext(serverheader);
    result = dialog.exec();
    if(result){
            database=dialog.cdatabase;
            user=dialog.cuser;
            password=dialog.cpassword;
            server=dialog.cserver;
           this->setWindowTitle("Edit HTML entries --- Database:"+database);

           ui->InstDesc->databaseconnection=databaseconnection;
           if(ui->comboBox->currentIndex()==1){
               listlabel="Instruments Description";
               listtable="InstDescription";
               ui->InstDesc->ext = ".desc";
               ui->InstDesc->extb = ".Descp";
               ui->InstDesc->sfolder = "oinstd";
               filllist("NAMEID");
               ui->InstDesc->keyname="NAMEID";
               ui->InstDesc->listtable=listtable;
               ui->InstDesc->htmledit=true;


           } else if(ui->comboBox->currentIndex()==0){
               listlabel="Elog Entries";
               listtable="LOGTABLE";
               ui->InstDesc->ext = ".elog";
               ui->InstDesc->extb = ".LOG";
               ui->InstDesc->sfolder = "oelog";
               ui->InstDesc->keyname="RunNumber";
               filllist("RunNumber");
               ui->InstDesc->listtable=listtable;
               ui->InstDesc->htmledit=true;


           }else if(ui->comboBox->currentIndex()==2){
               listlabel="Local html files";

               listtable="Localfiles";
               listmodel.clear();
               ui->InstDesc->htmledit=false;

           }


           if (!QDir(folder + "oelog").exists())
               QDir().mkdir(folder + "oelog");
           if (!QDir(folder + "oinstd").exists())
               QDir().mkdir(folder + "oinstd");
     }

}
void MainWindow::choosetable()
{
    if(ui->comboBox->currentIndex()==1){
        listlabel="Instruments Description";
        listtable="InstDescription";
        ui->InstDesc->ext = ".desc";
        ui->InstDesc->extb = ".Descp";
        ui->InstDesc->sfolder = "oinstd";
        filllist("NAMEID");
        ui->InstDesc->keyname="NAMEID";
        ui->InstDesc->listtable=listtable;
        ui->InstDesc->htmledit=true;


    } else if(ui->comboBox->currentIndex()==0){
        listlabel="Elog Entries";
        listtable="LOGTABLE";
        ui->InstDesc->ext = ".elog";
        ui->InstDesc->extb = ".LOG";
        ui->InstDesc->sfolder = "oelog";
        ui->InstDesc->keyname="RunNumber";
        filllist("RunNumber");
        ui->InstDesc->listtable=listtable;
        ui->InstDesc->htmledit=true;


    }else if(ui->comboBox->currentIndex()==2){
        listlabel="Local html files";

        listtable="Localfiles";
        listmodel.clear();
        ui->InstDesc->htmledit=false;

    }
     ui->InstDesc->changeuploadicon(ui->comboBox->currentIndex());

}

void MainWindow::filllist(QString columnname){

    listmodel.clear();

    QStandardItem *parentItem = listmodel.invisibleRootItem();
    QList<QVariant> grafid;
    QStringList rootData;
    QString     ttip;
    rootData << listlabel;
    listmodel.setHorizontalHeaderLabels(rootData);
    dbapi table;
    table.connectionName=databaseconnection;
    connect (&table,SIGNAL(logmessages(QString)),this,SLOT(LogMessagesINST(QString)));
    connect (&table,SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesRINST(QString)));
    QStringList cnames;

          QList<std::vector<QString> * >  insts;

            cnames.clear();
            cnames <<columnname ;
            for(int i=0; i<cnames.count(); i++) insts << new ( std::vector<QString>);
            table.fechnotimecolumns_string(listtable,cnames,&insts);

            for(unsigned long i=0;i<insts.at(0)->size(); i++){
            QStandardItem *item = new QStandardItem(insts.at(0)->at(i));
            grafid.clear();
            grafid <<columnname<<insts.at(0)->at(i);
            item->setData(grafid);
            item->setEditable(false);
             parentItem->appendRow(item);

            grafid.clear();
      }
            for(int i=0; i<insts.count();i++) delete insts.at(i);
            ui->treeView->setModel(&listmodel);
            ui->treeView->show();


}

void MainWindow::readEntry(QModelIndex index)
{
    if (ui->InstDesc->Texthaschanged) {
        QMessageBox msgBox;
        msgBox.setText("The Entry has been modified.");
        msgBox.setInformativeText("Do you want to ignore the changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Yes:
            // Save was clicked
            break;

        case QMessageBox::Cancel:
            // Cancel was clicked
            return;
            break;
        default:
            // should never be reached
            break;
        }
    }
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    dbapi table;
    table.connectionName=databaseconnection;
    connect (&table,SIGNAL(logmessages(QString)),this,SLOT(LogMessagesINST(QString)));
    connect (&table,SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesRINST(QString)));
    QStringList cnames;
    cnames.clear();
    cnames <<"ENTRY";
    QList <std::vector<QString> * > c;

    for(int i=0; i<cnames.count(); i++) c << new ( std::vector<QString>);

    QList<QVariant> list,list1;
    list = listmodel.itemFromIndex(index)->data().toList();
    ui->InstDesc->rootname=list[1].toString();
    ui->InstDesc->listtable=listtable;
    table.fechnotimecolumns_string_filterstring(listtable, cnames,list[0].toString(),list[1].toString() , &c );
    if (c.at(0)->size()){
        ui->InstDesc->fillcanvas(c.at(0)->at(0));
         ui->InstDesc->writetextname(list[1].toString());
      }

      for(int i=0; i<cnames.count(); i++) delete c[i]; //acrescentei sem testar
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

}

void MainWindow::LogMessages(QString mess){
    ui->textBrowser->append(mess);

}
void MainWindow::LogMessagesR(QString mess){
    ui->textBrowser->setTextColor("red");
    ui->textBrowser->append(mess);
    ui->textBrowser->setTextColor("black");


}
void MainWindow::setextfolder() {
    ui->InstDesc->folder = folder;

}
