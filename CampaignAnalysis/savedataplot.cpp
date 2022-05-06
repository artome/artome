#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

#include "savedataplot.h"
#include "ui_savedataplot.h"

SavedataPlot::SavedataPlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SavedataPlot)
{
    ui->setupUi(this);
    filltimestrings();
    readtimesettings();
    connect(ui->save, &QAbstractButton::pressed,this,&SavedataPlot::doIt);
    connect(ui->okbutton, &QAbstractButton::pressed,this,&SavedataPlot::readvalues);
    connect(ui->sometexttimeformats, &QComboBox::activated, this,&SavedataPlot::settimestrings);
    connect(ui->savedefault, &QAbstractButton::pressed, this,&SavedataPlot::savetimesettings);

}

SavedataPlot::~SavedataPlot()
{
    delete ui;
}

void SavedataPlot::doIt(){

fileName = QFileDialog::getSaveFileName(this, tr("Save data as"), suggestname, tr("ascii text files (*.csv *.txt *.dat);;All files (*)"));
if (fileName.isEmpty()) {
    fileName="NOT SET TRY AGAIN";
    ui->okbutton->setDisabled(true);
}
ui->save->setText(fileName);
 ui->okbutton->setDisabled(false);
}
void SavedataPlot:: savetimesettings()
{
    QString setstring;
    setstring.clear();
    if(ui->Igortime->isChecked()) setstring.append("igor"); else if (ui->unixtime->isChecked())setstring.append("unixtime");
    else if (ui->TimeString->isChecked())setstring.append(ui->datef->text());else if (ui->miliunixtime->isChecked())setstring.append("unix_mili_time");
    else if (ui->matlabtime->isChecked())setstring.append("matlabtime");
QSettings settings("CampaignAnalysis", "data");
settings.setValue("savedataplot/savetimesformat", setstring);
}
void SavedataPlot:: readtimesettings()
{
    QString setstring;
    QSettings settings("CampaignAnalysis", "data");
    setstring=settings.value("savedataplot/savetimesformat").toString();

    if(setstring.contains("igor")) ui->Igortime->setChecked(true);
    else if (setstring.contains("unixtime")) ui->unixtime->setChecked(true);
    else if (setstring.contains("unix_mili_time")) ui->miliunixtime->setChecked(true);
    else if (setstring.contains("matlabtime")) ui->matlabtime->setChecked(true);
    else {
        ui->TimeString->setChecked(true);
        if(!setstring.count())setstring="yyyy/MM/dd hh:mm:ss";
        ui->datef->setText(setstring);

      }
}

void SavedataPlot::readvalues()
{

    if(ui->Igortime->isChecked()) timeString="igor"; else if (ui->unixtime->isChecked())timeString="unixtime";
    else if (ui->miliunixtime->isChecked())timeString="unix_mili_time";else if (ui->matlabtime->isChecked())timeString="matlabtime";
    else if (ui->TimeString->isChecked())timeString=ui->datef->text();
    if(ui->comma->isChecked()) separator="comma"; else if (ui->tab->isChecked()) separator="tab";
    else if(ui->space->isChecked())separator="space";
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


       this->accept();
}

void SavedataPlot::settimestrings(int index)
{
    ui->datef->setText(ui->sometexttimeformats->itemText(index));
}

void SavedataPlot::filltimestrings()
{
    ui->sometexttimeformats->addItem("yyyy/MM/dd hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyy/MM/dd hh:mm:ss.zzz", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyy-MM-dd hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyy-MM-dd hh:mm:ss.zzz", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yy/MM/dd hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yy-MM-dd hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("MM/dd/yyyy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("MM-dd-yyyy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("MM/dd/yy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("MM-dd-yy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("dd/MM/yyyy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("dd-MM-yyyy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("dd/MM/yy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("dd-MM-yy hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyMMddhh", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyMMddhhmm", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyMMddhhmmss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyyMMddhh", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyyMMddhhmm", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyyMMddhhmmss", Qt::DisplayRole);

}



void SavedataPlot::fillcurvesSurface(QVector<plotproprieties *> *p,
                                        QVector <QVector <xyproprieties *> > *xyp,
                                        QVector <QVector <histoproprieties *> > *histop,
                                        QVector <xyzproprieties *> *xyzp)
{

    suggestname.append(p->at(0)->Plotname);
    suggestname.replace(":", "");
    suggestname.append(".csv");

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
