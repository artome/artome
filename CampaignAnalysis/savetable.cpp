#include <QSettings>
#include <QFileDialog>
#include "savetable.h"
#include "ui_savetable.h"

SaveTable::SaveTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveTable)
{
    ui->setupUi(this);
    filltimestrings();
    readtimesettings();
    suggestname="tablesave";
    connect(ui->save, &QAbstractButton::pressed,this,&SaveTable::doIt);
    connect(ui->okbutton, &QAbstractButton::pressed,this,&SaveTable::readvalues);
    connect(ui->sometexttimeformats, &QComboBox::activated, this,&SaveTable::settimestrings);
    connect(ui->savedefault, &QAbstractButton::pressed, this,&SaveTable::savetimesettings);

}

SaveTable::~SaveTable()
{
    delete ui;
}
void SaveTable::hidetimeoptions()
{
  ui->timeoptions->hide();
}

void SaveTable::doIt(){

suggestname.append(".csv");
fileName = QFileDialog::getSaveFileName(this, tr("Save data as"), suggestname, tr("ascii text files (*.csv *.txt *.dat);;All files (*)"));
if (fileName.isEmpty()) {
    fileName="NOT SET TRY AGAIN";
    ui->okbutton->setDisabled(true);
}
ui->save->setText(fileName);
 ui->okbutton->setDisabled(false);
}
void SaveTable:: savetimesettings()
{
    QString setstring;
    setstring.clear();
    if(ui->Igortime->isChecked()) setstring.append("igor"); else if (ui->unixtime->isChecked())setstring.append("unixtime");
    else if (ui->TimeString->isChecked())setstring.append(ui->datef->text());else if (ui->miliunixtime->isChecked())setstring.append("unix_mili_time");
    else if (ui->matlabtime->isChecked())setstring.append("matlabtime");
QSettings settings("Aeroplot", "data");
settings.setValue("savedataplot/savetimesformat", setstring);
}
void SaveTable:: readtimesettings()
{
    QString setstring;
    QSettings settings("Aeroplot", "data");
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

void SaveTable::readvalues()
{


    if(ui->Igortime->isChecked()) timeString="igor"; else if (ui->unixtime->isChecked())timeString="unixtime";
    else if (ui->miliunixtime->isChecked())timeString="unix_mili_time";else if (ui->matlabtime->isChecked())timeString="matlabtime";
    else if (ui->TimeString->isChecked())timeString=ui->datef->text();
    if(ui->comma->isChecked()) separator="comma"; else if (ui->tab->isChecked()) separator="tab";
    else if(ui->space->isChecked())separator="space";

}
void SaveTable::settimestrings(int index)
{
    ui->datef->setText(ui->sometexttimeformats->itemText(index));
}

void SaveTable::filltimestrings()
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

