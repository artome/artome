#include "statisticswindow.h"
#include "ui_statisticswindow.h"

StatisticsWindow::StatisticsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatisticsWindow)
{
    ui->setupUi(this);
}

StatisticsWindow::~StatisticsWindow()
{
    delete ui;
}
void StatisticsWindow::inserttext(QString text, QColor color)
{
    ui->textBrowser->setTextColor(color);
    ui->textBrowser->insertPlainText(text);



}
