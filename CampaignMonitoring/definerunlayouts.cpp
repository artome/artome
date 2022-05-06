#include <QSettings>
#include <QMessageBox>
#include "definerunlayouts.h"
#include "ui_definerunlayouts.h"

DefineRunLayouts::DefineRunLayouts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DefineRunLayouts)
{
    ui->setupUi(this);
    connect(ui->addupdate,SIGNAL(pressed()),this,SLOT(savelayouts()));
    connect(ui->deleterun,SIGNAL(pressed()),this,SLOT(deletelayout()));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(readlayout(QModelIndex)));
     connect(&listrunmodel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(sellect(QStandardItem*)));

}

DefineRunLayouts::~DefineRunLayouts()
{
    delete ui;
}
void DefineRunLayouts::filllayouts()
{
    struct RunsL{
        QString name;
        QString command;
    };
    listrunmodel.clear();
    QStandardItem *parentItem = listrunmodel.invisibleRootItem();
    QList<QVariant> grafid;
    QStringList rootData;
    rootData << "Run Layouts";
    listrunmodel.setHorizontalHeaderLabels(rootData);

    QList<RunsL> runs;
    runs.clear();
    RunsL run;

    QSettings settings("CampaignMonitoring","runlayouts");
    int size = settings.beginReadArray("layouts");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        run.name = settings.value("name").toString();
        run.command = settings.value("command").toString();
        runs.append(run);
}
    settings.endArray();

    for(int i=0; i<runs.count();i++){
       QStandardItem *item = new QStandardItem(runs.at(i).name);
       grafid.clear();
       item->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/responsive12.svg")));
       grafid <<1 <<runs.at(i).name << runs.at(i).command;
       item->setData(grafid);
       item->setEditable(false);
       item->setCheckable(true);
       parentItem->appendRow(item);

    }

    ui->treeView->setModel(&listrunmodel);
    ui->treeView->show();

}
void DefineRunLayouts:: readlayout(QModelIndex index){
    QList<QVariant> list;
    list = listrunmodel.itemFromIndex(index)->data().toList();
    if (list[0].toInt() == -1) {
        return;
    }
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(list.at(2).toString());
    ui->layoutname->setText(list.at(1).toString());
}
void DefineRunLayouts::savelayouts()
{
    struct RunsL{
        QString name;
        QString command;
    };

    QList<RunsL> runs;
    runs.clear();
    RunsL run;

    QSettings settings("CampaignMonitoring","runlayouts");
    int size = settings.beginReadArray("layouts");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        run.name = settings.value("name").toString();
        run.command = settings.value("command").toString();
        runs.append(run);
}
    settings.endArray();

    settings.beginWriteArray("layouts");
    int k=0;
    bool alreadythere =false;
    for (int i = 0; i < runs.size(); ++i) {
        settings.setArrayIndex(k);
        if(runs.at(i).name==ui->layoutname->text().simplified()) {
                alreadythere=true;
            QMessageBox msgBox;
            msgBox.setText("This run layout name already exists");
            msgBox.setInformativeText("are you sure you want to replace it?");
            msgBox.setIcon(QMessageBox::Question);

            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

            int ret = msgBox.exec();
            switch (ret) {
            case QMessageBox::Yes:
                settings.setValue("name",ui->layoutname->text().simplified());
                settings.setValue("command",ui->plainTextEdit->toPlainText());
                k++;
                continue;
              break;
            case QMessageBox::No:
                settings.setValue("name",runs.at(i).name);
                settings.setValue("command",runs.at(i).command);
                k++;
                 continue;
              break;
            default:
              // should never be reached
              break;
            }
        }
            settings.setValue("name",runs.at(i).name);
            settings.setValue("command",runs.at(i).command);
             k++;

}
    if(!alreadythere){
        settings.setArrayIndex(k);
        settings.setValue("name",ui->layoutname->text().simplified());
        settings.setValue("command",ui->plainTextEdit->toPlainText());

    }
    settings.endArray();
    filllayouts();
}
void DefineRunLayouts::deletelayout()
{
    QString name;
    QList<QVariant> list;

    for (int i = 0; i < listrunmodel.invisibleRootItem()->rowCount(); i++) {
         if(listrunmodel.invisibleRootItem()->child(i)->checkState()){
             list = listrunmodel.invisibleRootItem()->child(i)->data().toList();
             name =list.at(1).toString();
             break;
         }
        }
    QMessageBox msgBox;

    msgBox.setText("Are you sure you want to remove the run");
    msgBox.setInformativeText(name);
    msgBox.setIcon(QMessageBox::Question);

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
      break;
    case QMessageBox::No:
     return;
      break;
    default:
      // should never be reached
      break;
    }
    struct RunsL{
        QString name;
        QString command;
    };

    QList<RunsL> runs;
    runs.clear();
    RunsL run;

    QSettings settings("CampaignMonitoring","runlayouts");
    int size = settings.beginReadArray("layouts");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        run.name = settings.value("name").toString();
        run.command = settings.value("command").toString();
        runs.append(run);
}
    settings.endArray();

    settings.beginWriteArray("layouts");
    int k=0;
     for (int i = 0; i < runs.size(); ++i) {
         if(runs.at(i).name==name) continue;
         settings.setArrayIndex(k);
         settings.setValue("name",runs.at(i).name);
         settings.setValue("command",runs.at(i).command);
         k++;
     }
settings.endArray();
filllayouts();
}
void DefineRunLayouts::sellect(QStandardItem *item)
{
   if(!(item->checkState()==Qt::Checked)){

       return;
   }
   QList<QVariant> list;
   list = item->data().toList();

   for (int j = 0; j <listrunmodel.invisibleRootItem()->rowCount(); j++) {
       for (int i = 0; i <listrunmodel.invisibleRootItem()->child(j)->rowCount(); i++) {
           if (listrunmodel.invisibleRootItem()->child(j)->child(i)->checkState() && listrunmodel.invisibleRootItem()->child(j)->child(i) !=item) {
               listrunmodel.invisibleRootItem()->child(j)->child(i)->setCheckState(Qt::Unchecked);


           }
       }
   }
}
