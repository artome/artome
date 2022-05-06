#ifndef TABLECANVAS_H
#define TABLECANVAS_H

#include <QMainWindow>
#include "xytablemodel.h"
#include "plotdata.h"
#include <QDateTime>
namespace Ui {
class TableCanvas;
}

class TableCanvas : public QMainWindow
{
    Q_OBJECT

public:
    explicit TableCanvas(QWidget *parent = 0);
    ~TableCanvas();

    QString TableTitle;
    void filltable();
    xytablemodel xymodel;
    QDateTime Ltime,Itime;
    QStringList tnames,cnames;
    QStringList aliasname;
    QList<QVariant> ctypes;
    QString TableType;

    QString databaseconnection,databasename;
    int databaseindex;
    int igl;
    QString timeString;
    QList<int> columnstoplot;
    void settimetitle();
   QString fileName,timestring,separator, timestringformat;


   void filltablefromplot(QList<int> subpag, QList<int> element);

   void tablefromlocalfile(QString,QString);
   void tablefromlocalfilefornanana();
   void tablefromlocalformatedfile();
   void costumize();
   bool superuser;

   QVector<plotproprieties *> *p;
   QVector<plotdata *> *data;

public slots:
   void getdatabaseinfo( QList<QVariant> , int );

   // void timechanged(long long,long long);
  //  void updatetable();

private slots:
    void  savedata();
    void showhelp();
    void LogMessage(QString);
    void LogMessageR(QString);

    void Surface_Plot_triggered();
    void Make_a_plot_triggered();
    void Choose_columns_to_plot_triggered();
    void Column_Statistics_triggered();
    void Local_Polynomial_fit_triggered();
    void settimestrings(int);
    void Save_into_database_triggered();
    void Applydisplayformat_clicked();

signals:

    void logmessagesR(QString);
    void logmessages(QString);
    void addtable(TableCanvas *, int);

private:

private:
    Ui::TableCanvas *ui;
    QList<double> bananatime, bananadiameter;
    long long gettime(QStringList *);
    QList<int> indexes,colstime;
    QStringList colstimeformat;
    QStringList datevalues;
    QString     gtimeformat,gtimestring;
    QDateTime   time;
    bool eventFilter( QObject * , QEvent *  );
    QMenu *menutableview;
    void filltimestrings();
    void readtimesettings();
    void readvalues();
};

#endif // TABLECANVAS_H
