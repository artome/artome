#include "managingtime.h"
#include "dbapi.h"
#include "timefromruns.h"
#include "ui_managingtime.h"
#include <QSettings>

ManagingTime::ManagingTime(QWidget *parent)
    : QDialog(parent), ui(new Ui::ManagingTime) {
  ui->setupUi(this);

  connect(ui->Rtimeselect, SIGNAL(pressed()), this, SLOT(timefromruns()));
  connect(ui->Byevents, SIGNAL(pressed()), this, SLOT(timefromevents()));
  connect(ui->IdateEdit, SIGNAL(dateChanged(QDate)), this,
          SLOT(manualchangeItime()));
  connect(ui->LdateEdit, SIGNAL(dateChanged(QDate)), this,
          SLOT(manualchangeLtime()));
  connect(ui->ItimeEdit, SIGNAL(timeChanged(QTime)), this,
          SLOT(manualchangeItime()));
  connect(ui->LtimeEdit, SIGNAL(timeChanged(QTime)), this,
          SLOT(manualchangeLtime()));
  connect(ui->okButton, SIGNAL(pressed()), this, SLOT(readvalues()));
  connect(ui->cancelButton, SIGNAL(pressed()), this, SLOT(close()));
  connect(ui->min20, SIGNAL(pressed()), this, SLOT(setlast20()));
  connect(ui->min40, SIGNAL(pressed()), this, SLOT(setlast40()));
  connect(ui->hour, SIGNAL(pressed()), this, SLOT(setlast60()));

  Itime.setTimeSpec(Qt::UTC);
  Ltime.setTimeSpec(Qt::UTC);
  current_time.setTimeSpec(Qt::UTC);
  firsttime.setTimeSpec(Qt::UTC);
}

ManagingTime::~ManagingTime() { delete ui; }
void ManagingTime::Logmessages(QString text) { emit logmessages(text); }
void ManagingTime::LogmessagesR(QString text) { emit logmessagesR(text); }
void ManagingTime::setTimeWidgets(QDate datai, QTime timei, QDate dataf,
                                  QTime timef) {
  ui->IdateEdit->setDate(datai);
  ui->LdateEdit->setDate(dataf);
  ui->ItimeEdit->setTime(timei);
  ui->LtimeEdit->setTime(timef);
  readvalues();
}

void ManagingTime::writeinterface() {
  QString savedi, savedf;
  savedi = Itimeslabel;
  savedf = Ltimeslabel;
  ui->IdateEdit->setDate(IItime.date());
  ui->LdateEdit->setDate(LLtime.date());
  ui->ItimeEdit->setTime(IItime.time());
  ui->LtimeEdit->setTime(LLtime.time());
  Itimeslabel = savedi;
  Ltimeslabel = savedf;
  ui->startrun->setText(Itimeslabel);
  ui->endrun->setText(Ltimeslabel);
}
void ManagingTime::timefromevents() {

  TimeFromRuns dialog;
  dialog.setWindowTitle(databaseinfo.at(1).toString());
  struct Events {
    QString RunNumber;
    long long StartTime;
    long long EndTime;
    QString Type;
    QString Description;
    QString Comments;
  };
  QString Eventsfor;
  Eventsfor = "Eventsfor";
  Eventsfor.append(databaseinfo.at(7).toString());
  QList<Events> runevents;
  runevents.clear();
  QSettings settings("CampaignAnalysis", Eventsfor);
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

  model.m_rowCount = runevents.count();
  model.rownames.clear();
  model.startrun.clear();
  model.endrun.clear();
  model.description.clear();
  model.commentaries.clear();
  model.type.clear();
  model.startc.clear();
  model.endc.clear();
  for (int i = 0; i < runevents.count(); i++) {
    model.rownames << runevents.at(i).RunNumber;
    model.startrun << QDateTime::fromMSecsSinceEpoch(runevents.at(i).StartTime)
                          .toUTC().toSecsSinceEpoch();

    model.endrun << QDateTime::fromMSecsSinceEpoch(runevents.at(i).EndTime)
                        .toUTC().toSecsSinceEpoch();
    model.description << runevents.at(i).Description;
    model.commentaries << runevents.at(i).Comments;
    model.type << runevents.at(i).Type;
    model.startc << 0;
    model.endc << 0;
  }

  dialog.model = &model;
  connect(&model, SIGNAL(checkchanged(int, int, bool)), &dialog,
          SLOT(checkevent(int, int, bool)));
  dialog.fill();
  int result;
  result = dialog.exec();
  if (result) {
    model.vscrollpos = dialog.Getvscrollpos();
    IItime = dialog.Itime;
    LLtime = dialog.Ltime;

    writeinterface();
    // Este comando ser chamado antes das duas linhas seguintes.
    // porque vai mudar o valor no widgets do tempo e por conseguinte mudaria os
    // labels para manual setted.

    Itimeslabel = dialog.Sstart;
    Ltimeslabel = dialog.Send;

    ui->startrun->setText(Itimeslabel);
    ui->endrun->setText(Ltimeslabel);

    readvalues();
  }
}

void ManagingTime::timefromruns() {

  TimeFromRuns dialog;
  dialog.setWindowTitle(databaseinfo.at(1).toString());
  dbapi table;

  table.connectionName = databaseinfo.at(0).toString();
  connect(&table, SIGNAL(logmessages(QString)), this,
          SLOT(Logmessages(QString)));
  connect(&table, SIGNAL(logmessagesR(QString)), this,
          SLOT(LogmessagesR(QString)));
  QStringList cnames;
  QList<std::vector<QString> *> runrows;

  cnames.clear();
  cnames << "RunNumber"
         << "StartTime"
         << "EndTime"
         << "Type"
         << "Description"
         << "Comments";
  for (int i = 0; i < cnames.count(); i++)
    runrows << new (std::vector<QString>);
  table.fechnotimecolumns_string_orderby("Runtable", cnames, "RunNumber",
                                         &runrows);
  model.m_rowCount = runrows.at(0)->size();
  QString Lcom;
  model.rownames.clear();
  model.startrun.clear();
  model.endrun.clear();
  model.description.clear();
  model.commentaries.clear();
  model.type.clear();
  model.startc.clear();
  model.endc.clear();
  for (int i = 0; i < (int)runrows.at(0)->size(); i++) {
    model.rownames << runrows.at(0)->at(i);
    model.startrun << QDateTime::fromMSecsSinceEpoch(
                          runrows.at(1)->at(i).toLongLong())
                          .toUTC()
                          .toSecsSinceEpoch();
    model.endrun << QDateTime::fromMSecsSinceEpoch(
                        runrows.at(2)->at(i).toLongLong())
                        .toUTC()
                        .toSecsSinceEpoch();
    model.description << runrows.at(4)->at(i);
    model.commentaries << runrows.at(5)->at(i);
    model.type << runrows.at(3)->at(i);
    model.startc << 0;
    model.endc << 0;
  }

  dialog.model = &model;
  connect(&model, SIGNAL(checkchanged(int, int, bool)), &dialog,
          SLOT(check(int, int, bool)));
  dialog.fill();
  int result;
  result = dialog.exec();
  if (result) {
    model.vscrollpos = dialog.Getvscrollpos();
    IItime = dialog.Itime;
    LLtime = dialog.Ltime;

    writeinterface();
    // Este comando ser chamado antes das duas linhas seguintes.
    // porque vai mudar o valor no widgets do tempo e por conseguinte mudaria os
    // labels para manual setted.

    Itimeslabel = dialog.Sstart;
    Ltimeslabel = dialog.Send;

    ui->startrun->setText(Itimeslabel);
    ui->endrun->setText(Ltimeslabel);

    readvalues();
  }
  for (int i = 0; i < cnames.count(); i++)
    delete runrows.at(i);
}

void ManagingTime::manualchangeItime() {

  Itime.setDate(ui->IdateEdit->date());
  Ltime.setDate(ui->LdateEdit->date());
  Itime.setTime(ui->ItimeEdit->time());
  Ltime.setTime(ui->LtimeEdit->time());

  int secinterval = Itime.secsTo(Ltime);
  int minutes = secinterval / 60;
  int hours = minutes / 60;
  minutes = minutes % 60;
  int seconds = secinterval % 60;
  QString lixo, lixo2;
  lixo2 = "<html><head/><body><p><span style=\" font-weight:600; "
          "font-style:italic; "
          "color:#000000;\">PARASUBSTITUIROLARE</span></p></body></html>";
  lixo.clear();
  lixo.append("Interval ");
  lixo.append(QString::number(hours));
  lixo.append("h:");
  lixo.append(QString::number(minutes));
  lixo.append("min:");
  lixo.append(QString::number(seconds));
  lixo.append("s");
  lixo2.replace("PARASUBSTITUIROLARE", lixo);
  if (((long)Ltime.toSecsSinceEpoch() - (long)Itime.toSecsSinceEpoch()) > 0) {
    lixo2.replace("000000", "0000ff");
  } else
    lixo2.replace("000000", "ff0000");
  ui->TimeInterval->setText(lixo2);
  ui->startrun->setText("from manual selected time ");
  ui->label_2->setPixmap(
      QPixmap(QString::fromUtf8(":/Icons/Icons/Timewarningnew.png")));
}
void ManagingTime::manualchangeLtime() {

  Itime.setDate(ui->IdateEdit->date());
  Ltime.setDate(ui->LdateEdit->date());
  Itime.setTime(ui->ItimeEdit->time());
  Ltime.setTime(ui->LtimeEdit->time());
  int secinterval = Itime.secsTo(Ltime);
  int minutes = secinterval / 60;
  int hours = minutes / 60;
  minutes = minutes % 60;
  int seconds = secinterval % 60;
  QString lixo, lixo2;
  lixo2 = "<html><head/><body><p><span style=\" font-weight:600; "
          "font-style:italic; "
          "color:#000000;\">PARASUBSTITUIROLARE</span></p></body></html>";
  lixo.clear();
  lixo.append("Interval ");
  lixo.append(QString::number(hours));
  lixo.append("h:");
  lixo.append(QString::number(minutes));
  lixo.append("min:");
  lixo.append(QString::number(seconds));
  lixo.append("s");
  lixo2.replace("PARASUBSTITUIROLARE", lixo);
  if (((long)Ltime.toSecsSinceEpoch() - (long)Itime.toSecsSinceEpoch()) > 0) {
    lixo2.replace("000000", "0000ff");
  } else
    lixo2.replace("000000", "ff0000");
  ui->TimeInterval->setText(lixo2);
  ui->endrun->setText("till manual selected time ");
  ui->label_2->setPixmap(
      QPixmap(QString::fromUtf8(":/Icons/Icons/Timewarningnew.png")));
}
void ManagingTime::getdatabaseinfo(QList<QVariant> dinfo, int index) {

  databaseindex = index;
  databaseinfo = dinfo;
  ui->databasename->setText(dinfo.at(1).toString());
  Itime = Itime.fromSecsSinceEpoch(databaseinfo.at(2).toLongLong()).toUTC();
  ui->IdateEdit->setDate(Itime.date());
  ui->ItimeEdit->setTime(Itime.time());
  Ltime = Ltime.fromSecsSinceEpoch(databaseinfo.at(3).toLongLong()).toUTC();
  ui->LdateEdit->setDate(Ltime.date());  
  ui->LtimeEdit->setTime(Ltime.time());
  ui->startrun->setText(databaseinfo.at(4).toString());
  ui->endrun->setText(databaseinfo.at(5).toString());
  if(dinfo.at(4).toString().contains("plotcanvas")){
     ui->label_2->setPixmap(
          QPixmap(QString::fromUtf8(":/Icons/Icons/timewarningcleraed.png")));
  }
}
void ManagingTime::setlast20() {
  current_time = current_time.currentDateTime().toTimeSpec(Qt::UTC);
  firsttime = current_time.toTimeSpec(Qt::UTC).addSecs(-20 * 60);
  ui->LdateEdit->setDate(current_time.date());
  ui->IdateEdit->setDate(firsttime.date());
  ui->LtimeEdit->setTime(current_time.time());
  ui->ItimeEdit->setTime(firsttime.time());
  readvalues();
}
void ManagingTime::setlast40() {
  current_time = current_time.currentDateTime().toTimeSpec(Qt::UTC);
  firsttime = current_time.toTimeSpec(Qt::UTC).addSecs(-40 * 60);
  ui->LdateEdit->setDate(current_time.date());
  ui->IdateEdit->setDate(firsttime.date());
  ui->LtimeEdit->setTime(current_time.time());
  ui->ItimeEdit->setTime(firsttime.time());
  readvalues();
}
void ManagingTime::setlast60() {
  current_time = current_time.currentDateTime().toTimeSpec(Qt::UTC);
  firsttime = current_time.toTimeSpec(Qt::UTC).addSecs(-40 * 60);
  ui->LdateEdit->setDate(current_time.date());
  ui->IdateEdit->setDate(firsttime.date());
  ui->LtimeEdit->setTime(current_time.time());
  ui->ItimeEdit->setTime(firsttime.time());
  readvalues();
}

void ManagingTime::readvalues() {

  Itime.setDate(ui->IdateEdit->date());
  Ltime.setDate(ui->LdateEdit->date());
  Itime.setTime(ui->ItimeEdit->time());
  Ltime.setTime(ui->LtimeEdit->time());

  databaseinfo[2] = (long long)Itime.toUTC().toSecsSinceEpoch();
  databaseinfo[3] = (long long)Ltime.toUTC().toSecsSinceEpoch();
  databaseinfo[4] = ui->startrun->text();
  databaseinfo[5] = ui->endrun->text();
  emit senddatabaseinfo(databaseinfo, databaseindex);
  ui->label_2->setPixmap(
      QPixmap(QString::fromUtf8(":/Icons/Icons/timewarningcleraed.png")));
  //  this->close();
}
