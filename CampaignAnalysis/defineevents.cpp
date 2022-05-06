#include "defineevents.h"
#include "ui_defineevents.h"
#include <QSettings>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>
#include <QMessageBox>

DefineEvents::DefineEvents(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DefineEvents)
{
    ui->setupUi(this);
    connect(ui->AddButton,&QAbstractButton::clicked,this, &DefineEvents::addevent);
    connect(ui->loadevents,&QAbstractButton::clicked,this, &DefineEvents::fillfileevents);
    connect(ui->commitevent,&QAbstractButton::clicked,this, &DefineEvents::commitevent);
    connect(&listeventmodel,&QStandardItemModel::itemChanged,this,&DefineEvents::sellect);
    connect(&dlisteventmodel,&QStandardItemModel::itemChanged,this,&DefineEvents::sellectd);
    connect(ui->EditEvents,&QAbstractButton::clicked,this, &DefineEvents::editevent);
    connect(ui->removeevent,&QAbstractButton::clicked,this, &DefineEvents::removeevent);
    connect(ui->updateevent,&QAbstractButton::clicked,this, &DefineEvents::updateevent);
}

DefineEvents::~DefineEvents()
{
    delete ui;
}

void DefineEvents::updateevent(){

  int k;
  k= ui->spinBox->value();
  if(k==-1) return;
    struct Events {
        QString RunNumber;
        long long StartTime;
        long long EndTime;
        QString Type;
        QString Description;
        QString Comments;

    };
    QString Eventsfor;
    Eventsfor="Eventsfor";
    Eventsfor.append(databasename);

    QList<Events> runevents;
    runevents.clear();
    QSettings settings("CampaignAnalysis",Eventsfor);
    int size = settings.beginReadArray("Events");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        Events runs;
        runs.RunNumber = settings.value("RunNumber").toString();
        runs.StartTime = settings.value("StarTime").toLongLong();
        runs.EndTime = settings.value("EndTime").toLongLong();
        runs.Type = settings.value("Type").toString();
        runs.Description = settings.value("Description").toString();
        runs.Comments = settings.value("Comments").toString();
        runevents.append(runs);
    }
    settings.endArray();

    runevents[k].RunNumber=ui->RunNumber->text();
    runevents[k].StartTime=ui->start->dateTime().toMSecsSinceEpoch();
    runevents[k].EndTime=ui->end->dateTime().toMSecsSinceEpoch();
    runevents[k].Type=ui->Type->text();
    runevents[k].Description=ui->Description->text();
    runevents[k].Comments=ui->Comments->text();

    settings.remove("Events");
    settings.beginWriteArray("Events");
       for (int i = 0; i < runevents.size(); ++i) {
           settings.setArrayIndex(i);
           settings.setValue("RunNumber",runevents.at(i).RunNumber);
           settings.setValue("StartTime",runevents.at(i).StartTime);
           settings.setValue("EndTime",runevents.at(i).EndTime);
           settings.setValue("Type",runevents.at(i).Type);
           settings.setValue("Description",runevents.at(i).Description);
           settings.setValue("Comments",runevents.at(i).Comments);
       }
       settings.endArray();


    fillevents();
    ui->spinBox->setValue(-1);

}

void DefineEvents::editevent()
{

    int k=-1;

    QList<QVariant> list;
    QString eventname;  eventname.clear();
     for (int j = 0; j < listeventmodel.invisibleRootItem()->rowCount(); j++) {
             if (listeventmodel.invisibleRootItem()->child(j)->checkState()) {
                 list = listeventmodel.invisibleRootItem()->child(j)->data().toList();
                 eventname=listeventmodel.invisibleRootItem()->child(j)->text();
                   k=j;
                 break;

         }
     }


     if(k==-1) return;
 //    list=listeventmodel.itemFromIndex(index)->data().toList();
     ui->RunNumber->setText(list.at(1).toString());
     QDateTime time;
     time.setTimeSpec(Qt::UTC);
     time=time.fromString(list.at(0).toString(),"yyyy/MM/dd hh:mm:ss");
     time.setTimeSpec(Qt::UTC);
     ui->start->setDateTime(time);
     time=time.fromString(list.at(4).toString(),"yyyy/MM/dd hh:mm:ss");
     ui->end->setDateTime(time);
     ui->Description->setText(list.at(3).toString());
     ui->Type->setText(list.at(2).toString());
     ui->Comments->setText(list.at(5).toString());
     ui->spinBox->setValue(k);



}







void DefineEvents::sellectd(QStandardItem *item)
{
   if(!(item->checkState()==Qt::Checked)){

       return;
   }
   QList<QVariant> list;
   list = item->data().toList();

   for (int j = 0; j < dlisteventmodel.invisibleRootItem()->rowCount(); j++) {
           if (dlisteventmodel.invisibleRootItem()->child(j)->checkState() && dlisteventmodel.invisibleRootItem()->child(j) !=item) {
               dlisteventmodel.invisibleRootItem()->child(j)->setCheckState(Qt::Unchecked);
           }
   }

}
void DefineEvents::sellect(QStandardItem *item)
{
   if(!(item->checkState()==Qt::Checked)){

       return;
   }
   QList<QVariant> list;
   list = item->data().toList();

   for (int j = 0; j < listeventmodel.invisibleRootItem()->rowCount(); j++) {
           if (listeventmodel.invisibleRootItem()->child(j)->checkState() && listeventmodel.invisibleRootItem()->child(j) !=item) {
               listeventmodel.invisibleRootItem()->child(j)->setCheckState(Qt::Unchecked);
           }
   }

}


void DefineEvents::settimewidgets()
{
   ui->start->setDateTime(QDateTime::fromMSecsSinceEpoch(itime));
   ui->end->setDateTime(QDateTime::fromMSecsSinceEpoch(ltime));

}
void DefineEvents::commitevent(){
   QList<QVariant> list;
   QString eventname;  eventname.clear();
    for (int j = 0; j < dlisteventmodel.invisibleRootItem()->rowCount(); j++) {
            if (dlisteventmodel.invisibleRootItem()->child(j)->checkState()) {
                list = dlisteventmodel.invisibleRootItem()->child(j)->data().toList();
                eventname=dlisteventmodel.invisibleRootItem()->child(j)->text();
                break;

        }
    }
    ui->RunNumber->setText(list.at(1).toString());
    QDateTime time;
    time.setTimeSpec(Qt::UTC);
    time=time.fromString(list.at(0).toString(),"yyyy/MM/dd hh:mm:ss");
    time.setTimeSpec(Qt::UTC);
    ui->start->setDateTime(time);
    time=time.fromString(list.at(4).toString(),"yyyy/MM/dd hh:mm:ss");
    ui->end->setDateTime(time);
    ui->Description->setText(list.at(3).toString());
    ui->Type->setText(list.at(2).toString());
    ui->Comments->setText(list.at(5).toString());

}
void DefineEvents::fillfileevents(){
    dlisteventmodel.clear();
    QStandardItem *parentItem = dlisteventmodel.invisibleRootItem();
    QList<QVariant> itemdata;
    QStringList rootData;
    rootData << "Events at an external file";
    dlisteventmodel.setHorizontalHeaderLabels(rootData);

    QString line,filename;
    QStringList filesnames;

      QFileDialog dialog(this);
      dialog.setFileMode(QFileDialog::ExistingFile);
      dialog.setNameFilter(tr(" Events (*)"));

      if(dialog.exec())filesnames=dialog.selectedFiles();
      if(filesnames.count())filename=filesnames.at(0); else return;
      QFile fevents(filename);
      if(!fevents.open(QFile::ReadOnly)){
          ui->textBrowser->append("unable to open file " + filename);
          return;
        }
      int n=dlisteventmodel.invisibleRootItem()->rowCount();
      for(int i=0; i< n; i++ ){
          dlisteventmodel.invisibleRootItem()->removeRow(0);
      }
      QTextStream stream(&fevents);
       QFileInfo info(filename);
       ui->loadevents->setText(info.baseName());
       QStringList columns;
      stream.readLine();
      do {
          line = stream.readLine();
          if(!line.isNull()){
             columns=line.split(QRegularExpression("\t"));
             if(columns.count()!=6) continue;
             itemdata.clear();
             QStandardItem *item = new QStandardItem(columns.at(1));
             itemdata << columns.at(0)<< columns.at(1)<< columns.at(2)<< columns.at(3) << columns.at(4)<< columns.at(5);
             item->setData(itemdata);
             item->setEditable(false);
             item->setCheckable(true);
             item->setToolTip(columns.at(3));
             parentItem->appendRow(item);

          }

    } while (!line.isNull());
    fevents.close();
    ui->eventstoload->setModel(&dlisteventmodel);
    ui->eventstoload->show();


}
 void DefineEvents::fillevents()
 {
     struct Events {
         QString RunNumber;
         long long StartTime;
         long long EndTime;
         QString Type;
         QString Description;
         QString Comments;

     };
     QString Eventsfor;
     Eventsfor="Eventsfor";
     Eventsfor.append(databasename);

     QList<Events> runevents;
     runevents.clear();
     QSettings settings("CampaignAnalysis",Eventsfor);
     int size = settings.beginReadArray("Events");
     for (int i = 0; i < size; ++i) {
         settings.setArrayIndex(i);
         Events runs;
         runs.RunNumber = settings.value("RunNumber").toString();
         runs.StartTime = settings.value("StartTime").toLongLong();
         runs.EndTime = settings.value("EndTime").toLongLong();
         runs.Type = settings.value("Type").toString();
         runs.Description = settings.value("Description").toString();
         runs.Comments = settings.value("Comments").toString();
         runevents.append(runs);
     }
     settings.endArray();

     listeventmodel.clear();
     int n=listeventmodel.invisibleRootItem()->rowCount();
     for(int i=0; i< n; i++ ){
         listeventmodel.invisibleRootItem()->removeRow(0);
     }

     QStandardItem *parentItem = listeventmodel.invisibleRootItem();
     QList<QVariant> itemdata;
     QStringList rootData;
     rootData << "Stored events ";
     listeventmodel.setHorizontalHeaderLabels(rootData);

     for(int i=0; i< runevents.count(); i++){
        itemdata.clear();
        QStandardItem *item = new QStandardItem(runevents.at(i).RunNumber);
        itemdata <<runevents.at(i).StartTime<< runevents.at(i).RunNumber<< runevents.at(i).Type<< runevents.at(i).Description << runevents.at(i).EndTime<< runevents.at(i).Comments;
        item->setData(itemdata);
        item->setEditable(false);
        item->setCheckable(true);
        item->setToolTip(runevents.at(i).Description);
        parentItem->appendRow(item);
   }
     ui->storedevents->setModel(&listeventmodel);
     ui->storedevents->show();

 }
void DefineEvents::addevent()
{
    struct Events {
        QString RunNumber;
        long long StartTime;
        long long EndTime;
        QString Type;
        QString Description;
        QString Comments;

    };
    QString Eventsfor;
    Eventsfor="Eventsfor";
     Eventsfor.append(databasename);

    QList<Events> runevents;
    runevents.clear();
    QSettings settings("CampaignAnalysis",Eventsfor);
    int size = settings.beginReadArray("Events");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        Events runs;
        runs.RunNumber = settings.value("RunNumber").toString();
        runs.StartTime = settings.value("StartTime").toLongLong();
        runs.EndTime = settings.value("EndTime").toLongLong();
        runs.Type = settings.value("Type").toString();
        runs.Description = settings.value("Description").toString();
        runs.Comments = settings.value("Comments").toString();
        runevents.append(runs);
    }
    settings.endArray();
    Events newrun;
    newrun.RunNumber=ui->RunNumber->text();
    newrun.StartTime=ui->start->dateTime().toMSecsSinceEpoch();
    newrun.EndTime=ui->end->dateTime().toMSecsSinceEpoch();
    newrun.Type=ui->Type->text();
    newrun.Description=ui->Description->text();
    newrun.Comments=ui->Comments->text();
    runevents.append(newrun);


    settings.remove("Events");
    settings.beginWriteArray("Events");
       for (int i = 0; i < runevents.size(); ++i) {
           settings.setArrayIndex(i);
           settings.setValue("RunNumber",runevents.at(i).RunNumber);
           settings.setValue("StartTime",runevents.at(i).StartTime);
           settings.setValue("EndTime",runevents.at(i).EndTime);
           settings.setValue("Type",runevents.at(i).Type);
           settings.setValue("Description",runevents.at(i).Description);
           settings.setValue("Comments",runevents.at(i).Comments);
       }
       settings.endArray();
fillevents();
ui->spinBox->setValue(-1);
}
void DefineEvents::removeevent(){
    QList<QVariant> list;
    int kd;
    QString eventname;  eventname.clear();
     for (int j = 0; j < listeventmodel.invisibleRootItem()->rowCount(); j++) {
              list = listeventmodel.invisibleRootItem()->child(j)->data().toList();
             if (listeventmodel.invisibleRootItem()->child(j)->checkState()) {
                 list = listeventmodel.invisibleRootItem()->child(j)->data().toList();
                 eventname=listeventmodel.invisibleRootItem()->child(j)->text();
                 kd=j;
                 break;
         }
     }

     QMessageBox msgBox;

         msgBox.setText("Are you sure you want to remove the event");
         msgBox.setInformativeText(eventname);
         msgBox.setIcon(QMessageBox::Question);
         msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
         msgBox.setDefaultButton(QMessageBox::Yes);
         int ret = msgBox.exec();
         switch (ret) {
         case QMessageBox::Yes:
         {
                struct Events {
                 QString RunNumber;
                 long long StartTime;
                 long long EndTime;
                 QString Type;
                 QString Description;
                 QString Comments;

             };
             QString Eventsfor;
             Eventsfor="Eventsfor";
              Eventsfor.append(databasename);

             QList<Events> runevents;
             runevents.clear();
             QSettings settings("CampaignAnalysis",Eventsfor);
             int size = settings.beginReadArray("Events");
             for (int i = 0; i < size; ++i) {
                 settings.setArrayIndex(i);
                 Events runs;
                 runs.RunNumber = settings.value("RunNumber").toString();
                 runs.StartTime = settings.value("StarTime").toLongLong();
                 runs.EndTime = settings.value("EndTime").toLongLong();
                 runs.Type = settings.value("Type").toString();
                 runs.Description = settings.value("Description").toString();
                 runs.Comments = settings.value("Comments").toString();
                 runevents.append(runs);
             }
             settings.endArray();

             settings.remove("Events");
             settings.beginWriteArray("Events");
             int k=0;
             for (int i = 0; i < runevents.size(); ++i) {
                 if(kd==i) continue;
                 settings.setArrayIndex(k);
                 settings.setValue("RunNumber",runevents.at(i).RunNumber);
                 settings.setValue("StartTime",runevents.at(i).StartTime);
                 settings.setValue("EndTime",runevents.at(i).EndTime);
                 settings.setValue("Type",runevents.at(i).Type);
                 settings.setValue("Description",runevents.at(i).Description);
                 settings.setValue("Comments",runevents.at(i).Comments);
                 k++;
             }
             settings.endArray();
             fillevents();
             ui->spinBox->setValue(-1);
             // Save was clicked
         }
             break;
         case QMessageBox::Cancel:
             // Do nothing
             break;

         default:
             // should never be reached
             break;
         }

}
