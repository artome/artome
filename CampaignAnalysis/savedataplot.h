#ifndef SAVEDATAPLOT_H
#define SAVEDATAPLOT_H

#include <QDialog>
#include <QStandardItemModel>
#include "plotdata.h"

namespace Ui {
class SavedataPlot;
}

class SavedataPlot : public QDialog
{
    Q_OBJECT
    
public:
    explicit SavedataPlot(QWidget *parent = 0);
    ~SavedataPlot();

    QStandardItemModel model;
    void fillcurvesSurface(QVector<plotproprieties *> *,
                           QVector <QVector <xyproprieties *> > *,
                           QVector <QVector <histoproprieties *> > *,
                           QVector <xyzproprieties *> *);
    QString fileName,timeString,separator;

    QList <int> subpagina;
    QList <int> element;
    QString suggestname;

private slots:
    void doIt();
    void readvalues();
    void settimestrings(int);
    void savetimesettings();

private:

    Ui::SavedataPlot *ui;
    QList<QStandardItem *> subpageslabels;
    void filltimestrings();
    void readtimesettings();

};

#endif // SAVEDATAPLOT_H
