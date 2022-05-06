#include "previouslogins.h"
#include "ui_previouslogins.h"

PreviousLogins::PreviousLogins(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviousLogins)
{
    ui->setupUi(this);
    connect(ui->listView,&QAbstractItemView::doubleClicked ,this,&PreviousLogins::chosen);


}

PreviousLogins::~PreviousLogins()
{
    delete ui;
}
void PreviousLogins::fillListView(QStringList stored){

    QStandardItem *cparentItem = model.invisibleRootItem();
    QList<QVariant> itemdata;
    QStringList crootData;
    crootData.clear();
    crootData << "Previous Logins";
    model.setHorizontalHeaderLabels(crootData);
    QStringList one;

    for(int i=0; i<stored.size(); i++){
        one=stored[i].split(QRegularExpression(","));
        QStandardItem *item = new QStandardItem(stored.at(i));
        item->setEditable(false);
        itemdata.clear();
        itemdata<< i;
        item->setData(itemdata);
        item->setToolTip(stored.at(i));
        cparentItem->appendRow(item);
    }
   ui->listView->setModel(&model);
   ui->listView->show();
}

void PreviousLogins::chosen(QModelIndex index){
    QList<QVariant> list;
    list = model.itemFromIndex(index)->data().toList();
    ch=list[0].toInt();
    this->accept();

}
