#include "searchtext.h"
#include "ui_searchtext.h"

searchtext::searchtext(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::searchtext)
{
    ui->setupUi(this);

    connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&searchtext::acceptc);

}

searchtext::~searchtext()
{
    delete ui;
}
void searchtext::acceptc()
{
  stringtosearch=ui->string->text();
  this->accept();
}
void searchtext::settext(QString text)
{

  ui->string->setText(text);
}
