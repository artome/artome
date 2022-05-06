#include "inststatus.h"
#include "ui_inststatus.h"
#include "dbapi.h"
#include <QDateTime>

InstStatus::InstStatus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstStatus)
{
    ui->setupUi(this);
    connect(ui->refresh,SIGNAL(clicked(bool)),this,SLOT(recheck()));
}

InstStatus::~InstStatus()
{
    delete ui;
}
void InstStatus::check(){
 ui->textBrowser->clear();


 QDateTime lastinsert,currenttime;
 currenttime.setTimeSpec(Qt::UTC);
 currenttime=QDateTime::currentDateTimeUtc();
 long long deltat,minutes,hours,seconds;
 QString mess;

dbapi table;
table.connectionName=connectionName;
QStringList cnames;
QList<std::vector<QString> * > institens;
cnames.clear();
cnames <<"NAMEID" << "INJECTIONTIME" <<"LASTINJECTEDTIME" ;
for(int i=0; i<cnames.count(); i++) institens << new ( std::vector<QString>);

table.fechnotimecolumns_string_orderby("TablesforDSV",cnames,"NAMEID",&institens);

ui->textBrowser->setTextColor("black");
ui->textBrowser->append("Instrument not updated in the last 30 min\n");
ui->textBrowser->setTextColor("red");

for(unsigned long i=0;i<institens.at(0)->size(); i++){
    if(!institens.at(1)->at(i).toInt()) continue;
    mess.clear();
    lastinsert.setTimeSpec(Qt::UTC);
    lastinsert=lastinsert.fromString(institens.at(2)->at(i),"yyyy-MM-dd hh:mm:ss");
    lastinsert.setTimeSpec(Qt::UTC);
    deltat=lastinsert.secsTo(currenttime);
    if(deltat<1800) continue;
    if(deltat >86400){
      ui->textBrowser->setTextColor("cyan");
    }
    minutes=deltat/60;
    hours=minutes/60;
    minutes=minutes%60;
    seconds=deltat%60;
    mess=institens.at(0)->at(i);
    mess.append("------------> has a delay  of "+QString::number(hours)+" hours "+QString::number(minutes)+" minutes "+ QString::number(seconds) +" secs");
    ui->textBrowser->append(mess);
    ui->textBrowser->setTextColor("red");

}
ui->textBrowser->setTextColor("black");
ui->textBrowser->append("\n Instrument Ingesting recent data into database\n");
ui->textBrowser->setTextColor("blue");
for(unsigned long i=0;i<institens.at(0)->size(); i++){
    if(!institens.at(1)->at(i).toInt()) continue;
    mess.clear();
    lastinsert.setTimeSpec(Qt::UTC);
    lastinsert=lastinsert.fromString(institens.at(2)->at(i),"yyyy-MM-dd hh:mm:ss");
    lastinsert.setTimeSpec(Qt::UTC);
    deltat=lastinsert.secsTo(currenttime);
    if(deltat >1800) continue;
    minutes=deltat/60;
    hours=minutes/60;
    minutes=minutes%60;
    seconds=deltat%60;
    mess=institens.at(0)->at(i);
    mess.append(" ------------> has a delay  of "+QString::number(hours)+"hours "+QString::number(minutes)+"minutes "+ QString::number(seconds)+" secs");
    ui->textBrowser->append(mess);
}
ui->textBrowser->setTextColor("black");
ui->textBrowser->append("\n Unactive Instruments");
ui->textBrowser->setTextColor("yellow");
for(unsigned long i=0;i<institens.at(0)->size(); i++){
    if(institens.at(1)->at(i).toInt()) continue;
    mess=institens.at(0)->at(i);
    ui->textBrowser->append(mess);


}
QTextCursor trash =ui->textBrowser->textCursor();
trash.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
ui->textBrowser->setTextCursor(trash);

for(int i=0; i<institens.count(); i++) delete institens.at(i) ;


}

void InstStatus::recheck(){
    check();
}
