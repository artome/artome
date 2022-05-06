#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QDateTime>
#include <QMap>
#include <QSignalMapper>
#include <QTextStream>
#include <QCloseEvent>
#include "databases.h"
#include "definingscreens.h"
#include "plplot.h"
#include "channelwindow.h"
#include "plotcanvas.h"
#include "managingtime.h"
#include "tablecanvas.h"
#include "downloaditensthread.h"
#include "TextView.h"
#include "editlocalplots.h"
#include "statisticswindow.h"
#include "inststatus.h"
#include "maketext.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     QStringList OpenDatabases;
      PLFLT hdpifont,hdpithickeness,charheight,hdpi;
      double hdpiwindows,wfontsize;
      void scaledock();

public slots:

      void getdatabaseinfo( QList <QList<QVariant> >, int) ;
      void getdatabasechanged(QString, QString, int);
      void channeldialogDoIT(QStringList,QStringList,QStringList,QList<QVariant>, int );
      void LogMessage(QString mess);
      void LogMessageR(QString mess);
      void gettimefromtimedialog(QList<QVariant>, int);
      void timefromcanvas(long long, long long);
      void addtablefromplot(QVector<plotproprieties *> *,QVector<plotdata *>*,QList<int>, QList<int>);
      void Enddownloadthread();
      void createplot_fromEditlocal(QList<QVariant>,QString );
      void addactionfromtable(TableCanvas *,int);
      void plotall( QList<QVariant> );
      void doreport(QList <QList<QVariant> >);
  protected:

      void closeEvent(QCloseEvent *event);

private slots:
     void databasedialog();
     void setdpifactores();
     void Listdatabaseitens();
     void setActiveSubWindow(QWidget *window);
     void setActiveSubWindowO(QObject *window);
     void updateWindowMenu();
     plotcanvas *activeplotcanvas();
     void addplotcanvas(plotcanvas *);
     void addtablecanvas(TableCanvas *);
     void selectwindows(QModelIndex);
     void createplots(QModelIndex);
     void createplotsI(QModelIndex);
     void lateralwidgetsactions(int );
     void helpondefiningscreens();
     void Inststatusdialog();

     void choosetimeinterval();
     void addTextcanvas(TextView *);
     void addreportcanvas(MakeText *);
     void viewelog();
     void viewreport();
     void viewdatatdesc();
     TableCanvas *activetablecanvas();
     TextView *activeTextView();
     MakeText *activeMakeText();
     StatisticsWindow *activestatisticswindow();
     void editlocalplotsdialog();
     void addstatisticswindow(StatisticsWindow *);
     void Import_Table_for_banana();

signals:

     void sendcurrentdatabase(QString);
     void senddatabases(QStringList);
     void timechanged(long long, long long, int);
     void senttime(QList <QDateTime>, QList <QDateTime>,QList <QString>, QList <QString>);
     void senddatabaseinfo( QList<QVariant>, int );

private:
    Ui::MainWindow *ui;
    Databases *opendatabasesdialog;
    channelWindow *listchanneltree;
    QList <QList<QVariant> > databaseinfo;
    int currentindex;
    QString currentconnection,currentdatabasename,currentuser,currentserver,currentpassword;
    QDateTime Itime,Ltime;
    long long itime;
    long long ltime;
    bool superuser;
    QSignalMapper *windowMapper;
    QAction       *closeAct;
    QAction       *closeAllAct;
    QAction       *tileAct;
    QAction       *cascadeAct;
    QAction       *nextAct;
    QAction       *previousAct;
    QAction       *separatorAct;
    void WindowsActions();
    QStandardItemModel explorermodel;
    void cursorbusy();
    void freecursor();
    void plotsfilltree();
    void plotsfilltreeInstr();

    QStandardItemModel listplotmodel,listplotmodelInst;
    QList <QStandardItem *> plostInst;
    void createplot_fromlist(QList<QVariant> );
    bool downloadingintofile;
    ManagingTime *managingtimedialog;
    DownloadItensThread Dthread;
    Editlocalplots *Editlocalplotsdialog;
    DefiningScreens *screendialog;
    InstStatus *inststatusdialog;
    void plottyfromlist(QString );
    void plotSfromlist(QString );
    void plotHfromlist(QString );

};
#endif // MAINWINDOW_H
