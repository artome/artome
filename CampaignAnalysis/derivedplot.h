#ifndef DERIVEDPLOT_H
#define DERIVEDPLOT_H

#include <QDialog>
#include <QStandardItemModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>

namespace Ui {
class DerivedPlot;
}

class DerivedPlot : public QDialog
{
    Q_OBJECT

public:
    explicit DerivedPlot(QWidget *parent = nullptr);
    ~DerivedPlot();
    void fillcurves();

    QStringList plot;
    QString databaseconnection;

signals:
    void plots(QList<QVariant>,QString );

private slots:

void   select(QModelIndex );
void   erasefromlist(QModelIndex );
void   dplot();
void   additem();

private:
    Ui::DerivedPlot *ui;
    QStandardItemModel itemmodel;
    QStandardItemModel itemmodeltoplot;
    QStandardItem   *itemplot  ;
    QString sc,sv,ss;
    QString sepf,sepc,atf;

};

#endif // DERIVEDPLOT_H
