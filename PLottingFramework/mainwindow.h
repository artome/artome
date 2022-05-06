#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include "plotproprieties.h"
#include "plotting.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString databaseconnection;

private slots:
   void firsttest();
   void testebanana();
   void testebananaxy();
   void testehisto1();
   void testehisto2();
   void LogMessages(QString mess);
   void LogMessagesR(QString mess);

private:
    Ui::MainWindow *ui;
    QString serverheader;
    void ConnectDatabase();
    QGraphicsScene scene;
    QStringList colors;
};
#endif // MAINWINDOW_H
