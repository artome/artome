#include "help.h"
#include "ui_help.h"

Help::Help(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);
}

Help::~Help()
{
    delete ui;
}
void Help::fill(QString data){
ui->textBrowser->setHtml(data);
}
void Help::Text(QString text)
{
    ui->textBrowser->append(text);
    ui->textBrowser->moveCursor(QTextCursor::Start,QTextCursor::MoveAnchor);
}
