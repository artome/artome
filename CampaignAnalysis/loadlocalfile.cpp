#include "loadlocalfile.h"
#include "ui_loadlocalfile.h"
#include "QSettings"
#include "QFileDialog"

loadLocalFile::loadLocalFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadLocalFile)
{
    ui->setupUi(this);
    filltimestrings();
    readtimesettings();
    connect(ui->choosefile, SIGNAL(pressed()),this,SLOT(doIt()));
    connect(ui->Gokbutton, SIGNAL(pressed()),this,SLOT(readvalues()));
    connect(ui->alreadyformatedokbutton, SIGNAL(pressed()),this,SLOT(readvaluesF()));

    connect(ui->sometexttimeformats, SIGNAL(activated(int)), this,SLOT(settimestrings(int)));
    connect(ui->savedefault, SIGNAL(pressed()), this,SLOT(savetimesettings()));

}

loadLocalFile::~loadLocalFile()
{
    delete ui;
}
void loadLocalFile::doIt(){

fileName = QFileDialog::getOpenFileName(this, tr("Openfile"),"", tr("ascii text files (*.csv *.txt *.dat);;All files (*)"));
if (fileName.isEmpty()) {
    fileName="NOT SET TRY AGAIN";
    ui->alreadyformatedokbutton->setDisabled(true);
    ui->Gokbutton->setDisabled(true);
}else{
ui->choosefile->setText(fileName);
 ui->alreadyformatedokbutton->setDisabled(false);
 ui->Gokbutton->setDisabled(false);}

}
void loadLocalFile:: savetimesettings()
{
    QString setstring;
    setstring.clear();
    if(ui->Igortime->isChecked()) setstring.append("igor"); else if (ui->unixtime->isChecked())setstring.append("unixtime");
    else if (ui->TimeString->isChecked())setstring.append(ui->datef->text());
QSettings settings("CampaignAnalysis", "LoadFile");
settings.setValue("savedataplot/savetimesformat", setstring);
}
void loadLocalFile::readtimesettings()
{
    QString setstring;
    QSettings settings("CampaignAnalysis", "LoadFile");
    setstring=settings.value("savedataplot/savetimesformat").toString();

    if(setstring.contains("igor")) ui->Igortime->setChecked(true);
    else if (setstring.contains("unixtime")) ui->unixtime->setChecked(true);
    else {
        ui->TimeString->setChecked(true);
        ui->datef->setText(setstring);
      }
}

void loadLocalFile::readvalues()
{


    if(ui->comma->isChecked()) separator="comma"; else if (ui->tab->isChecked()) separator="tab";
    else if(ui->space->isChecked())separator="space";else if(ui->semicolon->isChecked())separator="semicolon";
    columnstoload=ui->columnstoinclude->text();
    timecolumns=ui->columnsoftime->text();
    igl=ui->igl->value();
    formatted=false;
    this->accept();
}
void loadLocalFile::readvaluesF()
{

    if(ui->Igortime->isChecked()) timeString="IGORTIME"; else if (ui->unixtime->isChecked())timeString="UNIXTIME";
    else if(ui->miliunixtime->isChecked()) timeString="UNIXTIMEMILI";
    else if (ui->TimeString->isChecked())timeString=ui->datef->text();
    if(ui->comma->isChecked()) separator="comma"; else if (ui->tab->isChecked()) separator="tab";
    else if(ui->space->isChecked())separator="space";else if(ui->semicolon->isChecked())separator="semicolon";
    columnstoload.clear();;
    timecolumns.clear();
    igl=0;
    formatted=true;
    this->accept();

}
void loadLocalFile::settimestrings(int index)
{
    ui->datef->setText(ui->sometexttimeformats->itemText(index));
}

void loadLocalFile::filltimestrings()
{
    ui->sometexttimeformats->addItem("yyyy/MM/dd hh:mm:ss", Qt::DisplayRole);
    ui->sometexttimeformats->addItem("yyyy-MM-dd hh:mm:ss", Qt::DisplayRole);
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
void loadLocalFile::specialbanana()
{
  ui->label->setText(" Loading files that are fitted to perform a banana type of plot.\n Fomat of the file should be:\n first column with time value, remaining columns with concentrations. \n first row should be: \"time\", diameter1,.., diameter#n");
  ui->tabWidget->removeTab(1);
  ui->tabWidget->setTabText(0,"load banana table");
}

void loadLocalFile:: settitle(QString title)
{
   this->setWindowTitle(title);
}
