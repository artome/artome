#include "configplotwidget.h"
#include "ui_configplotwidget.h"
#include <QColorDialog>
#include <QMessageBox>
#include <QBrush>
#include <QSettings>
#include <QInputDialog>

#include "dbapi.h"
#include "builtcolorscheme.h"
#include "plplot_utils.h"

configplotwidget::configplotwidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configplotwidget)
{
    ui->setupUi(this);
    nplot=-1;
    nsurf=-1;
    ncurve=-1;
    nhisto=-1;
    globalpage=false;
    derivedplotdialog=nullptr;
    sc="|>";
    sv=":=";
    ss="|;";

    filllinestyle();
    fillfontset();
    fillthick();
    fillmarkers();
    fillcolorsScheme();
    fillhistotype();
   tempfile.open();

    enabledisabletabs("all");
    connect(ui->treeView, &QAbstractItemView::pressed,
            this, &configplotwidget::fillall);
    connect(ui->replot, &QAbstractButton::pressed,
            this, &configplotwidget::replotslot);
    connect(ui->backcolor,&QAbstractButton::pressed,this, &configplotwidget::changebackcolor);
    connect(ui->color,&QAbstractButton::pressed,this, &configplotwidget::changecurvecolor);
    connect(ui->storeplot,&QAbstractButton::pressed,this, &configplotwidget::storeplotindatabase);
    connect(ui->storelocal,&QAbstractButton::pressed,this, &configplotwidget::storeplotinlocaldrive);
    connect(ui->savescheme,&QAbstractButton::pressed,this, &configplotwidget::writesettings);
    connect(ui->cpredefined,&QComboBox::activated,this, &configplotwidget::setpredefinedcolor);
    connect(ui->colorscheme,&QLineEdit::textChanged,this, &configplotwidget::colorpalete);
    connect(ui->setcolormanual,&QAbstractButton::pressed,this, &configplotwidget::buildcolorpalete);
    connect(ui->hcolor,&QAbstractButton::pressed,this, &configplotwidget::changehistocolor);
    connect(ui->derivedplot,&QAbstractButton::pressed,this, &configplotwidget::createderivedplot);
}

configplotwidget::~configplotwidget()
{
    delete ui;
}
void configplotwidget::disablestores(){
    ui->storeplot->setDisabled(true);
    ui->storelocal->setDisabled(true);
    ui->derivedplot->setDisabled(true);
}
void configplotwidget::createderivedplot(){
    if(!derivedplotdialog){
        derivedplotdialog=new DerivedPlot(this);
        derivedplotdialog->databaseconnection=databaseconnection;
        derivedplotdialog->plot.clear();
        createstringstostorety(0);

             derivedplotdialog->plot<<p->at(0)->Plotname;  //plotname
             derivedplotdialog->plot<<closanstables; //columnsandtables
             derivedplotdialog->plot<< plotp;     // proprieties
             derivedplotdialog->plot<<curvesp; //curves
            derivedplotdialog->fillcurves();
            connect(derivedplotdialog,SIGNAL(plots(QList<QVariant>,QString)),this,SLOT(createplot_fromEditlocal(QList<QVariant>,QString)));

           }

     derivedplotdialog->show();
     derivedplotdialog->raise();
     derivedplotdialog->activateWindow();
}
void configplotwidget::createplot_fromEditlocal(QList<QVariant> list,QString plotname)
{
    emit dplots(list,plotname);
}

void configplotwidget::hidederivedplot()
{
 ui->derivedplot->setHidden(true);
}
void configplotwidget::enabledisabletabs(QString tab)
{
    if(tab=="all"){
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->global),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->curves),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->histo),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->surface),false);
    }else if(tab=="curves"){
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->global),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->curves),true);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->histo),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->surface),false);
    }else if(tab=="histo"){
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->global),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->curves),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->histo),true);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->surface),false);
    }else if(tab=="surface"){
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->global),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->curves),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->histo),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->surface),true);
    }else if(tab=="global"){
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->global),true);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->curves),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->histo),false);
        ui->CSHproprieties->setTabEnabled(ui->CSHproprieties->indexOf(ui->surface),false);
    }

}
void configplotwidget::setbackcolor()
{
    QColor color;
    QPalette palette;
    QBrush brush;
    color.setRgb(p->at(0)->bred,p->at(0)->bgreen,p->at(0)->bblue);

    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    ui->backcolor->setPalette(palette);

}
void configplotwidget::changebackcolor()
{

         QColor color = QColorDialog::getColor(Qt::black, this, "Background  color", QColorDialog::DontUseNativeDialog);
        if(!color.isValid())
        {
            QMessageBox::information(this, "Curve color", "Not a valid color");
        }
        QBrush brush;

    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    QPalette palette;
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    ui->backcolor->setPalette(palette);
    p->at(0)->bred=color.red();
    p->at(0)->bgreen=color.green();
    p->at(0)->bblue=color.blue();

}
void configplotwidget::fillall(QModelIndex index)
{
    readvalues();// está aqui porque assim sempre que se muda de curva ou tab os valores da curva anterior são lidos.
    ncurve=-1;
    nplot=-1;
    nsurf=-1;
    nhisto=-1;
    QStringList legin;
    QList<QVariant> list;
    list = model.itemFromIndex(index)->data().toList();
    if(list.at(0).toInt()==-1){
        globalpage=true;
        enabledisabletabs("global");
        ui->CSHproprieties->setCurrentIndex(ui->CSHproprieties->indexOf(ui->global));
        nplot=list.at(1).toInt();
        ui->subpageoptions->setTitle("Subpage #"+QString::number(nplot)+"---> "+p->at(nplot)->plottitle);
        ui->xvmin->setValue(p->at(nplot)->xvmin);
        ui->xvmax->setValue(p->at(nplot)->xvmax);
        ui->yvmin->setValue(p->at(nplot)->yvmin);
        ui->yvmax->setValue(p->at(nplot)->yvmax);
        if(p->at(nplot)->withoutxaxis) ui->withtimeaxis->setChecked(false);
        else ui->withtimeaxis->setChecked(true);
        if(p->at(nplot)->firsttimelabel) ui->firsttimelabel->setChecked(true);
        else ui->withtimeaxis->setChecked(false);
        ui->plottitle->setText(p->at(nplot)->plottitle);
        if(p->at(nplot)->runstart==0) ui->runstart->setChecked(true);
        else if (p->at(nplot)->runstart==1)ui->onlymiddle->setChecked(true);
        else ui->norunstart->setChecked(true);
        if(p->at(nplot)->stylex) ui->Gridx->setChecked(true);
        if(p->at(nplot)->styley) ui->Gridy->setChecked(true);

        //next proprieties are chosen to be uniforme in all page so it only matters what is set in the first plot

        ui->alphavalue->setValue(p->at(0)->balpha);
        ui->font->setCurrentIndex(p->at(0)->fontset-1);
        ui->fontsize->setValue(p->at(0)->fontsize);
        setbackcolor();


        if(p->at(nplot)->plottype==0){
            if(p->at(nplot)->Multixais==1)ui->multiaxis->setChecked(true);
            else if (p->at(nplot)->Multixais==0)ui->singleaxis->setChecked(true);
            else if (p->at(nplot)->Multixais==2)ui->twoaxis->setChecked(true);
            ui->multiaxissep->setText(QString::number(p->at(nplot)->multiaxissep));
            ui->labelsep->setText(QString::number(p->at(nplot)->labelsep));
            if(p->at(nplot)->leftaxislimits.count()){
                ui->allleftaxisvaluesflag->setChecked(true);
                ui->allfetaxisvalues->setText(p->at(nplot)->leftaxislimits);
            }
            if(p->at(nplot)->rightaxislimits.count()){
                ui->allrightaxisvaluesflag->setChecked(true);
                ui->allrightaxisvalues->setText(p->at(nplot)->rightaxislimits);
            }

    }

    }else if(list.at(0).toInt()==1){
        globalpage=false;
        enabledisabletabs("curves");
        ui->CSHproprieties->setCurrentIndex(ui->CSHproprieties->indexOf(ui->curves));
        nplot=list.at(1).toInt();
        ncurve=list.at(2).toInt();
        setcurvecolor();
        if(xyp->at(nplot).at(ncurve)->rightaxis) ui->right->setChecked(true);
        else  ui->left->setChecked(true);
        if(xyp->at(nplot).at(ncurve)->ylog) ui->log->setChecked(true);
        else ui->linear->setChecked(true);

        if(xyp->at(nplot).at(ncurve)->derivative)ui->derivative->setChecked(true);
        else if (xyp->at(nplot).at(ncurve)->onlysmooth)ui->onlysmooth->setChecked(true);
        else if (xyp->at(nplot).at(ncurve)->smooth)ui->smooth->setChecked(true);
        else ui->nosmooth->setChecked(true);
        if(xyp->at(nplot).at(ncurve)->derivative) ui->averagepoints->setValue(xyp->at(nplot).at(ncurve)->devnpoints);
        else ui->averagepoints->setValue(xyp->at(nplot).at(ncurve)->smoothpoints);
        if(!xyp->at(nplot).at(ncurve)->linetype.contains("line"))ui->linetype->setCurrentIndex(0);
        else ui->linetype->setCurrentIndex(xyp->at(nplot).at(ncurve)->curvestyle);
        if(!xyp->at(nplot).at(ncurve)->linetype.contains("markers"))ui->markers->setCurrentIndex(0);
        else ui->markers->setCurrentIndex(xyp->at(nplot).at(ncurve)->marker+1);
        ui->MarkersScale->setValue(xyp->at(nplot).at(ncurve)->markersscale);
        if(xyp->at(nplot).at(ncurve)->minfreelimits) ui->minfreelimits->setChecked(true);
        if(xyp->at(nplot).at(ncurve)->maxfreelimits) ui->maxfreelimits->setChecked(true);
        ui->min->setText(QString::number(xyp->at(nplot).at(ncurve)->Ymin));
        ui->max->setText(QString::number(xyp->at(nplot).at(ncurve)->Ymax));
        ui->thickness->setCurrentIndex(xyp->at(nplot).at(ncurve)->pwidth-1);
        ui->yfactor->setText(xyp->at(nplot).at(ncurve)->factor);
        ui->clabel->setText(xyp->at(nplot).at(ncurve)->yaxisLabel);

    }else if(list.at(0).toInt()==2)
    {
        globalpage=false;
        nsurf=0;
        enabledisabletabs("surface");
        ui->CSHproprieties->setCurrentIndex(ui->CSHproprieties->indexOf(ui->surface));
        nplot=list.at(1).toInt();
        ui->colorscheme->setText(xyzp->at(nplot)->Surfacecolor);
        if(xyzp->at(nplot)->ylog)ui->sylog->setChecked(true);else ui->sylinear->setChecked(true);
        if(xyzp->at(nplot)->zlog)ui->slog->setChecked(true);else ui->slinear->setChecked(true);
        ui->cmin->setText(QString::number(xyzp->at(nplot)->Zmin));
        ui->cmax->setText(QString::number(xyzp->at(nplot)->Zmax));
        if(xyzp->at(nplot)->maxfreelimits)ui->cmaxfreelimits->setChecked(true);else ui->cmaxfreelimits->setChecked(false);
        if(xyzp->at(nplot)->minfreelimits)ui->cminfreelimits->setChecked(true);else ui->cminfreelimits->setChecked(false);
        ui->slabel->setText(xyzp->at(nplot)->yaxisLabel);
        if(xyzp->at(nplot)->surfacetype=="tiled2d") ui->tiled->setChecked(true);
        else if(xyzp->at(nplot)->surfacetype=="contour")ui->contour->setChecked(true);
        if(xyzp->at(nplot)->colorlimits=="Bizarreway")ui->Bizarre->setChecked(true);
        legin=xyzp->at(nplot)->Surfacelegendinside.split(";");
        ui->leginsidexsize->setValue(legin.at(0).toDouble());
        ui->leginsideysize->setValue(legin.at(1).toDouble());
        ui->cinsidexi->setValue(legin.at(2).toDouble());
        ui->cinsidexl->setValue(legin.at(3).toDouble());
        ui->cinsideyi->setValue(legin.at(4).toDouble());
        ui->cinsideyl->setValue(legin.at(5).toDouble());
        if(xyzp->at(nplot)->colorscale=="nocolorscale")ui->cleg0->setChecked(true);
        else if(xyzp->at(nplot)->colorscale=="rightcolorscale")ui->cleg1->setChecked(true);
        else if(xyzp->at(nplot)->colorscale=="insidecolorscale")ui->cleg2->setChecked(true);
        ui->BananaScale->setText(QString::number(xyzp->at(nplot)->factor));
        ui->surfaceid->setText(xyzp->at(nplot)->plotidentifier);
    }else if(list.at(0).toInt()==3)
    {
        globalpage=false;
        enabledisabletabs("histo");
        ui->CSHproprieties->setCurrentIndex(ui->CSHproprieties->indexOf(ui->histo));
        nplot=list.at(1).toInt();
        nhisto=list.at(2).toInt();
        ui->hylabel->setText(histop->at(nplot).at(nhisto)->ylabel);
        ui->hxlabel->setText(histop->at(nplot).at(nhisto)->xlabel);
        sethistocolor(histop->at(nplot).at(nhisto)->histocolor);
        ui->sscale->setText(QString::number(histop->at(nplot).at(nhisto)->factor));
        ui->hidlabel->setText(histop->at(nplot).at(nhisto)->plotidentifier);
        ui->histotype->setCurrentIndex(histop->at(nplot).at(0)->htype);

    }


}

void configplotwidget::readvalues()
{
    if(nplot==-1) return;
    if(globalpage){
    p->at(nplot)->xvmin=ui->xvmin->value();
    p->at(nplot)->xvmax=ui->xvmax->value();
    p->at(nplot)->yvmin=ui->yvmin->value();
    p->at(nplot)->yvmax=ui->yvmax->value();
    if(ui->withtimeaxis->isChecked())p->at(nplot)->withoutxaxis=false;
    else p->at(nplot)->withoutxaxis=true;
    if(ui->firsttimelabel->isChecked())p->at(nplot)->firsttimelabel=true;
    else p->at(nplot)->firsttimelabel=false;
    p->at(nplot)->plottitle=ui->plottitle->text();
    if(ui->runstart->isChecked()) p->at(nplot)->runstart=0;
    else if (ui->norunstart->isChecked()) p->at(nplot)->runstart=-1;
    else  p->at(nplot)->runstart=1;
    if(ui->Gridx->isChecked())p->at(nplot)->stylex=true;else p->at(nplot)->stylex=false;
    if(ui->Gridy->isChecked())p->at(nplot)->styley=true;else p->at(nplot)->styley=false;

    //the next proprieties are chosen to be homogeneous in all the page so is just saved in the first plot
    p->at(0)->fontset=ui->font->currentIndex()+1;
    p->at(0)->fontsize=ui->fontsize->value();
      p->at(0)->balpha=ui->alphavalue->value();

    //
    if(p->at(nplot)->plottype==0){
       p->at(nplot)->multiaxissep=ui->multiaxissep->text().toDouble();
       p->at(nplot)->labelsep=ui->labelsep->text().toDouble();
       if(ui->multiaxis->isChecked())p->at(nplot)->Multixais=1;
       else if (ui->singleaxis->isChecked())p->at(nplot)->Multixais=0;
       else if (ui->twoaxis->isChecked()) p->at(nplot)->Multixais=2;

       if(ui->allleftaxisvaluesflag->isChecked()) p->at(nplot)->leftaxislimits=ui->allfetaxisvalues->text(); else p->at(nplot)->leftaxislimits.clear();
       if(ui->allrightaxisvaluesflag->isChecked()) p->at(nplot)->rightaxislimits=ui->allrightaxisvalues->text(); else p->at(nplot)->rightaxislimits.clear();

    }
}
    if(ncurve !=-1){
        if (ui->right->isChecked()) {xyp->at(nplot).at(ncurve)->rightaxis=true; xyp->at(nplot).at(ncurve)->leftaxis=false;}
        else {xyp->at(nplot).at(ncurve)->rightaxis=false; xyp->at(nplot).at(ncurve)->leftaxis=true;}
        if( ui->log->isChecked()) xyp->at(nplot).at(ncurve)->ylog=true; else xyp->at(nplot).at(ncurve)->ylog=false;
        if(ui->derivative->isChecked()) {xyp->at(nplot).at(ncurve)->derivative=true;xyp->at(nplot).at(ncurve)->onlysmooth=false;xyp->at(nplot).at(ncurve)->smooth=false;}
        else if(ui->onlysmooth->isChecked()){xyp->at(nplot).at(ncurve)->derivative=false;xyp->at(nplot).at(ncurve)->onlysmooth=true;xyp->at(nplot).at(ncurve)->smooth=false;}
        else if(ui->smooth->isChecked()){xyp->at(nplot).at(ncurve)->derivative=false;xyp->at(nplot).at(ncurve)->onlysmooth=false;xyp->at(nplot).at(ncurve)->smooth=true;}
        else {
            xyp->at(nplot).at(ncurve)->derivative=false;
            xyp->at(nplot).at(ncurve)->onlysmooth=false;
            xyp->at(nplot).at(ncurve)->smooth=false;
        }
        if(ui->derivative->isChecked())  {
            xyp->at(nplot).at(ncurve)->devnpoints=ui->averagepoints->value();
        } else {
             xyp->at(nplot).at(ncurve)->smoothpoints=ui->averagepoints->value();
        }
        if(ui->linetype->currentIndex() && ui->markers->currentIndex()) xyp->at(nplot).at(ncurve)->linetype="lineandmarkers";
        else if (ui->linetype->currentIndex())xyp->at(nplot).at(ncurve)->linetype="line";
        else if (ui->markers->currentIndex())xyp->at(nplot).at(ncurve)->linetype="markers";
        if(ui->linetype->currentIndex())xyp->at(nplot).at(ncurve)->curvestyle= ui->linetype->currentIndex();
        if (ui->markers->currentIndex())xyp->at(nplot).at(ncurve)->marker=ui->markers->currentIndex()-1;
        xyp->at(nplot).at(ncurve)->markersscale=ui->MarkersScale->value();
        if(ui->minfreelimits->isChecked()){
            xyp->at(nplot).at(ncurve)->minfreelimits=true;
        }else{
             xyp->at(nplot).at(ncurve)->minfreelimits=false;
             xyp->at(nplot).at(ncurve)->Ymin=ui->min->text().toDouble();
        }
        if(ui->maxfreelimits->isChecked()){
            xyp->at(nplot).at(ncurve)->maxfreelimits=true;
        }else{
            xyp->at(nplot).at(ncurve)->maxfreelimits=false;
            xyp->at(nplot).at(ncurve)->Ymax=ui->max->text().toDouble();
        }
        xyp->at(nplot).at(ncurve)->pwidth=ui->thickness->currentIndex()+1;
        xyp->at(nplot).at(ncurve)->factor=ui->yfactor->text();

        xyp->at(nplot).at(ncurve)->yaxisLabel=ui->clabel->text();
    }
    if(nsurf!=-1){
        xyzp->at(nplot)->Surfacecolor=ui->colorscheme->text();
        if(ui->sylog->isChecked())xyzp->at(nplot)->ylog=true; else xyzp->at(nplot)->ylog=false;
        if(ui->slog->isChecked())xyzp->at(nplot)->zlog=true; else xyzp->at(nplot)->zlog=false;
        if(ui->cminfreelimits->isChecked()){
            xyzp->at(nplot)->minfreelimits=true;
        }else {
            xyzp->at(nplot)->minfreelimits=false;
            xyzp->at(nplot)->Zmin=ui->cmin->text().toDouble();
        }
        if(ui->cmaxfreelimits->isChecked()){
            xyzp->at(nplot)->maxfreelimits=true;
        }else {
            xyzp->at(nplot)->maxfreelimits=false;
            xyzp->at(nplot)->Zmax=ui->cmax->text().toDouble();
        }
        xyzp->at(nplot)->yaxisLabel= ui->slabel->text();
        if(ui->tiled->isChecked()) xyzp->at(nplot)->surfacetype="tiled2d";
        else xyzp->at(nplot)->surfacetype="contour";
        if(ui->Bizarre->isChecked())xyzp->at(nplot)->colorlimits="Bizarreway"; else xyzp->at(nplot)->colorlimits="";
        QString lixo;
        lixo=QString::number(ui->leginsidexsize->value()); lixo.append(";");
        lixo.append(QString::number(ui->leginsideysize->value())); lixo.append(";");
        lixo.append(QString::number(ui->cinsidexi->value())); lixo.append(";");
        lixo.append(QString::number(ui->cinsidexl->value())); lixo.append(";");
        lixo.append(QString::number(ui->cinsideyi->value())); lixo.append(";");
        lixo.append(QString::number(ui->cinsideyl->value())); lixo.append(";");
        xyzp->at(nplot)->Surfacelegendinside=lixo;
        if(ui->cleg0->isChecked()) xyzp->at(nplot)->colorscale="nocolorscale";
        else if (ui->cleg1->isChecked()) xyzp->at(nplot)->colorscale="rightcolorscale";
        else if (ui->cleg2->isChecked()) xyzp->at(nplot)->colorscale="insidecolorscale";
        xyzp->at(nplot)->factor=ui->BananaScale->text().toDouble();
        xyzp->at(nplot)->plotidentifier=ui->surfaceid->text();

    }
   if(nhisto!=-1){
       histop->at(nplot).at(nhisto)->ylabel=ui->hylabel->text();
       histop->at(nplot).at(nhisto)->xlabel=ui->hxlabel->text();
       histop->at(nplot).at(0)->htype=ui->histotype->currentIndex();
       histop->at(nplot).at(nhisto)->histocolor=histocolor;
       histop->at(nplot).at(nhisto)->factor=ui->sscale->text().toDouble();
       histop->at(nplot).at(nhisto)->plotidentifier=ui->hidlabel->text();

   }

}
void configplotwidget::replotslot()
{
    if(nplot==-1) return;
    readvalues();
    emit replot();
}
void configplotwidget::filltreeview()
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
            item->setCheckable(false);
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
            item->setCheckable(false);
            cgrafid.clear();
            subpageslabels[subpage]->appendRow(item);
        }
        //
        /*
        for(int i=0;i<Plot->plot[subpage].Data.xyz.count();i++)
        {
            if(Plot->plot[subpage].Data.histo.count()) break;
            cgrafid.clear();
            lixo=Plot->plot[subpage].Data.xyz[i].zname;
            QStandardItem *item = new QStandardItem(lixo);
            item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/banana4.svg")));
            item->setEditable(false);
            cgrafid <<2 <<subpage<< i;
            item->setData(cgrafid);
            item->setCheckable(false);
            cgrafid.clear();
            subpageslabels[subpage]->appendRow(item);
        }

        for(int i=0;i<Plot->plot[subpage].Data.histo.count();i++)
        {
            cgrafid.clear();
            lixo=Plot->plot[subpage].Data.histo[i].xlabel;
            QStandardItem *item = new QStandardItem(lixo);
            item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/chart15.svg")));
            item->setEditable(false);
            cgrafid <<3 <<subpage<< i;
            item->setData(cgrafid);
            item->setCheckable(false);
            cgrafid.clear();
            subpageslabels[subpage]->appendRow(item);
        }
        */

    }//endsubpage

    for (int j = 0; j < subpageslabels.count(); j++) {
        cparentItem->appendRow(subpageslabels[j]);
    }

    ui->treeView->setModel(&model);
    ui->treeView->expand(model.index(0,0,QModelIndex()));

}


void configplotwidget::filllinestyle()
{
    ui->linetype->addItem("No line", Qt::DisplayRole);
    ui->linetype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/linestyle1.png")),"1", Qt::DisplayRole);
    ui->linetype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/linestyle2.png")),"2",  Qt::DisplayRole);
    ui->linetype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/linestyle3.png")),"3",  Qt::DisplayRole);
    ui->linetype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/linestyle4.png")),"4",  Qt::DisplayRole);
    ui->linetype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/linestyle5.png")),"5",  Qt::DisplayRole);
    ui->linetype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/linestyle6.png")),"6",  Qt::DisplayRole);
    ui->linetype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/linestyle7.png")),"7",  Qt::DisplayRole);
    ui->linetype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/linestyle8.png")),"8",  Qt::DisplayRole);

}
void configplotwidget::fillfontset()
{
    ui->font->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/font1.png")),"", Qt::DisplayRole);
    ui->font->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/font2.png")),"",  Qt::DisplayRole);
    ui->font->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/font3.png")),"",  Qt::DisplayRole);

}
void configplotwidget::fillhistotype()
{
    ui->histotype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/histotype2.png")),"1", Qt::DisplayRole);
    ui->histotype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/histotype1.png")),"2", Qt::DisplayRole);
    ui->histotype->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/histotype3.png")),"3", Qt::DisplayRole);

}
void configplotwidget::fillmarkers()
{
    ui->markers->addItem("No Markers", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker1.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker2.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker3.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker4.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker5.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker6.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker7.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker8.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker9.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker10.png")),"", Qt::DisplayRole);
    ui->markers->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/marker11.png")),"", Qt::DisplayRole);

}
void configplotwidget::fillcolorsScheme()
{
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow_cloud.png")),"270", "rainbow_cloud#300");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow16.png")),"16","rainbow#16");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow15.png")),"15", "rainbow#15");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow14.png")),"14", "rainbow#14");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow13.png")),"13", "rainbow#13");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow12.png")),"12", "rainbow#12");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow11.png")),"11", "rainbow#11");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow10.png")),"10", "rainbow#10");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow9.png")),"9", "rainbow#9");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow8.png")),"8", "rainbow#8");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/rainbow6.png")),"6", "rainbow#6");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/BluetoOrange14.png")),"14", "BluetoOrange#14");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/BluetoOrange8.png")),"8","BluetoOrange#8");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/vitoroa24.png")),"24","vitoria#24");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/jet16.png")),"16", "jet#16");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/nogreen16.png")),"16", "nogreen#16");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/wysiwyg14.png")),"14",  "wysiwyg#14");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/wysiwyg8.png")),"8",  "wysiwyg#8");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/gray6.png")),"6",  "gray#6");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/gray8.png")),"8", "gray#8");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/gray9.png")),"9",  "gray#9");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/gray10.png")),"10",  "gray#10");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/gray11.png")),"11",  "gray#11");
    ui->cpredefined->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/gray12.png")),"12",  "gray#12");
    ui->cpredefined->addItem("BCYR", "0,0,1;0,1,1;1,1,0;1,0,0;#300");
    readsettings();
}
void configplotwidget::readsettings()
{
    QSettings settings("CampaignAnalysis", "ConfigPlot");
    QString setstring=settings.value("plotedit/colorschemes").toString();
    QStringList stored, one;
    stored=setstring.split(QRegularExpression(">"));
    for(int i=0; i<stored.size(); i++){
        one=stored[i].split(QRegularExpression(":"));
        if(one.count()!=2) continue;
        ui->cpredefined->addItem(one[1],  one[0]);
    }
}
void configplotwidget::fillthick()
{
    ui->thickness->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/thickness1.png")),"", Qt::DisplayRole);
    ui->thickness->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/thickness2.png")),"",  Qt::DisplayRole);
    ui->thickness->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/thickness3.png")),"",  Qt::DisplayRole);
    ui->thickness->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/thickness4.png")),"", Qt::DisplayRole);
    ui->thickness->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/thickness5.png")),"",  Qt::DisplayRole);
    ui->thickness->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/thickness6.png")),"",  Qt::DisplayRole);
    ui->thickness->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/thickness7.png")),"",  Qt::DisplayRole);
    ui->thickness->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/thickness8.png")),"",  Qt::DisplayRole);

}
void configplotwidget::changecurvecolor()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "Curve color", QColorDialog::DontUseNativeDialog);
    if(!color.isValid())
    {
        QMessageBox::information(this, "Curve color", "Not a valid color");
    }
    QBrush brush;
    QPalette palette;
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    ui->color->setPalette(palette);
    QString lixo;
    lixo=color.name();
    lixo.remove("#");
    xyp->at(nplot).at(ncurve)->Color=lixo;

}
void configplotwidget::setcurvecolor()
{
    QColor color;
    QPalette palette;
    QBrush brush;
    QString ccolor=xyp->at(nplot).at(ncurve)->Color;
    ccolor.prepend("#");
    color.setNamedColor(ccolor);
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    ui->color->setPalette(palette);

}
void configplotwidget::storeplotindatabase(){
    readvalues();
    bool ok;
    QString plotname = QInputDialog::getText(this, tr("Name  of the plot"),
                                     tr("Name of de plot?"), QLineEdit::Normal,
                                             p->at(0)->Plotname, &ok);
    dbapi table;
    table.connectionName=databaseconnection;
    connect(&table, &dbapi::logmessagesR,this,&configplotwidget::LogMR);
    connect(&table, &dbapi::logmessages,this,&configplotwidget::LogM);
    if(!ok) return;
    if(!plotname.simplified().count()){
        QMessageBox::information(this, "plot name ", "name cannot be empty");
        return;
    }
    p->at(0)->Plotname=plotname;
    if(p->count()==1){
        if(p->at(0)->plottype==0){
            QString message;
            message="INSERT  INTO typlots (plotname,columnsandtables,proprieties,curves) VALUES ";
            message.append("(?,?,?,?) ON DUPLICATE KEY UPDATE columnsandtables=VALUES(columnsandtables), proprieties=VALUES(proprieties), curves=VALUES(curves);");
            QStringList binds;
            binds.clear();
            createstringstostorety(0);
            binds <<plotname <<closanstables<<plotp <<curvesp;
           table.preparebind(message,&binds);
        }else if(p->at(0)->plottype==1){
            QString message;
            message="INSERT  INTO Surfaceplots (plotname,columnsandtables,proprieties,surface) VALUES ";
            message.append("(?,?,?,?) ON DUPLICATE KEY UPDATE columnsandtables=VALUES(columnsandtables), proprieties=VALUES(proprieties), surface=VALUES(surface);");
            QStringList binds;
            binds.clear();
            createstringstostoresurface(0);
            binds <<plotname <<closanstables<<plotp <<surfacep;
            table.preparebind(message,&binds);

        }else if(p->at(0)->plottype==2){
            QString message;
            message="INSERT  INTO Histoplots (plotname,columnsandtables,proprieties,histos) VALUES ";
            message.append("(?,?,?,?) ON DUPLICATE KEY UPDATE columnsandtables=VALUES(columnsandtables), proprieties=VALUES(proprieties), histos=VALUES(histos);");
            QStringList binds;
            binds.clear();
            createstringstostorehisto(0);
            binds <<plotname <<closanstables<<plotp <<histosp;
            table.preparebind(message,&binds);

    }
    }else{
        int order=-1;
         table.Delrowsmachingacolumnvalue("Screens","screenname",plotname.simplified());
         QString message;
         message="INSERT  INTO Screens (screenname,plottype,plotorder,columnsandtables,proprieties,surface,curves,histos) VALUES ";
         message.append("(?,?,?,?,?,?,?,?);");
         QStringList binds;

        for(int i=0; i<p->count();i++){
            order++;
            if(p->at(i)->plottype==0){
                surfacep="not relevant"; curvesp="not relevant"; histosp="not relevant";
                createstringstostorety(i);
                binds.clear();
                binds<<plotname<<QString::number(p->at(i)->plottype)<<QString::number(order)<<closanstables<<plotp<<surfacep<<curvesp<<histosp;
                table.preparebind(message,&binds);
            }else if(p->at(i)->plottype==1){
                surfacep="not relevant"; curvesp="not relevant"; histosp="not relevant";
                createstringstostoresurface(i);
                binds.clear();
                binds<<plotname<<QString::number(p->at(i)->plottype)<<QString::number(order)<<closanstables<<plotp<<surfacep<<curvesp<<histosp;
                table.preparebind(message,&binds);
            }else if(p->at(i)->plottype==2){
                surfacep="not relevant"; curvesp="not relevant"; histosp="not relevant";
                createstringstostorehisto(i);
                binds.clear();
                binds<<plotname<<QString::number(p->at(i)->plottype)<<QString::number(order)<<closanstables<<plotp<<surfacep<<curvesp<<histosp;
                table.preparebind(message,&binds);
            }

        }
    }
}
void configplotwidget::createstringstostorehisto(int s){
    curvesp.clear();plotp.clear();
    plotproprieties pd;
    if(s==0){
        if(p->at(s)->balpha!=pd.balpha)plotp.append("balpha:="+QString::number(p->at(s)->balpha)+ss);
        if(p->at(s)->fontset!=pd.fontset)plotp.append("fontset:="+QString::number(p->at(s)->fontset)+ss);
        if(p->at(s)->fontsize!=pd.fontsize)plotp.append("fontsize:="+QString::number(p->at(s)->fontsize)+ss);
        if(p->at(s)->bred!=pd.bred)plotp.append("bred:="+QString::number(p->at(s)->bred)+ss);
        if(p->at(s)->bgreen!=pd.bgreen)plotp.append("bgreen:="+QString::number(p->at(s)->bgreen)+ss);
        if(p->at(s)->bblue!=pd.bblue)plotp.append("bblue:="+QString::number(p->at(s)->bblue)+ss);

    }
    if(p->at(s)->xvmin!=pd.xvmin) plotp.append("xvmin:="+QString::number(p->at(s)->xvmin)+ss);
    if(p->at(s)->xvmax!=pd.xvmax) plotp.append("xvmax:="+QString::number(p->at(s)->xvmax)+ss);
    if(p->at(s)->yvmin!=pd.yvmin) plotp.append("yvmin:="+QString::number(p->at(s)->yvmin)+ss);
    if(p->at(s)->yvmax!=pd.yvmax) plotp.append("yvmax:="+QString::number(p->at(s)->yvmax)+ss);
    plotp.append("Plotname:="+p->at(s)->Plotname+ss);
    plotp.append("plottype:="+QString::number(p->at(s)->plottype));


    histoproprieties hd;
    QString col,tname;
    col.clear();tname.clear();
    histosp.clear();
    for(int i=0;i<histop->at(s).count();i++){
        if(i>0)histosp.append(sc);
        histosp.append("plotidentifier:="+histop->at(s).at(i)->plotidentifier+ss);
        if(histop->at(s).at(i)->histocolor!=hd.histocolor) histosp.append("histocolor:="+histop->at(s).at(i)->histocolor+ss);
        if(i==0){
            histosp.append("closanstables:="+histop->at(s).at(i)->closanstables+ss);
            histosp.append("htype:="+QString::number(histop->at(s).at(i)->htype) +ss);
            histosp.append("Bins:="+histop->at(s).at(i)->Bins+ss);
        }
        histosp.append("xlabel:="+histop->at(s).at(i)->xlabel+ss);
        histosp.append("ylabel:="+histop->at(s).at(i)->ylabel);
    }
    closanstables=histop->at(s).at(0)->closanstables;

}
void configplotwidget::createstringstostorety(int s){
    curvesp.clear();plotp.clear();
    plotproprieties pd;
    if(s==0){
        if(p->at(s)->balpha!=pd.balpha)plotp.append("balpha:="+QString::number(p->at(s)->balpha)+ss);
        if(p->at(s)->fontset!=pd.fontset)plotp.append("fontset:="+QString::number(p->at(s)->fontset)+ss);
        if(p->at(s)->fontsize!=pd.fontsize)plotp.append("fontsize:="+QString::number(p->at(s)->fontsize)+ss);
        if(p->at(s)->bred!=pd.bred)plotp.append("bred:="+QString::number(p->at(s)->bred)+ss);
        if(p->at(s)->bgreen!=pd.bgreen)plotp.append("bgreen:="+QString::number(p->at(s)->bgreen)+ss);
        if(p->at(s)->bblue!=pd.bblue)plotp.append("bblue:="+QString::number(p->at(s)->bblue)+ss);

    }
    if(p->at(s)->xvmin!=pd.xvmin) plotp.append("xvmin:="+QString::number(p->at(s)->xvmin)+ss);
    if(p->at(s)->xvmax!=pd.xvmax) plotp.append("xvmax:="+QString::number(p->at(s)->xvmax)+ss);
    if(p->at(s)->yvmin!=pd.yvmin) plotp.append("yvmin:="+QString::number(p->at(s)->yvmin)+ss);
    if(p->at(s)->yvmax!=pd.yvmax) plotp.append("yvmax:="+QString::number(p->at(s)->yvmax)+ss);
    if(p->at(s)->withoutxaxis!=pd.withoutxaxis) plotp.append("withoutxaxis:="+QString::number(p->at(s)->withoutxaxis)+ss);
    if(p->at(s)->firsttimelabel !=pd.firsttimelabel) plotp.append("firsttimelabel:="+QString::number(p->at(s)->firsttimelabel)+ss);
    if(p->at(s)->runstart!=pd.runstart)plotp.append("runstart:="+QString::number(p->at(s)->runstart)+ss);
    if(p->at(s)->stylex!=pd.stylex)plotp.append("stylex:="+QString::number(p->at(s)->stylex)+ss);
    if(p->at(s)->styley!=pd.styley)plotp.append("styley:="+QString::number(p->at(s)->styley)+ss);

    if(p->at(s)->Multixais!=pd.Multixais)  plotp.append("Multixais:="+QString::number(p->at(s)->Multixais)+ss);
    if(p->at(s)->multiaxissep!=pd.multiaxissep)plotp.append("multiaxissep:="+QString::number(p->at(s)->multiaxissep)+ss);
    if(p->at(s)->labelsep!=pd.labelsep)plotp.append("labelsep:="+QString::number(p->at(s)->labelsep)+ss);
    if(p->at(s)->leftaxislimits!=pd.leftaxislimits) plotp.append("leftaxislimits:="+p->at(s)->leftaxislimits+ss);
    if(p->at(s)->rightaxislimits!=pd.rightaxislimits) plotp.append("rightaxislimits:="+p->at(s)->rightaxislimits+ss);

    plotp.append("plottitle:="+p->at(s)->plottitle+ss);
    plotp.append("Plotname:="+p->at(s)->Plotname+ss);
    plotp.append("plottype:="+QString::number(p->at(s)->plottype));


    xyproprieties xypd;
    QString col,tname;
    col.clear();tname.clear();
    for(int i=0;i<xyp->at(s).count();i++){
        col.append(xyp->at(s).at(i)->cname);if(i<xyp->at(s).count()-1) col.append(",");
        tname.append(xyp->at(s).at(i)->tname);if(i<xyp->at(s).count()-1) tname.append(",");

        if(i>0)curvesp.append(sc);
        if(xyp->at(s).at(i)->rightaxis!=xypd.rightaxis)curvesp.append("rightaxis:="+QString::number(xyp->at(s).at(i)->rightaxis)+ss);
        if(xyp->at(s).at(i)->ylog!=xypd.ylog)curvesp.append("ylog:="+QString::number(xyp->at(s).at(i)->ylog)+ss);
        if(xyp->at(s).at(i)->derivative!=xypd.derivative) curvesp.append("derivative:="+QString::number(xyp->at(s).at(i)->derivative)+ss);
        if(xyp->at(s).at(i)->devnpoints!=xypd.devnpoints)curvesp.append("devnpoints:="+QString::number(xyp->at(s).at(i)->devnpoints)+ss);
        if(xyp->at(s).at(i)->smoothpoints !=xypd.smoothpoints)curvesp.append("smoothpoints:="+QString::number(xyp->at(s).at(i)->smoothpoints)+ss);
        if(xyp->at(s).at(i)->linetype !=xypd.linetype)curvesp.append("linetype:="+xyp->at(s).at(i)->linetype+ss);
        if(xyp->at(s).at(i)->markersscale!=xypd.markersscale)curvesp.append("markersscale:="+QString::number(xyp->at(s).at(i)->markersscale)+ss);
        if(xyp->at(s).at(i)->marker!=xypd.marker)curvesp.append("marker:="+QString::number(xyp->at(s).at(i)->marker)+ss);
        if(xyp->at(s).at(i)->minfreelimits !=xypd.minfreelimits) curvesp.append("minfreelimits:="+QString::number(xyp->at(s).at(i)->minfreelimits)+ss);
        if(xyp->at(s).at(i)->maxfreelimits !=xypd.maxfreelimits) curvesp.append("maxfreelimits:="+QString::number(xyp->at(s).at(i)->maxfreelimits)+ss);
        if(!xyp->at(s).at(i)->minfreelimits) curvesp.append("Ymin:="+QString::number(xyp->at(s).at(i)->Ymin)+ss);
        if(!xyp->at(s).at(i)->maxfreelimits) curvesp.append("Ymax:="+QString::number(xyp->at(s).at(i)->Ymax)+ss);
        if(xyp->at(s).at(i)->pwidth!=xypd.pwidth)curvesp.append("pwidth:="+QString::number(xyp->at(s).at(i)->pwidth)+ss);
        if(xyp->at(s).at(i)->factor !=xypd.factor)curvesp.append("factor:="+xyp->at(s).at(i)->factor+ss);
        if(xyp->at(s).at(i)->Color !=xypd.Color) curvesp.append("Color:="+xyp->at(s).at(i)->Color+ss);
        if(xyp->at(s).at(i)->onlysmooth !=xypd.onlysmooth) curvesp.append("onlysmooth:="+QString::number(xyp->at(s).at(i)->onlysmooth)+ss);
        if(xyp->at(s).at(i)->smooth !=xypd.smooth) curvesp.append("smooth:="+QString::number(xyp->at(s).at(i)->smooth)+ss);
        curvesp.append("yaxisLabel:="+xyp->at(s).at(i)->yaxisLabel+ss);
        curvesp.append("tname:="+xyp->at(s).at(i)->tname+ss);
        curvesp.append("cname:="+xyp->at(s).at(i)->cname);


    }
    closanstables=col;
    closanstables.append(";");
    closanstables.append(tname);

}

void configplotwidget::createstringstostoresurface(int s){
    surfacep.clear();plotp.clear();
    plotproprieties pd;
    if(s==0){
        if(p->at(s)->balpha!=pd.balpha)plotp.append("balpha:="+QString::number(p->at(s)->balpha)+ss);
        if(p->at(s)->fontset!=pd.fontset)plotp.append("fontset:="+QString::number(p->at(s)->fontset)+ss);
        if(p->at(s)->fontsize!=pd.fontsize)plotp.append("fontsize:="+QString::number(p->at(s)->fontsize)+ss);
        if(p->at(s)->bred!=pd.bred)plotp.append("bred:="+QString::number(p->at(s)->bred)+ss);
        if(p->at(s)->bgreen!=pd.bgreen)plotp.append("bgreen:="+QString::number(p->at(s)->bgreen)+ss);
        if(p->at(s)->bblue!=pd.bblue)plotp.append("bblue:="+QString::number(p->at(s)->bblue)+ss);

    }
    if(p->at(s)->xvmin!=pd.xvmin) plotp.append("xvmin:="+QString::number(p->at(s)->xvmin)+ss);
    if(p->at(s)->xvmax!=pd.xvmax) plotp.append("xvmax:="+QString::number(p->at(s)->xvmax)+ss);
    if(p->at(s)->yvmin!=pd.yvmin) plotp.append("yvmin:="+QString::number(p->at(s)->yvmin)+ss);
    if(p->at(s)->yvmax!=pd.yvmax) plotp.append("yvmax:="+QString::number(p->at(s)->yvmax)+ss);
    if(p->at(s)->withoutxaxis!=pd.withoutxaxis) plotp.append("withoutxaxis:="+QString::number(p->at(s)->withoutxaxis)+ss);
    if(p->at(s)->firsttimelabel !=pd.firsttimelabel) plotp.append("firsttimelabel:="+QString::number(p->at(s)->firsttimelabel)+ss);
    if(p->at(s)->runstart!=pd.runstart)plotp.append("runstart:="+QString::number(p->at(s)->runstart)+ss);
    if(p->at(s)->stylex!=pd.stylex)plotp.append("stylex:="+QString::number(p->at(s)->stylex)+ss);
    if(p->at(s)->styley!=pd.styley)plotp.append("styley:="+QString::number(p->at(s)->styley)+ss);

    if(p->at(s)->Multixais!=pd.Multixais)  plotp.append("Multixais:="+QString::number(p->at(s)->Multixais)+ss);
    if(p->at(s)->multiaxissep!=pd.multiaxissep)plotp.append("multiaxissep:="+QString::number(p->at(s)->multiaxissep)+ss);
    if(p->at(s)->labelsep!=pd.labelsep)plotp.append("labelsep:="+QString::number(p->at(s)->labelsep)+ss);
    if(p->at(s)->leftaxislimits!=pd.leftaxislimits) plotp.append("leftaxislimits:="+p->at(s)->leftaxislimits+ss);
    if(p->at(s)->rightaxislimits!=pd.rightaxislimits) plotp.append("rightaxislimits:="+p->at(s)->rightaxislimits+ss);
    plotp.append("plottitle:="+p->at(s)->plottitle+ss);
    plotp.append("Plotname:="+p->at(s)->Plotname+ss);
    plotp.append("plottype:="+QString::number(p->at(s)->plottype));


    xyzproprieties xyzpd;
    if(xyzp->at(s)->Surfacecolor!=xyzpd.Surfacecolor) surfacep.append("Surfacecolor:="+xyzp->at(s)->Surfacecolor+ss);
    if(xyzp->at(s)->ylog!=xyzpd.ylog)surfacep.append("ylog:="+QString::number(xyzp->at(s)->ylog)+ss);
    if(xyzp->at(s)->zlog!=xyzpd.zlog)surfacep.append("zlog:="+QString::number(xyzp->at(s)->zlog)+ss);
    if(xyzp->at(s)->minfreelimits!=xyzpd.minfreelimits) surfacep.append("minfreelimits:="+QString::number(xyzp->at(s)->minfreelimits)+ss);
    if(xyzp->at(s)->maxfreelimits!=xyzpd.maxfreelimits) surfacep.append("maxfreelimits:="+QString::number(xyzp->at(s)->maxfreelimits)+ss);
    if(!xyzp->at(s)->minfreelimits)surfacep.append("Zmin:="+QString::number(xyzp->at(s)->Zmin)+ss);
    if(!xyzp->at(s)->maxfreelimits)surfacep.append("Zmax:="+QString::number(xyzp->at(s)->Zmax)+ss);
    surfacep.append("yaxisLabel:="+xyzp->at(s)->yaxisLabel+ss);
    if(xyzp->at(s)->surfacetype!=xyzpd.surfacetype) surfacep.append("surfacetype:="+xyzp->at(s)->surfacetype+ss);
    if(xyzp->at(s)->colorlimits!=xyzpd.colorlimits) surfacep.append("colorlimits:="+xyzp->at(s)->colorlimits)+ss;
    if(xyzp->at(s)->colorscale!=xyzpd.colorscale) surfacep.append("colorscale:="+xyzp->at(s)->colorscale+ss);
    if(xyzp->at(s)->factor!=xyzpd.factor) surfacep.append("factor:="+QString::number(xyzp->at(s)->factor)+ss);
    if(xyzp->at(s)->colorscale!=xyzpd.Surfacelegendinside) surfacep.append("Surfacelegendinside:="+xyzp->at(s)->Surfacelegendinside+ss);
    surfacep.append("plotidentifier:="+xyzp->at(s)->plotidentifier+ss);
    surfacep.append("diameters:=" +xyzp->at(s)->diameters);
    closanstables=xyzp->at(s)->tableandcolumns;

}
void configplotwidget::writesettings()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Name the color scheme"),
                                         tr("Color Scheme name?"), QLineEdit::Normal,
                                         ui->colorscheme->text(), &ok);
    if (ok && !text.isEmpty()){
        if(setstring.size()){
            setstring.append(">");
        }
        setstring.append(ui->colorscheme->text());
        setstring.append(":");
        setstring.append(text);
        QSettings settings("CampaignAnalysis", "ConfigPlot");
        settings.setValue("plotedit/colorschemes", setstring);
        ui->cpredefined->addItem(text,ui->colorscheme->text()  );
    }
}
void configplotwidget::buildcolorpalete()
{
    builtcolorscheme dialog(this);
    int result=dialog.exec();
    if(result){
        QString lixo;
        lixo=dialog.palete1;
        lixo.append("#");
        lixo.append(QString::number(dialog.ncol));
        ui->colorscheme->setText(lixo);
    }

}

void configplotwidget::setpredefinedcolor(int index)
{
    ui->colorscheme->setText(ui->cpredefined->itemData (index, Qt::UserRole ).toString());
    ui->colorscheme->setToolTip(ui->cpredefined->itemData (index, Qt::UserRole ).toString());
}

void configplotwidget::colorpalete(QString palete)
{

    QStringList palete1;
    palete1=palete.split(QRegularExpression("#"));
    Plplot_utils::Testcolormap(100,30,tempfile.fileName(),palete1[0],palete1[1].toInt());
    QPixmap renderer = QPixmap(tempfile.fileName().toLatin1().constData());
    ui->lcolorpaletetest->setPixmap(renderer);
}
void configplotwidget::sethistocolor(QString ccolor)
{
    ccolor.prepend("#");
    QColor color;
    QBrush brush;
    QPalette palette;
    color.setNamedColor(ccolor);
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    ui->hcolor->setPalette(palette);
    QString lixo;
    lixo=color.name();
    lixo.remove("#");
    histocolor=lixo;

}
void configplotwidget::changehistocolor()
{
    QColor color;


    color = QColorDialog::getColor(Qt::black, this, "histo color", QColorDialog::DontUseNativeDialog);
    if(!color.isValid())
    {
        QMessageBox::information(this, "Histo color", "Not a valid color");
    }
    QBrush brush;
    QPalette palette;

    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    ui->hcolor->setPalette(palette);
    QString lixo;
    lixo=color.name();
    lixo.remove("#");
    histocolor=lixo;

}
void configplotwidget::storeplotinlocaldrive(){
    readvalues();
    bool ok;
    QString plotname = QInputDialog::getText(this, tr("Name  of the plot"),
                                     tr("Name of de plot?"), QLineEdit::Normal,
                                             p->at(0)->Plotname, &ok);
    if(!ok) return;
    if(!plotname.simplified().count()){
        QMessageBox::information(this, "plot name ", "name cannot be empty");
        return;
    }
    p->at(0)->Plotname=plotname;
    if(p->count()==1){
        if(p->at(0)->plottype==0){
            struct typlots{
             QString plotname;
             QString columnsandtables;
             QString proprieties;
             QString curves;
            };
            QList <typlots> plots;
            plots.clear();
            QSettings settings("CampaignAnalysis","typlotsdef");
            int size = settings.beginReadArray("typlots");
            for (int i = 0; i < size; ++i) {
                settings.setArrayIndex(i);
                typlots plot;
                plot.plotname=settings.value("plotname").toString();
                plot.columnsandtables=settings.value("columnsandtables").toString();
                plot.proprieties=settings.value("proprieties").toString();
                plot.curves=settings.value("curves").toString();
                plots.append(plot);
           }
            settings.endArray();
            createstringstostorety(0);
            typlots plot;
            plot.plotname=plotname;
            plot.columnsandtables=closanstables;
            plot.proprieties=plotp;
            plot.curves= curvesp;
            bool exist;
            exist=false;
            int k;
            for (int i = 0; i < plots.count(); ++i) {
                if(plots.at(i).plotname==plotname) {
                   exist=true;
                   k=i;
                   break;
                }
            }
            if(exist){
                plots[k].plotname=plot.plotname;
                plots[k].columnsandtables=plot.columnsandtables;
                plots[k].proprieties=plot.proprieties;
                plots[k].curves=plot.curves;


            }else  plots.append(plot);

            settings.beginWriteArray("typlots");
            for (int i = 0; i < plots.size(); ++i) {
                settings.setArrayIndex(i);
                settings.setValue("plotname",plots.at(i).plotname);
                settings.setValue("columnsandtables",plots.at(i).columnsandtables);
                settings.setValue("proprieties",plots.at(i).proprieties);
                settings.setValue("curves",plots.at(i).curves);

           }
            settings.endArray();


        }else if(p->at(0)->plottype==1){

            struct SurfacePlots{
             QString plotname;
             QString columnsandtables;
             QString proprieties;
             QString surface;
            };
            QList <SurfacePlots> plots;
            plots.clear();
            QSettings settings("CampaignAnalysis","surfaceplotsdef");
            int size = settings.beginReadArray("bananaplots");
            for (int i = 0; i < size; ++i) {
                settings.setArrayIndex(i);
                SurfacePlots plot;
                plot.plotname=settings.value("plotname").toString();
                plot.columnsandtables=settings.value("columnsandtables").toString();
                plot.proprieties=settings.value("proprieties").toString();
                plot.surface=settings.value("surface").toString();
                plots.append(plot);
           }
            settings.endArray();
            createstringstostoresurface(0);
            SurfacePlots plot;
            plot.plotname=plotname;
            plot.columnsandtables=closanstables;
            plot.proprieties=plotp;
            plot.surface= surfacep;
            bool exist;
            exist=false;
            int k;
            for (int i = 0; i < plots.count(); ++i) {
                if(plots.at(i).plotname==plotname) {
                   exist=true;
                   k=i;
                   break;
                }
            }
            if(exist){
                plots[k].plotname=plot.plotname;
                plots[k].columnsandtables=plot.columnsandtables;
                plots[k].proprieties=plot.proprieties;
                plots[k].surface=plot.surface;


            }else  plots.append(plot);

            settings.beginWriteArray("bananaplots");
            for (int i = 0; i < plots.size(); ++i) {
                settings.setArrayIndex(i);
                settings.setValue("plotname",plots.at(i).plotname);
                settings.setValue("columnsandtables",plots.at(i).columnsandtables);
                settings.setValue("proprieties",plots.at(i).proprieties);
                settings.setValue("surface",plots.at(i).surface);

           }
            settings.endArray();


        }else if(p->at(0)->plottype==2){

            struct HistoPlots{
             QString plotname;
             QString columnsandtables;
             QString proprieties;
             QString histos;
            };
            QList <HistoPlots> plots;
            plots.clear();
            QSettings settings("CampaignAnalysis","histoplotsdef");
            int size = settings.beginReadArray("histoplots");
            for (int i = 0; i < size; ++i) {
                settings.setArrayIndex(i);
                HistoPlots plot;
                plot.plotname=settings.value("plotname").toString();
                plot.columnsandtables=settings.value("columnsandtables").toString();
                plot.proprieties=settings.value("proprieties").toString();
                plot.histos=settings.value("histos").toString();
                plots.append(plot);
           }
            settings.endArray();
            createstringstostorehisto(0);
            HistoPlots plot;
            plot.plotname=plotname;
            plot.columnsandtables=closanstables;
            plot.proprieties=plotp;
            plot.histos= histosp;
            bool exist;
            exist=false;
            int k;
            for (int i = 0; i < plots.count(); ++i) {
                if(plots.at(i).plotname==plotname) {
                   exist=true;
                   k=i;
                   break;
                }
            }
            if(exist){
                plots[k].plotname=plot.plotname;
                plots[k].columnsandtables=plot.columnsandtables;
                plots[k].proprieties=plot.proprieties;
                plots[k].histos=plot.histos;


            }else  plots.append(plot);

            settings.beginWriteArray("histoplots");
            for (int i = 0; i < plots.size(); ++i) {
                settings.setArrayIndex(i);
                settings.setValue("plotname",plots.at(i).plotname);
                settings.setValue("columnsandtables",plots.at(i).columnsandtables);
                settings.setValue("proprieties",plots.at(i).proprieties);
                settings.setValue("histos",plots.at(i).histos);

           }
            settings.endArray();



    }
    }else{
        int order=-1;

         struct ScreenPlots{
          QString screenname;
          QString plottype;
          QString plotorder;
          QString columnsandtables;
          QString proprieties;
          QString surface;
          QString curves;
          QString histos;
         };
         QList <ScreenPlots> plots;
         plots.clear();
         QSettings settings("CampaignAnalysis","screendef");
         int size = settings.beginReadArray("screenplots");

         for (int i = 0; i < size; ++i) {
             settings.setArrayIndex(i);
             if(settings.value("screenname").toString()==plotname) continue;
             ScreenPlots plot;
             plot.screenname=settings.value("screenname").toString();
             plot.plottype=settings.value("plottype").toString();
             plot.plotorder=settings.value("plotorder").toString();
             plot.columnsandtables=settings.value("columnsandtables").toString();
             plot.proprieties=settings.value("proprieties").toString();
             plot.surface=settings.value("surface").toString();
             plot.curves=settings.value("curves").toString();
             plot.histos=settings.value("histos").toString();

             plots.append(plot);
        }
         settings.endArray();

         ScreenPlots plot;
         for(int i=0; i<p->count();i++){
             order++;

             if(p->at(i)->plottype==0){
                 surfacep="not relevant"; curvesp="not relevant"; histosp="not relevant";
                 createstringstostorety(i);
                 plot.screenname=plotname;
                 plot.plottype=QString::number(p->at(i)->plottype);
                 plot.plotorder=QString::number(order);
                 plot.columnsandtables=closanstables;
                 plot.proprieties=plotp;
                 plot.surface=surfacep;
                 plot.curves=curvesp;
                 plot.histos=histosp;

             }else if(p->at(i)->plottype==1){
                 surfacep="not relevant"; curvesp="not relevant"; histosp="not relevant";
                 createstringstostoresurface(i);
                 plot.screenname=plotname;
                 plot.plottype=QString::number(p->at(i)->plottype);
                 plot.plotorder=QString::number(order);
                 plot.columnsandtables=closanstables;
                 plot.proprieties=plotp;
                 plot.surface=surfacep;
                 plot.curves=curvesp;
                 plot.histos=histosp;

             }else if(p->at(i)->plottype==2){

                 surfacep="not relevant"; curvesp="not relevant"; histosp="not relevant";
                 createstringstostorehisto(i);
                 plot.screenname=plotname;
                 plot.plottype=QString::number(p->at(i)->plottype);
                 plot.plotorder=QString::number(order);
                 plot.columnsandtables=closanstables;
                 plot.proprieties=plotp;
                 plot.surface=surfacep;
                 plot.curves=curvesp;
                 plot.histos=histosp;

             }
              plots.append(plot);
         }




         settings.beginWriteArray("screenplots");
         for (int i = 0; i < plots.size(); ++i) {
             settings.setArrayIndex(i);
             settings.setValue("screenname",plots.at(i).screenname);
             settings.setValue("plottype",plots.at(i).plottype);
             settings.setValue("plotorder",plots.at(i).plotorder);
             settings.setValue("columnsandtables",plots.at(i).columnsandtables);
             settings.setValue("proprieties",plots.at(i).proprieties);
             settings.setValue("surface",plots.at(i).surface);
              settings.setValue("curves",plots.at(i).curves);
             settings.setValue("histos",plots.at(i).histos);

        }
         settings.endArray();


        }
    }


void configplotwidget::LogM(QString text)
{
    emit logmessages(text);
}
void configplotwidget::LogMR(QString text)
{
    emit logmessagesR(text);
}
