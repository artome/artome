#ifndef CONFIGPLOTWIDGET_H
#define CONFIGPLOTWIDGET_H

#include <QDialog>
#include <QStandardItemModel>
#include <QTemporaryFile>

#include "plotproprieties.h"
#include "xyproprieties.h"
#include "xyzproprieties.h"
#include "histoproprieties.h"
#include "derivedplot.h"

namespace Ui {
class configplotwidget;
}

class configplotwidget : public QDialog
{
    Q_OBJECT

public:
    explicit configplotwidget(QWidget *parent = nullptr);
    ~configplotwidget();
    QVector<plotproprieties *> *p;
    QVector <QVector <xyproprieties *> > *xyp;
    QVector <QVector <histoproprieties *> > *histop;
    QVector <xyzproprieties *> *xyzp;
    void filltreeview();
    QString databaseconnection;
    void hidederivedplot();
    void disablestores();
signals:
    void replot();
    void logmessagesR(QString);
    void logmessages(QString);
    void dplots(QList<QVariant>,QString);
public slots:
    void LogM(QString);
    void LogMR(QString);
    void createplot_fromEditlocal(QList<QVariant>,QString);
private slots:
    void fillall(QModelIndex);
    void replotslot();
    void changebackcolor();
    void changecurvecolor();
    void storeplotindatabase();
    void writesettings();
    void buildcolorpalete();
    void setpredefinedcolor(int);
    void colorpalete(QString);
    void changehistocolor();
    void storeplotinlocaldrive();
private:
    Ui::configplotwidget *ui;
    QStandardItemModel model;
    QList<QStandardItem *> subpageslabels;
    void enabledisabletabs(QString);
    int nplot,ncurve,nsurf,nhisto;
    QString sc,sv,ss;
    QString curvesp,plotp,closanstables,surfacep,histosp;
    void readvalues();
    void setbackcolor( );
    void filllinestyle();
    void fillfontset();
    void fillhistotype();
    void fillthick();
    void fillmarkers();
    void fillcolorsScheme();
    void setcurvecolor();
    void sethistocolor(QString );
    bool globalpage;
    void createstringstostorety(int);
    void createstringstostoresurface(int);
    void createstringstostorehisto(int);
    void createderivedplot();
    QString setstring;
    void readsettings();
    QTemporaryFile tempfile; //legenda
    QString histocolor;
    DerivedPlot *derivedplotdialog;
};

#endif // CONFIGPLOTWIDGET_H
