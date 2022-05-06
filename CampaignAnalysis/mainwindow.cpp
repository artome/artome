#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>

#include "mainwindow.h"
#include "maketext.h"
#include "ui_mainwindow.h"

#include "hdpifactores.h"
#include "dbapi.h"
#include "definingscreens.h"
#include "savetable.h"
#include "TextView.h"
#include "gslkindofwrapper.h"
#include "loadlocalfile.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->DockMessages->setVisible(false);
    ui->actionLog_Messages_Area->setChecked(false);
    ui->mainToolBar->setVisible(true);
    ui->actionMain_toolBar->setChecked(true);
    ui->actionExplorer_dock_widget->setChecked(true);
    ui->dockWidget->setVisible(true);



    WindowsActions();
    windowMapper = new QSignalMapper(this);
 //   connect(windowMapper, SIGNAL(mappedWidget(QWidget*)),
 //           this, SLOT(setActiveSubWindow(QWidget*)));
    connect(windowMapper, &QSignalMapper::mappedObject,
            this, &MainWindow::setActiveSubWindowO);

    connect(ui->mdiArea, &QMdiArea::subWindowActivated,
            this, &MainWindow::updateWindowMenu);


    connect(ui->actionOpenDatabase,&QAction::triggered,this, &MainWindow::databasedialog);
    connect(ui->actionFactores_for_HighDPI,&QAction::triggered,this, &MainWindow::setdpifactores);
    connect(ui->actionDownload_data,&QAction::triggered,this, &MainWindow::Listdatabaseitens);
    connect(ui->actionChoose_time_interval,&QAction::triggered,this, &MainWindow::choosetimeinterval);
    connect(ui->explorerView, &QAbstractItemView::doubleClicked,
            this, &MainWindow::selectwindows);

    connect(ui->PlotsView, &QAbstractItemView::doubleClicked,
            this, &MainWindow::createplots);

    connect(ui->byinstrument, &QAbstractItemView::doubleClicked,
            this, &MainWindow::createplotsI);


    connect(ui->LateraltabWidget, &QTabWidget::tabBarClicked,this,&MainWindow::lateralwidgetsactions);

    connect(ui->actionDefining_Screens,&QAction::triggered,this, &MainWindow::helpondefiningscreens);
    connect(ui->actionInstrument_Status,&QAction::triggered,this, &MainWindow::Inststatusdialog);


    connect(&Dthread, &DownloadItensThread::Endwork, this,
            &MainWindow::Enddownloadthread);
    connect(&Dthread, &DownloadItensThread::LogMessage, this,
            &MainWindow::LogMessage);

    connect(ui->actionElog,&QAction::triggered,this, &MainWindow::viewelog);
    connect(ui->actionShiftReport,&QAction::triggered,this, &MainWindow::viewreport);


    connect(ui->actionShow_data_table_commentaries,&QAction::triggered,this, &MainWindow::viewdatatdesc);
    connect(ui->actionEdilt_local_plots,&QAction::triggered,this,&MainWindow::editlocalplotsdialog);
    connect(ui->actionImport_Table_for_banana,&QAction::triggered,this,&MainWindow::Import_Table_for_banana);

    opendatabasesdialog=nullptr;
    listchanneltree=nullptr;
    managingtimedialog=nullptr;
    Editlocalplotsdialog=nullptr;
    screendialog=nullptr;
    inststatusdialog=nullptr;
    hdpiwindows=1.0;
    currentindex=-1;
    downloadingintofile=false;
}

MainWindow::~MainWindow()
{
    delete ui;
    if(opendatabasesdialog) delete opendatabasesdialog;
    if(managingtimedialog) delete managingtimedialog;
    if(listchanneltree) delete listchanneltree;
    if(screendialog) delete screendialog;
    if(inststatusdialog) delete inststatusdialog;

}

void MainWindow::selectwindows(QModelIndex index)
{
    QList<QVariant> list;
    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    list = explorermodel.itemFromIndex(index)->data().toList();
    if(list.at(0).toInt()==-1) return;
    setActiveSubWindow(windows.at(list.at(1).toInt()));



}
void MainWindow::updateWindowMenu()
{
    ui->menuWindows->clear();
    ui->menuWindows->addAction(closeAct);
    ui->menuWindows->addAction(closeAllAct);
    ui->menuWindows->addSeparator();
    ui->menuWindows->addAction(tileAct);
    ui->menuWindows->addAction(cascadeAct);
    ui->menuWindows->addAction(tileAct);
    ui->menuWindows->addSeparator();
    ui->menuWindows->addAction(nextAct);
    ui->menuWindows->addAction(previousAct);
    ui->menuWindows->addAction(separatorAct);
    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    explorermodel.clear();
    QStandardItem *parentItem = explorermodel.invisibleRootItem();
    QList<QVariant> grafid;
    QStandardItem   *Tables   = new QStandardItem("Tables");
    QStandardItem   *Plots  = new QStandardItem("Plots");
    QStandardItem   *Texts  = new QStandardItem("Text");
    QStringList rootData;
    QString     ttip;
    long long   iparent;
    iparent = -1;
    rootData << "Windows";
    explorermodel.setHorizontalHeaderLabels(rootData);

    grafid.clear();
    grafid <<  iparent;
    Plots->setData(grafid);
    ttip.clear();
    ttip.append("Plots ");
    Plots->setToolTip(ttip);
    parentItem->appendRow(Plots);
    grafid.clear();
    grafid <<  iparent;
    Tables->setData(grafid);
    ttip.clear();
    ttip.append("Tables ");
    Tables->setToolTip(ttip);
    parentItem->appendRow(Tables);
    grafid.clear();
    grafid <<  iparent;
    Texts->setData(grafid);
    ttip.clear();
    ttip.append("Texts ");
    Texts->setToolTip(ttip);
    parentItem->appendRow(Texts);
    grafid.clear();
    grafid <<  iparent;

 for (int i = 0; i < windows.size(); ++i) {
     if (strncmp(windows.at(i)->widget()->metaObject()->className(), "plotcanvas", 10) == 0) {
         plotcanvas *child  = qobject_cast<plotcanvas*>(windows.at(i)->widget());
         if(!child) continue;
         QAction *action = ui->menuWindows->addAction(windows.at(i)->windowTitle());
         action->setCheckable(true);
         action->setChecked(child == activeplotcanvas());
         connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
         windowMapper->setMapping(action, windows.at(i));

         QStandardItem *item = new QStandardItem(windows.at(i)->windowTitle());
         grafid.clear();
        if(child->type==0) item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/educational15.svg")));
         else    if (child->type==1)  item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/banana4.svg")));
        else    if (child->type==2)  item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/chart15.svg")));
        else    if (child->type==10)  item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/screens.svg")));
         grafid <<1 << i;
         item->setData(grafid);
         item->setEditable(false);
         ttip.clear();
         ttip.append("activates this window");
         item->setToolTip(ttip);
         Plots->appendRow(item);


     } else if (strncmp(windows.at(i)->widget()->metaObject()->className(), "TableCanvas", 11) == 0) {
         TableCanvas *child  = qobject_cast< TableCanvas *>(windows.at(i)->widget());
         QAction  *action = ui->menuWindows->addAction(windows.at(i)->windowTitle());
         action->setCheckable(true);
         action->setChecked(child == activetablecanvas());
         connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
         windowMapper->setMapping(action, windows.at(i));

         QStandardItem *item = new QStandardItem(windows.at(i)->windowTitle());
         grafid.clear();
         item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/spreadsheet7.svg")));
         grafid <<1 << i;
         item->setData(grafid);
         item->setEditable(false);
         ttip.clear();
         ttip.append("activates this window");
         item->setToolTip(ttip);
         Tables->appendRow(item);


     }else if (strncmp(windows.at(i)->widget()->metaObject()->className(), "TextView", 8) == 0) {
         TextView *child  = qobject_cast< TextView *>(windows.at(i)->widget());
         QAction  *action = ui->menuWindows->addAction(windows.at(i)->windowTitle());
         action->setCheckable(true);
         action->setChecked(child == activeTextView());
         connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
         windowMapper->setMapping(action, windows.at(i));

         QStandardItem *item = new QStandardItem(windows.at(i)->windowTitle());
         grafid.clear();
         item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/spreadsheet7.svg")));
         grafid <<1 << i;
         item->setData(grafid);
         item->setEditable(false);
         ttip.clear();
         ttip.append("activates this window");
         item->setToolTip(ttip);
         Tables->appendRow(item);


     }else if (strncmp(windows.at(i)->widget()->metaObject()->className(), "StatisticsWindow", 16) == 0) {
         StatisticsWindow *child  = qobject_cast< StatisticsWindow *>(windows.at(i)->widget());
         QAction  *action = ui->menuWindows->addAction(windows.at(i)->windowTitle());
         action->setCheckable(true);
         action->setChecked(child == activestatisticswindow());
         connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
         windowMapper->setMapping(action, windows.at(i));

         QStandardItem *item = new QStandardItem(windows.at(i)->windowTitle());
         grafid.clear();
         item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/spreadsheet1.svg")));
         grafid <<1 << i;
         item->setData(grafid);
         item->setEditable(false);
         ttip.clear();
         ttip.append("activates this window");
         item->setToolTip(ttip);
         Tables->appendRow(item);

     }else if (strncmp(windows.at(i)->widget()->metaObject()->className(), "MakeText", 8) == 0) {
         MakeText *child  = qobject_cast< MakeText *>(windows.at(i)->widget());
         QAction  *action = ui->menuWindows->addAction(windows.at(i)->windowTitle());
         action->setCheckable(true);
         action->setChecked(child == activeMakeText());
         connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
         windowMapper->setMapping(action, windows.at(i));

         QStandardItem *item = new QStandardItem(windows.at(i)->windowTitle());
         grafid.clear();
         item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/spreadsheet1.svg")));
         grafid <<1 << i;
         item->setData(grafid);
         item->setEditable(false);
         ttip.clear();
         ttip.append("activates this window");
         item->setToolTip(ttip);
         Tables->appendRow(item);
 }
 ui->explorerView->setModel(&explorermodel);
 ui->explorerView->expandAll();
 ui->explorerView->show();

}
}
plotcanvas *MainWindow::activeplotcanvas()
{
    if (QMdiSubWindow * activeSubWindow = ui->mdiArea->activeSubWindow()) {
        return(qobject_cast<plotcanvas *>(activeSubWindow->widget()));
    }
    return nullptr;
}
void MainWindow::addplotcanvas(plotcanvas *widget)
{

    ui->mdiArea->addSubWindow(widget);
    QRect   geo    = widget->parentWidget()->geometry();
    widget->parentWidget()->setGeometry(geo.x(), geo.y(), 450, 300);

}
void MainWindow::addstatisticswindow(StatisticsWindow *widget)
{

    ui->mdiArea->addSubWindow(widget);
    QRect   geo    = widget->parentWidget()->geometry();
    widget->parentWidget()->setGeometry(geo.x(), geo.y(), 450, 300);

}
void MainWindow::addtablecanvas(TableCanvas *widget)
{
    ui->mdiArea->addSubWindow(widget);
    QRect   geo    = widget->parentWidget()->geometry();
    widget->parentWidget()->setGeometry(geo.x(), geo.y(), 450, 300);

}
TableCanvas *MainWindow::activetablecanvas()
{
    if (QMdiSubWindow * activeSubWindow = ui->mdiArea->activeSubWindow()) {
        return(qobject_cast<TableCanvas *>(activeSubWindow->widget()));
    }
    return nullptr;
}
StatisticsWindow *MainWindow::activestatisticswindow()
{
    if (QMdiSubWindow * activeSubWindow = ui->mdiArea->activeSubWindow()) {
        return(qobject_cast<StatisticsWindow *>(activeSubWindow->widget()));
    }
    return nullptr;

}
void MainWindow::WindowsActions()
{

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setShortcut(tr("Ctrl+F4"));
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()),
            ui->mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));

    connect(closeAllAct, SIGNAL(triggered()),
            ui->mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            ui->mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            ui->mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);
}
void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window) {
        return;
    }
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
    window->showNormal();
}
void MainWindow::setActiveSubWindowO(QObject *window)
{
    if (!window) {
        return;
    }
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
   // window->showNormal();
}


void MainWindow::databasedialog()
{
    if(!opendatabasesdialog){

        opendatabasesdialog=new Databases(this);
        connect(opendatabasesdialog,SIGNAL(sendselecteddatabase(QString,QString,int)),this,SLOT(getdatabasechanged(QString,QString,int)));
        connect(opendatabasesdialog,SIGNAL(senddatabaseinfo(QList<QList<QVariant> >,int)),this,SLOT(getdatabaseinfo(QList<QList<QVariant> >,int)));
        opendatabasesdialog->opendatabases=OpenDatabases;
        opendatabasesdialog->setWindowFlags(Qt::Window);
        if(!(hdpiwindows >.99 && hdpiwindows <1.01)){
            QRect size=opendatabasesdialog->geometry();
            int x=size.width();
            int y=size.height();
            size.setWidth((int)(hdpiwindows*x));
            size.setHeight((int)(hdpiwindows*y));
            opendatabasesdialog->setGeometry(size);
        }
    }
    opendatabasesdialog->show();
    opendatabasesdialog->raise();
    opendatabasesdialog->activateWindow();

}
void MainWindow::scaledock(){
    int x=ui->dockWidget->minimumWidth();
    int y=ui->dockWidget->minimumHeight();
     x=(int)(hdpiwindows*x);
    y=(int)(hdpiwindows*y);
     ui->dockWidget->setMinimumWidth(x);
     ui->dockWidget->setMinimumHeight(y);
}
void MainWindow::setdpifactores()
{
    Hdpifactores dialog;
    dialog.fillvalues(hdpifont,hdpithickeness,hdpiwindows,charheight,hdpi,wfontsize);
    if(!(hdpiwindows >.99 && hdpiwindows <1.01)){
    QRect size=dialog.geometry();
    int x=size.width();
    int y=size.height();
    size.setWidth((int)(hdpiwindows*x));
    size.setHeight((int)(hdpiwindows*y));
    dialog.setGeometry(size);
   }
    int result;
    result = dialog.exec();
    if(result){
       hdpifont=dialog.fontfactor;
       hdpithickeness=dialog.thickfactor;
       hdpiwindows=dialog.windowsfactor;
       charheight=dialog.charheight;
       hdpi=dialog.dpi;
       wfontsize=dialog.wfontsize;
    }
}
void MainWindow::Listdatabaseitens()
{
    if(currentindex <0){
        QMessageBox msgBox;
        msgBox.setText("No Active database connection. Please connect to a data base first.");
        msgBox.exec();
      return;
    }
    if(!listchanneltree){
        listchanneltree=new channelWindow(this);

        connect(listchanneltree,SIGNAL(DoIt(QStringList,QStringList,QStringList,QList<QVariant>,int)),
                this, SLOT(channeldialogDoIT(QStringList,QStringList,QStringList,QList<QVariant>,int)));
        connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),listchanneltree,SLOT(getdatabaseinfo(QList<QVariant>,int)));
     connect(listchanneltree,SIGNAL(logmessages(QString)),this,SLOT(LogMessage(QString)));
     connect(listchanneltree,SIGNAL(logmessagesR(QString)),this,SLOT(LogMessageR(QString)));

     listchanneltree->setWindowFlags( Qt::Window);

if(!(hdpiwindows >.99 && hdpiwindows <1.01)){
QRect size=listchanneltree->geometry();
int x=size.width();
int y=size.height();
size.setWidth((int)(hdpiwindows*x));
size.setHeight((int)(hdpiwindows*y));
listchanneltree->setGeometry(size);
}

    }
    emit senddatabaseinfo( databaseinfo.at(currentindex), currentindex );
    listchanneltree->show();
    listchanneltree->raise();
    listchanneltree->activateWindow();
}
void MainWindow::getdatabaseinfo( QList <QList<QVariant> > dinfo, int index)
{
    databaseinfo=dinfo;
    currentconnection=dinfo.at(index).at(0).toString();
    currentdatabasename=dinfo.at(index).at(1).toString();
    emit sendcurrentdatabase(currentconnection);
    this->setWindowTitle(currentdatabasename);
    emit senddatabaseinfo(dinfo.at(index),index);
    currentindex=index;
    itime=databaseinfo.at(index).at(2).toLongLong();
    ltime=databaseinfo.at(index).at(3).toLongLong();
    if(!itime){
        ltime=QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
        itime=QDateTime::currentDateTimeUtc().toSecsSinceEpoch()-3600;
    }
    Itime=Itime.fromSecsSinceEpoch(static_cast<unsigned int>(itime)).toUTC();
    Ltime=Ltime.fromSecsSinceEpoch(static_cast<unsigned int>(ltime)).toUTC();

    if(!databaseinfo.at(index).at(10).toBool()){
        superuser=false;
    }else{
        superuser=true;

    }

     plotsfilltree();
    plotsfilltreeInstr();

}
void MainWindow::getdatabasechanged(QString con, QString dbase, int index){
    databaseinfo[index][0]=con;
    databaseinfo[index][1]=dbase;

    currentconnection=databaseinfo.at(index).at(0).toString();
    currentdatabasename=databaseinfo.at(index).at(1).toString();
    emit sendcurrentdatabase(currentconnection);
    this->setWindowTitle(currentdatabasename);
    emit senddatabaseinfo(databaseinfo.at(index),index);
    currentindex=index;
    itime=databaseinfo.at(index).at(2).toLongLong();
    ltime=databaseinfo.at(index).at(3).toLongLong();
    if(!itime){
        ltime=QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
        itime=QDateTime::currentDateTimeUtc().toSecsSinceEpoch()-3600;
    }
    Itime=Itime.fromSecsSinceEpoch(static_cast<unsigned int>(itime)).toUTC();
    Ltime=Ltime.fromSecsSinceEpoch(static_cast<unsigned int>(ltime)).toUTC();
    if(!databaseinfo.at(index).at(10).toBool()){
        superuser=false;
    }else{
        superuser=true;

    }



     plotsfilltree();
    plotsfilltreeInstr();

}
void MainWindow::channeldialogDoIT(QStringList tnames,QStringList cnames,QStringList aliasname, QList<QVariant> ctype, int type)
{

    if(!cnames.count()){
        QMessageBox::warning(this, tr("Warning"),
                             "you have no selected items \n at the right panel!");

        return;

    }

    if(type==0) {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Name of the plot"),
                                             tr("Name of the plot?"), QLineEdit::Normal,
                                             tnames[0], &ok);
        if (!(ok && !text.isEmpty())){
            return;
        }
        cursorbusy();
        plotcanvas *plotwidget=new plotcanvas;
        plotwidget->choosemode();
        plotwidget->hdpi=hdpi;

        connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),plotwidget,SLOT(getdatabaseinfo(QList<QVariant>,int)));
        connect(plotwidget,SIGNAL(addtable(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)),this, SLOT(addtablefromplot(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)));
        connect(plotwidget,SIGNAL(logmessages(QString)),this, SLOT(LogMessage(QString)));
        connect(plotwidget,SIGNAL(logmessagesR(QString)),this, SLOT(LogMessageR(QString)));
        connect(plotwidget,SIGNAL(timeinterval(long long,long long)),this, SLOT(timefromcanvas(long long,long long)));
        connect(plotwidget,SIGNAL(plots(QList<QVariant>,QString)),this,SLOT(createplot_fromEditlocal(QList<QVariant>,QString)));
        addplotcanvas(plotwidget);
        plotwidget->databaseconnection=currentconnection;
        plotwidget->databasename=databaseinfo.at(currentindex).at(7).toString();
        plotwidget->tnames=tnames;
        plotwidget->cnames=cnames;
        plotwidget->aliasname=aliasname;
        plotwidget->mindex=currentindex;
        plotwidget->timei=itime*1000;
        plotwidget->timef=ltime*1000;
        plotwidget->settimewidgets(itime*1000,ltime*1000);

        plotwidget->plotfromchannel(text);

        plotwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/plotwidget.svg")));
        plotwidget->show();
        freecursor();

    } else if(type==1){
        bool ok;
        QString text = QInputDialog::getText(this, tr("Name of the table"),
                                             tr("Name of the table?"), QLineEdit::Normal,
                                             "downloaded items", &ok);
        if (!(ok && !text.isEmpty())){
            return;
        }

        TableCanvas *windowtable=new TableCanvas;
        //windowtable->Variaveis=&Variaveis;
        //
        // windowtable->setWindowTitle(text);
        windowtable->databaseconnection=currentconnection;
        connect(windowtable,SIGNAL(logmessages(QString)),this, SLOT(LogMessage(QString)));
        connect(windowtable,SIGNAL(logmessagesR(QString)),this, SLOT(LogMessageR(QString)));
       connect(windowtable,SIGNAL(addtable(TableCanvas*,int)),this,SLOT(addactionfromtable(TableCanvas*,int)));

        connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),windowtable,SLOT(getdatabaseinfo(QList<QVariant>,int)));

        windowtable->TableTitle=text;

        ui->mdiArea->addSubWindow(windowtable);
        QRect   geo    = windowtable->parentWidget()->geometry();
        windowtable->parentWidget()->setGeometry(geo.x(), geo.y(), 450, 300);
        windowtable->Itime=Itime;
        windowtable->Ltime=Ltime;
        windowtable->tnames=tnames;
        windowtable->cnames=cnames;
        windowtable->aliasname=aliasname;
        windowtable->ctypes=ctype;
        cursorbusy();

        freecursor();
        windowtable->TableType="Downloaded:time_versus_axisdata";
        windowtable->filltable();

        //    windowtable->tablecurves=true;
        windowtable->settimetitle();
        windowtable->show();


    }else if(type==3){


        if(downloadingintofile){
            QMessageBox::warning(this, tr("MDI"),
                                 "you are already dowloading a file\n Please wait until it finishes before starting a new download");
            return;
        }
        SaveTable dialog;
        int result;
        dialog.suggestname="download_data";
        result = dialog.exec();
        if(result){
            downloadingintofile=true;
            Dthread.tnames=tnames;
            Dthread.cnames=cnames;
            Dthread.ctypes=ctype;
            Dthread.aliasname=aliasname;
            Dthread.filename=dialog.fileName;
            Dthread.timestring=dialog.timeString;
            Dthread.separator=dialog.separator;
            Dthread.itime=static_cast<long long>(Itime.toSecsSinceEpoch());
            Dthread.ltime=static_cast<long long>(Ltime.toSecsSinceEpoch());
            Dthread.user=databaseinfo.at(currentindex).at(8).toString();
            Dthread.database=databaseinfo.at(currentindex).at(7).toString();
            Dthread.server=databaseinfo.at(currentindex).at(6).toString();
            Dthread.password=databaseinfo.at(currentindex).at(9).toString();
            Dthread.localport=databaseinfo.at(currentindex).at(11).toInt();
            Dthread.port=databaseinfo.at(currentindex).at(12).toInt();

            Dthread.start();




        }



    }else if(type==4 || type==-4){

        bool ok;
        QString text = QInputDialog::getText(this, tr("Name of the plot"),
                                             tr("Name of the plot?"), QLineEdit::Normal,
                                             tnames.at(0), &ok);
        if (!(ok && !text.isEmpty())){
            return;
        }
        cursorbusy();
        plotcanvas *plotwidget=new plotcanvas;
        plotwidget->choosemode();
        plotwidget->hdpi=hdpi;

        connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),plotwidget,SLOT(getdatabaseinfo(QList<QVariant>,int)));
        connect(plotwidget,SIGNAL(addtable(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)),this,SLOT(addtablefromplot(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)));
        connect(plotwidget,SIGNAL(logmessages(QString)),this, SLOT(LogMessage(QString)));
        connect(plotwidget,SIGNAL(logmessagesR(QString)),this, SLOT(LogMessageR(QString)));
        connect(plotwidget,SIGNAL(timeinterval(long long,long long)),this, SLOT(timefromcanvas(long long,long long)));
        connect(plotwidget,SIGNAL(plots(QList<QVariant>,QString)),this,SLOT(createplot_fromEditlocal(QList<QVariant>,QString)));
        addplotcanvas(plotwidget);
        plotwidget->databaseconnection=currentconnection;
        plotwidget->databasename=databaseinfo.at(currentindex).at(7).toString();
        plotwidget->tnames=tnames;
        plotwidget->cnames=cnames;
        plotwidget->diameters=tnames.at(1);
        plotwidget->mindex=currentindex;
        plotwidget->timei=itime*1000;
        plotwidget->timef=ltime*1000;
        plotwidget->settimewidgets(itime*1000,ltime*1000);

        plotwidget->plotSfromchannel(text);

        plotwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/plotwidget.svg")));
        plotwidget->show();
        freecursor();


    }else if(type==5){

        bool ok;
        QString text = QInputDialog::getText(this, tr("Name of the plot"),
                                             tr("Name of the plot?"), QLineEdit::Normal,
                                             tnames[0], &ok);
        if (!(ok && !text.isEmpty())){
            return;
        }
        cursorbusy();
        plotcanvas *plotwidget=new plotcanvas;
        plotwidget->choosemode();
        plotwidget->hdpi=hdpi;

        connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),plotwidget,SLOT(getdatabaseinfo(QList<QVariant>,int)));
        connect(plotwidget,SIGNAL(addtable(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)),this,SLOT(addtablefromplot(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)));
        connect(plotwidget,SIGNAL(logmessages(QString)),this, SLOT(LogMessage(QString)));
        connect(plotwidget,SIGNAL(logmessagesR(QString)),this, SLOT(LogMessageR(QString)));
        connect(plotwidget,SIGNAL(timeinterval(long long,long long)),this, SLOT(timefromcanvas(long long,long long)));
        connect(plotwidget,SIGNAL(plots(QList<QVariant>,QString)),this,SLOT(createplot_fromEditlocal(QList<QVariant>,QString)));

        addplotcanvas(plotwidget);
        plotwidget->databaseconnection=currentconnection;
        plotwidget->databasename=databaseinfo.at(currentindex).at(7).toString();
        plotwidget->tnames=tnames;
        plotwidget->Bins=tnames.at(1);
        plotwidget->cnames=cnames;
        plotwidget->mindex=currentindex;
        plotwidget->timei=itime*1000;
        plotwidget->timef=ltime*1000;
        plotwidget->settimewidgets(itime*1000,ltime*1000);

        plotwidget->plotHfromchannel(text);


        plotwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/plotwidget.svg")));
        plotwidget->show();
        freecursor();

    }
}
void MainWindow::cursorbusy()
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
}
void MainWindow::freecursor()
{
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

}
void MainWindow::closeEvent(QCloseEvent *event)
{
    QList<QMdiSubWindow *> list = ui->mdiArea->subWindowList();


    QMessageBox msgBox;


    if(downloadingintofile){
        msgBox.setText("Downloading itens into a file is still active");
        msgBox.setInformativeText("Download will be crashed if you press yes");
        msgBox.setIcon(QMessageBox::Warning);
    }else{
        msgBox.setText("This will close the Program");
        msgBox.setInformativeText("are you sure?");
        msgBox.setIcon(QMessageBox::Question);
    }

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret= msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:


        for (int i = 0; i < list.size(); i++) {
            if (!list[i]->close()) {
                LogMessageR("the windown did not close");

            }
        }
        event->accept();
        break;
    case QMessageBox::Cancel:

        event->ignore();
        break;

    default:
        // should never be reached
        break;
    }


}
void MainWindow::LogMessage(QString mess)
{
    ui->Messages->append(mess);
}
void MainWindow::LogMessageR(QString text)
{
    ui->Messages->setTextColor("red");
    ui->Messages->append(text);
    ui->Messages->setTextColor("black");

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
    table.connectionName=currentconnection;
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
    //Banana local
 {
    struct SurfacePlots{
     QString plotname;
     QString columnsandtables;
     QString proprieties;
     QString surface;
    };
    QList <SurfacePlots> plots;
    plots.clear();
    QSettings settings("CampaignAnalysis","surfaceplotsdef");
    int size = settings.beginReadArray("bananaplots");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        SurfacePlots plot;
        plot.plotname=settings.value("plotname").toString();
        plot.columnsandtables=settings.value("columnsandtables").toString();
        plot.proprieties=settings.value("proprieties").toString();
        plot.surface=settings.value("surface").toString();
        plots.append(plot);
   }
    settings.endArray();


        for (int i = 0; i < plots.count(); ++i) {

            QStandardItem *item = new QStandardItem(plots.at(i).plotname);
            grafid.clear();
            item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/banana4.svg")));
            grafid <<-12 <<plots.at(i).plotname << plots.at(i).columnsandtables ;
            QStringList lixo;
            lixo=plots.at(i).columnsandtables.split(";");
            item->setData(grafid);
            item->setEditable(false);
            QBrush blue;
            blue.setColor(Qt::blue);
            item->setForeground(blue);
            Bananaitem->appendRow(item);

            grafid.clear();

        }

    }

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
{ //local
        struct typlots{
         QString plotname;
         QString columnsandtables;
         QString proprieties;
         QString curves;
        };
    QList <typlots> plots;
    plots.clear();
    QSettings settings("CampaignAnalysis","typlotsdef");
    int size = settings.beginReadArray("typlots");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        typlots plot;
        plot.plotname=settings.value("plotname").toString();
        plot.columnsandtables=settings.value("columnsandtables").toString();
        plot.proprieties=settings.value("proprieties").toString();
        plot.curves=settings.value("curves").toString();
        plots.append(plot);
   }
    settings.endArray();


        for (int i = 0; i < plots.count(); ++i) {

            QStandardItem *item = new QStandardItem(plots.at(i).plotname);
            grafid.clear();
            item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/educational15.svg")));
            grafid <<-11 <<plots.at(i).plotname << plots.at(i).columnsandtables ;
            QStringList lixo;
            lixo=plots.at(i).columnsandtables.split(";");
            item->setData(grafid);
            item->setEditable(false);
            QBrush blue;
            blue.setColor(Qt::blue);
            item->setForeground(blue);
            Graphitem->appendRow(item);
            grafid.clear();

        }

}
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

    { //local
            struct HistoPlots{
             QString plotname;
             QString columnsandtables;
             QString proprieties;
             QString histos;
            };
        QList <HistoPlots> plots;
        plots.clear();
        QSettings settings("CampaignAnalysis","histoplotsdef");
        int size = settings.beginReadArray("histoplots");
        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            HistoPlots plot;
            plot.plotname=settings.value("plotname").toString();
            plot.columnsandtables=settings.value("columnsandtables").toString();
            plot.proprieties=settings.value("proprieties").toString();
            plot.histos=settings.value("histos").toString();
            plots.append(plot);
       }
        settings.endArray();


            for (int i = 0; i < plots.count(); ++i) {

                QStandardItem *item = new QStandardItem(plots.at(i).plotname);
                grafid.clear();
                item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/chart15.svg")));
                grafid <<-13 <<plots.at(i).plotname << plots.at(i).columnsandtables ;
                QStringList lixo;
                lixo=plots.at(i).columnsandtables.split(";");
                item->setData(grafid);
                item->setEditable(false);
                QBrush blue;
                blue.setColor(Qt::blue);
                item->setForeground(blue);
                Histoitem->appendRow(item);
                grafid.clear();

            }

    }
// screens

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
//screen local
{         struct ScreenPlots{
            QString screenname;
            QString plottype;
            QString plotorder;
            QString columnsandtables;
            QString proprieties;
            QString surface;
            QString curves;
            QString histos;
           };

        QList <ScreenPlots> plots;
        plots.clear();
        QSettings settings("CampaignAnalysis","screendef");
        int size = settings.beginReadArray("screenplots");

        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            ScreenPlots plot;
            plot.screenname=settings.value("screenname").toString();
            plot.plottype=settings.value("plottype").toString();
            plot.plotorder=settings.value("plotorder").toString();
            plot.columnsandtables=settings.value("columnsandtables").toString();
            plot.proprieties=settings.value("proprieties").toString();
            plot.surface=settings.value("columnsandtables").toString();
            plot.curves=settings.value("surface").toString();
            plot.histos=settings.value("histos").toString();

            plots.append(plot);
       }
        settings.endArray();
        QString NamePlot;
        QStringList scrennames;
        scrennames.clear();
        NamePlot="NAMEEXDRUXULOQUE NUNCADEVERÀEXISTIR";
        for (int i = 0; i < plots.count(); ++i) {
           if(plots.at(i).screenname!=NamePlot) scrennames <<  plots.at(i).screenname;
           NamePlot=plots.at(i).screenname;
        }
        for (int i = 0; i < scrennames.count(); ++i) {
        QStandardItem *item = new QStandardItem(scrennames.at(i));
        grafid.clear();
        item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/screens.svg")));
        grafid <<-110 <<scrennames.at(i) ;
        item->setData(grafid);
        item->setEditable(false);
        QBrush blue;
        blue.setColor(Qt::blue);
        item->setForeground(blue);

        Screens->appendRow(item);

        grafid.clear();
        }

}
    ui->PlotsView->setModel(&listplotmodel);
    ui->PlotsView->show();

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
    table.connectionName=currentconnection;
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

void MainWindow::createplots(QModelIndex index)
{
    QList<QVariant> list;
    list = listplotmodel.itemFromIndex(index)->data().toList();
    if (list[0].toInt() == -1) {
        return;
    }
    createplot_fromlist(list);

}
void MainWindow::createplotsI(QModelIndex index)
{
    QList<QVariant> list;
    list = listplotmodelInst.itemFromIndex(index)->data().toList();
    if (list[0].toInt() == -1) {
        return;
    }
    createplot_fromlist(list);

}
void MainWindow::createplot_fromlist(QList<QVariant> list)
{

    cursorbusy();

        plotcanvas *plotwidget=new plotcanvas;
        plotwidget->choosemode();
        plotwidget->hdpi=hdpi;
        if(list.count()>3) plotwidget->startinlinkedtimemode();
        connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),plotwidget,SLOT(getdatabaseinfo(QList<QVariant>,int)));
        connect(plotwidget,SIGNAL(addtable(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)),this,SLOT(addtablefromplot(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)));
        connect(plotwidget,SIGNAL(logmessages(QString)),this, SLOT(LogMessage(QString)));
        connect(plotwidget,SIGNAL(logmessagesR(QString)),this, SLOT(LogMessageR(QString)));
        connect(plotwidget,SIGNAL(timeinterval(long long,long long)),this, SLOT(timefromcanvas(long long,long long)));
        connect(plotwidget,SIGNAL(plots(QList<QVariant>,QString)),this,SLOT(createplot_fromEditlocal(QList<QVariant>,QString)));

        addplotcanvas(plotwidget);
        plotwidget->databaseconnection=currentconnection;
        plotwidget->databasename=databaseinfo.at(currentindex).at(7).toString();
        plotwidget->mindex=currentindex;
        plotwidget->timei=itime*1000;
        plotwidget->timef=ltime*1000;
        plotwidget->settimewidgets(itime*1000,ltime*1000);
        plotwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/plotwidget.svg")));
        if(list.at(0).toInt()==1 || list.at(0).toInt()==-11) {
            QStringList coltable,tnames,cnames;
            coltable=list.at(2).toString().split(";");
            cnames=coltable.at(0).split(",");
            tnames=coltable.at(1).split(",");
            plotwidget->cnames=cnames;
            plotwidget->tnames=tnames;
             if(list.at(0).toInt()==1)plotwidget->plottyfromlist(list.at(1).toString());
             else plotwidget->plottyfromlistl(list.at(1).toString());
        }else if(list.at(0).toInt()==2 || list.at(0).toInt()==-12 ) {
            QStringList coltable,cnames;
            coltable=list.at(2).toString().split(";");
            cnames=coltable.at(1).split(",");
            plotwidget->cnames=cnames;
            plotwidget->tnames <<coltable.at(0);

            if(list.at(0).toInt()==2)plotwidget->plotSfromlist(list.at(1).toString());
            else plotwidget->plotSfromlistl(list.at(1).toString());
        }else if(list.at(0).toInt()==3 || list.at(0).toInt()==-13) {
            QStringList coltable,cnames;
            coltable=list.at(2).toString().split(";");
            cnames=coltable.at(1).split(",");
            plotwidget->cnames=cnames;
            plotwidget->tnames <<coltable.at(0);

            if(list.at(0).toInt()==3)plotwidget->plotHfromlist(list.at(1).toString());
            else plotwidget->plotHfromlistl(list.at(1).toString());
        }else if(list.at(0).toInt()==10 || list.at(0).toInt()==-110 ) {
             if(list.at(0).toInt()==10) plotwidget->plotAfromlist(list.at(1).toString());
             else plotwidget->plotAfromlistl(list.at(1).toString());
}
        plotwidget->show();
        plotwidget->fitintowidget();
        freecursor();


}
void MainWindow::createplot_fromEditlocal(QList<QVariant> list,QString name)
{

    cursorbusy();

        plotcanvas *plotwidget=new plotcanvas;
        plotwidget->choosemode();
        plotwidget->hdpi=hdpi;

        connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),plotwidget,SLOT(getdatabaseinfo(QList<QVariant>,int)));
        connect(plotwidget,SIGNAL(addtable(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)),this,SLOT(addtablefromplot(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)));
        connect(plotwidget,SIGNAL(logmessages(QString)),this, SLOT(LogMessage(QString)));
        connect(plotwidget,SIGNAL(logmessagesR(QString)),this, SLOT(LogMessageR(QString)));
        connect(plotwidget,SIGNAL(timeinterval(long long,long long)),this, SLOT(timefromcanvas(long long,long long)));
        connect(plotwidget,SIGNAL(plots(QList<QVariant>,QString)),this,SLOT(createplot_fromEditlocal(QList<QVariant>,QString)));

        addplotcanvas(plotwidget);
        plotwidget->databaseconnection=currentconnection;
        plotwidget->databasename=databaseinfo.at(currentindex).at(7).toString();
        plotwidget->mindex=currentindex;
        plotwidget->timei=itime*1000;
        plotwidget->timef=ltime*1000;
        plotwidget->settimewidgets(itime*1000,ltime*1000);
        plotwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/plotwidget.svg")));
        if(list.at(0).toInt()==1) {
            QStringList coltable,tnames,cnames;
            coltable=list.at(2).toString().split(";");
            cnames=coltable.at(0).split(",");
            tnames=coltable.at(1).split(",");
            plotwidget->cnames=cnames;
            plotwidget->tnames=tnames;
            plotwidget->plottyfromfulllist(list);

        }else if(list.at(0).toInt()==2  ) {
            QStringList coltable,cnames;
            coltable=list.at(2).toString().split(";");
            cnames=coltable.at(1).split(",");
            plotwidget->cnames=cnames;
            plotwidget->tnames <<coltable.at(0);
            plotwidget->plotSfromfulllist(list);
        }else if(list.at(0).toInt()==3 ) {
            QStringList coltable,cnames;
            coltable=list.at(2).toString().split(";");
            cnames=coltable.at(1).split(",");
            plotwidget->cnames=cnames;
            plotwidget->tnames <<coltable.at(0);
            plotwidget->plotHfromfulllist(list);
        }else if(list.at(0).toInt()==10) {
             plotwidget->plotAfromfulllist(list);

}
        plotwidget->show();
        plotwidget->fitintowidget();
        freecursor();


}
void MainWindow::addtablefromplot(QVector<plotproprieties *> *p,QVector<plotdata *>*data,QList<int> subpag, QList<int> element)
{

    TableCanvas *windowtable=new TableCanvas;


    // windowtable->setWindowTitle(text);
    connect(windowtable,SIGNAL(logmessages(QString)),this, SLOT(LogMessage(QString)));
    connect(windowtable,SIGNAL(logmessagesR(QString)),this, SLOT(LogMessageR(QString)));
    connect(windowtable,SIGNAL(addtable(TableCanvas*,int)),this,SLOT(addplotfromtable(TableCanvas*,int)));
    connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),windowtable,SLOT(getdatabaseinfo(QList<QVariant>,int)));
    connect(windowtable,SIGNAL(addtable(TableCanvas*,int)),this,SLOT(addactionfromtable(TableCanvas*,int)));

    windowtable->TableTitle=p->at(0)->Plotname;
    windowtable->setWindowTitle( p->at(0)->Plotname);
    windowtable->TableType="FromPlot";
    ui->mdiArea->addSubWindow(windowtable);
    QRect   geo    = windowtable->parentWidget()->geometry();
    windowtable->parentWidget()->setGeometry(geo.x(), geo.y(), 450, 300);
    windowtable->p=p;
    windowtable->data=data;

    windowtable->filltablefromplot(subpag,element);

    windowtable->show();

}
void MainWindow::lateralwidgetsactions(int i)
{
    if(i==ui->LateraltabWidget->indexOf(ui->tabplots)) plotsfilltree();
    else if(i==ui->LateraltabWidget->indexOf(ui->plotsbyinstruments)) {} plotsfilltreeInstr();
}
void MainWindow::helpondefiningscreens()
{

    if(!screendialog){
        screendialog=new DefiningScreens(this);
 screendialog->setWindowFlags(Qt::Window);
 screendialog->databaseconnection=currentconnection;
  connect(screendialog, SIGNAL(logmessages(QString)), this,SLOT(LogMessage(QString)));
 connect(screendialog, SIGNAL(logmessagesR(QString)), this,SLOT(LogMessageR(QString)));
 connect(screendialog, SIGNAL(dplot(QList<QVariant>)), this,SLOT(plotall(QList<QVariant>)));
 connect(screendialog, SIGNAL(doreport(QList<QList<QVariant >>)), this,SLOT(doreport(QList <QList<QVariant> >)));
 screendialog->fill();
  if(!(hdpiwindows >.99 && hdpiwindows <1.01)){
 QRect size=screendialog->geometry();
 int x=size.width();
 int y=size.height();
 size.setWidth((int)(hdpiwindows*x));
 size.setHeight((int)(hdpiwindows*y));
 screendialog->setGeometry(size);
}
    }
    screendialog->show();
    screendialog->raise();
    screendialog->activateWindow();
}
void MainWindow::plotall( QList<QVariant> list){
     createplot_fromlist(list);
}
void MainWindow::doreport(QList <QList<QVariant> > slist)
{
 cursorbusy();
 MakeText *textwidget=new MakeText;
 addreportcanvas(textwidget);
 textwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/report.svg")));
 textwidget->remote=true;
 textwidget->reportmode();
 textwidget->databaseconnection=currentconnection;
 textwidget->setWindowTitle("Shift Report");
    QList<QVariant> list;
    for(int i=0; i<slist.count(); i++){
        plotcanvas *plotwidget=new plotcanvas;

        list=slist.at(i);
    plotwidget->databaseconnection=currentconnection;
    plotwidget->databasename=databaseinfo.at(currentindex).at(7).toString();
    plotwidget->mindex=currentindex;
    plotwidget->timei=itime*1000;
    plotwidget->timef=ltime*1000;
    plotwidget->factorreport=1.3;
    plotwidget->xlenreport=900;
    plotwidget->ylenreport=400;
    plotwidget->forplotreport=true;


    plotwidget->settimewidgets(itime*1000,ltime*1000);
    //plotwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/plotwidget.svg")));
    if(list.at(0).toInt()==1 || list.at(0).toInt()==-11) {
        QStringList coltable,tnames,cnames;
        coltable=list.at(2).toString().split(";");
        cnames=coltable.at(0).split(",");
        tnames=coltable.at(1).split(",");
        plotwidget->cnames=cnames;
        plotwidget->tnames=tnames;
         if(list.at(0).toInt()==1)plotwidget->plottyfromlist(list.at(1).toString());
       //  else plotwidget->plottyfromlistl(list.at(1).toString());
    }else if(list.at(0).toInt()==2 || list.at(0).toInt()==-12 ) {
        QStringList coltable,cnames;
        coltable=list.at(2).toString().split(";");
        cnames=coltable.at(1).split(",");
        plotwidget->cnames=cnames;
        plotwidget->tnames <<coltable.at(0);

        if(list.at(0).toInt()==2)plotwidget->plotSfromlist(list.at(1).toString());
       // else plotwidget->plotSfromlistl(list.at(1).toString());
    }else if(list.at(0).toInt()==3 || list.at(0).toInt()==-13) {
        QStringList coltable,cnames;
        coltable=list.at(2).toString().split(";");
        cnames=coltable.at(1).split(",");
        plotwidget->cnames=cnames;
        plotwidget->tnames <<coltable.at(0);

        if(list.at(0).toInt()==3)plotwidget->plotHfromlist(list.at(1).toString());
        //else plotwidget->plotHfromlistl(list.at(1).toString());
    }
//    plotwidget->show();
//    plotwidget->fitintowidget();

/*
    plot.Plot_page(xlen,ylen,  "pngqt", "doreporttrash.png", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();
    plot.setfont(p.at(0)->fontset);
    plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);

    for(int i=0;i<p.count();i++)
    {
        if(p.at(i)->plottype==0)typlot(i,data.at(0)->timei, data.at(0)->timef);
        if(p.at(i)->plottype==1)Splot(i,data.at(0)->timei, data.at(0)->timef);
        if(p.at(i)->plottype==2)Hplot(i);



    }

    plot.deleteStream();
 */
    delete plotwidget;
    textwidget->insertImageFile("doreporttrash.png");
}

    textwidget->show();
    freecursor();
}



void MainWindow::choosetimeinterval()
{
    if(currentindex <0){
        QMessageBox msgBox;
        msgBox.setText("No Active database connection. Please connect to a data base first.");
        msgBox.exec();
        return;
    }

    if(!managingtimedialog){
        managingtimedialog=new ManagingTime(this);
        managingtimedialog->databaseinfo=databaseinfo.at(currentindex);
        managingtimedialog->databaseindex=currentindex;
        connect(managingtimedialog,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),this,SLOT(gettimefromtimedialog(QList<QVariant>,int)));
        connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),managingtimedialog, SLOT(getdatabaseinfo(QList<QVariant>,int)));
        connect(managingtimedialog,SIGNAL(logmessages(QString)),this,SLOT(LogMessage(QString)));
        connect(managingtimedialog,SIGNAL(logmessagesR(QString)),this,SLOT(LogMessageR(QString)));
        managingtimedialog->setTimeWidgets(QDate::currentDate(),QTime::currentTime().addSecs(-3600),QDate::currentDate(),QTime::currentTime());
        managingtimedialog->setWindowFlags(Qt::Window);
        if(!(hdpiwindows >.99 && hdpiwindows <1.01)){
            QRect size=managingtimedialog->geometry();
            int x=size.width();
            int y=size.height();
            size.setWidth((int)(hdpiwindows*x));
            size.setHeight((int)(hdpiwindows*y));
            managingtimedialog->setGeometry(size);
        }
    }

    managingtimedialog->show();
    managingtimedialog->raise();
    managingtimedialog->activateWindow();
}
void MainWindow::gettimefromtimedialog(QList<QVariant> dinfo, int index)
{
    databaseinfo[index]=dinfo;

    itime=databaseinfo.at(index).at(2).toLongLong();
    ltime=databaseinfo.at(index).at(3).toLongLong();
    Itime=Itime.fromSecsSinceEpoch(itime).toUTC();
    Ltime=Ltime.fromSecsSinceEpoch(ltime).toUTC();

    emit senddatabaseinfo(dinfo,index);
}
void MainWindow::timefromcanvas(long long timei,long long timef)
{
    itime=timei/1000;
    ltime=timef/1000;
    Itime=Itime.fromMSecsSinceEpoch(itime).toUTC();
    Ltime=Ltime.fromMSecsSinceEpoch(ltime).toUTC();
    databaseinfo[currentindex][2]= timei/1000;
    databaseinfo[currentindex][3]= timef/1000;
    databaseinfo[currentindex][4]= "from a plotcanvas";
    databaseinfo[currentindex][5]= "from a plotcanvas";

    emit senddatabaseinfo(databaseinfo.at(currentindex),currentindex);
  //  LogMessageR(Itime.toString("yyyy/MM/dd hh:mm:ss")+" "+Ltime.toString("yyyy/MM/dd hh:mm:ss"));
}
void MainWindow::Enddownloadthread(){
    downloadingintofile=false;
}
void MainWindow::viewelog()
{


    TextView *textwidget=new TextView;
    addTextcanvas(textwidget);
    textwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/log1.svg")));
    textwidget->databaseconnection=currentconnection;
    textwidget->superuser=superuser;
    textwidget->instrument=false;
    textwidget->filltree();
    textwidget->show();


}
void MainWindow::viewreport()
{


    MakeText *textwidget=new MakeText;
    addreportcanvas(textwidget);
    textwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/report.svg")));
    textwidget->remote=true;
    textwidget->reportmode();
    textwidget->setWindowTitle("Shift Report");
    textwidget->databaseconnection=currentconnection;
    textwidget->show();


}
void MainWindow::viewdatatdesc()
{

    TextView *textwidget=new TextView;
    addTextcanvas(textwidget);
    textwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/applications-development-4.svg")));
    textwidget->databaseconnection=currentconnection;
    textwidget->superuser=superuser;
    textwidget->instrument=true;
    textwidget->Tableentry="InstDescription";
    textwidget->filltree();
    textwidget->show();
}
void MainWindow::addTextcanvas(TextView *widget)

{

    ui->mdiArea->addSubWindow(widget);
    QRect   geo    = widget->parentWidget()->geometry();
    widget->parentWidget()->setGeometry(geo.x(), geo.y(), 450, 300);

}void MainWindow::addreportcanvas(MakeText *widget)

{

    ui->mdiArea->addSubWindow(widget);
    QRect   geo    = widget->parentWidget()->geometry();
    widget->parentWidget()->setGeometry(geo.x(), geo.y(), 450, 300);

}
MakeText *MainWindow::activeMakeText()
{
    if (QMdiSubWindow * activeSubWindow = ui->mdiArea->activeSubWindow()) {
        return(qobject_cast<MakeText *>(activeSubWindow->widget()));
    }
    return nullptr;
}

TextView *MainWindow::activeTextView()
{
    if (QMdiSubWindow * activeSubWindow = ui->mdiArea->activeSubWindow()) {
        return(qobject_cast<TextView *>(activeSubWindow->widget()));
    }
    return nullptr;
}
void MainWindow::editlocalplotsdialog()
{
    if(!Editlocalplotsdialog){
        Editlocalplotsdialog=new Editlocalplots(this);
 Editlocalplotsdialog->setWindowFlags(Qt::Window);
 Editlocalplotsdialog->fillplots();
 Editlocalplotsdialog->plotsfilltree();
 connect(Editlocalplotsdialog,SIGNAL(plots(QList<QVariant>,QString)),this,SLOT(createplot_fromEditlocal(QList<QVariant>,QString)));
 if(!(hdpiwindows >.99 && hdpiwindows <1.01)){
 QRect size=Editlocalplotsdialog->geometry();
 int x=size.width();
 int y=size.height();
 size.setWidth((int)(hdpiwindows*x));
 size.setHeight((int)(hdpiwindows*y));
 Editlocalplotsdialog->setGeometry(size);
}
    }
    Editlocalplotsdialog->show();
    Editlocalplotsdialog->raise();
    Editlocalplotsdialog->activateWindow();

}
void MainWindow::Inststatusdialog()
{
    if(!inststatusdialog){
        inststatusdialog=new InstStatus(this);
         inststatusdialog->setWindowFlags( Qt::Window);
         inststatusdialog->connectionName=currentconnection;
      }
    inststatusdialog->check();
    inststatusdialog->show();
    inststatusdialog->raise();
    inststatusdialog->activateWindow();

}
void MainWindow::addactionfromtable(TableCanvas *table, int type)
{


    switch (type) {

    case 1:
 {
       cursorbusy();
        plotcanvas *plotwidget=new plotcanvas;
        plotwidget->hdpi=hdpi;
        plotwidget->fromtable=true;
        connect(plotwidget,&plotcanvas::logmessages,this,&MainWindow::LogMessage);
        connect(plotwidget,SIGNAL(addtable(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)),this,SLOT(addtablefromplot(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)));
        connect(plotwidget,&plotcanvas::logmessages,this, &MainWindow::LogMessage);
        connect(plotwidget,&plotcanvas::logmessagesR,this, &MainWindow::LogMessageR);
        addplotcanvas(plotwidget);
        long long iitime,lltime;

       iitime = static_cast<long long>(table->xymodel.m_datatime.at(0)->at(0));
       lltime=static_cast<long long>(table->xymodel.m_datatime.at(0)->at(static_cast<unsigned int>(table->xymodel.rowCount())-1));
       QString diameters;
       std::vector<long long> time;
       QVector<std::vector<double> > c;
       std::vector<double> y;
       time.clear();

    if (table->xymodel.rowCount() == 0) {


    }
for (int i = 0; i < table->xymodel.columnCount()-1; i++) {
      c<<y;
      c[i].clear();
      diameters.append(table->xymodel.headernames.at(i+1));
      if(i<table->xymodel.columnCount()-2 ) diameters.append(",");
 }

 //   plotwidget->Plot.plot[0].Data.xyz[0].setsize(0,table->xymodel.rowCount());
 //   plotwidget->Plot.plot[0].Data.xyz[0].setsize(1,table->xymodel.columnCount()-1);
 //   plotwidget->Plot.plot[0].Data.xyz[0].setsizeZ();

    for(int i=0;i<table->xymodel.rowCount(); i++){
              time.push_back(table->xymodel.m_datatime.at(0)->at(static_cast<unsigned int>(i)));
      for (int kk = 0; kk < table->xymodel.columnCount()-1; kk++) {

              c[kk].push_back(table->xymodel.m_datareal.at(kk)->at(static_cast<unsigned int>(i)));
      }

    }


 //   plotwidget->Plot.plot[0].Data.xyz[0].surfacetype=  plotwidget->Plot.plot[0].Data.xyz[0].surfacetype |CloudDax_enums::logy;
 //   plotwidget->Plot.plot[0].Data.xyz[0].surfacetype = plotwidget->Plot.plot[0].Data.xyz[0].surfacetype |CloudDax_enums::logz;
 //   plotwidget->Plot.plot[0].Data.xyz[0].surfacetype = plotwidget->Plot.plot[0].Data.xyz[0].surfacetype |CloudDax_enums::tiled;
 //   plotwidget->Plot.plot[0].Data.xyz[0].surfacetype = plotwidget->Plot.plot[0].Data.xyz[0].surfacetype |CloudDax_enums::Bizarreway;
 //   plotwidget->Plot.plot[0].Data.xyz[0].surfacetype = plotwidget->Plot.plot[0].Data.xyz[0].surfacetype &~CloudDax_enums::insidecolorscale;
 //   plotwidget->Plot.plot[0].Data.xyz[0].surfacetype = plotwidget->Plot.plot[0].Data.xyz[0].surfacetype |CloudDax_enums::rightcolorscale;
 //   plotwidget->Plot.plot[0].Data.plottype=CloudDax_enums::Banana;
 //   plotwidget->Plot.plot[0].Data.plotname=plotwidget->Plot.plot[0].Data.xyz[0].zname;
 //   plotwidget->windowtype="bananafromtable";
 //   plotwidget->setthings();
 //   plotwidget->Plot.dpi=hdpi;

 //   plotwidget->tempfile.open();
    plotwidget->plotfromtabledisabeactions();
    plotwidget->timei=iitime;
    plotwidget->timef=lltime;
    plotwidget->plotSfromtable(diameters,time,c);


   plotwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/plotwidget.svg")));
   plotwidget->show();
    freecursor();
    break;

 }
 case 2:
 {
        plotcanvas *plotwidget=new plotcanvas;
        plotwidget->hdpi=hdpi;
        plotwidget->fromtable=true;
        connect(plotwidget,&plotcanvas::logmessages,this,&MainWindow::LogMessage);
        connect(plotwidget,SIGNAL(addtable(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)),this,SLOT(addtablefromplot(QVector<plotproprieties*>*,QVector<plotdata*>*,QList<int>,QList<int>)));
        connect(plotwidget,&plotcanvas::logmessages,this, &MainWindow::LogMessage);
        connect(plotwidget,&plotcanvas::logmessagesR,this, &MainWindow::LogMessageR);
        addplotcanvas(plotwidget);
        QList <std::vector<double> > x,y;
        QStringList names;
        names.clear();
        x.clear();y.clear();
        std::vector<double> x1,y1;
        x1.clear();y1.clear();
        long long iitime,lltime;
        iitime=0;
        lltime=0;
        int t = 0;
        xydata dados;
        int indexoftime,indexofdata;
        int ii=-1;
        for(int i=0; i<table->columnstoplot.count(); i++){
            if(table->xymodel.ctype.at(table->columnstoplot.at(i))!=1) continue;
            ii++;
            x<<x1;y<<y1;
            indexofdata=table->xymodel.cindex.at(table->columnstoplot.at(i));
            for(int j=table->columnstoplot.at(i)-1; j>-1; j--){
                if(table->xymodel.ctype.at(j)==0){t=j; break;}
            }
            indexoftime=table->xymodel.cindex.at(t);
            names<<table->xymodel.headernames.at(table->columnstoplot.at(i));
           for(int j=0; j< static_cast<int>( table->xymodel.m_datareal.at(indexofdata)->size()); j++)
               {
               x[ii].push_back(static_cast<double>(table->xymodel.m_datatime.at(indexoftime)->at(static_cast<unsigned int>(j))));
               y[ii].push_back(static_cast<double>(table->xymodel.m_datareal.at(indexofdata)->at(static_cast<unsigned int>(j))));
            }

            if(!iitime) iitime= (long long) (table->xymodel.m_datatime.at(indexoftime)->at(0));
             else iitime=std::min(iitime,(long long)(table->xymodel.m_datatime.at(indexoftime)->at(0)) )  ;
            if(!lltime) lltime=   (long long) (table->xymodel.m_datatime.at(indexoftime)->at(table->xymodel.m_datareal.at(indexofdata)->size()-1));
             else lltime=std::max(lltime,(long long)(table->xymodel.m_datatime.at(indexoftime)->at(table->xymodel.m_datareal.at(indexofdata)->size()-1)))  ;
        }
        plotwidget->plotfromtabledisabeactions();
        plotwidget->timei=iitime;
        plotwidget->timef=lltime;
        plotwidget->plottyfromtable(names,x,y);


    plotwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/plotwidget.svg")));
    plotwidget->show();
    freecursor();
        break;

}
    case 3:
{
        GslKindofwrapper a;
        int indexoftime,indexofdata;
        StatisticsWindow *statswidget=new StatisticsWindow;
        addstatisticswindow(statswidget);

        int ii=-1;
        int t;

        for(int i=0; i<table->columnstoplot.count(); i++){
            if(table->xymodel.ctype.at(table->columnstoplot.at(i))!=1) continue;
            ii++;
            indexofdata=table->xymodel.cindex.at(table->columnstoplot.at(i));
            for(int j=table->columnstoplot.at(i)-1; j>-1; j--){
                if(table->xymodel.ctype.at(j)==0){t=j; break;}
            }
            indexoftime=table->xymodel.cindex.at(t);
            double med,sd;

            a.error.clear();


            statswidget->inserttext("------------* Some stats for ","dark");
            statswidget->inserttext(table->xymodel.headernames.at(table->columnstoplot.at(i)),"blue");
            statswidget->inserttext(" column "+QString::number(table->columnstoplot.at(i)),"dark");
            statswidget->inserttext(" *------------\n","dark");
            statswidget->inserttext("number of rows "+QString::number(table->xymodel.m_datatime.at(indexoftime)->size()),"dark");
            statswidget->inserttext(" from time "+QDateTime::fromMSecsSinceEpoch(table->xymodel.m_datatime.at(indexoftime)->at(0)).toUTC().toString("yyyy/MM/dd hh:mm:ss.zzz"),"dark");
            statswidget->inserttext(" until "+QDateTime::fromMSecsSinceEpoch(table->xymodel.m_datatime.at(indexoftime)->at(table->xymodel.m_datatime.at(indexoftime)->size()-1)).toUTC().toString("yyyy/MM/dd hh:mm:ss.zzz"),"dark");
            statswidget->inserttext("\n","dark");
            med=a.vector_mean(table->xymodel.m_datareal.at(indexofdata));
             if(a.error.count()) {statswidget->inserttext(a.error+"\n","red");} else{
            statswidget->inserttext("mean value="+QString::number(med)+"\n","dark");
            sd=a.vector_std(table->xymodel.m_datareal.at(indexofdata),med);
            statswidget->inserttext("Standard deviation="+QString::number(sd)+"\n","dark");
            size_t imin,imax;
            a.vector_maxmin_index(table->xymodel.m_datareal.at(indexofdata),&imin, &imax);
            statswidget->inserttext("minimum value="+QString::number(table->xymodel.m_datareal.at(indexofdata)->at(imin)),"dark");
            statswidget->inserttext(" at "+QDateTime::fromMSecsSinceEpoch(table->xymodel.m_datatime.at(indexoftime)->at(imin)).toUTC().toString("yyyy/MM/dd hh:mm:ss.zzz")+"\n","dark");
            statswidget->inserttext("maximum value="+QString::number(table->xymodel.m_datareal.at(indexofdata)->at(imax)),"dark");
            statswidget->inserttext(" at "+QDateTime::fromMSecsSinceEpoch(table->xymodel.m_datatime.at(indexoftime)->at(imax)).toUTC().toString("yyyy/MM/dd hh:mm:ss.zzz")+"\n","dark");
        }
    }
        statswidget->show();
        break;
}
    case 4:
{/*
        int indexoftime,indexofdata;
        int t;
        QString label;
        for(int i=0; i<table->columnstoplot.count(); i++){
            if(table->xymodel.ctype.at(table->columnstoplot.at(i))!=1) continue;

            indexofdata=table->xymodel.cindex.at(table->columnstoplot.at(i));
            for(int j=table->columnstoplot.at(i)-1; j>-1; j--){
                if(table->xymodel.ctype.at(j)==0){t=j; break;}
            }
            indexoftime=table->xymodel.cindex.at(t);
            label=table->xymodel.headernames.at(table->columnstoplot.at(i));
       break;
        }
     MeanDerivative dialog;
     int result=dialog.exec();
     if(result){
     GslKindofwrapper a;
     std::vector<double> xm,ym, xd,yd;


     int res=a.Movingpolynomialfit(table->xymodel.m_datatime.at(indexoftime),table->xymodel.m_datareal.at(indexofdata),
             &xm,&ym,
             &xd,&yd,
            dialog.order, dialog.npoints);

     cursorbusy();
     WindowCanvas *plotwidget=new WindowCanvas;
     plotwidget->hdpi=hdpi;

     connect(plotwidget,SIGNAL(logmessages(QString)),this,SLOT(LogMessage(QString)));
     connect(this,SIGNAL(senddatabaseinfo(QList<QVariant>,int)),plotwidget,SLOT(getdatabaseinfo(QList<QVariant>,int)));
     connect(plotwidget,SIGNAL(addtable(WindowCanvas *)),this, SLOT(addtablefromplot(WindowCanvas*)));
     connect(plotwidget,SIGNAL(logmessages(QString)),this, SLOT(LogMessage(QString)));
     connect(plotwidget,SIGNAL(logmessagesR(QString)),this, SLOT(LogMessageR(QString)));
     addwindowcanvas(plotwidget);
     plotwidget->windowtype="plotfromtable";
     plotwidget->Plot.plot[0].Data.plotname="Polynomial fit ";
     plotwidget->Plot.plot[0].Data.cloudid=CloudDax_enums::IDtwoaxis;
     plotwidget->Plot.plot[0].Data.plottype=CloudDax_enums::Twoaxis;

     long long iitime,lltime;
     iitime=0;
     lltime=0;
     xydata dados;

         plotwidget->Plot.plot[0].Data.xy <<dados <<dados<<dados  ;

          plotwidget->Plot.plot[0].Data.xy[0].setsize(0,table->xymodel.m_datareal.at(indexofdata)->size());
         plotwidget->Plot.plot[0].Data.xy[0].setsize(1,table->xymodel.m_datareal.at(indexofdata)->size());
        for(int j=0; j< (int) table->xymodel.m_datareal.at(indexofdata)->size(); j++)
            {
  plotwidget->Plot.plot[0].Data.xy[0].setarrayvalue(table->xymodel.m_datareal.at(indexofdata)->at(j),1,j);
  plotwidget->Plot.plot[0].Data.xy[0].setarrayvalue((double)table->xymodel.m_datatime.at(indexoftime)->at(j)/1000.,0,j);

         }
        plotwidget->Plot.plot[0].Data.xy[1].setsize(0,xm.size());
        plotwidget->Plot.plot[0].Data.xy[1].setsize(1,ym.size());
        for(int j=0; j< (int) xm.size(); j++)
            {
  plotwidget->Plot.plot[0].Data.xy[1].setarrayvalue(ym.at(j),1,j);
  plotwidget->Plot.plot[0].Data.xy[1].setarrayvalue(xd.at(j)/1000.,0,j);

         }
        plotwidget->Plot.plot[0].Data.xy[2].setsize(0,xd.size());
        plotwidget->Plot.plot[0].Data.xy[2].setsize(1,yd.size());
        for(int j=0; j< (int) xd.size(); j++)
            {
  plotwidget->Plot.plot[0].Data.xy[2].setarrayvalue(yd.at(j)*dialog.factor,1,j);
  plotwidget->Plot.plot[0].Data.xy[2].setarrayvalue(xd.at(j)/1000.,0,j);

         }

            plotwidget->Plot.plot[0].Data.derivative << false<< false<< false;
            plotwidget->Plot.plot[0].Data.devnpoints << 0<< 0<< 0;

           for(int ii=0; ii<3; ii++){
               plotwidget->Plot.plot[0].Data.xy[ii].yaxistype=CloudDax_enums::line;
           }

           plotwidget->Plot.plot[0].Data.xy[0].yaxisLabel=label;
           plotwidget->Plot.plot[0].Data.xy[1].yaxisLabel="mean value";
           plotwidget->Plot.plot[0].Data.xy[2].yaxisLabel="Derivative";


         if(!iitime) iitime= (long long) (table->xymodel.m_datatime.at(indexoftime)->at(0)/1000.);
          else iitime=std::min(iitime,(long long)(table->xymodel.m_datatime.at(indexoftime)->at(0)/1000.) )  ;
         if(!lltime) lltime=   (long long) (table->xymodel.m_datatime.at(indexoftime)->at(table->xymodel.m_datareal.at(indexofdata)->size()-1)/1000.);
          else lltime=std::max(lltime,(long long)(table->xymodel.m_datatime.at(indexoftime)->at(table->xymodel.m_datareal.at(indexofdata)->size()-1)/1000.))  ;

     plotwidget->timei=iitime;
     plotwidget->timef=lltime;
     plotwidget->Plot.plot[0].Data.itime=iitime;
     plotwidget->Plot.plot[0].Data.ltime=lltime;
     plotwidget->setthings();
     plotwidget->Plot.plot[0].Data.xy[2].yaxistype=plotwidget->Plot.plot[0].Data.xy[2].yaxistype|CloudDax_enums::rightaxis;
     plotwidget->Plot.plot[0].Data.xy[2].CurveColor="FF1313";
     plotwidget->Plot.plot[0].Data.xy[1].CurveColor="66FF19";
     plotwidget->Plot.plot[0].Data.xy[1].pwidth=3;
     plotwidget->Plot.plot[0].Data.leftaxis=1;
     plotwidget->Plot.plot[0].Data.rightaxis=1;




 plotwidget->tempfile.open();
 QRect   geo    = plotwidget->parentWidget()->geometry();
plotwidget->parentWidget()->setGeometry(geo.x(), geo.y(), 500, 330);
 plotwidget->Plot.Output_DeviceAndPage(plotwidget->tempfile.fileName(),"pngqt",450,300);
 plotwidget->Plot.StartPlot("DDDDDD",0.5,true);

 plotwidget->renderplot();

 plotwidget->parentWidget()->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/plotwidget.svg")));
 plotwidget->show();
 freecursor();


}//dialog
*/
}

    }//endcase

}
void MainWindow::Import_Table_for_banana()
{
    loadLocalFile dialog;
    dialog.specialbanana();
    dialog.settitle("Load a local file to plot a banana");
    int result;

     result = dialog.exec();
      if(result){
          TableCanvas *windowtable=new TableCanvas;
          connect(windowtable,SIGNAL(logmessages(QString)),this, SLOT(LogMessage(QString)));
          connect(windowtable,SIGNAL(logmessagesR(QString)),this, SLOT(LogMessageR(QString)));
          connect(windowtable,SIGNAL(addtable(TableCanvas*,int)),this,SLOT(addactionfromtable(TableCanvas*,int)));
          windowtable->databaseconnection=currentconnection;
          ui->mdiArea->addSubWindow(windowtable);
          QRect   geo    = windowtable->parentWidget()->geometry();
          windowtable->parentWidget()->setGeometry(geo.x(), geo.y(), 450, 300);
          windowtable->fileName=dialog.fileName;
          windowtable->timestring=dialog.timeString;
          windowtable->separator=dialog.separator;
          windowtable->TableType="FromLocalFileTobanana";
          windowtable->igl=dialog.igl;
          windowtable->tablefromlocalfilefornanana();
    //     windowtable->disactiveThings();
         windowtable->setWindowTitle(dialog.fileName);
         windowtable->superuser=superuser;
         windowtable->costumize();
         windowtable->show();

      }
}
