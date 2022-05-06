#ifndef PLOTCANVAS_H
#define PLOTCANVAS_H

#include <QMainWindow>
#include <QGraphicsView>
#include<QRubberBand>
#include <QGraphicsLineItem>
#include <QTimer>
#include <QSignalMapper>

#include "plotdata.h"
#include "configplotwidget.h"
#include "plplot.h"
#include "timerunsmodel.h"

namespace Ui {
class plotcanvas;
}

class plotcanvas : public QMainWindow
{
    Q_OBJECT

public:
    explicit plotcanvas(QWidget *parent = nullptr);
    ~plotcanvas();
    QVector<plotdata *> data;
    QVector<plotproprieties *> p;
    QVector <QVector <xyproprieties *> > xyp;
    QVector <QVector <histoproprieties *> > histop;
    QVector<xyzproprieties *> xyzp;
    PLFLT hdpi;
    QString databaseconnection;
    QString databasename;
    long long timei,timef;
    QStringList tnames, cnames,aliasname;
    QString diameters;
    QString Bins;
    PLINT xlenreport,ylenreport;
    double factorreport;

    void plotfromchannel(QString );
    void plottyfromlist(QString );
    void plotSfromlist(QString );
    void plotHfromlist(QString );
    void plottyfromlistl(QString );
    void plotSfromlistl(QString );
    void plotHfromlistl(QString );
    void plotAfromlist(QString );
    void plotAfromlistl(QString );
    void plottyfromfulllist(QList<QVariant> );
    void plotSfromfulllist(QList<QVariant> );
    void plotAfromfulllist(QList<QVariant> );
    void plotHfromfulllist(QList<QVariant> );
    void plottyfromtable(QStringList names,QList <std::vector<double> >,QList <std::vector<double> >);
    void plotfromtabledisabeactions();
    void plotSfromtable(QString, std::vector<long long>, QVector<std::vector<double> > );

    void plotSfromchannel(QString );
    void plotHfromchannel(QString );
    void settimewidgets(long long,long long);
    PLFLT dpi;
    int mindex;
    int type; // 0 ty, 1 surface, 2, histo, 10 screen

    void startinlinkedtimemode(); // plots  report
    bool forplotreport;

 // things needed only in monitoring
    bool Monitoring;
    void choosemode();
    bool fromtable;
signals:
    void addtable(QVector<plotproprieties *> *,QVector<plotdata *>*,QList<int>, QList<int>);
// things needed only in monitoring
    void buttonclicked(int );
    void plots(QList<QVariant>,QString);
public slots:
    void LogMessages(QString mess);
    void LogMessagesR(QString mess);
    void replot();
    void fitintowidget();
    void getdatabaseinfo( QList<QVariant> , int );
    void update(long long, long long);
    // things needed only in monitoring
    void plotrefresh();
    void setinterval(int , int );
    void dplots(QList<QVariant>,QString );

private slots:

    void configplot();
    void editevents();
    void Syncthisplotwiththeguiperiod();
    void actingonlinkingtime(bool);
    void setglobaltime();
    void Acceptzoomaction();
    void Ignorezoomaction();
    void showvaluesaction();
    void showStatsaction();
    void timefromruns();
    void timefromevents();
    void Treplot();
    void Make_a_Table_triggered();
    void Savedata_triggered();
    void Auxiliary_lines_triggered(bool checked);
    void Export_plot_triggered();
    void Scale_triggered();
    void playmovie();
    void Movie_triggered();
 // things needed only in monitoring
    void setplotrefresh();
    void predefinedintervals(int);
    void timeintervalchanged();
    void manualtimeintervalchanged();
    void synctimewithserver();
    void changingmode(bool);
    void getoutsidetime();
    void showlifetime();

private:
    Ui::plotcanvas *ui;
    Plotting plot;
    QStringList colors;
    void renderplot();
    QGraphicsScene scene;
    void typlot(int,long long,long long );
    void Splot(int,long long,long long  );
    void Hplot(int);


    void cursorbusy();
    void freecursor();
    bool eventFilter( QObject * , QEvent *  );
    double xlen,ylen;
    configplotwidget *configdialog;
    QString sc,sv,ss;
    void fillplotproprieties(plotproprieties *,QString);
    void fillxyproprieties(plotdata *,QString);
    void fillxyzproprieties(plotdata *,QString);
    void fillhistoproprieties(plotdata *,QString);
    void PleastSqrRegression(PLFLT *, PLFLT *, int , double& , double& , double& );
    QString getbins(QString);
    QRubberBand *rubberBand;
    int rubbersquarexi,rubbersquareyi,rubbersquarexf,rubbersquareyf;
    QPoint origin;
    QMenu *menugraphicsview;
    QAction *Ignorezoom;
    QAction *Acceptzomm;
    QAction *showvalues;
    QAction *showstats;
    QAction *addlifetime;
    void createActions();
    TimeRunsModel  model;
    int  Ssubpage;
    QGraphicsLineItem *hline1,*vline1,*hline2,*vline2;
    QTimer *animate;
    long long itimemovie, ltimemovie, stepmovie, intervalmovie;
     // things needed only in monitoring
     QVector<QPushButton*> Mtimeinterval;
     QVector<int> timeintervalvalues;
     QDateTime current_time;
     QSignalMapper *intervalsignalMapper;
     QTimer refreshplot;
     void manualplot();
    QTimer *outsideTime;
    int mbred,mbblue,mbgreen;
    int mbredd,mbblued,mbgreend;
    long long gitime,gltime;
signals:
    void logmessages(QString);
    void logmessagesR(QString);
    void timeinterval(long long, long long);

};

#endif // PLOTCANVAS_H
