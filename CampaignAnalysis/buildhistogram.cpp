#include "buildhistogram.h"
#include "ui_buildhistogram.h"
#include <QMessageBox>
#include <cmath>
BuildHistogram::BuildHistogram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuildHistogram)
{
    ui->setupUi(this);
    connect(ui->okbutton,&QAbstractButton::pressed,this,&BuildHistogram::readvalues);
    connect(ui->checkManual,&QAbstractButton::toggled,this,&BuildHistogram::freemanual);
    connect(ui->setnob,&QAbstractButton::pressed,this,&BuildHistogram::evaluatenofbins);
    bins.clear();
}

BuildHistogram::~BuildHistogram()
{
    delete ui;
}
void BuildHistogram::freemanual(bool state)
{
    if(state)ui->Bins->setEnabled(true);else ui->Bins->setEnabled(false);
}
void BuildHistogram::evaluatenofbins()
{
    QString lixo;
    lixo=ui->Bins->text();
    bins=lixo.split(",");
    setnofbin(bins.count());


}

void BuildHistogram::setnofbin(int bins)
{
    ui->numberofbins->setText(QString::number(bins));
    nofbins=bins;
}

void BuildHistogram::readvalues()
{
    binlabels=ui->binlabels->text();
    initialvalue=ui->initialvalue->text().toDouble();
    finalvalue=ui->Finalvalue->text().toDouble();
    sampling=ui->Sampling->value();
    if(binlabels.count()){
      int nbins;
      nbins=0;
      QStringList bins1,bins2;
      bins1=binlabels.split(QRegularExpression(";"));
      for(int i=0; i<bins1.count(); i++ ){
          bins2=bins1.at(i).split(QRegularExpression(","));
          nbins=nbins+bins2.count();
      }
      if(nbins != nofbins){
          QMessageBox msgBox;
          msgBox.setText("Number of bins does not match the selected channels  "+QString::number(nofbins)+ " !=" +QString::number(nbins));
          msgBox.exec();
          return;
      }


    } else{

            double y;
            if(ui->log->isChecked()){
               for (int i=0; i<nofbins;i++)
               {

                   if(i%sampling==0){
                        y=log(ui->initialvalue->text().toDouble()) + i * (log(ui->Finalvalue->text().toDouble()) - log(ui->initialvalue->text().toDouble())) / (nofbins - 1.);
                        binlabels.append(QString::number(exp(y),'g',3));
                   }
                   if(i<nofbins-1)binlabels.append(",");

               }
           }else{
                for (int i=0; i<nofbins;i++)
                {
                    y=ui->initialvalue->text().toDouble() + i * (ui->Finalvalue->text().toDouble() - ui->initialvalue->text().toDouble()) / (nofbins - 1.);
                    binlabels.append(QString::number(y,'g',3));
                   if(i<nofbins-1)binlabels.append(",");
                }

           }

        }






    this->accept();
}
