#ifndef PLOT2TABLE_H
#define PLOT2TABLE_H

#include <QDialog>
#include <QStandardItemModel>
#include "plotdata.h"
#include "xytablemodel.h"

namespace Ui {
class Plot2Table;
}

class Plot2Table : public QDialog
{
    Q_OBJECT

public:
    explicit Plot2Table(QWidget *parent = 0);
    ~Plot2Table();
    QStandardItemModel model;
    void fillcurvesSurface(QVector<plotproprieties *> *,
                           QVector <QVector <xyproprieties *> > *,
                           QVector <QVector <histoproprieties *> > *,
                           QVector <xyzproprieties *> *);
    void filltablecolumns();
    QString fileName,timeString,separator;
    QList <int> subpagina;
    QList <int> element;
    xytablemodel *xymodel;
    QList <int> listofcolumns;

private slots:
    void readvalues();

private:
    Ui::Plot2Table *ui;
    QList<QStandardItem *> subpageslabels;
    bool fromtable;

};

#endif // PLOT2TABLE_H
