#ifndef DEFINEEVENTS_H
#define DEFINEEVENTS_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class DefineEvents;
}

class DefineEvents : public QDialog
{
    Q_OBJECT

public:
    explicit DefineEvents(QWidget *parent = 0);
    ~DefineEvents();
    long long itime;
    long long ltime;
    QString databasename;
    void settimewidgets();
    void fillevents();

private slots:
    void addevent();
    void fillfileevents();
    void commitevent();
    void removeevent();
    void updateevent();
    void editevent();
    void sellect(QStandardItem *);
    void sellectd(QStandardItem *);


private:
    Ui::DefineEvents *ui;
    QStandardItemModel listeventmodel;
    QStandardItemModel dlisteventmodel;

};

#endif // DEFINEEVENTS_H
