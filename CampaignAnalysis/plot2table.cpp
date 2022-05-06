#include <QMessageBox>

#include "plot2table.h"
#include "ui_plot2table.h"

Plot2Table::Plot2Table(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plot2Table)
{
    ui->setupUi(this);
    connect(ui->okbutton, &QAbstractButton::pressed,this,&Plot2Table::readvalues);
    fromtable=false;
}

Plot2Table::~Plot2Table()
{
    delete ui;
}



void Plot2Table::readvalues()
{

   if(!fromtable){
   int surfaces=0;
   int curves=0;
   int histo=0;
    subpagina.clear();
    element.clear();
    QList<QVariant> list;

    for (int j = 0; j < subpageslabels.size(); j++) {
        for (int i = 0; i < subpageslabels[j]->rowCount(); i++) {
            if (subpageslabels[j]->child(i)->checkState()) {
              list = subpageslabels[j]->child(i)->data().toList();
              subpagina<<j;
              element<<i;

              if(list[0].toInt() == 1)curves++;
              if(list[0].toInt() == 2)surfaces++;
              if(list[0].toInt() == 3)histo++;

                }

            }
        }
       if(surfaces>1 || (surfaces==1 && (curves>0 || histo >0)) || (histo >0 && curves >0) ){
           QMessageBox msgBox;

           msgBox.setText("Too many non consistent selected fiels");
           msgBox.setInformativeText("Select as many curves as you want. \n But do not mixed them with surfaces or charts.\n If you are choosing surfaces you can only choose one at the time ");
           msgBox.setIcon(QMessageBox::Warning);
           msgBox.exec();
         return;
       }
  } else{
       listofcolumns.clear();
       QList<QVariant> list;

       for (int j = 0; j < model.invisibleRootItem()->rowCount(); j++) {
           if(model.invisibleRootItem()->child(j)->checkState()){
             list=model.invisibleRootItem()->child(j)->data().toList();
             listofcolumns << list[0].toInt();
           }
        }

}

       this->accept();
}

void Plot2Table::filltablecolumns(){
    QStandardItem *cparentItem = model.invisibleRootItem();
    QList<QVariant> cgrafid;
    QStringList crootData;
    model.setHorizontalHeaderLabels(crootData);
    for (int i=0;i<xymodel->columnCount(); i++)
     {
       if(xymodel->ctype.at(i) !=1) continue;
       QStandardItem *item = new QStandardItem(xymodel->headernames.at(i));
       cgrafid.clear();
       item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/spreadsheet5.svg")));
       item->setEditable(false);
       cgrafid <<i ;
       item->setData(cgrafid);
       item->setCheckable(true);
       item->setCheckState(Qt::Unchecked);
       cparentItem->appendRow(item);

    }
    ui->treeView->setModel(&model);
    fromtable=true;

}

void Plot2Table::fillcurvesSurface(QVector<plotproprieties *> *p,
                                   QVector <QVector <xyproprieties *> > *xyp,
                                   QVector <QVector <histoproprieties *> > *histop,
                                   QVector <xyzproprieties *> *xyzp)
{

        QStandardItem *cparentItem = model.invisibleRootItem();
        QList<QVariant> cgrafid;
        QStringList crootData;
        QString     ttip;
        long long   iparent;

        crootData.clear();
        crootData << "Subpages";
        model.setHorizontalHeaderLabels(crootData);
        //
        iparent = -1;
        subpageslabels.clear();
        for (int i = 0; i < p->count(); i++) {
            subpageslabels << new QStandardItem(p->at(i)->plottitle +" ->Subpage "+QString::number(i));
            cgrafid.clear();
            cgrafid  << iparent <<i;
            subpageslabels[i]->setData(cgrafid);
            ttip.clear();
            ttip.append(p->at(i)->plottitle +" in the subpage"+QString::number(i));
            subpageslabels[i]->setToolTip(ttip);


        }

        QString lixo;
        for(int subpage=0; subpage< p->count(); subpage++)
        {
            for(int i=0;i<xyp->at(subpage).count();i++)
            {
                cgrafid.clear();
                QStandardItem *item = new QStandardItem(xyp->at(subpage).at(i)->yaxisLabel);
                item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/educational15.svg")));
                item->setEditable(false);
                cgrafid <<1 <<subpage<< i; //tipe, subpage, numero da curva
                item->setData(cgrafid);
                item->setCheckable(true);
                ttip.clear();
                ttip.append(" column "+ xyp->at(subpage).at(i)->cname +" of the table "+ xyp->at(subpage).at(i)->tname);
                item->setToolTip(ttip);
                cgrafid.clear();
                subpageslabels[subpage]->appendRow(item);
            }
            if(p->at(subpage)->plottype==1){
                cgrafid.clear();
                QStandardItem *item = new QStandardItem(xyzp->at(subpage)->plotidentifier);
                item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/banana4.svg")));
                item->setEditable(false);
                cgrafid <<2 <<subpage; //tipe, subpage, numero da curva
                item->setCheckable(true);
                item->setData(cgrafid);

                subpageslabels[subpage]->appendRow(item);

            }
            for(int i=0;i<histop->at(subpage).count();i++)
            {
                cgrafid.clear();
                QStandardItem *item = new QStandardItem(histop->at(subpage).at(i)->plotidentifier);
                item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/chart15.svg")));
                item->setEditable(false);
                cgrafid <<3 <<subpage<< i; //tipe, subpage, numero do histo
                item->setData(cgrafid);
                item->setCheckable(true);
                cgrafid.clear();
                subpageslabels[subpage]->appendRow(item);
            }



        for (int j = 0; j < subpageslabels.count(); j++) {
            cparentItem->appendRow(subpageslabels[j]);
        }

        ui->treeView->setModel(&model);
        ui->treeView->expand(model.index(0,0,QModelIndex()));

    }
}
