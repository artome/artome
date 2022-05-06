#include "hdpifactores.h"
#include "ui_hdpifactores.h"
#include <QSettings>

Hdpifactores::Hdpifactores(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Hdpifactores)
{
    ui->setupUi(this);
    connect(ui->set,&QAbstractButton::clicked,this,&Hdpifactores::readvalues);
    connect(ui->store,&QAbstractButton::clicked,this,&Hdpifactores::storevalues);
    connect(ui->refresh,&QAbstractButton::clicked,this,&Hdpifactores::setdefault);
}

Hdpifactores::~Hdpifactores()
{
    delete ui;
}
void Hdpifactores::setdefault()
{
    ui->scalefonts->setValue(1.0);
    ui->scalethick->setValue(0.0);
    ui->scalewindows->setValue(1.0);
    ui->charheight->setValue(0.0);
    ui->dpi->setValue(0.0);
    ui->wfontsize->setValue(0.0);
    readvalues();
    storevalues();


}
void Hdpifactores::readvalues()
{
  fontfactor=ui->scalefonts->value();
  thickfactor=ui->scalethick->value();
  windowsfactor=ui->scalewindows->value();
  charheight=ui->charheight->value();
  dpi=ui->dpi->value();
  wfontsize=ui->wfontsize->value();

this->accept();

}
void Hdpifactores::storevalues(){

QSettings settings("CampaignAnalysis", "hdpi");
settings.setValue("scale/fonts",ui->scalefonts->value());
settings.setValue("scale/thick",ui->scalethick->value());
settings.setValue("scale/windows",ui->scalewindows->value());
settings.setValue("scale/char",ui->charheight->value());
settings.setValue("scale/dpi",ui->dpi->value());
settings.setValue("wfontsize/points",ui->wfontsize->value());
readvalues();

}
void Hdpifactores::fillvalues(double font, double thick,double window,double charh,double dd,double wf){

   ui->scalefonts->setValue(font);
   ui->scalethick->setValue(thick);
   ui->scalewindows->setValue(window);
   ui->charheight->setValue(charh);
   ui->dpi->setValue(dd);
   ui->wfontsize->setValue(wf);
}
