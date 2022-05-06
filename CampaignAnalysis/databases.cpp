#include "databases.h"
#include "ui_databases.h"
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include <previouslogins.h>
#include <unistd.h>
#include <algorithm>

Databases::Databases(QWidget *parent) : QDialog(parent), ui(new Ui::Databases) {
  ui->setupUi(this);
  connect(ui->okbutton, &QAbstractButton::clicked, this, &Databases::addadatabase);
  connect(ui->Previouslogins, &QAbstractButton::clicked, this, &Databases::lastlogins);
  initlist();
  comentaries = false;
  opendatabases.clear();
  connect(&opendatabasemodel, &QStandardItemModel::itemChanged, this,
          &Databases::selectchanged);
  connect(ui->SetTunnel, &QAbstractButton::clicked, this, &Databases::settunnel);
  connect(ui->help,&QAbstractButton::clicked, this, &Databases::help);

  readsettings();
  if (host.size() && user.size() && password.size() && database.size() &&
       mysqlport.size())

  {

    ui->host->setText(host.at(0));
    ui->user->setText(user.at(0));
    ui->password->setText(password.at(0));
    ui->database->setText(database.at(0));
    ui->mysqlport->setText(mysqlport.at(0));
  }

  if (sssuser.size() && sssserver.size() && remoteport.size() &&
      localport.size() && remotehost.size() )
  {

    ui->sshuser->setText(sssuser.at(0));
    ui->sshserver->setText(sssserver.at(0));
    ui->databaseserverport->setText(remoteport.at(0));
    ui->localport->setText(localport.at(0));
    ui->remotehost->setText(remotehost.at(0));
  }





  comentaries = false;
  sshtunel = false;
  Version = "CampaignAnalysis V unset ";
}

Databases::~Databases() {
  delete ui;
  if (sshtunel)
    ssh.close();
}
void Databases::selectchanged(QStandardItem *item) {
  if (item->checkState()) {
    //        currentconnection="AeroPlot";
    //        currentconnection.append(QString::number(item->row()));
    currentindex = item->row();
    emit sendselecteddatabase(databaseinfo.at(currentindex).at(0).toString(),
                              databaseinfo.at(currentindex).at(1).toString(),
                              currentindex);
    clearselected();
    ui->databaseactivelabel->setText(item->text());
  }
}

void Databases::clearselected() {
  for (int i = 0; i < opendatabasemodel.rowCount(); i++) {
    if (i == currentindex) {
      continue;
    }
    if (opendatabasemodel.item(i)->checkState()) {
      opendatabasemodel.item(i)->setCheckState(Qt::Unchecked);
    }
  }
}

void Databases::initlist() {
  cparentItem = opendatabasemodel.invisibleRootItem();
  QStringList crootData;
  crootData.clear();
  crootData << "Databases";
  opendatabasemodel.setHorizontalHeaderLabels(crootData);
}
void Databases::settunnel() {
  bool portok, localok;
  int port, localport;
  port = ui->databaseserverport->text().toInt(&portok);
  localport = ui->localport->text().toInt(&localok);
  if (!portok || !localok) {
    QMessageBox msgBox;
    msgBox.setText("MYSQL PORTs should be an integer");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    return;
  }
  QString sshcommand;
  QStringList arguments;

  if (!sshtunel) {
    sshcommand = "ssh";
       arguments << "-L";
    arguments <<   QString::number(localport).simplified() + ":" +
                     ui->remotehost->text().simplified() + ":" +
                     QString::number(port).simplified();
    arguments <<  ui->sshuser->text().simplified() + "@" + ui->sshserver->text().simplified() ;//<< "-N" <<"-f";

  /*  for(int i=0; i<arguments.count();i++){
        qDebug()<< arguments.at(i);
    }
    */
   ssh.start(sshcommand, arguments,QIODevice::ReadWrite);
   if (!ssh.waitForReadyRead())  {qDebug() << "problems in tunnel";ssh.close(); return ;}

      tunnelstring =tunnelstring.prepend(
          ui->sshuser->text() + "," + ui->sshserver->text() + "," + ui->remotehost->text() +
          "," + QString::number(localport) + "," + QString::number(port)+ ";");
      tstored =tunnelstring.split(QRegularExpression(";"));
      QStringList one;
      for (int i = 0; i < std::min(10, (int) tstored.size()); i++) {

        one = tstored.at(i).split(QRegularExpression(","));
        if (one.count() < 5)
          continue;
        if (!one[0].compare(ui->sshuser->text()) &&
            !one.at(1).compare(ui->sshserver->text()) &&
            !one.at(2).compare(ui->remotehost->text()) &&
            !one.at(3).compare(ui->localport->text()) &&
            !one.at(4).compare(ui->databaseserverport->text())

        )  continue;
       tunnelstring.append(";");
       tunnelstring.append(one.at(0));
       tunnelstring.append(",");
       tunnelstring.append(one.at(1));
       tunnelstring.append(",");
       tunnelstring.append(one.at(2));
       tunnelstring.append(",");
       tunnelstring.append(one.at(3));
       tunnelstring.append(",");
       tunnelstring.append(one.at(4));

      }
      writesettings(1);
     sshtunel = true;
   }
}

void Databases::addadatabase() {

  QString dname;
  dname.clear();
  dname.append(ui->host->text());
  dname.append("/");
  dname.append(ui->user->text());
  dname.append("/");
  dname.append(ui->database->text());
  bool already = false;
  for (int k = 0; k < opendatabases.count(); k++) {
    if (opendatabases.at(k) == dname)
      already = true;
  }

  if (already) {
    QMessageBox msgBox;
    msgBox.setText("There is already open a database in the same host with the "
                   "same name.");
    msgBox.setInformativeText("Do you still want to continue?");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
      // Save was clicked
      break;
    case QMessageBox::No:
      // Don't Save was clicked
      return;
      break;
    default:
      // should never be reached
      break;
    }
  }
#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
  maxindex = opendatabases.count();
  currentconnection = "CampaignAnalysis";
  currentconnection.append(QString::number(maxindex));

#ifndef QT_NO_CURSOR
  QApplication::restoreOverrideCursor();
#endif
  QStringList stored, one;

  //   ssh.start("ssh -L 3306:localhost:3306 cloud-daq@cloudaq.sim.ul.pt -N -f
  //   \n"); if (!ssh.waitForStarted()) qDebug()<<"problems in tunnel";
  //      if (!ssh.waitForReadyRead()) qDebug()<<"problems in tunnel";

  int port;
  bool portok;
  port = ui->mysqlport->text().toInt(&portok);
  if (!portok) {
    QMessageBox msgBox;
    msgBox.setText("MYSQL PORTs should be an integer");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    return;
  }

  QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", currentconnection);
  /* QString lixo;
  lixo.clear();
  for (auto i=0;i<db.drivers().count() ;i++ ) {
      lixo.append(db.drivers().at(i));
      lixo.append(";");
  }
  QMessageBox::critical(this, tr("drivers list"),
                        lixo);
  */
  db.setHostName(ui->host->text());
  db.setDatabaseName(ui->database->text());
  db.setUserName(ui->user->text());
  db.setPassword(ui->password->text());
  db.setPort(port);
  db.setConnectOptions("MYSQL_OPT_RECONNECT=TRUE");

  if (db.open()) {

    bool superuser = false;
    QSqlQuery query(db);
    if (!query.exec("SHOW GRANTS")) {
      QSqlError Error = query.lastError();
      qDebug() << Error.databaseText();
    }
    while (query.next()) {
      QString lixo;
      lixo = query.value(0).toString();
      // qDebug()<< lixo;
      if (lixo.contains("GRANT ALL") && lixo.contains(ui->database->text()))
        superuser = true;
      else if (lixo.contains("GRANT ALL PRIVILEGES ON *.* TO"))
        superuser = true;
      else if (lixo.contains("CREATE"))
        superuser = true;
    }
    // superuser=true;

    setstring = setstring.prepend(
        ui->host->text() + "," + ui->user->text() + "," + ui->password->text() +
        "," + ui->database->text() + "," + QString::number(port)+ ";");
    stored = setstring.split(QRegularExpression(";"));
    setstring = ui->host->text() + "," + ui->user->text() + "," +
                ui->password->text() + "," + ui->database->text() + "," +
                QString::number(port);
    for (int i = 0; i < std::min(10, (int) stored.size()); i++) {

      one = stored.at(i).split(QRegularExpression(","));
      if (one.count() < 5)
        continue;
      if (!one[0].compare(ui->host->text()) &&
          !one.at(3).compare(ui->database->text()) &&
          !one.at(1).compare(ui->user->text()) &&
          !one.at(2).compare(ui->password->text()) &&
          !one.at(4).compare(QString::number(port))

      )  continue;
      setstring.append(";");
      setstring.append(one.at(0));
      setstring.append(",");
      setstring.append(one.at(1));
      setstring.append(",");
      setstring.append(one.at(2));
      setstring.append(",");
      setstring.append(one.at(3));
      setstring.append(",");
      setstring.append(one.at(4));

    }

    writesettings(0);
    titlestring = Version;
    titlestring.append("-->Connected to ");
    titlestring.append(ui->host->text().toLatin1().data());
    titlestring.append(" user:");
    titlestring.append(ui->user->text().toLatin1().data());
    titlestring.append(" database:");
    titlestring.append(ui->database->text().toLatin1().data());
    {
      QList<QVariant> itemdata;
      QStringList crootData;
      crootData.clear();

      QStandardItem *item = new QStandardItem(dname);
      item->setEditable(false);
      itemdata.clear();
      itemdata << maxindex << comentaries;
      item->setData(itemdata);
      item->setCheckable(true);
      item->setCheckState(Qt::Checked);
      cparentItem->appendRow(item);
      ui->listView->setModel(&opendatabasemodel);
      ui->databaseactivelabel->setText(item->text());
      opendatabases << dname;
      currentindex = maxindex;
      QList<QVariant> temp;
      temp.clear();
      temp << currentconnection << item->text() << 0 << 0 << "notdefinedYet"
           << "notdefinedYet" << ui->host->text();
      temp << ui->database->text() << ui->user->text() << ui->password->text()
           << superuser << localport << port;
      databaseinfo << temp;
      emit senddatabaseinfo(databaseinfo, currentindex);
      clearselected();
    }
  } else {

    maxindex--;
    QMessageBox::critical(this, tr("Server connect error"),
                          db.lastError().text());
    db.removeDatabase(currentconnection);
  }
}

void Databases::fillfields(int x) {
  if (x == 0)
    return;
  if (host.size() >= x - 1 || user.size() >= x - 1 ||
      password.size() >= x - 1 || database.size() >= x - 1 || mysqlport.size() >= x - 1){

  ui->host->setText(host.at(x - 1));
  ui->user->setText(user.at(x - 1));
  ui->password->setText(password.at(x - 1));
  ui->database->setText(database.at(x - 1));
   ui->mysqlport->setText(mysqlport.at(x - 1));
 }

 if(sssuser.size() >= x - 1 || sssserver.size() >= x - 1 ||
  remoteport.size() >= x - 1 || localport.size() >= x - 1 ||
  remotehost.size() >= x - 1 ){
  ui->sshuser->setText(sssuser.at(x - 1));
  ui->sshserver->setText(sssserver.at(x - 1));
  ui->databaseserverport->setText(remoteport.at(x - 1));
  ui->localport->setText(localport.at(x - 1));
  ui->remotehost->setText(remotehost.at(x - 1));
 }
}
void Databases::writesettings(int i) {
  QSettings settings("CampaignAnalysis", "database");
  if(i==0)settings.setValue("server/fields", setstring);
  if(i==1)settings.setValue("server/tunnel", tunnelstring);
}
void Databases::readsettings() {
  QSettings settings("CampaignAnalysis", "database");
  setstring = settings.value("server/fields").toString();
  tunnelstring = settings.value("server/tunnel").toString();
  QStringList one;
  stored = setstring.split(QRegularExpression(";"));
  password.clear();
  user.clear();
  database.clear();
  host.clear();
  remoteport.clear();
  sssserver.clear();
  sssuser.clear();
  mysqlport.clear();

  //   ui->Previouslogins->addItem(QIcon(QString::fromUtf8(":/Icons/Icons/databse.svg")),"select
  //   Previous",Qt::DisplayRole);
  for (int i = 0; i < stored.size(); i++) {
    one = stored.at(i).split(QRegularExpression(","));
    if (one.count() < 5)
      continue;
    host << one.at(0);
    user << one.at(1);
    password << one.at(2);
    database << one.at(3);
    mysqlport << one.at(4);

  }

  tstored=tunnelstring.split(QRegularExpression(";"));
  for (int i = 0; i < tstored.size(); i++) {
    one = tstored.at(i).split(QRegularExpression(","));
    if (one.count() < 5)
      continue;

    sssuser << one.at(0);
    sssserver << one.at(1);
    remotehost << one.at(2);
    localport << one.at(3);
    remoteport << one.at(4);

}
}

void Databases::lastlogins() {
PreviousLogins dialog(this);
  if(ui->tabWidget->currentIndex()==0){
  dialog.fillListView(stored);
 }else{
     dialog.fillListView(tstored);
 }
  int res=dialog.exec();
  if(res){
      QStringList one;

      if(ui->tabWidget->currentIndex()==0){
          one = stored.at(dialog.ch).split(QRegularExpression(","));
          ui->host->setText(one.at(0));
          ui->user->setText(one.at(1));
          ui->password->setText(one.at(2));
          ui->database->setText(one.at(3));
          ui->mysqlport->setText(one.at(4));

     }else{
           one = tstored.at(dialog.ch).split(QRegularExpression(","));
           ui->sshuser->setText(one.at(0));
           ui->sshserver->setText(one.at(1));
           ui->remotehost->setText(one.at(2));
           ui->localport->setText(one.at(3));
           ui->databaseserverport->setText(one.at(4));

     }
  }

}
void Databases::help(){

}
