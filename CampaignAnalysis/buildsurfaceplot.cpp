#include "buildsurfaceplot.h"
#include "ui_buildsurfaceplot.h"
#include <QMessageBox>
#include <cmath>

buildsurfaceplot::buildsurfaceplot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::buildsurfaceplot)
{
    ui->setupUi(this);
    connect(ui->aceptbutton, &QAbstractButton::pressed,this,&buildsurfaceplot::readvalues);
    connect(ui->evaluate,&QAbstractButton::pressed,this,&buildsurfaceplot::evaluate);
}

buildsurfaceplot::~buildsurfaceplot()
{
    delete ui;
}
void buildsurfaceplot::writeinformation(QString text)
{
    ui->informationlabel->setText(text);
}
void buildsurfaceplot::readvalues()
{
    channelstring=ui->Diameters->text();
    if(channelstring.isEmpty()){
     QMessageBox msgBox;
     msgBox.setText("Please set de diameters first");
     msgBox.exec();
     return;
   }
    if(ui->log->isChecked()) yscale="log"; else yscale="linear";
    if(ui->checkManual->isChecked()){
       itens=ui->Bins->text();
    }else itens.clear();
    this->accept();
}

void buildsurfaceplot::evaluate()
{
    double ymin, ymax,y;
    channelstring.clear();
    ymin=ui->firstValue->text().toDouble();
    ymax=ui->lastValue->text().toDouble();
    if(ymax<=ymin || ymin <0  || ymax < 0){
     QMessageBox msgBox;
     msgBox.setText("last value should be larger than first value and both should be greater than zero");
     msgBox.exec();
     return;
   }
    if(ui->checkManual->isChecked()){
       QStringList lixo;
       lixo=ui->Bins->text().split(",");
       ndiameters=lixo.count();
    }
    channelstring.append(QString::number(ymin));
     if(ui->log->isChecked()){
        for (int i=1; i<ndiameters;i++)
        {
            channelstring.append(",");
            y=log(ymin) + i * ((std::log(ymax) - std::log(ymin)) / (ndiameters - 1.));
            channelstring.append(QString::number(std::exp(y)));
        }
    }else{
         for (int i=1; i<ndiameters;i++)
         {
             channelstring.append(",");
             y=ymin + i * ((ymax - ymin) / (ndiameters - 1.));
             channelstring.append(QString::number(y));
         }


    }
        ui->Diameters->setText(channelstring);

}
