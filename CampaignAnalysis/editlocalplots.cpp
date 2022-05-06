#include "editlocalplots.h"
#include "ui_editlocalplots.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>
#include <QMessageBox>
#include <QSettings>
#include <QRegularExpression>

Editlocalplots::Editlocalplots(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editlocalplots)
{
    ui->setupUi(this);
    connect(ui->loadtyplot,&QAbstractButton::clicked,this,&Editlocalplots::filltyplots);
    connect(ui->LoadSplot,&QAbstractButton::clicked,this,&Editlocalplots::fillSplots);
    connect(ui->LoadHplots,&QAbstractButton::clicked,this,&Editlocalplots::fillHplots);
    connect(ui->loadScreens,&QAbstractButton::clicked,this,&Editlocalplots::fillAplots);
    connect(ui->commitplot,&QAbstractButton::clicked,this,&Editlocalplots::commitplot);
    connect(ui->removeplot,&QAbstractButton::clicked,this,&Editlocalplots::deleteplots);
    connect(ui->refresh,&QAbstractButton::clicked,this,&Editlocalplots::plotsfilltree);

    connect(&listplotmodel, &QStandardItemModel::itemChanged, this, &Editlocalplots::sellect);
    connect(&dlistplotmodel, &QStandardItemModel::itemChanged, this, &Editlocalplots::sellectd);


}

Editlocalplots::~Editlocalplots()
{
    delete ui;
}

void Editlocalplots::fillSplots(){
    QString line,filename;
    QStringList filesnames;

      QFileDialog dialog(this);
      dialog.setFileMode(QFileDialog::ExistingFile);
      dialog.setNameFilter(tr(" Surface (surface*)"));

      if(dialog.exec())filesnames=dialog.selectedFiles();
      if(filesnames.count())filename=filesnames.at(0); else return;
      QFile fplots(filename);
      if(!fplots.open(QFile::ReadOnly)){
          ui->textBrowser->append("unable to open file " + filename);
          return;
        }
      int n=listplotmodel.invisibleRootItem()->child(1)->rowCount();
      for(int i=0; i< n; i++ ){
          listplotmodel.invisibleRootItem()->child(1)->removeRow(0);
      }
      QTextStream stream(&fplots);
      QList<QVariant> itemdata;
       QFileInfo info(filename);
       ui->LoadSplot->setText(info.baseName());
       QStringList columns;
      stream.readLine();
      do {
          line = stream.readLine();
          if(!line.isNull()){
             columns=line.split(QRegularExpression("\t"));
             if(columns.count()!=4) continue;
             itemdata.clear();
             QStandardItem *item = new QStandardItem(columns.at(0));
             itemdata << 2<< columns.at(0)<< columns.at(1)<< columns.at(2)<< columns.at(3);
             item->setData(itemdata);
             item->setEditable(false);
             item->setCheckable(true);
             item->setToolTip(columns.at(1));
             Bananaitem->appendRow(item);

          }

    } while (!line.isNull());
    fplots.close();
    ui->plotstoload->setModel(&listplotmodel);
    ui->plotstoload->show();

}
void Editlocalplots::fillHplots(){
    QString line,filename;
    QStringList filesnames;

      QFileDialog dialog(this);
      dialog.setFileMode(QFileDialog::ExistingFile);
      dialog.setNameFilter(tr(" histo (histo*)"));

      if(dialog.exec())filesnames=dialog.selectedFiles();
      if(filesnames.count())filename=filesnames.at(0); else return;
      QFile fplots(filename);
      if(!fplots.open(QFile::ReadOnly)){
          ui->textBrowser->append("unable to open file " + filename);
          return;
        }
      int n=listplotmodel.invisibleRootItem()->child(2)->rowCount();
      for(int i=0; i< n; i++ ){
          listplotmodel.invisibleRootItem()->child(2)->removeRow(0);
      }
      QTextStream stream(&fplots);
      QList<QVariant> itemdata;
       QFileInfo info(filename);
       ui->LoadHplots->setText(info.baseName());
       QStringList columns;
      stream.readLine();
      do {
          line = stream.readLine();
          if(!line.isNull()){
             columns=line.split(QRegularExpression("\t"));
             if(columns.count()!=4) continue;
             itemdata.clear();
             QStandardItem *item = new QStandardItem(columns.at(0));
             itemdata << 3<< columns.at(0)<< columns.at(1)<< columns.at(2)<< columns.at(3);
             item->setData(itemdata);
             item->setEditable(false);
             item->setCheckable(true);
             item->setToolTip(columns.at(1));
             Histoitem->appendRow(item);

          }

    } while (!line.isNull());
    fplots.close();
    ui->plotstoload->setModel(&listplotmodel);
    ui->plotstoload->show();

}
void Editlocalplots::fillAplots(){
    QString line,filename;
    QStringList filesnames;

      QFileDialog dialog(this);
      dialog.setFileMode(QFileDialog::ExistingFile);
      dialog.setNameFilter(tr(" screen (screen*)"));

      if(dialog.exec())filesnames=dialog.selectedFiles();
      if(filesnames.count())filename=filesnames.at(0); else return;
      QFile fplots(filename);
      if(!fplots.open(QFile::ReadOnly)){
          ui->textBrowser->append("unable to open file " + filename);
          return;
        }
      int n=listplotmodel.invisibleRootItem()->child(3)->rowCount();
      for(int i=0; i< n; i++ ){
          listplotmodel.invisibleRootItem()->child(3)->removeRow(0);
      }
      QTextStream stream(&fplots);
      QList<QVariant> itemdata;
       QFileInfo info(filename);
       ui->loadScreens->setText(info.baseName());
       QStringList columns;
      stream.readLine();
       itemdata.clear();
      bool first=true;
      QString name="first";
      do {
          line = stream.readLine();
          if(!line.isNull()){
             columns=line.split(QRegularExpression("\t"));
             if(columns.count()!=8) continue;

               if(!first && (name!=columns.at(0))){
                   QStandardItem *item = new QStandardItem(name);
                 item->setData(itemdata);
                 item->setEditable(false);
                 item->setCheckable(true);
                 item->setToolTip(columns.at(1));
                 Screenitem->appendRow(item);
                  itemdata.clear();
                  name=columns.at(0);
                  itemdata << 10<< columns.at(0)<< columns.at(1)<< columns.at(2)<< columns.at(3);
                  itemdata << columns.at(4)<< columns.at(5)<< columns.at(6)<< columns.at(7);
             }else{
                   if(first)name=columns.at(0);
             itemdata << 10<< columns.at(0)<< columns.at(1)<< columns.at(2)<< columns.at(3);
             itemdata << columns.at(4)<< columns.at(5)<< columns.at(6)<< columns.at(7);
             }

          }
   first=false;
    } while (!line.isNull());
      if(itemdata.count()){
         QStandardItem *item = new QStandardItem(name);
        item->setData(itemdata);
        item->setEditable(false);
        item->setCheckable(true);
        item->setToolTip(columns.at(1));
        Screenitem->appendRow(item);
         itemdata.clear();
      }
    fplots.close();
    ui->plotstoload->setModel(&listplotmodel);
    ui->plotstoload->show();

}




void Editlocalplots::fillplots(){
    listplotmodel.clear();


    listplotmodel.clear();
    QStandardItem *parentItem = listplotmodel.invisibleRootItem();
    QList<QVariant> grafid;
    Graphitem   = new QStandardItem("timevalues-plots");
    Bananaitem  = new QStandardItem("Surface-plots");
    Histoitem   = new QStandardItem("Histo-plots");
    Screenitem   = new QStandardItem("Agregated-plots");

    // parentItem->setColumnCount(3); o 1 que aparecia era a coluna
    QStringList rootData;
    QString     ttip;
    long long   iparent;
    iparent = -1;
    rootData << "Plots stored in files";
    listplotmodel.setHorizontalHeaderLabels(rootData);

    grafid.clear();
    grafid <<  iparent;
    Graphitem->setData(grafid);
    ttip.clear();
    ttip.append("Plots stored in a file. Multiaxis or single axis");
    Graphitem->setToolTip(ttip);
    parentItem->appendRow(Graphitem);

    grafid.clear();
    grafid  << iparent;
    Bananaitem->setData(grafid);
    ttip.clear();
    ttip.append("Surface  Plots (as bananas) stored in a file");
    Bananaitem->setToolTip(ttip);
    parentItem->appendRow(Bananaitem);

    grafid.clear();
    grafid << iparent;
    Histoitem->setData(grafid);
    ttip.clear();
    ttip.append("Histogram plots stored in file");
    Histoitem->setToolTip(ttip);
    parentItem->appendRow(Histoitem);

    grafid.clear();
    grafid << iparent;
    Screenitem->setData(grafid);
    ttip.clear();
    ttip.append("Agregated plots stored in file");
    Screenitem->setToolTip(ttip);
    parentItem->appendRow(Screenitem);


}

void Editlocalplots::filltyplots(){
    QString line,filename;
    QStringList filesnames;

      QFileDialog dialog(this);
      dialog.setFileMode(QFileDialog::ExistingFile);
      dialog.setNameFilter(tr(" Typlots (typlots*)"));

      if(dialog.exec())filesnames=dialog.selectedFiles();
      if(filesnames.count())filename=filesnames.at(0); else return;
      QFile fplots(filename);
      if(!fplots.open(QFile::ReadOnly)){
          ui->textBrowser->append("unable to open file " + filename);
          return;
        }
      int n=listplotmodel.invisibleRootItem()->child(0)->rowCount();
      for(int i=0; i< n; i++ ){
          listplotmodel.invisibleRootItem()->child(0)->removeRow(0);
      }
      QTextStream stream(&fplots);
      QList<QVariant> itemdata;
       QFileInfo info(filename);
       ui->loadtyplot->setText(info.baseName());
       QStringList columns;
      stream.readLine();
      do {
          line = stream.readLine();
          if(!line.isNull()){
             columns=line.split(QRegularExpression("\t"));
             if(columns.count()!=4) continue;
             itemdata.clear();
             QStandardItem *item = new QStandardItem(columns.at(0));
             itemdata << 1<< columns.at(0)<< columns.at(1)<< columns.at(2)<< columns.at(3);
             item->setData(itemdata);
             item->setEditable(false);
             item->setCheckable(true);
             item->setToolTip(columns.at(1));
             Graphitem->appendRow(item);

          }

    } while (!line.isNull());
    fplots.close();
    ui->plotstoload->setModel(&listplotmodel);
    ui->plotstoload->show();

}
void Editlocalplots::commitplot(){
   QList<QVariant> list;
   QString plotname;  plotname.clear();
    for (int j = 0; j < listplotmodel.invisibleRootItem()->rowCount(); j++) {
        for (int i = 0; i <listplotmodel.invisibleRootItem()->child(j)->rowCount(); i++) {
            if (listplotmodel.invisibleRootItem()->child(j)->child(i)->checkState()) {
                list = listplotmodel.invisibleRootItem()->child(j)->child(i)->data().toList();
                plotname=listplotmodel.invisibleRootItem()->child(j)->child(i)->text();
                goto theEnd; //leaving also the outer for  loop

            }
        }
    }
   theEnd:
   emit plots(list,plotname);

}


void Editlocalplots::sellectd(QStandardItem *item)
{
   if(!(item->checkState()==Qt::Checked)){

       return;
   }
   QList<QVariant> list;
   list = item->data().toList();

   for (int j = 0; j < dlistplotmodel.invisibleRootItem()->rowCount(); j++) {
           if (dlistplotmodel.invisibleRootItem()->child(j)->checkState() && dlistplotmodel.invisibleRootItem()->child(j) !=item) {
               dlistplotmodel.invisibleRootItem()->child(j)->setCheckState(Qt::Unchecked);
           }
   }

}
void Editlocalplots::sellect(QStandardItem *item)
{
   if(!(item->checkState()==Qt::Checked)){

       return;
   }
   QList<QVariant> list;
   list = item->data().toList();

   for (int j = 0; j < listplotmodel.invisibleRootItem()->rowCount(); j++) {
       for (int i = 0; i <listplotmodel.invisibleRootItem()->child(j)->rowCount(); i++) {
           if (listplotmodel.invisibleRootItem()->child(j)->child(i)->checkState() && listplotmodel.invisibleRootItem()->child(j)->child(i) !=item) {
               listplotmodel.invisibleRootItem()->child(j)->child(i)->setCheckState(Qt::Unchecked);


           }
       }
   }





}

void Editlocalplots::LogMessages(QString mess){
    ui->textBrowser->append(mess);

}
void Editlocalplots::LogMessagesR(QString mess){
    ui->textBrowser->setTextColor("red");
    ui->textBrowser->append(mess);
    ui->textBrowser->setTextColor("black");


}

void Editlocalplots::plotsfilltree()
{

        dlistplotmodel.clear();
        QStandardItem *parentItem = dlistplotmodel.invisibleRootItem();
        QList<QVariant> grafid;
        QStringList rootData;
        QString     ttip;
        rootData << "Plots";
        dlistplotmodel.setHorizontalHeaderLabels(rootData);
{
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


        for (int i = 0; i < plots.count(); ++i) {

            QStandardItem *item = new QStandardItem(plots.at(i).plotname);
            grafid.clear();
             item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/banana4.svg")));

             grafid <<-12 <<plots.at(i).plotname << plots.at(i).columnsandtables ;

            item->setData(grafid);
            item->setEditable(false);
            item->setCheckable(true);
             parentItem->appendRow(item);
}

 }


{


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

        for(int i=0;i<plots.count(); i++)
        {
        QStandardItem *item = new QStandardItem(plots.at(i).plotname );
        grafid.clear();
         item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/educational15.svg")));
         grafid <<-11 <<plots.at(i).plotname << plots.at(i).columnsandtables ;
        item->setData(grafid);
        item->setEditable(false);

        item->setCheckable(true);

        item->setToolTip(ttip);
         parentItem->appendRow(item);

        grafid.clear();

          }
}
{
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

        for (int i = 0; i <plots.count(); ++i) {


            QStandardItem *item = new QStandardItem(plots.at(i).plotname);
            grafid.clear();
            item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/chart15.svg")));
            grafid <<-13 <<plots.at(i).plotname << plots.at(i).columnsandtables ;
            item->setData(grafid);
            item->setEditable(false);
            item->setCheckable(true);
             parentItem->appendRow(item);

            grafid.clear();
      }


}
        {         struct ScreenPlots{
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
                    ScreenPlots plot;
                    plot.screenname=settings.value("screenname").toString();
                    plot.plottype=settings.value("plottype").toString();
                    plot.plotorder=settings.value("plotorder").toString();
                    plot.columnsandtables=settings.value("columnsandtables").toString();
                    plot.proprieties=settings.value("proprieties").toString();
                    plot.surface=settings.value("columnsandtables").toString();
                    plot.curves=settings.value("surface").toString();
                    plot.histos=settings.value("histos").toString();

                    plots.append(plot);
               }
                settings.endArray();
                QString NamePlot;
                QStringList scrennames;
                scrennames.clear();
                NamePlot="NAMEEXDRUXULOQUE NUNCADEVERÀEXISTIR";
                for (int i = 0; i < plots.count(); ++i) {
                   if(plots.at(i).screenname!=NamePlot) scrennames <<  plots.at(i).screenname;
                   NamePlot=plots.at(i).screenname;
                }
                for (int i = 0; i < scrennames.count(); ++i) {
                QStandardItem *item = new QStandardItem(scrennames.at(i));
                grafid.clear();
                item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/screens.svg")));
                grafid <<-110 <<scrennames.at(i) ;
                item->setData(grafid);
                item->setEditable(false);
                  item->setCheckable(true);
                parentItem->appendRow(item);

                grafid.clear();
                }
 }

                ui->plotsatdabase->setModel(&dlistplotmodel);
                ui->plotsatdabase->show();

}


void  Editlocalplots::deleteplots()
{


    QString name,tname;
    QList<QVariant> list;

    for (int i = 0; i < dlistplotmodel.invisibleRootItem()->rowCount(); i++) {
         if(dlistplotmodel.invisibleRootItem()->child(i)->checkState()){
             list = dlistplotmodel.invisibleRootItem()->child(i)->data().toList();
             if (list.at(0).toInt()==-11) tname="typlots";
             else if (list.at(0).toInt()==-12) tname="Surfaceplots";
             else if (list.at(0).toInt()==-13) tname="Histoplots";
             else if (list.at(0).toInt()==-110) tname="Screens";

             name =list.at(1).toString();
             break;
         }
        }


    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to delete the plot");
    msgBox.setInformativeText(name);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
    {
          if(tname.contains("typlots")){
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
            if(plot.plotname==name) continue;
            plots.append(plot);
       }
        settings.endArray();

        settings.beginWriteArray("typlots");
        for (int i = 0; i < plots.size(); ++i) {
            settings.setArrayIndex(i);
            settings.setValue("plotname",plots.at(i).plotname);
            settings.setValue("columnsandtables",plots.at(i).columnsandtables);
            settings.setValue("proprieties",plots.at(i).proprieties);
            settings.setValue("curves",plots.at(i).curves);

       }
        settings.endArray();

        }else if(tname.contains("Histoplots")){
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
                  if(plot.plotname==name) continue;
                  plots.append(plot);
             }
              settings.endArray();
              settings.beginWriteArray("histoplots");
              for (int i = 0; i < plots.size(); ++i) {
                  settings.setArrayIndex(i);
                  settings.setValue("plotname",plots.at(i).plotname);
                  settings.setValue("columnsandtables",plots.at(i).columnsandtables);
                  settings.setValue("proprieties",plots.at(i).proprieties);
                  settings.setValue("histos",plots.at(i).histos);
             }
              settings.endArray();

                        }else if(tname.contains("Surfaceplots")){

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
                     if(plot.plotname==name) continue;
                    plots.append(plot);
               }
                settings.endArray();

                settings.beginWriteArray("bananaplots");
                for (int i = 0; i < plots.size(); ++i) {
                    settings.setArrayIndex(i);
                    settings.setValue("plotname",plots.at(i).plotname);
                    settings.setValue("columnsandtables",plots.at(i).columnsandtables);
                    settings.setValue("proprieties",plots.at(i).proprieties);
                    settings.setValue("surface",plots.at(i).surface);

               }
                settings.endArray();

                        }else if(tname.contains("Screens")){
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
                   ScreenPlots plot;
                  plot.screenname=settings.value("screenname").toString();
                  plot.plottype=settings.value("plottype").toString();
                  plot.plotorder=settings.value("plotorder").toString();
                  plot.columnsandtables=settings.value("columnsandtables").toString();
                  plot.proprieties=settings.value("proprieties").toString();
                  plot.surface=settings.value("surface").toString();
                  plot.curves=settings.value("curves").toString();
                  plot.histos=settings.value("histos").toString();
                  if(plot.screenname==name) continue;
                  plots.append(plot);
             }
              settings.endArray();
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

                   plotsfilltree();
                    break;
            }     case QMessageBox::No:
        // Don't Save was clicked
        return;
        break;
      default:
        // should never be reached
        break;

    }


}

