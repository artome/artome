#include "movie.h"
#include "ui_movie.h"

Movie::Movie(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Movie)
{
    ui->setupUi(this);
    connect(ui->startbutton, &QAbstractButton::pressed,this, &Movie::getvalues);
}

Movie::~Movie()
{
    delete ui;
}

void Movie::setinitialtime(long long itime, long long ltime)
{
   ui->Itime->setDateTime(QDateTime::fromMSecsSinceEpoch(itime).toUTC());
   ui->Ltime->setDateTime(QDateTime::fromMSecsSinceEpoch(ltime).toUTC());

}
void Movie::getvalues()
{
    itimemovie=ui->Itime->dateTime().toMSecsSinceEpoch();
    ltimemovie=ui->Ltime->dateTime().toMSecsSinceEpoch();
    stepmovie=(long long)ui->step->value()*60*1000;
    refreshmovie=(long long)ui->rate->value();
    interval=(long long)ui->timeinterval->value()*60*1000;

}
