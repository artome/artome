#ifndef DATABASES_H
#define DATABASES_H

#include <QDialog>
#include <QStandardItemModel>
#include <QProcess>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

namespace Ui {
class Databases;
}

class Databases : public QDialog {
  Q_OBJECT

public:
  explicit Databases(QWidget *parent = 0);
  ~Databases();
  QStandardItemModel opendatabasemodel;
  QStandardItem *cparentItem;
  void writesettings(int );
  void readsettings();
  QString setstring,tunnelstring;
  QString titlestring;
  QStringList host, database, password, user, localport, remoteport, sssuser,
      sssserver, remotehost, mysqlport;
  QStringList opendatabases;
  bool comentaries;
  QString Version;
  QString currentconnection;
  int currentindex;
  bool sshtunel;
  QProcess ssh;

signals:
  void senddatabaseinfo(QList<QList<QVariant>>, int);
  void sendselecteddatabase(QString, QString, int);

private slots:
  void addadatabase();
  void fillfields(int);
  void clearselected();
  void selectchanged(QStandardItem *);
  void lastlogins();
  void settunnel();
  void help();

private:
  Ui::Databases *ui;
  void initlist();
  int maxindex;
  QList<QList<QVariant>> databaseinfo;
  QStringList stored, tstored;
};

#endif // DATABASES_H
