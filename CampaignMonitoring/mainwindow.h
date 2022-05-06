#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QDateTime>
#include <QStandardItemModel>
#include <QTimer>
#include "timerunsmodel.h"
#include "definerunlayouts.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool connected;
public slots:
    void ConnectDatabase();
    void LogMessage(QString);
    void LogMessageR(QString);
private slots:

    void timefromruns();
    void writeinterface();
    void sendtime();
    void unsynk();
    void last2h();
    void last4h();
    void last8h();


 void changeslavepath(QString);
 void callslave(QModelIndex);
 void callslaveI(QModelIndex);
 void callslaveR(QModelIndex);
 void killall();
 void layoutdialog();
 void refreshlists();

protected:

    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QTimer *refreshtimer;
    QString Monitoringdatabase;
    QString database,user,password,server;
    void plotsfilltree();
    void plotsfilltreeInstr();
    void runfilltree();
     QStandardItemModel listplotmodel,listplotmodelInst,listrunmodel;
    QList <QStandardItem *> plostInst;

    TimeRunsModel  model;
    QString Itimeslabel;
    QString Ltimeslabel;
    QDateTime Itime,Ltime,IItime,LLtime;
    QDateTime current_time,firsttime;
    QString slavepath;
    DefineRunLayouts *runlayouts;

};
#endif // MAINWINDOW_H
