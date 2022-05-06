#include "addnewentry.h"
#include "ui_addnewentry.h"

addnewentry::addnewentry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addnewentry)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted,this, &addnewentry::readvalues);
}

addnewentry::~addnewentry()
{
    delete ui;
}

void addnewentry::readvalues()
{
    newentry=ui->newentry->text().simplified();
    this->accept();
}
