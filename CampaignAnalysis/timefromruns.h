#ifndef TIMEFROMRUNS_H
#define TIMEFROMRUNS_H

#include <QDialog>
#include "timerunsmodel.h"

namespace Ui {
class TimeFromRuns;
}

class TimeFromRuns : public QDialog
{
    Q_OBJECT

public:
    explicit TimeFromRuns(QWidget *parent = 0);
    ~TimeFromRuns();

    QString Sstart, Send;
    QDateTime Itime,Ltime;
    int Getvscrollpos();
    int xrunnumber;
    int index;
    TimeRunsModel *model;
    void fill();
    void clearcheck();
    void hidesave();
public slots:
    void check(int,int,bool);
    void checkevent(int,int,bool);


private slots:
    void selectrun(const QModelIndex &);
    void showhelp();
    void save();
private:
    Ui::TimeFromRuns *ui;
};

#endif // TIMEFROMRUNS_H
