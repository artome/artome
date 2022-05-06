#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString folder;
   void setextfolder();

private slots:
    void ConnectDatabase();
    void filllist(QString);
    void LogMessages(QString);
    void LogMessagesR(QString);
    void choosetable();
    void readEntry(QModelIndex);


private:
    Ui::MainWindow *ui;
    QString database,user,password,server,databaseconnection,serverheader;
    QString listlabel,listtable;
    QStandardItemModel listmodel;

};

#endif // MAINWINDOW_H
