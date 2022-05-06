#include "plotcanvas.h"
#include "ui_plotcanvas.h"

#include "dbapi.h"
#include "defineevents.h"
#include <QDateTime>
#include <QMouseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QFileDialog>
#include <vector>
#include "timefromruns.h"
#include "dbapi.h"
#include "plot2table.h"
#include "savedataplot.h"
#include "saveplot.h"
#include "plotscale.h"
#include "movie.h"

plotcanvas::plotcanvas(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::plotcanvas)
{
    ui->setupUi(this);
    ui->dockWidget->setHidden(true);
    configdialog=nullptr;
    rubberBand=nullptr;
    menugraphicsview=nullptr;
    ui->graphicsView->viewport()->installEventFilter(this);
    createActions();
    sc="|>";
    sv=":=";
    ss="|;";

    colors.clear();
    dpi=0.;
    colors << "0000FF"
           << "FF0000"
           << "008000"
           << "FF00FF"
           << "800080"
           << "00FFFF"
           << "808000"
           << "800000"
           << "800080"
           << "69bdd2";

    connect(ui->actionFit_into_widget,&QAction::triggered,this, &plotcanvas::fitintowidget);
    connect(ui->actionConfigure_Plot,&QAction::triggered,this, &plotcanvas::configplot);
    connect(ui->actionDefine_store_events,&QAction::triggered,this, &plotcanvas::editevents);
    connect(ui->actionSync_this_plot_with_the_period,&QAction::triggered,this, &plotcanvas::Syncthisplotwiththeguiperiod);
    connect(ui->actionSet_overall_time_equal_to_this_window_time,&QAction::triggered,this, &plotcanvas::setglobaltime);
    connect(ui->timefromruns,&QAbstractButton::pressed,this, &plotcanvas::timefromruns);
    connect(ui->Byevents,&QAbstractButton::pressed,this, &plotcanvas::timefromevents);
    connect(ui->replot,&QAbstractButton::pressed,this, &plotcanvas::Treplot);
    connect(ui->actionMake_a_Table,&QAction::triggered,this, &plotcanvas::Make_a_Table_triggered);
    connect(ui->actionSavedata,&QAction::triggered,this, &plotcanvas::Savedata_triggered);
    connect(ui->actionAuxiliary_lines,&QAction::triggered,this, &plotcanvas::Auxiliary_lines_triggered);
    connect(ui->actionExport_plot,&QAction::triggered,this, &plotcanvas::Export_plot_triggered);
    connect(ui->actionScale,&QAction::triggered,this, &plotcanvas::Scale_triggered);
    connect(ui->actionMovie,&QAction::triggered,this, &plotcanvas::Movie_triggered);

    animate=new QTimer(this);
    connect(animate,&QTimer::timeout,this, &plotcanvas::playmovie);
    Monitoring=false;
    mbred=255;
    mbgreen=161;
    mbblue=161;
    mbredd=255;
    mbgreend=255;
    mbblued=255;
    fromtable=false;
    forplotreport=false;

}

plotcanvas::~plotcanvas()
{
    delete ui;
    for(int i=0; i< data.count(); i++) delete data[i];
    for(int i=0; i< p.count(); i++) delete p[i];
}

void plotcanvas::startinlinkedtimemode(){
   ui->actionLinked_with_time_changes->setChecked(true);
}
void plotcanvas::plotfromtabledisabeactions(){
    ui->actionMovie->setDisabled(true);
    ui->Byevents->setDisabled(true);
    ui->timefromruns->setDisabled(true);
    ui->toolBar->removeAction(ui->actionContinuously_updated);
    ui->toolBar->removeAction(ui->actionDefine_store_events);
    ui->toolBar->removeAction(ui->actionSync_this_plot_with_the_period);
    ui->dockWidget->setDisabled(true);
    ui->toolBar->removeAction(ui->actionLinked_with_time_changes);
    ui->toolBar->removeAction(ui->actionMovie);
    ui->toolBar->removeAction(ui->actionSet_overall_time_equal_to_this_window_time);
    ui->toolBar->removeAction(ui->actionSmall_time_adjustments);

}



void  plotcanvas::typlot(int nplot, long long ti, long long tf){
    if(p.at(nplot)->Multixais==0)plot.Singleaxis(ti / 1000, tf / 1000, &data.at(nplot)->xy, p.at(nplot));
    else if(p.at(nplot)->Multixais==1)plot.Multiaxis(ti / 1000, tf / 1000, &data.at(nplot)->xy,  p.at(nplot));
    else if(p.at(nplot)->Multixais==2)plot.Twoaxis(ti / 1000, tf / 1000, &data.at(nplot)->xy, p.at(nplot));
    if(p.at(nplot)->runstart!=-1) {
        QStringList runs=plot.getstartruns(ti,tf,databaseconnection).split(";");
        plot.putrunlabels(ti / 1000, tf / 1000,p.at(nplot),runs); //put labels at borders
    }
}
void  plotcanvas::Splot(int nplot, long long ti, long long tf){
    plot.SurfacePlot(ti / 1000., tf / 1000., &data.at(nplot)->xyz, p.at(nplot));
    if(p.at(nplot)->runstart!=-1) {
        QStringList runs=plot.getstartruns(timei,timef,databaseconnection).split(";");
        plot.putrunlabels(ti / 1000, tf / 1000,p.at(nplot),runs); //put labels at borders
    }
}
void  plotcanvas::Hplot(int nplot){
    plot.HistoPlot(&data.at(nplot)->histo, p.at(nplot));

}
void plotcanvas::plotHfromchannel(QString plotname){


    data<<new plotdata;

    this->type=2;

    data.at(0)->tname=tnames.at(0);
    data.at(0)->tnames=tnames;
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    data.at(0)->Bins=Bins;
    data.at(0)->databaseconnection=databaseconnection;
    data.at(0)->gethistoplotdata(true);
    for(int i=0;i<data.at(0)->histo.count(); i++){
        data.at(0)->histo.at(i)->p.histocolor=colors.at(i);
        data.at(0)->histo.at(i)->p.plotidentifier="histo #"+QString::number(i+1);
    }
     data.at(0)->histo.at(0)->p.period="period ";
     data.at(0)->histo.at(0)->p.period.append(QString::number((timef-timei)/60000.));
     data.at(0)->histo.at(0)->p.period.append(" minutes \n  since: ");
     data.at(0)->histo.at(0)->p.period.append(QDateTime::fromMSecsSinceEpoch(timei).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());
     data.at(0)->histo.at(0)->p.Bins=tnames.at(1);
     QString closanstables;
     closanstables=tnames.at(0);    closanstables.append(";");
     for(int i=0; i<cnames.count(); i++){
         closanstables.append(cnames.at(i));
         if(i<cnames.count()-1)closanstables.append(",");
     }
     data.at(0)->histo.at(0)->p.closanstables=closanstables;

    p<< new plotproprieties;
    p.at(0)->Plotname = plotname;
    p.at(0)->plottitle = "testing histograma";
    p.at(0)->plottype=2;

    xlen=500;ylen=330;
    QRect   geo    =this->parentWidget()->geometry();
    this->parentWidget()->setGeometry(geo.x(), geo.y(), xlen, ylen);


    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();


    plot.setfont(p.at(0)->fontset);
    plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);


    plot.HistoPlot(&data.at(0)->histo, p.at(0));

    plot.deleteStream();
    renderplot();

}

void plotcanvas::plotSfromchannel(QString plotname){


    data<<new plotdata;

    this->type=1;

    data.at(0)->tname=tnames.at(0);

    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    data.at(0)->databaseconnection=databaseconnection;
    data.at(0)->getxyzplotdata(true);
    p<< new plotproprieties;
    p.at(0)->Plotname = plotname;
    p.at(0)->plottitle = "testing banana";
    data.at(0)->xyz.p.colorscale="insidecolorscale";
    data.at(0)->xyz.p.ylog=true;
    data.at(0)->xyz.p.zlog=true;
    data.at(0)->xyz.p.zlogmin=1;
    data.at(0)->xyz.p.diameters=diameters;
    QString tableandcolumns;
    tableandcolumns=tnames.at(0);
    tableandcolumns.append(";");
    for(int i=0; i<cnames.count();i++){
        tableandcolumns.append(cnames.at(i));
        if(i<cnames.count()-1) tableandcolumns.append(",");
    }
    data.at(0)->xyz.p.tableandcolumns=tableandcolumns;

    QStringList d;
    d=diameters.split(",");
    for(int i=0;i<d.count();i++){
        data.at(0)->xyz.y.push_back(d.at(i).toDouble());
    }
    p.at(0)->plottype=1;
    QString lixo;
    lixo=tnames.at(0);
    lixo.remove("_data");
    data.at(0)->xyz.p.plotidentifier=lixo;

    xlen=500;ylen=330;
    QRect   geo    =this->parentWidget()->geometry();
    this->parentWidget()->setGeometry(geo.x(), geo.y(), xlen, ylen);


    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();


    plot.setfont(p.at(0)->fontset);
    plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);


    plot.SurfacePlot(timei / 1000., timef / 1000., &data.at(0)->xyz, p.at(0));
    if(p.at(0)->runstart!=-1) {
        QStringList runs=plot.getstartruns(timei,timef,databaseconnection).split(";");
        plot.putrunlabels(timei / 1000, timef / 1000,p.at(0),runs); //put labels at borders
    }
    plot.deleteStream();
    renderplot();

}

void plotcanvas::plotSfromlist(QString plotname)
{
    this->type=1;
    dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, &dbapi::logmessages, this,
            &plotcanvas::LogMessages);
    connect(&table, &dbapi::logmessagesR, this,
            &plotcanvas::LogMessagesR);
    QStringList names;
    names.clear();
    QList <std::vector<QString> * > c;
    names<<"proprieties"<<"surface";
    for(int i=0; i<names.count(); i++) c << new ( std::vector<QString>);
    table.fechnotimecolumns_string_filterstring("Surfaceplots", names,"plotname",plotname , &c );


    data<<new plotdata;
    connect(data.at(0),SIGNAL(logmessages(QString)),this,SLOT(LogMessages(QString)));
    connect(data.at(0),SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesR(QString)));

    p<< new plotproprieties;
    p.at(0)->Plotname=plotname;

    data.at(0)->tname=tnames.at(0);
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    QString tableandcolumns;
    tableandcolumns=tnames.at(0);
    tableandcolumns.append(";");
    for(int i=0; i<cnames.count();i++){
        tableandcolumns.append(cnames.at(i));
        if(i<cnames.count()-1) tableandcolumns.append(",");
    }
    data.at(0)->xyz.p.tableandcolumns=tableandcolumns;

    data.at(0)->databaseconnection=databaseconnection;
    data.at(0)->getxyzplotdata(true);
    fillplotproprieties(p.at(0),c.at(0)->at(0));
    fillxyzproprieties(data.at(0),c.at(1)->at(0));
    QStringList d;

    d=data[0]->xyz.p.diameters.split(",");
    for(int i=0;i<d.count();i++){
        data.at(0)->xyz.y.push_back(d.at(i).toDouble());
    }

    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);


    for(int i=0; i<names.count(); i++) delete c[i];
    c.clear();

    if(forplotreport){
        plot.Plot_page(xlenreport,ylenreport,  "pngqt", "doreporttrash.png", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
        plot.SetInicialColormap();
        plot.setfont(p.at(0)->fontset);
        plot.setcharheight(p.at(0)->charheight,factorreport*p.at(0)->fontsize*p.at(0)->hdpifont);
      Splot(0,data.at(0)->timei, data.at(0)->timef);
    plot.deleteStream();
    }else{
    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();

    Splot(0,timei,timef);
    plot.deleteStream();
    renderplot();
    }

}

void plotcanvas::plotSfromtable(QString diameters,std::vector<long long> time, QVector<std::vector<double> > c)
{


    data<<new plotdata;

    this->type=1;

//    data.at(0)->tname=tnames.at(0);

//    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
//    data.at(0)->getxyzplotdata(true);
    p<< new plotproprieties;
    p.at(0)->Plotname = "plotname";
    p.at(0)->plottitle = "testing banana";
    data.at(0)->xyz.p.colorscale="insidecolorscale";
    data.at(0)->xyz.p.ylog=true;
    data.at(0)->xyz.p.zlog=true;
    data.at(0)->xyz.p.zlogmin=1;
    data.at(0)->xyz.p.diameters=diameters;

/*    QString tableandcolumns;
    tableandcolumns=tnames.at(0);
    tableandcolumns.append(";");
    for(int i=0; i<cnames.count();i++){
        tableandcolumns.append(cnames.at(i));
        if(i<cnames.count()-1) tableandcolumns.append(",");
    }
    data.at(0)->xyz.p.tableandcolumns=tableandcolumns;
*/
    QStringList d;
    d=diameters.split(",");
    data.at(0)->xyz.z.resize(d.count());
    for(int i=0;i<d.count();i++){
        data.at(0)->xyz.y.push_back(d.at(i).toDouble());
    }
    p.at(0)->plottype=1;
    QString lixo;
  //  lixo=tnames.at(0);
  //  lixo.remove("_data");
    lixo="eeeeee";
    data.at(0)->xyz.p.plotidentifier=lixo;
    for (int j=0; j<(int)time.size(); j++){
        data.at(0)->xyz.x.push_back(time.at(j)/1000.);
        for (int kk = 0; kk <  d.count(); kk++) {
            data.at(0)->xyz.z[kk].push_back(c.at(kk).at(j));
        }
    }
    xlen=500;ylen=330;
    QRect   geo    =this->parentWidget()->geometry();
    this->parentWidget()->setGeometry(geo.x(), geo.y(), xlen, ylen);


    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();


    plot.setfont(p.at(0)->fontset);
    plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);


    plot.SurfacePlot(timei / 1000., timef / 1000., &data.at(0)->xyz, p.at(0));
    if(p.at(0)->runstart!=-1) {
        QStringList runs=plot.getstartruns(timei,timef,databaseconnection).split(";");
        plot.putrunlabels(timei / 1000, timef / 1000,p.at(0),runs); //put labels at borders
    }
    plot.deleteStream();
    renderplot();

}
void plotcanvas::plotSfromfulllist(QList <QVariant> list)
{
    this->type=1;


    data<<new plotdata;
    connect(data.at(0),SIGNAL(logmessages(QString)),this,SLOT(LogMessages(QString)));
    connect(data.at(0),SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesR(QString)));

    p<< new plotproprieties;
    p.at(0)->Plotname=list.at(1).toString();

    data.at(0)->tname=tnames.at(0);
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    QString tableandcolumns;
    tableandcolumns=tnames.at(0);
    tableandcolumns.append(";");
    for(int i=0; i<cnames.count();i++){
        tableandcolumns.append(cnames.at(i));
        if(i<cnames.count()-1) tableandcolumns.append(",");
    }
    data.at(0)->xyz.p.tableandcolumns=tableandcolumns;

    data.at(0)->databaseconnection=databaseconnection;
    data.at(0)->getxyzplotdata(true);
    fillplotproprieties(p.at(0),list.at(3).toString());
    fillxyzproprieties(data.at(0),list.at(4).toString());
    QStringList d;

    d=data[0]->xyz.p.diameters.split(",");
    for(int i=0;i<d.count();i++){
        data.at(0)->xyz.y.push_back(d.at(i).toDouble());
    }

    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);



    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();

    Splot(0,timei,timef);
    plot.deleteStream();
    renderplot();


}
void plotcanvas::plotSfromlistl(QString plotname)
{
    this->type=1;
    struct Surfaceplots{
      QString plotname;
      QString columnsandtables;
      QString proprieties;
      QString surface;
     };
     QList <Surfaceplots> plots;
     plots.clear();
     QSettings settings("CampaignAnalysis","surfaceplotsdef");
     int size = settings.beginReadArray("bananaplots");
     for (int i = 0; i < size; ++i) {
         settings.setArrayIndex(i);
         Surfaceplots plot;
         plot.plotname=settings.value("plotname").toString();
         plot.columnsandtables=settings.value("columnsandtables").toString();
         plot.proprieties=settings.value("proprieties").toString();
         plot.surface=settings.value("surface").toString();
         plots.append(plot);
    }
     settings.endArray();

     bool exist;
     exist=false;
     int k;
     for (int i = 0; i < plots.count(); ++i) {
         if(plots.at(i).plotname==plotname) {
            exist=true;
            k=i;
            break;
         }
     }
     if(!exist){
        return;

     }

    data<<new plotdata;
    connect(data.at(0),SIGNAL(logmessages(QString)),this,SLOT(LogMessages(QString)));
    connect(data.at(0),SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesR(QString)));

    p<< new plotproprieties;
    p.at(0)->Plotname=plotname;

    data.at(0)->tname=tnames.at(0);
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    QString tableandcolumns;
    tableandcolumns=tnames.at(0);
    tableandcolumns.append(";");
    for(int i=0; i<cnames.count();i++){
        tableandcolumns.append(cnames.at(i));
        if(i<cnames.count()-1) tableandcolumns.append(",");
    }
    data.at(0)->xyz.p.tableandcolumns=tableandcolumns;

    data.at(0)->databaseconnection=databaseconnection;
    data.at(0)->getxyzplotdata(true);
    fillplotproprieties(p.at(0),plots.at(k).proprieties);
    fillxyzproprieties(data.at(0),plots.at(k).surface);
    QStringList d;

    d=data[0]->xyz.p.diameters.split(",");
    for(int i=0;i<d.count();i++){
        data.at(0)->xyz.y.push_back(d.at(i).toDouble());
    }

    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);




    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();

    Splot(0,timei,timef);
    plot.deleteStream();
    renderplot();


}

void plotcanvas::plotAfromlist(QString plotname)
{
   this->type=10;
    dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, &dbapi::logmessages, this,
            &plotcanvas::LogMessages);
    connect(&table, &dbapi::logmessagesR, this,
            &plotcanvas::LogMessagesR);

    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);

    QStringList names;
    names.clear();
    QList <std::vector<QString> * > cp;
    names<<"plottype"<<"plotorder"<<"columnsandtables"<<"proprieties"<<"surface"<<"curves"<<"histos";
    for(int i=0; i<names.count(); i++) cp << new ( std::vector<QString>);
    table.fechnotimecolumns_string_filterstring_orderby("Screens", names,"screenname",plotname,"plotorder" , &cp );
    
    for(int s=0; s< (int) cp.at(0)->size(); s++){
        data<<new plotdata;
        p<< new plotproprieties;
        if(cp.at(0)->at(s).toInt()==0){
            QStringList coltable;
            coltable=cp.at(2)->at(s).split(";");
            data.at(s)->tnames=coltable.at(1).split(",");
            data.at(s)->cnames=coltable.at(0).split(",");
            data.at(s)->timei=timei;
            data.at(s)->timef=timef;
            data.at(s)->databaseconnection=databaseconnection;
            for(int i=0; i<data.at(s)->cnames.count(); i++){
               data.at(s)->xy << new xydata;
            }

            fillplotproprieties(p.at(s),cp.at(3)->at(s));
            fillxyproprieties(data.at(s),cp.at(5)->at(s));
            data.at(s)->getxyplotdata(true);
            if(s==0){
                plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();

         }
            typlot(s,timei,timef);

        }else if(cp.at(0)->at(s).toInt()==1){
            QStringList coltable;
            coltable=cp.at(2)->at(s).split(";");

            data.at(s)->tname= coltable.at(0);
            data.at(s)->cnames=coltable.at(1).split(",");
            data.at(s)->timei=timei;
            data.at(s)->timef=timef;
            data.at(s)->xyz.p.tableandcolumns=cp.at(2)->at(s);

            data.at(s)->databaseconnection=databaseconnection;
            data.at(s)->getxyzplotdata(true);
            fillplotproprieties(p.at(s),cp.at(3)->at(s));
            fillxyzproprieties(data.at(s),cp.at(4)->at(s));
            QStringList d;

            d=data.at(s)->xyz.p.diameters.split(",");
            for(int i=0;i<d.count();i++){
                data.at(s)->xyz.y.push_back(d.at(i).toDouble());
            }

            if(s==0){
                plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();

            }
            Splot(s,timei,timef);

        }else if(cp.at(0)->at(s).toInt()==2){
            QStringList coltable;
            coltable=cp.at(2)->at(s).split(";");

            data.at(s)->tname= coltable.at(0);
            data.at(s)->cnames=coltable.at(1).split(",");
            data.at(s)->timei=timei;
            data.at(s)->timef=timef;

            data.at(s)->databaseconnection=databaseconnection;
            data.at(s)->Bins=getbins(cp.at(6)->at(s));
            data.at(s)->gethistoplotdata(true);
            fillplotproprieties(p.at(s),cp.at(3)->at(s));
            fillhistoproprieties(data.at(s),cp.at(6)->at(s));
            data.at(s)->histo.at(0)->p.period="period ";
            data.at(s)->histo.at(0)->p.period.append(QString::number(timef-timei));
            data.at(s)->histo.at(0)->p.period.append(" \n seconds   since: ");
            data.at(s)->histo.at(0)->p.period.append(QDateTime::fromMSecsSinceEpoch(timei).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());



             if(s==0){
                plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();

            }
            Hplot(s);
        }

   }

    for(int s=0; s<cp.count(); s++) delete cp[s];

     plot.deleteStream();
     renderplot();

}    
void plotcanvas::plotAfromfulllist(QList <QVariant> list)
{
   this->type=10;

    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);

  int size=list.count()/9;
    for(int s=0; s< size; s++){
        data<<new plotdata;
        p<< new plotproprieties;
        if(list.at(s*9+2).toInt()==0){
            QStringList coltable;
            coltable=list.at(s*9+4).toString().split(";");
            data.at(s)->tnames=coltable.at(1).split(",");
            data.at(s)->cnames=coltable.at(0).split(",");
            data.at(s)->timei=timei;
            data.at(s)->timef=timef;
            data.at(s)->databaseconnection=databaseconnection;
            for(int i=0; i<data.at(s)->cnames.count(); i++){
               data.at(s)->xy << new xydata;
            }
            fillplotproprieties(p.at(s),list.at(s*9+5).toString());
            fillxyproprieties(data.at(s),list.at(s*9+7).toString());
            data.at(s)->getxyplotdata(true);

            if(s==0){
                p.at(0)->Plotname=list.at(1).toString();
                plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();

         }
            typlot(s,timei,timef);

        }else if(list.at(s*9+2).toInt()==1){
            QStringList coltable;
            coltable=list.at(s*9+4).toString().split(";");

            data.at(s)->tname= coltable.at(0);
            data.at(s)->cnames=coltable.at(1).split(",");
            data.at(s)->timei=timei;
            data.at(s)->timef=timef;
            data.at(s)->xyz.p.tableandcolumns=list.at(s*9+4).toString();

            data.at(s)->databaseconnection=databaseconnection;
            data.at(s)->getxyzplotdata(true);
            fillplotproprieties(p.at(s),list.at(s*9+5).toString());
            fillxyzproprieties(data.at(s),list.at(s*9+6).toString());
            QStringList d;

            d=data.at(s)->xyz.p.diameters.split(",");
            for(int i=0;i<d.count();i++){
                data.at(s)->xyz.y.push_back(d.at(i).toDouble());
            }

            if(s==0){
                 p.at(0)->Plotname=list.at(1).toString();
                plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();

            }
            Splot(s,timei,timef);

        }else if(list.at(s*9+2).toInt()==2){
            QStringList coltable;
            coltable=list.at(s*9+4).toString().split(";");

            data.at(s)->tname= coltable.at(0);
            data.at(s)->cnames=coltable.at(1).split(",");
            data.at(s)->timei=timei;
            data.at(s)->timef=timef;

            data.at(s)->databaseconnection=databaseconnection;
            data.at(s)->Bins=getbins(list.at(s*9+8).toString());
            data.at(s)->gethistoplotdata(true);
            fillplotproprieties(p.at(s),list.at(s*9+5).toString());
            fillhistoproprieties(data.at(s),list.at(s*9+8).toString());
            data.at(s)->histo.at(0)->p.period="period ";
            data.at(s)->histo.at(0)->p.period.append(QString::number(timef-timei));
            data.at(s)->histo.at(0)->p.period.append(" \n seconds   since: ");
            data.at(s)->histo.at(0)->p.period.append(QDateTime::fromMSecsSinceEpoch(timei).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());



             if(s==0){
                 p.at(0)->Plotname=list.at(1).toString();
                plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();

            }
            Hplot(s);
        }

   }

     plot.deleteStream();
     renderplot();

}

void plotcanvas::plotAfromlistl(QString plotname)
{
   this->type=10;

    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);


         struct ScreenPlots{
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
                plot.surface=settings.value("surface").toString();
                plot.curves=settings.value("curves").toString();
                plot.histos=settings.value("histos").toString();

               if(plot.screenname==plotname) plots.append(plot);
           }
            settings.endArray();





for (int i = 0; i < plots.count(); ++i) {
       data<<new plotdata;
        p<< new plotproprieties;
        if(plots.at(i).plottype.toInt()==0){
            QStringList coltable;
            coltable=plots.at(i).columnsandtables.split(";");
            data.at(i)->tnames=coltable.at(1).split(",");
            data.at(i)->cnames=coltable.at(0).split(",");
            data.at(i)->timei=timei;
            data.at(i)->timef=timef;
            data.at(i)->databaseconnection=databaseconnection;
            for(int s=0; s<data.at(i)->cnames.count(); s++){
               data.at(i)->xy << new xydata;
            }
            fillplotproprieties(p.at(i),plots.at(i).proprieties);
            fillxyproprieties(data.at(i),plots.at(i).curves);
            data.at(i)->getxyplotdata(true);
            if(i==0){
                plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();

         }
            typlot(i,timei,timef);

        }else if(plots.at(i).plottype.toInt()==1){
            QStringList coltable;
            coltable=plots.at(i).columnsandtables.split(";");

            data.at(i)->tname= coltable.at(0);
            data.at(i)->cnames=coltable.at(1).split(",");
            data.at(i)->timei=timei;
            data.at(i)->timef=timef;
            data.at(i)->xyz.p.tableandcolumns=plots.at(i).columnsandtables;

            data.at(i)->databaseconnection=databaseconnection;
            data.at(i)->getxyzplotdata(true);
            fillplotproprieties(p.at(i),plots.at(i).proprieties);
            fillxyzproprieties(data.at(i),plots.at(i).surface);
            QStringList d;

            d=data.at(i)->xyz.p.diameters.split(",");
            for(int s=0;s<d.count();s++){
                data.at(i)->xyz.y.push_back(d.at(s).toDouble());
            }

            if(i==0){
                plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();

            }
            Splot(i,timei,timef);

        }else if(plots.at(i).plottype.toInt()==2){
            QStringList coltable;
            coltable=plots.at(i).columnsandtables.split(";");

            data.at(i)->tname= coltable.at(0);
            data.at(i)->cnames=coltable.at(1).split(",");
            data.at(i)->timei=timei;
            data.at(i)->timef=timef;

            data.at(i)->databaseconnection=databaseconnection;
            data.at(i)->Bins=getbins(plots.at(i).histos);
            data.at(i)->gethistoplotdata(true);
            fillplotproprieties(p.at(i),plots.at(i).proprieties);
            fillhistoproprieties(data.at(i),plots.at(i).histos);
            data.at(i)->histo.at(0)->p.period="period ";
            data.at(i)->histo.at(0)->p.period.append(QString::number(timef-timei));
            data.at(i)->histo.at(0)->p.period.append(" \n seconds   since: ");
            data.at(i)->histo.at(0)->p.period.append(QDateTime::fromMSecsSinceEpoch(timei).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());



             if(i==0){
                plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();

            }
            Hplot(i);
        }

   }

     plot.deleteStream();
     renderplot();

}

void plotcanvas::plotHfromlist(QString plotname)
{
    this->type=2;
    dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, &dbapi::logmessages, this,
            &plotcanvas::LogMessages);
    connect(&table, &dbapi::logmessagesR, this,
            &plotcanvas::LogMessagesR);
    QStringList names;
    names.clear();
    QList <std::vector<QString> * > c;
    names<<"proprieties"<<"histos";
    for(int i=0; i<names.count(); i++) c << new ( std::vector<QString>);
    table.fechnotimecolumns_string_filterstring("Histoplots", names,"plotname",plotname , &c );


    data<<new plotdata;
    p<< new plotproprieties;
    p.at(0)->Plotname=plotname;

    data.at(0)->tname=tnames.at(0);
    data.at(0)->tnames=tnames;
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    QString tableandcolumns;
    tableandcolumns=tnames.at(0);
    tableandcolumns.append(";");
    for(int i=0; i<cnames.count();i++){
        tableandcolumns.append(cnames.at(i));
        if(i<cnames.count()-1) tableandcolumns.append(",");
    }

    data.at(0)->databaseconnection=databaseconnection;
    data.at(0)->Bins=getbins(c.at(1)->at(0));
    data.at(0)->gethistoplotdata(true);
    fillplotproprieties(p.at(0),c.at(0)->at(0));
    fillhistoproprieties(data.at(0),c.at(1)->at(0));
    data.at(0)->histo.at(0)->p.period="period ";
    data.at(0)->histo.at(0)->p.period.append(QString::number((timef-timei)/60000.));
    data.at(0)->histo.at(0)->p.period.append(" minutes \n  since: ");
    data.at(0)->histo.at(0)->p.period.append(QDateTime::fromMSecsSinceEpoch(timei).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());


    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);


    for(int i=0; i<names.count(); i++) delete c[i];
    c.clear();

    if(forplotreport){
        plot.Plot_page(xlenreport,ylenreport,  "pngqt", "doreporttrash.png", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
        plot.SetInicialColormap();
        plot.setfont(p.at(0)->fontset);
        plot.setcharheight(p.at(0)->charheight,factorreport*p.at(0)->fontsize*p.at(0)->hdpifont);
     Hplot(0);
    plot.deleteStream();
    }else{
    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();

    Hplot(0);
    plot.deleteStream();
    renderplot();
    }


}
void plotcanvas::plotHfromfulllist(QList <QVariant> list)
{
    this->type=2;


    data<<new plotdata;
    p<< new plotproprieties;
    p.at(0)->Plotname=list.at(1).toString();

    data.at(0)->tname=tnames.at(0);
    data.at(0)->tnames=tnames;
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    QString tableandcolumns;
    tableandcolumns=tnames.at(0);
    tableandcolumns.append(";");
    for(int i=0; i<cnames.count();i++){
        tableandcolumns.append(cnames.at(i));
        if(i<cnames.count()-1) tableandcolumns.append(",");
    }

    data.at(0)->databaseconnection=databaseconnection;
    data.at(0)->Bins=getbins(list.at(4).toString());
    data.at(0)->gethistoplotdata(true);
    fillplotproprieties(p.at(0),list.at(3).toString());
    fillhistoproprieties(data.at(0),list.at(4).toString());
    data.at(0)->histo.at(0)->p.period="period ";
    data.at(0)->histo.at(0)->p.period.append(QString::number((timef-timei)/60000.));
    data.at(0)->histo.at(0)->p.period.append(" minutes \n  since: ");
    data.at(0)->histo.at(0)->p.period.append(QDateTime::fromMSecsSinceEpoch(timei).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());


    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);



    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();

    Hplot(0);
    plot.deleteStream();
    renderplot();


}
void plotcanvas::plotHfromlistl(QString plotname)
{
    this->type=2;
    struct Histoplots{
      QString plotname;
      QString columnsandtables;
      QString proprieties;
      QString histos;
     };
     QList <Histoplots> plots;
     plots.clear();
     QSettings settings("CampaignAnalysis","histoplotsdef");
     int size = settings.beginReadArray("histoplots");
     for (int i = 0; i < size; ++i) {
         settings.setArrayIndex(i);
         Histoplots plot;
         plot.plotname=settings.value("plotname").toString();
         plot.columnsandtables=settings.value("columnsandtables").toString();
         plot.proprieties=settings.value("proprieties").toString();
         plot.histos=settings.value("histos").toString();
         plots.append(plot);
    }
     settings.endArray();

     bool exist;
     exist=false;
     int k;
     for (int i = 0; i < plots.count(); ++i) {
         if(plots.at(i).plotname==plotname) {
            exist=true;
            k=i;
            break;
         }
     }
     if(!exist){
        return;

     }

    data<<new plotdata;
    p<< new plotproprieties;
    p.at(0)->Plotname=plotname;

    data.at(0)->tname=tnames.at(0);
    data.at(0)->tnames=tnames;
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    QString tableandcolumns;
    tableandcolumns=tnames.at(0);
    tableandcolumns.append(";");
    for(int i=0; i<cnames.count();i++){
        tableandcolumns.append(cnames.at(i));
        if(i<cnames.count()-1) tableandcolumns.append(",");
    }

    data.at(0)->databaseconnection=databaseconnection;
    data.at(0)->Bins=getbins(plots.at(k).histos);
    data.at(0)->gethistoplotdata(true);
    fillplotproprieties(p.at(0),plots.at(k).proprieties);
    fillhistoproprieties(data.at(0),plots.at(k).histos);
    data.at(0)->histo.at(0)->p.period="period ";
    data.at(0)->histo.at(0)->p.period.append(QString::number((timef-timei)/60000.));
    data.at(0)->histo.at(0)->p.period.append(" minutes \n  since: ");
    data.at(0)->histo.at(0)->p.period.append(QDateTime::fromMSecsSinceEpoch(timei).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());


    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);




    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();

    Hplot(0);
    plot.deleteStream();
    renderplot();


}
void plotcanvas::plotfromchannel(QString plotname)
{
  //  plotdata plotd;
  //  plotd.databaseconnection=databaseconnection;

    this->type=0;
    data<<new plotdata;


    data.at(0)->tnames=tnames;
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    data.at(0)->databaseconnection=databaseconnection;
    for(int i=0; i<cnames.count(); i++){
       data.at(0)->xy << new xydata;
       data.at(0)->xy[i]->p.cname=cnames.at(i);
       data.at(0)->xy[i]->p.tname=tnames.at(i);
    }
    data.at(0)->getxyplotdata(true);

/*
        for (int j = 0; j < cnames.count(); j++) {
           unsigned long size = data.at(0)->xy.at(j)->x.size();
           if(size) LogMessages(cnames.at(j)+"@"+tnames.at(j)+QString::number(data.at(0)->xy.at(j)->x.at(0))+" "+ QString::number(data.at(0)->xy.at(j)->y.at(0))+" "+ QString::number(size));
        }
*/
    //    QRect geo = this->ui->graphicsView->geometry();
    //  int xlen = geo.width();
    //    int ylen = geo.height(); não pode ser usado porque ainda não temos a janela definida
           xlen=400; ylen=300;
    //    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ui->graphicsView->setGeometry(0,0,xlen,ylen);
        p<< new plotproprieties;
        p.at(0)->multiaxissep = .10;
        p.at(0)->labelsep = 1.0;
        p.at(0)->yvmin = 0.0;
        p.at(0)->xvmin=0.10;
        p.at(0)->Plotname = plotname;
        p.at(0)->plottitle = "testing proprieties";
        p.at(0)->plottype=0;

        p.at(0)->Multixais=1;
        p.at(0)->balpha=.5;

        plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
        plot.SetInicialColormap();


        plot.setfont(p.at(0)->fontset);
        plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);


        for(int i=0; i< data.at(0)->xy.size(); i++){
            data.at(0)->xy.at(i)->p.Color=colors.at(std::min(i,(int) colors.count()-1));
            data.at(0)->xy.at(i)->p.yaxisLabel=aliasname.at(i);
        }

        typlot(0,timei,timef);
        plot.deleteStream();
        renderplot();

}
void plotcanvas::plottyfromtable(QStringList names,QList <std::vector<double> >x,QList <std::vector<double> > y)
{
  //  plotdata plotd;
  //  plotd.databaseconnection=databaseconnection;

    this->type=0;
    data<<new plotdata;


//    data.at(0)->tnames=tnames;
//    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    for(int i=0; i<names.count(); i++){
       data.at(0)->xy << new xydata;
    }
  // data.at(0)->getxyplotdata(true);

        xlen=400; ylen=300;
        ui->graphicsView->setGeometry(0,0,xlen,ylen);
        p<< new plotproprieties;
        p.at(0)->multiaxissep = .10;
        p.at(0)->labelsep = 1.0;
        p.at(0)->yvmin = 0.0;
        p.at(0)->xvmin=0.10;
        p.at(0)->Plotname = names.at(0);
        p.at(0)->plottitle = "testing proprieties";
        p.at(0)->plottype=0;

        p.at(0)->Multixais=1;
        p.at(0)->balpha=.5;

        plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
        plot.SetInicialColormap();


        plot.setfont(p.at(0)->fontset);
        plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);


        for(int i=0; i< data.at(0)->xy.size(); i++){
            data.at(0)->xy.at(i)->p.Color=colors.at(std::min(i,(int) colors.count()-1));
            data.at(0)->xy.at(i)->p.yaxisLabel=names.at(i);
            for(int j=0; j<x.at(i).size();j++){
                data.at(0)->xy.at(i)->x.push_back(x.at(i).at(j)/1000.);
               data.at(0)->xy.at(i)->y.push_back(y.at(i).at(j));
            }
        }


        typlot(0,timei,timef);
        plot.deleteStream();
        renderplot();

}
void plotcanvas::plottyfromlist(QString plotname)
{
    this->type=0;
    dbapi table;
    table.connectionName = databaseconnection;
    connect(&table, &dbapi::logmessages, this,
            &plotcanvas::LogMessages);
    connect(&table, &dbapi::logmessagesR, this,
            &plotcanvas::LogMessagesR);
    QStringList names;
    names.clear();
    QList <std::vector<QString> * > c;
    names<<"proprieties"<<"curves";
    for(int i=0; i<names.count(); i++) c << new ( std::vector<QString>);
    table.fechnotimecolumns_string_filterstring("typlots", names,"plotname",plotname , &c );


    data<<new plotdata;
    p<< new plotproprieties;
    p.at(0)->Plotname=plotname;

    connect(data.at(0),SIGNAL(logmessages(QString)),this,SLOT(LogMessages(QString)));
    connect(data.at(0),SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesR(QString)));

    data.at(0)->tnames=tnames;
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    data.at(0)->databaseconnection=databaseconnection;
    for(int i=0; i<cnames.count(); i++){
       data.at(0)->xy << new xydata;
    }

 //   data.at(0)->getxyplotdata(true);
    fillplotproprieties(p.at(0),c.at(0)->at(0));
    fillxyproprieties(data.at(0),c.at(1)->at(0));
    data.at(0)->getxyplotdata(true);

    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);


    for(int i=0; i<names.count(); i++) delete c[i];
    c.clear();

    if(forplotreport){
    plot.Plot_page(xlenreport,ylenreport,  "pngqt", "doreporttrash.png", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();
    plot.setfont(p.at(0)->fontset);
    plot.setcharheight(p.at(0)->charheight,factorreport*p.at(0)->fontsize*p.at(0)->hdpifont);
    typlot(0,data.at(0)->timei, data.at(0)->timef);
    plot.deleteStream();
    }else{

    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();

    typlot(0,timei,timef);
    plot.deleteStream();
    renderplot();
    }

}
void plotcanvas::plottyfromfulllist(QList <QVariant> list)
{
    this->type=0;


    data<<new plotdata;
    p<< new plotproprieties;
    p.at(0)->Plotname=list.at(1).toString();

    connect(data.at(0),SIGNAL(logmessages(QString)),this,SLOT(LogMessages(QString)));
    connect(data.at(0),SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesR(QString)));

    data.at(0)->tnames=tnames;
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    data.at(0)->databaseconnection=databaseconnection;
    for(int i=0; i<cnames.count(); i++){
       data.at(0)->xy << new xydata;
    }

    fillplotproprieties(p.at(0),list.at(3).toString());
    fillxyproprieties(data.at(0),list.at(4).toString());
    data.at(0)->getxyplotdata(true);

    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);



    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();

    typlot(0,timei,timef);
    plot.deleteStream();
    renderplot();

}
void plotcanvas::plottyfromlistl(QString plotname)
{
    this->type=0;

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

     bool exist;
     exist=false;
     int k;
     for (int i = 0; i < plots.count(); ++i) {
         if(plots.at(i).plotname==plotname) {
            exist=true;
            k=i;
            break;
         }
     }
     if(!exist){
        return;

     }


    data<<new plotdata;
    p<< new plotproprieties;
    p.at(0)->Plotname=plotname;

    connect(data.at(0),SIGNAL(logmessages(QString)),this,SLOT(LogMessages(QString)));
    connect(data.at(0),SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesR(QString)));

    data.at(0)->tnames=tnames;
    data.at(0)->cnames=cnames;
    data.at(0)->timei=timei;
    data.at(0)->timef=timef;
    data.at(0)->databaseconnection=databaseconnection;
    for(int i=0; i<cnames.count(); i++){
       data.at(0)->xy << new xydata;
    }
    fillplotproprieties(p.at(0),plots[k].proprieties);
    fillxyproprieties(data.at(0),plots[k].curves);    
    data.at(0)->getxyplotdata(true);

    xlen=400;  ylen=300;
    ui->graphicsView->setGeometry(0,0,xlen,ylen);


    plot.Plot_page(xlen, ylen, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
    plot.SetInicialColormap();

    typlot(0,timei,timef);
    plot.deleteStream();
    renderplot();

}
void plotcanvas::fillplotproprieties(plotproprieties *p, QString text){
    QStringList fields, values;
    fields=text.split(ss);
    for(int i=0; i< fields.count();i++){
        values=fields.at(i).split(sv);
        if(values.at(0)=="balpha")p->balpha=values.at(1).toDouble();
        else if(values.at(0)=="fontset")p->fontset=values.at(1).toInt();
        else if (values.at(0)=="fontsize")p->fontsize=values.at(1).toDouble();
        else if (values.at(0)=="bred")p->bred=values.at(1).toInt();
        else if (values.at(0)=="bgreen")p->bgreen=values.at(1).toInt();
        else if (values.at(0)=="bblue")p->bblue=values.at(1).toInt();
        else if (values.at(0)=="xvmin")p->xvmin=values.at(1).toDouble();
        else if (values.at(0)=="xvmax")p->xvmax=values.at(1).toDouble();
        else if (values.at(0)=="yvmin")p->yvmin=values.at(1).toDouble();
        else if (values.at(0)=="yvmax")p->yvmax=values.at(1).toDouble();
        else if (values.at(0)=="withoutxaxis")p->withoutxaxis=(bool) values.at(1).toInt();
        else if (values.at(0)=="firsttimelabel")p->firsttimelabel=(bool) values.at(1).toInt();
        else if (values.at(0)=="runstart")p->runstart=values.at(1).toInt();
        else if (values.at(0)=="stylex")p->stylex=(bool) values.at(1).toInt();
        else if (values.at(0)=="styley")p->styley=(bool) values.at(1).toInt();
        else if (values.at(0)=="Multixais")p->Multixais=values.at(1).toInt();
        else if (values.at(0)=="labelsep")p->labelsep=values.at(1).toDouble();
        else if (values.at(0)=="leftaxislimits")p->leftaxislimits=values.at(1);
        else if (values.at(0)=="rightaxislimits")p->rightaxislimits=values.at(1);
        else if (values.at(0)=="plottitle")p->plottitle=values.at(1);
        else if (values.at(0)=="plottype")p->plottype=values.at(1).toInt();
        else if (values.at(0)=="Plotname")p->Plotname=values.at(1);

    }

}
void plotcanvas::fillxyproprieties(plotdata *data,QString text ){
    QStringList curves;
    curves=text.split(sc);
    QStringList fields,values;
    for(int i=0; i< data->xy.count(); i++){
        fields=curves.at(i).split(ss);
        for(int j=0; j< fields.count();j++){
            values=fields.at(j).split(sv);
            if(values.at(0)=="rightaxis")data->xy.at(i)->p.rightaxis=(bool)values.at(1).toInt();
            else if(values.at(0)=="ylog")data->xy.at(i)->p.ylog=(bool)values.at(1).toInt();
            else if(values.at(0)=="derivative")data->xy.at(i)->p.derivative=(bool)values.at(1).toInt();
            else if(values.at(0)=="devnpoints")data->xy.at(i)->p.devnpoints=values.at(1).toInt();
            else if(values.at(0)=="smoothpoints")data->xy.at(i)->p.smoothpoints=values.at(1).toInt();
            else if(values.at(0)=="linetype")data->xy.at(i)->p.linetype=values.at(1);
            else if(values.at(0)=="marker")data->xy.at(i)->p.marker=values.at(1).toInt();
            else if(values.at(0)=="markersscale")data->xy.at(i)->p.markersscale=values.at(1).toDouble();
            else if(values.at(0)=="minfreelimits")data->xy.at(i)->p.minfreelimits=(bool)values.at(1).toInt();
            else if(values.at(0)=="maxfreelimits")data->xy.at(i)->p.maxfreelimits=(bool)values.at(1).toInt();
            else if(values.at(0)=="factor")data->xy.at(i)->p.factor=values.at(1);
            else if(values.at(0)=="pwidth")data->xy.at(i)->p.pwidth=values.at(1).toInt();
            else if(values.at(0)=="Color")data->xy.at(i)->p.Color=values.at(1);
            else if(values.at(0)=="yaxisLabel")data->xy.at(i)->p.yaxisLabel=values.at(1);
            else if(values.at(0)=="tname")data->xy.at(i)->p.tname=values.at(1);
            else if(values.at(0)=="cname")data->xy.at(i)->p.cname=values.at(1);
            else if(values.at(0)=="Ymin")data->xy.at(i)->p.Ymin=values.at(1).toDouble();
            else if(values.at(0)=="Ymax")data->xy.at(i)->p.Ymax=values.at(1).toDouble();
            else if(values.at(0)=="onlysmooth")data->xy.at(i)->p.onlysmooth=(bool)values.at(1).toInt();
            else if(values.at(0)=="smooth")data->xy.at(i)->p.smooth=(bool)values.at(1).toInt();

    }

    }
}
QString plotcanvas::getbins(QString text){
    QString bins;
    QStringList histos;
    histos=text.split(sc);
    QStringList fields,values;
    fields=histos.at(0).split(ss);
    for(int j=0; j<fields.count(); j++){
        values=fields.at(j).split(sv);
        if(values.at(0)=="Bins") {
        bins=values.at(1);
        break;
        }
    }
    return bins;
}
void plotcanvas::fillhistoproprieties(plotdata *data,QString text ){
    QStringList histos;
    histos=text.split(sc);
    QStringList fields,values;

    for(int i=0; i< data->histo.count(); i++){
        fields=histos.at(i).split(ss);
        for(int j=0; j< fields.count();j++){
            values=fields.at(j).split(sv);
            if(values.at(0)=="histocolor")data->histo.at(i)->p.histocolor=values.at(1);
            else if(values.at(0)=="xlabel")data->histo.at(i)->p.xlabel=values.at(1);
            else if(values.at(0)=="ylabel")data->histo.at(i)->p.ylabel=values.at(1);
            else if(values.at(0)=="plotidentifier")data->histo.at(i)->p.plotidentifier=values.at(1);
            else if(values.at(0)=="htype")data->histo.at(i)->p.htype=values.at(1).toInt();
            else if(values.at(0)=="closanstables") data->histo.at(i)->p.closanstables=values.at(1);
            else if(values.at(0)=="Bins") data->histo.at(i)->p.Bins=values.at(1);

        }

    }
}
void plotcanvas::fillxyzproprieties(plotdata *data,QString text ){
    QStringList curves;
    QStringList fields,values;

        fields=text.split(ss);
        for(int j=0; j< fields.count();j++){
            values=fields.at(j).split(sv);
            if(values.at(0)=="Surfacecolor") data->xyz.p.Surfacecolor=values.at(1);
            else if(values.at(0)=="ylog")data->xyz.p.ylog=(bool)values.at(1).toInt();
            else if(values.at(0)=="zlog")data->xyz.p.zlog=(bool)values.at(1).toInt();
            else if(values.at(0)=="Zmin")data->xyz.p.Zmin=values.at(1).toDouble();
            else if(values.at(0)=="Zmax")data->xyz.p.Zmax=values.at(1).toDouble();
            else if(values.at(0)=="maxfreelimits")data->xyz.p.maxfreelimits=(bool)values.at(1).toInt();
            else if(values.at(0)=="minfreelimits")data->xyz.p.minfreelimits=(bool)values.at(1).toInt();
            else if(values.at(0)=="yaxisLabel")data->xyz.p.yaxisLabel=values.at(1);
            else if(values.at(0)=="surfacetype")data->xyz.p.surfacetype=values.at(1);
            else if(values.at(0)=="colorlimits")data->xyz.p.colorlimits=values.at(1);
            else if(values.at(0)=="Surfacelegendinside")data->xyz.p.Surfacelegendinside=values.at(1);
            else if(values.at(0)=="colorscale")data->xyz.p.colorscale=values.at(1);
            else if(values.at(0)=="factor")data->xyz.p.factor=values.at(1).toDouble();
            else if(values.at(0)=="diameters")data->xyz.p.diameters=values.at(1);
            else if(values.at(0)=="plotidentifier")data->xyz.p.plotidentifier=values.at(1);
        }


}

void plotcanvas::renderplot(){

    QPixmap *renderer = new QPixmap(QPixmap::fromImage(plot.image->rgbSwapped()));
    scene.clear();
    scene.addPixmap(*renderer);
    this->ui->graphicsView->setScene(&scene);
    delete renderer;
    plot.deleteImage();
    this->setWindowTitle( p.at(0)->Plotname);
}
 void plotcanvas::fitintowidget()
 {
    cursorbusy();
     QRect geo = this->ui->graphicsView->geometry();
     ui->graphicsView->setBaseSize(geo.width(), geo.height());

     scene.clear();
     this->ui->graphicsView->resetCachedContent();
     plot.dpi=hdpi;
     ui->graphicsView->setGeometry(0,0,geo.width(),geo.height());
     int xl=(int) (0.98*geo.width());
     int yl =(int) (0.98*geo.height());
     ui->graphicsView->setSceneRect(0, 0, xl, yl);
     scene.setSceneRect(0, 0, xl, yl);
   //  ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   //  ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

     plot.Plot_page(xl,yl, "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
     plot.SetInicialColormap();
     plot.setfont(p.at(0)->fontset);
     plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);

     for(int i=0;i<p.count();i++)
     {
         if(p.at(i)->plottype==0)typlot(i,data.at(0)->timei, data.at(0)->timef);
         if(p.at(i)->plottype==1)Splot(i,data.at(0)->timei, data.at(0)->timef);
         if(p.at(i)->plottype==2)Hplot(i);
     }

     plot.getxlenylen(&xlen,&ylen);
     plot.deleteStream();
     renderplot();

     freecursor();
     //logmessagesR(QString::number(xlen)+"  "+QString::number(ylen));
 }
 void plotcanvas::replot()
 {
     cursorbusy();
     plot.Plot_page(xlen,ylen,  "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
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
     renderplot();
     freecursor();

 }
 void plotcanvas::update(long long itime, long long ltime)
 {
     cursorbusy();
     if(ltime- itime <10  || itime >ltime ) {freecursor();return;}
     if(std::abs(data.at(0)->timei-itime)<10 && std::abs(data.at(0)->timef-ltime)<10 ) {freecursor();return;}
     plot.Plot_page(xlen,ylen,  "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
     plot.SetInicialColormap();
     plot.setfont(p.at(0)->fontset);
     plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);

     for(int i=0;i<p.count();i++)
     {
         if(data.at(i)->movie){
    //         data.at(i)->mtimei=std::min(data.at(i)->timei,QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());
    //         data.at(i)->mtimef=std::max(data.at(i)->timef,QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());
             data.at(i)->mtimei=data.at(i)->timei;
             data.at(i)->mtimef=data.at(i)->timef;

             }
         if(p.at(i)->plottype==0){
             data.at(i)->timei=itime;
             data.at(i)->timef=ltime;
             data.at(i)->getxyplotdata(false);
             typlot(i,itime,ltime);
         }else if(p.at(i)->plottype==1){            
             data.at(i)->timei=itime;
             data.at(i)->timef=ltime;
             data.at(i)->getxyzplotdata(false);
             Splot(i,itime,ltime);
         }
         else if(p.at(i)->plottype==2){
             data.at(i)->timei=itime;
             data.at(i)->timef=ltime;
             data.at(i)->histo.at(0)->p.period="period ";
             data.at(i)->histo.at(0)->p.period.append(QString::number((ltime-itime)/60000.));
             data.at(i)->histo.at(0)->p.period.append(" minutes  \n since: ");
             data.at(i)->histo.at(0)->p.period.append(QDateTime::fromMSecsSinceEpoch(itime).toUTC().toString("yyyy/MM/dd  hh:mm").toLatin1().constData());

             data.at(i)->gethistoplotdata(false);
             Hplot(i);
         }

     }

     plot.deleteStream();
     renderplot();
     freecursor();

 }
 void plotcanvas::configplot()
 {
if(!configdialog){
    configdialog=new configplotwidget(this);
    configdialog->databaseconnection=databaseconnection;
    connect(configdialog, &configplotwidget::replot,this,&plotcanvas::replot);
    connect(configdialog, &configplotwidget::dplots,this,&plotcanvas::dplots);
    connect(configdialog,&configplotwidget::logmessages,this,&plotcanvas::LogMessages);
    connect(configdialog,&configplotwidget::logmessagesR,this,&plotcanvas::LogMessagesR);
    configdialog->p=&p;
    if(!xyp.count()){
    QVector <xyproprieties *> xp;
    QVector <histoproprieties *> hp;
    for(int i=0; i<data.count(); i++){
        xp.clear();
        hp.clear();
        for(int j=0; j<data.at(i)->xy.count(); j++ ){
            xp << &data.at(i)->xy.at(j)->p;
        }
        xyp<< xp;
         for(int j=0; j<data.at(i)->histo.count(); j++ ){
            hp << &data.at(i)->histo.at(j)->p;
        }
        histop<<hp;
        xyzp <<&data.at(i)->xyz.p;

  }
    }
  configdialog->xyp=&xyp;
  configdialog->xyzp=&xyzp;
  configdialog->histop=&histop;
  configdialog->filltreeview();
  if(p.count()>1) configdialog->hidederivedplot();
  else if(p.at(0)->plottype!=0) configdialog->hidederivedplot();
  if(fromtable) configdialog->disablestores();
 }
 configdialog->show();
 configdialog->raise();
 configdialog->activateWindow();


 }
 void plotcanvas::dplots(QList<QVariant> list,QString plotname)
 {
     emit plots(list,plotname);
 }
 void plotcanvas::editevents()
 {
     DefineEvents dialog;
     dialog.itime=data.at(0)->timei;
     dialog.ltime=data.at(0)->timef;
     dialog.settimewidgets();
     dialog.databasename=databasename;
     dialog.fillevents();

     int result;
     result = dialog.exec();
     if(result){

     }
 }
 void plotcanvas::getdatabaseinfo( QList<QVariant> dinfo, int index) {
     if(index!=mindex){
         return;
     }else{
         timei=dinfo.at(2).toLongLong()*1000;
         timef=dinfo.at(3).toLongLong()*1000;
         if(ui->actionLinked_with_time_changes->isChecked()) {
             update(timei,timef);
             settimewidgets(timei,timef);
         }
     }

 }
 void plotcanvas::actingonlinkingtime(bool ck)
 {
     if(ck) update(timei,timef);

 }
 void plotcanvas::Syncthisplotwiththeguiperiod()
 {
     update(timei,timef);
     settimewidgets(timei,timef);
   }


   bool plotcanvas::eventFilter( QObject * watched, QEvent * event )
   {

       QWidget *canvas = qobject_cast<QWidget *>(watched);
       if ( canvas && event->type() == QEvent::MouseButtonPress )
       {
           QMouseEvent *mouseevent = (QMouseEvent *)event;
           if(mouseevent->button()==Qt::LeftButton){
               Ssubpage=-1;

               if (!rubberBand)
                   rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
               rubbersquarexi=mouseevent->pos().x();
               rubbersquareyi=mouseevent->pos().y();

               origin =this->mapFromGlobal(ui->graphicsView->mapToGlobal(mouseevent->pos()));

               rubberBand->setGeometry(QRect(origin, QSize()));
               rubberBand->show();
               double x0,y0;
               x0=mouseevent->pos().x()/(float)xlen ;
               y0=(ylen-mouseevent->pos().y())/(float)ylen ;
              // LogMessages("x0->"+QString::number(x0)+ " "+ QString::number(xlen));
              // LogMessages("y0->"+QString::number(y0)+ " "+ QString::number(ylen));
               for(int i=0;i<p.count();i++){
                   if(x0>=p.at(i)->xvminp && x0<= p.at(i)->xvmaxp){
                       if(y0>=p.at(i)->yvminp && y0<= p.at(i)->yvmaxp){
                           Ssubpage=i;
                           break;
                       }
                   }
               }

               //      qDebug() << mouseevent->pos().x() <<"  y=  " <<mouseevent->pos().y();
           } if(mouseevent->button()==Qt::RightButton){
               if (!menugraphicsview) {
                   menugraphicsview=new QMenu(this);
                   menugraphicsview->addAction(ui->actionFit_into_widget);
                   menugraphicsview->addAction(ui->actionConfigure_Plot);
               //    menugraphicsview->addAction(ui->actionScale);
                   menugraphicsview->addAction(ui->actionAuxiliary_lines);
               //    menugraphicsview->addAction(ui->actionExport_plot);
                   menugraphicsview->addAction(ui->actionLinked_with_time_changes);
                   menugraphicsview->addAction(ui->actionMake_a_Table);
                   menugraphicsview->addAction(ui->actionSavedata);
                   menugraphicsview->addAction(ui->actionSync_this_plot_with_the_period);

                   //          if() menugraphicsview.addAction()
               }
               menugraphicsview->exec(ui->graphicsView->mapToGlobal(mouseevent->pos()));
           }

       }else if ( canvas && event->type() == QEvent::MouseButtonRelease)
       {

           QMouseEvent *mouseevent = (QMouseEvent *)event;
           rubbersquarexf=mouseevent->pos().x();
           rubbersquareyf=mouseevent->pos().y();
            QMenu menu(this);
           menu.addAction(Acceptzomm);
           menu.addAction(Ignorezoom);
           if(Ssubpage!=-1){
               LogMessages("subpage="+QString::number(Ssubpage)+"type="+ QString::number(p.at(Ssubpage)->plottype));
               if(p.at(Ssubpage)->plottype==0)
               { menu.addAction(showvalues);
                 menu.addAction(showstats);
                 if(Ssubpage==0 && p.at(Ssubpage)->Multixais==1)menu.addAction(addlifetime);
               }
               else{
                   menu.removeAction(showvalues);
                   menu.removeAction(showstats);
               }

           }else{
               menu.removeAction(showvalues);
           }

        //   if(!Plot.screen)
        //  {
        //       if(!(Plot.plot[Ssubpage].Data.cloudid &CloudDax_enums::IDbanana) &&  !(Plot.plot[Ssubpage].Data.cloudid &CloudDax_enums::IDhisto))
        //       {
        //           menu.addAction(showvalues);
        //           menu.addAction(showspectra);
        //       }
        //   }

           menu.exec(ui->graphicsView->mapToGlobal(mouseevent->pos()));
           rubberBand->hide();
       }else if ( canvas && event->type() == QEvent::MouseMove)
       {
           QMouseEvent *mouseevent = (QMouseEvent *)event;
           if (!rubberBand)
               rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
           //       rubberBand->setGeometry(QRect(origin, mouseevent->pos()).normalized());;
           rubberBand->setGeometry(QRect(origin, this->mapFromGlobal(ui->graphicsView->mapToGlobal(mouseevent->pos()))).normalized());;
       }
       ///

       return QObject::eventFilter(watched, event);
   }

   void plotcanvas::createActions()
   {
       Acceptzomm = new QAction(tr("Zoom"), this);
       Acceptzomm->setStatusTip(tr("Redraw the plot only within the time of the selected area "));
       connect(Acceptzomm, &QAction::triggered, this, &plotcanvas::Acceptzoomaction);

       Ignorezoom = new QAction(tr("Cancel"), this);
       Ignorezoom->setStatusTip(tr("Cancel area selection"));
       connect(Ignorezoom, &QAction::triggered, this, &plotcanvas::Ignorezoomaction);

       showvalues=new QAction(tr("Showvalues"), this);
       showvalues->setStatusTip(tr("Show curves values"));
       connect(showvalues, &QAction::triggered, this, &plotcanvas::showvaluesaction);

       showstats=new QAction(tr("ShowStats"), this);
       showstats->setStatusTip(tr("Show mean values"));
       connect(showstats, &QAction::triggered, this, &plotcanvas::showStatsaction);

       addlifetime=new QAction(tr("lifetime for first curve"), this);
       addlifetime->setStatusTip(tr("Shows decay fitting for the fist curve. Better use in a single curve multiaxisplot"));
       connect(addlifetime, &QAction::triggered, this, &plotcanvas::showlifetime);


     //  showspectra=new QAction(tr("showspectra"), this);
     //  showspectra->setStatusTip(tr("Show Spectra"));
     //  connect(showspectra, SIGNAL(triggered(bool)), this, SLOT(showspectra_triggered()));



   }
   void plotcanvas::Acceptzoomaction()
   {
       // warning isto só faz sentido para plots de uma unica subpage.

       QRectF spos,rpos;
       long long itime, ltime;
       qreal w;
       double fact,tfact,left,right;
       if(Ssubpage==-1) return;
       if(p.at(Ssubpage)->plottype==2) return;
       left=p.at(Ssubpage)->xvminp;
       right=p.at(Ssubpage)->xvmaxp;
       spos=scene.sceneRect();
       w=spos.width();
      // LogMessages("wspos->"+QString::number(w));
       fact=1/w;
       tfact=((double)(data.at(Ssubpage)->timef -data.at(Ssubpage)->timei))/(right-left);
       //rpos=srect->rect();
       int temp;
       if(rubbersquarexi >rubbersquarexf){
           temp=rubbersquarexi;
           rubbersquarexi=rubbersquarexf;
           rubbersquarexf=temp;
       }
       rpos.setCoords(rubbersquarexi,rubbersquareyi,rubbersquarexf,rubbersquareyf);
      // LogMessages("ruberx->"+QString::number(rubbersquarexi)+ " "+ QString::number(rubbersquarexf));
      // LogMessages("rubery->"+QString::number(rubbersquareyi)+ " "+ QString::number(rubbersquareyf));


       if(fact*(rpos.right())< left){ emit logmessagesR("not a valid area"); return;}
       if(fact*(rpos.left())> right){ emit logmessagesR("not a valid area"); return;}

       if(fact*(rpos.left())< left) itime=data.at(Ssubpage)->timei;
       else{
           itime= data.at(Ssubpage)->timei+ (long long)((fact*(rpos.left())-left)*tfact);
       }
       if(fact*(rpos.right())>  right) ltime=data.at(Ssubpage)->timef;
       else{
           ltime=  data.at(Ssubpage)->timei+  (long long)((fact*(rpos.right())-left)*tfact);
       }

       if(ltime- itime >10 ) {
           if(ui->actionAuxiliary_lines->isChecked()){
               ui->actionAuxiliary_lines->setChecked(false);
               ui->actionAuxiliary_lines->triggered(false);
               //return false;
           }
           update(itime,ltime);
           ui->actionLinked_with_time_changes->setChecked(false);
           ui->initialtime->setDateTime(QDateTime::fromMSecsSinceEpoch(data.at(Ssubpage)->timei).toUTC());
           ui->finaltime->setDateTime(QDateTime::fromMSecsSinceEpoch(data.at(Ssubpage)->timef).toUTC());

       }
   }
   void plotcanvas::Ignorezoomaction()
   {

   }
    void plotcanvas::showlifetime(){

        if(!data.at(0)->xy.at(0)->x.size()) return;


          std::vector<double> xxx,yyy;
          std::vector<double> xxd,yyd;

          xxx.clear();yyy.clear(); xxd.clear();yyd.clear();
          for(int i=0;i< data.at(0)->xy.at(0)->x.size(); i++){
            if( data.at(0)->xy.at(0)->x.at(i)<data.at(0)->timei/1000. || data.at(0)->xy.at(0)->x.at(i)>data.at(0)->timef/1000.) continue;
            xxd.push_back( data.at(0)->xy.at(0)->x.at(i));
            yyd.push_back( data.at(0)->xy.at(0)->y.at(i));
          }
            if(!xxd.size()) return;
             double slope, y_intercept, ymini;
            PleastSqrRegression(&xxd[0],&yyd[0],(int)xxd.size(), slope, y_intercept, ymini);

            PLFLT xx0, xx1, yy0, yy1, xxi;
            xxi = xxd[0];
            double average;
            average = yyd[0];
            for (int k = 1; k < (int) xxd.size(); k++) {
                xx0 =xxd[k - 1];
                xx1 =xxd[k];
                yy0 = ymini + y_intercept *exp((xx0 - xxi) * slope);
                yy1 = ymini + y_intercept *exp((xx1 - xxi) * slope);
                xxx.push_back(xx0);xxx.push_back(xx1);
                yyy.push_back(yy0);yyy.push_back(yy1);

 //              plsjoin(xx0, yy0, xx1, yy1);
                average = average + yyd[k];
            }

                average = average / (float)xxd.size();

                plot.Plot_page(xlen,ylen,  "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
                plot.SetInicialColormap();
                plot.setfont(p.at(0)->fontset);
                plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);
                typlot(0,data.at(0)->timei, data.at(0)->timef);



            plot.lifetime(xxx,yyy,average,slope,y_intercept, ymini,xxi, p.at(0)->xvminp, p.at(0)->xvmaxp, p.at(0)->yvminp, p.at(0)->yvmaxp);


            plot.deleteStream();
            renderplot();
    }


void plotcanvas::PleastSqrRegression(PLFLT *x, PLFLT *y, int dataSize, double& slope, double& y_intercept, double& ymini)
    {
        double SUMx     = 0; //sum of x values
        double SUMy     = 0; //sum of y values
        double SUMxy    = 0; //sum of x * y
        double SUMxx    = 0; //sum of x^2
     //   double SUMres   = 0; //sum of squared residue
     //   double res      = 0; //residue squared
     //   double SUM_Yres = 0;
        double AVGy     = 0; //mean of y
        double AVGx     = 0; //mean of x
     //   double Yres     = 0; //squared of the discrepancies
     //   double Rsqr     = 0; //coefficient of determination
        double yy, x0;
        double A1;

        slope       = 0; //slope of regression line
        y_intercept = 0; //y intercept of regression line

        ymini = 1.e38;
        x0    = x[0];
        int n;


        for (int i = 0; i < dataSize; i++) {
            ymini = std::min(ymini, y[i]);
        }

     //   for (int k = 1; k < 4; k++) { //só faço dois ciclos porque de facto não faz sentido a convergência porque não possso te logaritmos de números negativos.

        //1º interação co ymin, depois calcular novo ymin com o slope, depois o novo slop com o ymin mas ignorando os pontos abixo do ymin.

        //depois um novo y min e paramos por aqui caso contrário acabaremos a ignorar quase toda a cuada e o backgound a subir muito.
        //    Este processo não funciona. vou apenas fazer o caso linear com ymin=ao valor mínimo da função.
             SUMx     = .0;
             SUMy     = .0;
             SUMxy    = .0;
             SUMxx    = .0;
             n=0;
            //calculate various sums
            for (int i = 0; i < dataSize; i++) {
                //sum of x
                yy = (y[i] - ymini);
                if (yy < 1.) {
                     continue; //para evitar valores muito elevados do log que estragam os resultados.
                               // desta forma os valores do y perto do ymin são ignorados.
                              // e evitar casos de y-y0 negativos
                }
                n++;
                SUMx = SUMx + (x[i] - x0);
                //sum of y
                SUMy = SUMy + log(yy);
                //sum of squared x*y
                SUMxy = SUMxy + (x[i] - x0) * log(yy);
                //sum of squared x
                SUMxx = SUMxx + (x[i] - x0) * (x[i] - x0);
            }

            //calculate the means of x and y
            AVGy = SUMy / n;
            AVGx = SUMx / n;

            //slope or a1
            slope = (n * SUMxy - SUMx * SUMy) / (n * SUMxx - SUMx * SUMx);

            //y itercept or a0
            y_intercept = AVGy - slope * AVGx;
            A1          = exp(y_intercept);
            y_intercept = A1;
    }


   void plotcanvas::showvaluesaction(){

       // warning isto só faz sentido para plots de uma unica subpage.

       QRectF spos,rpos;
       long long itime, ltime;
       qreal w;
       double fact,tfact,left,right;
       if(Ssubpage==-1) return;
       if(p.at(Ssubpage)->plottype!=0) return;
       left=p.at(Ssubpage)->xvminp;
       right=p.at(Ssubpage)->xvmaxp;
       spos=scene.sceneRect();
       w=spos.width();
       fact=1/w;
       tfact=((double)(data.at(Ssubpage)->timef-data.at(Ssubpage)->timei))/(right-left);
       //rpos=srect->rect();
       int temp;
       if(rubbersquarexi >rubbersquarexf){
           temp=rubbersquarexi;
           rubbersquarexi=rubbersquarexf;
           rubbersquarexf=temp;
       }
       rpos.setCoords(rubbersquarexi,rubbersquareyi,rubbersquarexf,rubbersquareyf);


       if(fact*(rpos.right())< left){ emit logmessagesR("not a valid area"); return;}
       if(fact*(rpos.left())> right){ emit logmessagesR("not a valid area"); return;}

       if(fact*(rpos.left())< left) itime=data.at(Ssubpage)->timei;
       else{
           itime= data.at(Ssubpage)->timei+ (long long)((fact*(rpos.left())-left)*tfact);
       }
       if(fact*(rpos.right())>  right) ltime=data.at(Ssubpage)->timef;
       else{
           ltime=  data.at(Ssubpage)->timei+  (long long)((fact*(rpos.right())-left)*tfact);
       }

       QString message;
       message= QDateTime::fromMSecsSinceEpoch(itime).toUTC().toString("yyyy/MM/dd hh:mm:ss");
       message.append("\n");

       for(int i=0; i< data.at(Ssubpage)->xy.count();i++){
           for(int j=0; j<(int )data.at(Ssubpage)->xy.at(i)->x.size()-1; j++){
               if(itime>=1000.*data.at(Ssubpage)->xy.at(i)->x.at(j) && itime<1000.*data.at(Ssubpage)->xy.at(i)->x.at(j+1)){
                   message.append(data.at(Ssubpage)->xy.at(i)->p.yaxisLabel+"="+QString::number(data.at(Ssubpage)->xy.at(i)->y.at(j)));
                   if(data.at(Ssubpage)->xy.at(i)->p.onlysmooth||data.at(Ssubpage)->xy.at(i)->p.smooth ){
                       message.append("->Smooth value="+QString::number(data.at(Ssubpage)->xy.at(i)->ysmooth.at(j)));
                   }

                   break;
               }
           }
           if(i< data.at(Ssubpage)->xy.count()-1) message.append("\n");

       }

       QMessageBox msgBox;
       msgBox.setText(message);
       msgBox.exec();
       //     qDebug().noquote() << message;
   }


   void plotcanvas::showStatsaction(){

       // warning isto só faz sentido para plots de uma unica subpage.

       QRectF spos,rpos;
       long long itime, ltime;
       qreal w;
       double fact,tfact,left,right;
       if(Ssubpage==-1) return;
       if(p.at(Ssubpage)->plottype!=0) return;
       left=p.at(Ssubpage)->xvminp;
       right=p.at(Ssubpage)->xvmaxp;
       spos=scene.sceneRect();
       w=spos.width();
       fact=1/w;
       tfact=((double)(data.at(Ssubpage)->timef-data.at(Ssubpage)->timei))/(right-left);
       //rpos=srect->rect();
       int temp;
       if(rubbersquarexi >rubbersquarexf){
           temp=rubbersquarexi;
           rubbersquarexi=rubbersquarexf;
           rubbersquarexf=temp;
       }
       rpos.setCoords(rubbersquarexi,rubbersquareyi,rubbersquarexf,rubbersquareyf);


       if(fact*(rpos.right())< left){ emit logmessagesR("not a valid area"); return;}
       if(fact*(rpos.left())> right){ emit logmessagesR("not a valid area"); return;}

       if(fact*(rpos.left())< left) itime=data.at(Ssubpage)->timei;
       else{
           itime= data.at(Ssubpage)->timei+ (long long)((fact*(rpos.left())-left)*tfact);
       }
       if(fact*(rpos.right())>  right) ltime=data.at(Ssubpage)->timef;
       else{
           ltime=  data.at(Ssubpage)->timei+  (long long)((fact*(rpos.right())-left)*tfact);
       }

       QString message;
       message= QDateTime::fromMSecsSinceEpoch(itime).toUTC().toString("yyyy/MM/dd hh:mm:ss");
       message.append( " to " );
       message.append(QDateTime::fromMSecsSinceEpoch(ltime).toUTC().toString("yyyy/MM/dd hh:mm:ss"));

       message.append("\n");

       for(int i=0; i< data.at(Ssubpage)->xy.count();i++){
           std::vector<double> yy;
           yy.clear();
           for(int j=0; j<(int )data.at(Ssubpage)->xy.at(i)->x.size()-1; j++){
               if(1000.*data.at(Ssubpage)->xy.at(i)->x.at(j) >= itime && 1000.*data.at(Ssubpage)->xy.at(i)->x.at(j+1)<=ltime){
                   yy.push_back(data.at(Ssubpage)->xy.at(i)->y.at(j));
               }
           }
           if(yy.size()==0){
              message.append(data.at(Ssubpage)->xy.at(i)->p.yaxisLabel+" No data in the sellected interval");
           }else{
               double mean;
               mean=0.0;
               for(auto k=0; k<yy.size();k++){
                   mean=mean+yy.at(k);
               }
               mean=mean/((double) yy.size() );
               message.append(data.at(Ssubpage)->xy.at(i)->p.yaxisLabel+" Mean Value="+QString::number(mean));
           }

           if(i< data.at(Ssubpage)->xy.count()-1) message.append("\n");

       }

       QMessageBox msgBox;
       msgBox.setText(message);
       msgBox.exec();
       //     qDebug().noquote() << message;
   }


   void plotcanvas::Make_a_Table_triggered()
   {
       Plot2Table dialog;

       if(!xyp.count()){
           QVector <xyproprieties *> xp;
           QVector <histoproprieties *> hp;
           for(int i=0; i<data.count(); i++){
               xp.clear();
               hp.clear();
               for(int j=0; j<data.at(i)->xy.count(); j++ ){
                   xp << &data.at(i)->xy.at(j)->p;
               }
               xyp<< xp;
               for(int j=0; j<data.at(i)->histo.count(); j++ ){
                   hp << &data.at(i)->histo.at(j)->p;
               }
               histop<<hp;
               xyzp <<&data.at(i)->xyz.p;

           }
       }

       dialog.fillcurvesSurface(&p,&xyp,&histop,&xyzp);
       int result;
       result = dialog.exec();

       if(result){

          emit addtable(&p,&data,dialog.subpagina,dialog.element);

   }
   }


   void plotcanvas::Savedata_triggered()
   {

       SavedataPlot dialog;
       QString timestring,separator,sep,filedataName;
       if(!xyp.count()){
           QVector <xyproprieties *> xp;
           QVector <histoproprieties *> hp;
           for(int i=0; i<data.count(); i++){
               xp.clear();
               hp.clear();
               for(int j=0; j<data.at(i)->xy.count(); j++ ){
                   xp << &data.at(i)->xy.at(j)->p;
               }
               xyp<< xp;
               for(int j=0; j<data.at(i)->histo.count(); j++ ){
                   hp << &data.at(i)->histo.at(j)->p;
               }
               histop<<hp;
               xyzp <<&data.at(i)->xyz.p;

           }
       }

       dialog.fillcurvesSurface(&p,&xyp,&histop,&xyzp);
       int result;
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
               QMessageBox::warning(this, tr("MDI"),
                                    tr("Cannot write file %1:\n%2.")
                                        .arg(filedataName)
                                        .arg(file.errorString()));
               emit logmessagesR("couldn't open de file to save data");
               return;
           }

           QTextStream out(&file);

           out.setRealNumberNotation(QTextStream::SmartNotation);
           out.setRealNumberPrecision(10);


           QString lixo;
           int nlines=0;
           lixo.clear();
           int curves=0;

           for(int s=0;s<dialog.subpagina.count();s++){
               if(p.at(dialog.subpagina.at(s))->plottype ==0){

                    if(s>0)  lixo.append(sep);
                    lixo.append("time");
                    lixo.append(sep);
                    lixo.append(data.at(dialog.subpagina.at(s))->xy.at(dialog.element.at(s))->p.yaxisLabel);
                    nlines=std::max(nlines, (int) data.at(dialog.subpagina.at(s))->xy.at(dialog.element.at(s))->x.size());
                    curves++;
               }

               }

               if(curves) {
                   out <<  lixo.toLatin1().constData() <<Qt::endl;

           for (int i = 0; i < nlines; i++) {
               lixo.clear();
               for (int j = 0; j < dialog.subpagina.count(); j++) {
                   if(j!=0)lixo.append(sep);
                   if (i < (int) data.at(dialog.subpagina.at(j))->xy.at(dialog.element.at(j))->x.size()){
                       if(timestring.contains("unixtime"))lixo.append(QString::number((long long) data.at(dialog.subpagina.at(j))->xy.at(dialog.element.at(j))->x.at(i)));
                       else if (timestring.contains("igor"))lixo.append(QString::number((long long)
                                                           (data.at(dialog.subpagina.at(j))->xy.at(dialog.element.at(j))->x.at(i)+2082844800)));
                       else if (timestring.contains("unix_mili_time"))lixo.append(QString::number((long long)
                                                           (data.at(dialog.subpagina.at(j))->xy.at(dialog.element.at(j))->x.at(i)*1000)));
                       else if (timestring.contains("matlabtime"))lixo.append(QString::number(
                               (data.at(dialog.subpagina.at(j))->xy.at(dialog.element.at(j))->x.at(i)/86400.+719529),'g',12));

                       else  lixo.append(QDateTime::fromMSecsSinceEpoch((long long) (data.at(dialog.subpagina.at(j))->xy.at(dialog.element.at(j))->x.at(i))*1000).toUTC().toString(timestring));
                       lixo.append(sep);
                       lixo.append(QString::number(data.at(dialog.subpagina.at(j))->xy.at(dialog.element.at(j))->y.at(i),'g',12));
                   }else lixo.append(sep);
               }

               out << lixo.toLatin1().constData() <<Qt::endl;

           }
            file.close();
           return;
               }
           lixo.clear();
           for(int s=0;s<dialog.subpagina.count();s++){
               if(p.at(dialog.subpagina.at(s))->plottype ==1){
                   lixo.append("time");


                   for (int i = 0; i < (int) data.at(dialog.subpagina.at(s))->xyz.y.size(); i++) {
                       lixo.append(sep);
                       lixo.append(QString::number(data.at(dialog.subpagina.at(s))->xyz.y.at(i),'g',12));
                   }
                   out << lixo.toLatin1().constData() <<Qt::endl;
                   for (int i = 0; i <  (int) data.at(dialog.subpagina.at(s))->xyz.x.size(); i++) {
                       lixo.clear();
                       if(timestring.contains("unixtime"))lixo.append(QString::number((long long) data.at(dialog.subpagina.at(s))->xyz.x.at(i)));
                       else if (timestring.contains("igor"))lixo.append(QString::number((long long)
                                                           (data.at(dialog.subpagina.at(s))->xyz.x.at(i)+2082844800)));
                       else if (timestring.contains("unix_mili_time"))lixo.append(QString::number((long long)
                                                           (data.at(dialog.subpagina.at(s))->xyz.x.at(i)*1000)));
                       else if (timestring.contains("matlabtime"))lixo.append(QString::number(
                               (data.at(dialog.subpagina.at(s))->xyz.x.at(i)/86400.+719529),'g',12));

                       else  lixo.append(QDateTime::fromMSecsSinceEpoch((long long) (data.at(dialog.subpagina.at(s))->xyz.x.at(i))*1000).toUTC().toString(timestring));
                       for (int k = 0; k <( int) data.at(dialog.subpagina.at(s))->xyz.y.size(); k++) {
                           lixo.append(sep);
                           lixo.append(QString::number(data.at(dialog.subpagina.at(s))->xyz.z.at(k).at(i),'g',12));

                       }
                       out << lixo.toLatin1().constData() <<Qt::endl;
                   }
                   file.close();
                   return;
               }

           }
           // histo

           nlines=0;
           lixo.clear();
           int histo=0;
           for(int s=0;s<dialog.subpagina.count();s++){
               if(p.at(dialog.subpagina.at(s))->plottype ==2){
                   if(s>0)  lixo.append(sep);
                   lixo.append("binnames");
                   lixo.append(sep);
                   lixo.append("values");

                   nlines=std::max(nlines, (int)data.at(dialog.subpagina.at(s))->histo.at(dialog.element.at(s))->y.size());
                   histo++;
               }
           }
           if(histo){
           out <<  lixo.toLatin1().constData() <<Qt::endl;
           for (int j=0; j<nlines; j++){
               lixo.clear();
               for(int s=0;s<dialog.subpagina.count();s++){
                   if(p.at(dialog.subpagina.at(s))->plottype ==2){
                       if(s!=0)lixo.append(sep);
                       if(j< (int)data.at(dialog.subpagina.at(s))->histo.at(dialog.element.at(s))->y.size()){

                           lixo.append(data.at(dialog.subpagina.at(s))->histo.at(dialog.element.at(s))->x.at(j));
                           lixo.append(sep);
                           lixo.append(QString::number(data.at(dialog.subpagina.at(s))->histo.at(dialog.element.at(s))->y.at(j),'g',12));
                       }

                   }
               }
               out << lixo.toLatin1().constData() <<Qt::endl;
           }
       }
           // end histo
           file.close();

       }

   }

   void plotcanvas::Export_plot_triggered()
   {
       SavePlot dialog;
       int result;
       result = dialog.exec();
       if(result){

           QString lixo;
           lixo=p.at(0)->Plotname;

           QString fileName;
           QString DeviceName=dialog.device;

           if(DeviceName.contains("pdfqt")){
               lixo.append(".pdf");

               fileName = QFileDialog::getSaveFileName(this, tr("Export plot  as"),
                                                       lixo, tr("pdf files (*.pdf);;All files (*)"));
           }else if (DeviceName.contains("svgqt")){
               lixo.append(".svg");
               fileName = QFileDialog::getSaveFileName(this, tr("Export plot  as"),
                                                       lixo, tr("svg files (*.svg);;All files (*)"));

           }else if (DeviceName.contains("xfig")){
               lixo.append(".fig");
               fileName = QFileDialog::getSaveFileName(this, tr("Export plot  as"),
                                                       lixo, tr("xfig  files (*.fig *.xfig);;All files (*)"));
           }else if (DeviceName.contains("pngqt")){
               lixo.append(".png");
               fileName = QFileDialog::getSaveFileName(this, tr("Export plot  as"),
                                                       lixo, tr("png files (*.png);;All files (*)"));
           }else if (DeviceName.contains("psc")){
               lixo.append(".ps");
               fileName = QFileDialog::getSaveFileName(this, tr("Export plot  as"),
                                                       lixo, tr("ps files (*.ps *.eps);;All files (*)"));
           }

           if (fileName.isEmpty()) {
               return;
           }
           plot.Plot_page(xlen,ylen,  DeviceName, fileName, p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
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


       }
   }

   void plotcanvas::Scale_triggered()
   {
       Plotscale dialog;
       int result;
       result = dialog.exec();
       if(result){
           double xfactor,yfactor;
           xfactor=dialog.xfactor;
           yfactor=dialog.yfactor;
           scene.clear();
           this->ui->graphicsView->resetCachedContent();
           ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
           ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
           ui->graphicsView->setSceneRect(0, 0, (int)(xfactor *xlen),(int)(yfactor * ylen));

           cursorbusy();

           plot.Plot_page((int)(xfactor *xlen),(int)(yfactor * ylen), "memqt", "", p.at(0)->bred, p.at(0)->bgreen, p.at(0)->bblue, p.at(0)->balpha);
           plot.SetInicialColormap();
           plot.setfont(p.at(0)->fontset);
           plot.setcharheight(p.at(0)->charheight,p.at(0)->fontsize*p.at(0)->hdpifont);

           for(int i=0;i<p.count();i++)
           {
               if(p.at(i)->plottype==0)typlot(i,data.at(0)->timei, data.at(0)->timef);
               if(p.at(i)->plottype==1)Splot(i,data.at(0)->timei, data.at(0)->timef);
               if(p.at(i)->plottype==2)Hplot(i);
           }

           plot.getxlenylen(&xlen,&ylen);

           plot.deleteStream();
           renderplot();

           freecursor();

//           ui->actionAutomaticaly_fit_plot_to_window->setChecked(true);
           //   ui->graphicsView->scene()->setSceneRect(QRectF());
           //  ui->act(false);
       }
   }
void plotcanvas::setglobaltime(){
    emit timeinterval(data.at(0)->timei,data.at(0)->timef);
   }

void plotcanvas::timefromruns()
   {

       TimeFromRuns dialog;
       dialog.setWindowTitle("time from runs");
       dbapi table;

       table.connectionName=databaseconnection;
       connect (&table,&dbapi::logmessages,this,&plotcanvas::LogMessages);
       connect (&table,&dbapi::logmessagesR,this,&plotcanvas::LogMessagesR);
       QStringList cnames;
       QList<std::vector<QString> * >  runrows;


       cnames.clear();
       //   cnames <<"StartTime" <<"RunNumber" <<"Type" <<"Description" <<"EndTime"<<"Comments" ;
       cnames <<"RunNumber" <<"StartTime" <<"EndTime"<<"Type" <<"Description"  <<"Comments" ;
       for(int i=0; i<cnames.count(); i++) runrows << new ( std::vector<QString>);
       table.fechnotimecolumns_string_orderby("Runtable", cnames, "RunNumber",
                                              &runrows);
       model.m_rowCount=runrows.at(0)->size();
       model.rownames.clear();
       model.startrun.clear();
       model.endrun.clear();
       model.description.clear();
       model.commentaries.clear();
       model.type .clear();
       model.startc.clear();
       model.endc.clear();

       for(int i=0;i<(int) runrows.at(0)->size(); i++){

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
       connect(&model,&TimeRunsModel::checkchanged, &dialog,&TimeFromRuns::check);
       dialog.fill();
       int result;
       result = dialog.exec();
       if(result){
           model.vscrollpos=dialog.Getvscrollpos();
           ui->initialtime->setDateTime(dialog.Itime);
           ui->finaltime->setDateTime(dialog.Ltime);
           ui->minustime->setValue(0);
           ui->plustime->setValue(0);
           if(Monitoring) {
               ui->actionContinuously_updated->setChecked(true);

           }
           Treplot();
       }

   }

void plotcanvas::timefromevents()
   {

       TimeFromRuns dialog;
       dialog.setWindowTitle(databasename);
       struct Events {
           QString RunNumber;
           long long StartTime;
           long long EndTime;
           QString Type;
           QString Description;
           QString Comments;

       };
       QString Eventsfor;
       Eventsfor="Eventsfor";
       Eventsfor.append(databasename);
       QList<Events> runevents;
       runevents.clear();
       QSettings settings("CampaignAnalysis",Eventsfor);
       int size = settings.beginReadArray("Events");
       for (int i = 0; i < size; ++i) {
           settings.setArrayIndex(i);
           Events runs;
           runs.RunNumber = settings.value("RunNumber").toString();
           runs.StartTime = settings.value("StartTime").toLongLong();
           runs.EndTime = settings.value("EndTime").toLongLong();
           runs.Type = settings.value("Type").toString();
           runs.Description = settings.value("Description").toString();
           runs.Comments = settings.value("Comments").toString();
           runevents.append(runs);
       }
       settings.endArray();

       model.m_rowCount=runevents.count();
       model.rownames.clear();
       model.startrun.clear();
       model.endrun.clear();
       model.description.clear();
       model.commentaries.clear();
       model.type .clear();
       model.startc.clear();
       model.endc.clear();
       for(int i=0;i< runevents.count(); i++){
           model.rownames <<  runevents.at(i).RunNumber;
           model.startrun << QDateTime::fromMSecsSinceEpoch(runevents.at(i).StartTime).toUTC().toSecsSinceEpoch();
           model.endrun << QDateTime::fromMSecsSinceEpoch(runevents.at(i).EndTime).toUTC().toSecsSinceEpoch();
           model.description <<  runevents.at(i).Description;
           model.commentaries <<   runevents.at(i).Comments;
           model.type <<  runevents.at(i).Type;
           model.startc << 0;
           model.endc << 0;

       }

       dialog.model=&model;
       connect(&model,&TimeRunsModel::checkchanged, &dialog,&TimeFromRuns::checkevent);
       dialog.fill();
       int result;
       result = dialog.exec();
       if(result){
           model.vscrollpos=dialog.Getvscrollpos();
           ui->initialtime->setDateTime(dialog.Itime);
           ui->finaltime->setDateTime(dialog.Ltime);
           ui->minustime->setValue(0);
           ui->plustime->setValue(0);
           Treplot();

       }

   }
   void plotcanvas::Treplot()
   {
 //      bool lixo;
 //      lixo= ui->actionContinuously_updated->isChecked();

       if(Monitoring){
 //        bool lixo;
 //        lixo= ui->actionContinuously_updated->isChecked();
         manualplot();
         fitintowidget();
         return;
       }
       long long initialtime, finaltime;
       initialtime=ui->initialtime->dateTime().toUTC().toMSecsSinceEpoch()-60000*ui->minustime->value();
       finaltime=ui->finaltime->dateTime().toUTC().toMSecsSinceEpoch()+60000*ui->plustime->value();
       update(initialtime,finaltime);
       settimewidgets(initialtime,finaltime);
       ui->minustime->setValue(0);
       ui->plustime->setValue(0);
       fitintowidget();

   }

   void plotcanvas::playmovie()
   {

        if(itimemovie > ltimemovie){
           animate->stop();
           ui->actionMovie->setChecked(false);
           for(int i=0;i<p.count();i++)
           {
               data.at(i)->movie=false;
           }
           return;
       }
       update(itimemovie,itimemovie+intervalmovie);
       settimewidgets(itimemovie,itimemovie+intervalmovie);

       itimemovie=itimemovie+stepmovie;

   }

   void plotcanvas::Movie_triggered()
   {

       if(!ui->actionMovie->isChecked()) {
           if(Monitoring) ui->actionContinuously_updated->setChecked(false);
           if(animate->isActive()) animate->stop();
           for(int i=0;i<p.count();i++)
           {
               data.at(i)->movie=false;
           }
           return;
       }
       if(Monitoring) ui->actionContinuously_updated->setChecked(true);
       Movie dialog;
       dialog.setinitialtime(data.at(0)->timei,data.at(0)->timef+5*60*60*1000); //5 horas por defeito
       int result;
       result = dialog.exec();
       if(result){
           itimemovie=dialog.itimemovie;
           ltimemovie=dialog.ltimemovie;
           stepmovie=dialog.stepmovie;
           intervalmovie=dialog.interval;
           animate->start(dialog.refreshmovie);
           update(itimemovie,itimemovie+intervalmovie); //aqui tudo bem porque movie is still false
           itimemovie=itimemovie+stepmovie;
           for(int i=0;i<p.count();i++)
           {
               data.at(i)->movie=true;
           }

       }else{
           ui->actionMovie->setChecked(false);

       }

   }

void plotcanvas::settimewidgets(long long ti, long long tf){
       ui->initialtime->setDateTime(QDateTime::fromMSecsSinceEpoch(ti).toUTC());
       ui->finaltime->setDateTime(QDateTime::fromMSecsSinceEpoch(tf).toUTC());

   }

   void plotcanvas::Auxiliary_lines_triggered(bool checked)
   {
       if(!checked){
           scene.removeItem(hline1);
           scene.removeItem(vline1);
           scene.removeItem(hline2);
           scene.removeItem(vline2);
       }else{
           qreal h,w;
           h=scene.height();
           w=scene.width();
           hline1=scene.addLine(QLineF(0.,h/4., w,h/4.),QPen(Qt::black, 2, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
           vline1=scene.addLine(QLineF(w/4.,0,w/4.,h),QPen(Qt::black, 2, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
           hline1->setFlag(QGraphicsItem::ItemIsMovable);
           hline1->setSelected ( true );
           vline1->setFlag(QGraphicsItem::ItemIsMovable);
           vline1->setSelected ( true );

           hline2=scene.addLine(QLineF(0.,3*h/4., w,3*h/4.),QPen(Qt::black, 2, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
           vline2=scene.addLine(QLineF(3*w/4.,0,3*w/4.,h),QPen(Qt::black, 2, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
           hline2->setFlag(QGraphicsItem::ItemIsMovable);
           hline2->setSelected ( true );
           vline2->setFlag(QGraphicsItem::ItemIsMovable);
           vline2->setSelected ( true );

       }
       ui->graphicsView->show();
   }
void plotcanvas::LogMessages(QString mess)
{
    emit logmessages(mess);
}
void plotcanvas::LogMessagesR(QString text)
{

    emit logmessagesR(text);


}
void plotcanvas::plotrefresh()
{
   int interval;
   ui->finaltime->setDateTime(current_time.currentDateTime().toTimeSpec(Qt::UTC));
   interval=ui->hourstimeinterval->value()*60*60+ui->minutestimeinterval->value()*60;
   ui->initialtime->setDateTime(current_time.currentDateTime().toTimeSpec(Qt::UTC).addSecs(-interval));
   update(current_time.currentDateTime().toTimeSpec(Qt::UTC).addSecs(-interval).toMSecsSinceEpoch(),current_time.currentDateTime().toTimeSpec(Qt::UTC).toMSecsSinceEpoch() );

}
void plotcanvas::choosemode() {
    if(Monitoring){
        connect(ui->actionLinked_with_time_changes,&QAction::triggered,this,&plotcanvas::synctimewithserver);
        connect(ui->actionContinuously_updated,&QAction::toggled, this,&plotcanvas::changingmode);
    outsideTime=new QTimer(this);
    ui->onlyinAnalysis->setHidden(true);
    ui->Byevents->setHidden(true);
    ui->actionMake_a_Table->setEnabled(false);
    ui->actionDefine_store_events->setEnabled(false);
    ui->actionSync_this_plot_with_the_period->setEnabled(false);
    ui->actionSet_overall_time_equal_to_this_window_time->setEnabled(false);
    ui->toolBar->removeAction(ui->actionMake_a_Table);
    ui->toolBar->removeAction(ui->actionDefine_store_events);
    ui->toolBar->removeAction(ui->actionSync_this_plot_with_the_period);
    ui->toolBar->removeAction(ui->actionSet_overall_time_equal_to_this_window_time);
    for(int i=0;i<p.count();i++)
    {
        data.at(i)->movie=true;
    }
    ui->actionLinked_with_time_changes->setToolTip("Time Sync With  a Local server controler");
    Mtimeinterval.push_back(ui->timeinterval1); Mtimeinterval.push_back(ui->timeinterval2); Mtimeinterval.push_back(ui->timeinterval3);
    Mtimeinterval.push_back(ui->timeinterval4); Mtimeinterval.push_back(ui->timeinterval5); Mtimeinterval.push_back(ui->timeinterval6);
    Mtimeinterval.push_back(ui->timeinterval7); Mtimeinterval.push_back(ui->timeinterval8); Mtimeinterval.push_back(ui->timeinterval9);
    Mtimeinterval.push_back(ui->timeinterval10); Mtimeinterval.push_back(ui->timeinterval11); Mtimeinterval.push_back(ui->timeinterval12);
    timeintervalvalues.push_back(5); timeintervalvalues.push_back(10); timeintervalvalues.push_back(20);
    timeintervalvalues.push_back(30); timeintervalvalues.push_back(45); timeintervalvalues.push_back(60);
    timeintervalvalues.push_back(90); timeintervalvalues.push_back(120); timeintervalvalues.push_back(180);
    timeintervalvalues.push_back(360); timeintervalvalues.push_back(720); timeintervalvalues.push_back(1440);

    intervalsignalMapper=new QSignalMapper(this);
    for(int i=0; i<12; i++){
    connect(Mtimeinterval.at(i), SIGNAL(clicked()),
                intervalsignalMapper,SLOT(map()) );
        intervalsignalMapper->setMapping(Mtimeinterval.at(i),i);
    }
    connect(intervalsignalMapper, &QSignalMapper::mappedInt,
                this, &plotcanvas::buttonclicked);

    connect(this,&plotcanvas::buttonclicked, this, &plotcanvas::predefinedintervals);
    connect(ui->hourstimeinterval,&QSpinBox::valueChanged,this, &plotcanvas::timeintervalchanged);
    connect(ui->minutestimeinterval,&QSpinBox::valueChanged,this, &plotcanvas::timeintervalchanged);
    connect(ui->initialtime,&QAbstractSpinBox::editingFinished,this, &plotcanvas::manualtimeintervalchanged);
    connect(ui->finaltime,SIGNAL(editingFinished()),this, SLOT(manualtimeintervalchanged()));
  //  connect(ui->actionContinuously_updated,&QAction::toggled,this, &plotcanvas::setplotrefresh);
    connect(&refreshplot, &QTimer::timeout,this, &plotcanvas::plotrefresh);
    connect(outsideTime,&QTimer::timeout,this, &plotcanvas::getoutsidetime);
    setplotrefresh();
    ui->timeinterval3->setChecked(true);

    }else{
      ui->onlyinMonitoring->setHidden(true);
      ui->actionContinuously_updated->setEnabled(false);
      ui->toolBar->removeAction(ui->actionContinuously_updated);
      connect(ui->actionLinked_with_time_changes,&QAction::triggered,this, &plotcanvas::actingonlinkingtime);

    }
}

void plotcanvas::predefinedintervals(int i)
{
 //fprintf(stderr, "butão %d\n", i);
    if(Mtimeinterval.at(i)->isChecked()){
        for(int k=0;k<12;k++){
           if(k!=i) Mtimeinterval.at(k)->setChecked(false);
        }
        ui->minutestimeinterval->setValue(timeintervalvalues.at(i)%60);
        ui->hourstimeinterval->setValue(timeintervalvalues.at(i)/60);
    }
}

void plotcanvas::timeintervalchanged()
{
  int minutes;
  minutes=ui->minutestimeinterval->value()+60*ui->hourstimeinterval->value();
  for(int i=0; i<12; i++)
  {
    if(minutes == timeintervalvalues.at(i))Mtimeinterval.at(i)->setChecked(true);
    else Mtimeinterval.at(i)->setChecked(false);
  }

      if(!ui->actionContinuously_updated->isChecked()){
          for(int i=0;i<p.count();i++)
          {
              data.at(i)->movie=false;
          }

      plotrefresh();
      for(int i=0;i<p.count();i++)
      {
          data.at(i)->movie=true;
      }

      }

}
void plotcanvas::manualtimeintervalchanged()
{
  int minutes;
  long long segundos;
  segundos=ui->finaltime->dateTime().toSecsSinceEpoch()-ui->initialtime->dateTime().toSecsSinceEpoch();
  minutes=((int) segundos)/60;
  ui->actionContinuously_updated->setChecked(true);
 // ui->minutestimeinterval->setValue(minutes%60);
 // ui->hourstimeinterval->setValue(minutes/60);


}
void plotcanvas::manualplot()
{

    if(!ui->actionContinuously_updated->isChecked())plotrefresh();
    else {
        for(int i=0;i<p.count();i++)
        {
            data.at(i)->movie=false;
        }
    update(ui->initialtime->dateTime().toTimeSpec(Qt::UTC).toMSecsSinceEpoch(),ui->finaltime->dateTime().toTimeSpec(Qt::UTC).toMSecsSinceEpoch());
    }
}
void plotcanvas::changingmode(bool state)
{

    if(refreshplot.isActive())refreshplot.stop();
    if(state){
            mbredd=p.at(0)->bred;
            mbgreend=p.at(0)->bgreen;
            mbblued=p.at(0)->bblue;
            p.at(0)->bred=mbred;
            p.at(0)->bgreen=mbgreen;
            p.at(0)->bblue=mbblue;
      }else{
             p.at(0)->bred=mbredd;
             p.at(0)->bgreen=mbgreend;
             p.at(0)->bblue=mbblued;
           }
      replot();
    setplotrefresh();

    }


void plotcanvas::setplotrefresh()
{

  if(refreshplot.isActive()){refreshplot.stop();
      for(int i=0;i<p.count();i++)
      {
          data.at(i)->movie=false;
      }

      update(ui->initialtime->dateTime().toUTC().toMSecsSinceEpoch(),ui->finaltime->dateTime().toUTC().toMSecsSinceEpoch());
  }
  if(!ui->actionContinuously_updated->isChecked()){
      refreshplot.start(1000*ui->refreshrate->value());
      for(int i=0;i<p.count();i++)
      {
          data.at(i)->movie=false;//para garantir que na primeira chamada não é feita em modo filme.
      }

      plotrefresh();
      for(int i=0;i<p.count();i++)
      {
          data.at(i)->movie=true;
      }

      update(ui->initialtime->dateTime().toUTC().toMSecsSinceEpoch(),ui->finaltime->dateTime().toUTC().toMSecsSinceEpoch());

  }else {
      for(int i=0;i<p.count();i++)
      {
          data.at(i)->movie=false;
      }
       update(ui->initialtime->dateTime().toUTC().toMSecsSinceEpoch()-1,ui->finaltime->dateTime().toUTC().toMSecsSinceEpoch());
  }

}
void plotcanvas::synctimewithserver()
{
    if(ui->actionLinked_with_time_changes->isChecked()){
       ui->actionContinuously_updated->setChecked(true);
       ui->actionContinuously_updated->setDisabled(true);
       ui->actionMovie->setChecked(false);
       ui->actionMovie->setDisabled(true);
       ui->dockWidget->setDisabled(true);
       ui->actionSmall_time_adjustments->setDisabled(true);
       outsideTime->start(1000);

    }else{
        outsideTime->stop();
        ui->actionMovie->setDisabled(false);
        ui->actionContinuously_updated->setDisabled(false);
        ui->dockWidget->setDisabled(false);
        ui->actionSmall_time_adjustments->setDisabled(false);

    }
}
void plotcanvas::getoutsidetime()
{
    QFile localpipe("clovisoutsidetime.txt");
    if(!localpipe.exists()) return;
    QTextStream in(&localpipe);
    if(localpipe.open(QFile::ReadOnly)){
      long long it,lt;
      QString line;
      QStringList times;
      line=in.readLine();
      times=line.split("\t");
      if(times.count()!=2) return;
      it=times.at(0).toLongLong();
      lt=times.at(1).toLongLong();
      if(it <0){
          ui->actionLinked_with_time_changes->setChecked(false);
          ui->actionContinuously_updated->setChecked(false);
          ui->actionSmall_time_adjustments->setDisabled(false);
          ui->actionContinuously_updated->setDisabled(false);
          ui->actionMovie->setDisabled(false);
          ui->dockWidget->setDisabled(false);

      }
      if(it== gitime && lt==gltime) return;
      if(it>=lt) return;
      if(it <946684800 || lt > 4102444800  ) return; //inicio do ano 2000 até inici0 do ano 2100
      gitime=it;
      gltime=lt;
      update(1000*gitime,1000*gltime);
    }

}
void plotcanvas::setinterval(int hours, int minutes){
   ui->hourstimeinterval->setValue(hours);
   ui->minutestimeinterval->setValue(minutes);
}
void plotcanvas::cursorbusy()
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
}
void plotcanvas::freecursor()
{
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

}
