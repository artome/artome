/*!
 * \file dbapi.cpp
 *   <summary>
 *   <B> dsapi</B> class provides an easy wrapper from QT/MYSQL framework to the
needs of the HDaxSystem framework
 *   </summary>
 *

*
*
*/

#include "dbapi.h"
#include <QVariant>

dbapi::dbapi(QObject *parent) : QObject(parent) { debugflag = false; }
void dbapi::bindquery(QString prepare, QStringList *binds) {
  QSqlDatabase db = QSqlDatabase::database(connectionName);

  QSqlQuery query(db);
  query.prepare(prepare);
  for (int i = 0; i < binds->count(); i++) {
    query.bindValue(i, binds->at(i));
  }
  if (!query.exec()) {
    QSqlError Error = query.lastError();
    emit logmessagesR(Error.databaseText());
  }
}

void dbapi::preparebind(QString prepare, QStringList *binds) {
  QSqlDatabase db = QSqlDatabase::database(connectionName);

  QSqlQuery query(db);
  query.prepare(prepare);
  for (int i = 0; i < binds->count(); i++) {
    query.bindValue(i, binds->at(i));
  }
  if (!query.exec()) {
    QSqlError Error = query.lastError();
    emit logmessagesR(Error.databaseText());
  }
}
void dbapi::createtable_onekey(QString tname, QString keyname,
                               QStringList cnames, QStringList ctypes) {

  QString Query, Pkey;
  bool bkey;
  bkey = false;
  Query = "CREATE TABLE IF NOT EXISTS ";
  Query.append(tname);
  Query.append(" (");
  emit logmessages("Creating table " + tname);
  for (int i = 0; i < cnames.count(); i++) {
    if (keyname == cnames.at(i)) {
      bkey = true;
      Pkey = cnames.at(i);
    }
  }
  for (int i = 0; i < cnames.count(); i++) {
    Query.append(cnames.at(i));
    Query.append(" ");
    Query.append(ctypes.at(i));
    if (i < cnames.count() - 1)
      Query.append(", ");
    else {
      if (bkey) {
        Query.append(", PRIMARY KEY (");
        Query.append(Pkey);
        Query.append(")");
      }
      Query.append(")");
      Query.append(" ENGINE = MYISAM;");
    }
  }
  emit logmessages(Query);
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  QSqlQuery query(db);
  if (!query.exec(Query)) {
    QSqlError Error = query.lastError();
    emit logmessagesR(Error.databaseText());
  }
}
void dbapi::createtable_twokeys(QString tname, QString keyname1,
                                QString keyname2, QStringList cnames,
                                QStringList ctypes) {

  QString Query;
  Query = "CREATE TABLE IF NOT EXISTS ";
  Query.append(tname);
  Query.append(" (");
  emit logmessages(tname);

  for (int i = 0; i < cnames.count(); i++) {
    Query.append(cnames.at(i));
    Query.append(" ");
    Query.append(ctypes.at(i));
    if (i < cnames.count() - 1)
      Query.append(", ");
    else {

      Query.append(", PRIMARY KEY (");
      Query.append(keyname1);
      Query.append(",");
      Query.append(keyname2);
      Query.append(")");
      Query.append(")");
      Query.append(" ENGINE = MYISAM;");
    }
  }
  emit logmessages(Query);
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  QSqlQuery query(db);
  if (!query.exec(Query)) {
    QSqlError Error = query.lastError();
    emit logmessagesR(Error.databaseText());
  }
}
QStringList dbapi::enclosestrings(QStringList cnames) {
  QStringList results;
  QString temp;
  results.clear();
  for (int i = 0; i < cnames.count(); i++) {
    temp = "'";
    temp.append(cnames.at(i));
    temp.append("'");
    results << temp;
  }
  return results;
}
void dbapi::addseveralrows(QString tname, QStringList cnames,
                           QList<QStringList> rowvalues) {
  QStringList values;
  QString Query;
  Query = "INSERT IGNORE INTO ";
  Query.append(tname);
  Query.append(" (");

  for (int i = 0; i < cnames.count(); i++) {
    Query.append(cnames.at(i));
    if (i < cnames.count() - 1)
      Query.append(",");
    else
      Query.append(")");
  }
  Query.append(" VALUES ");
  for (int j = 0; j < rowvalues.count(); j++) {

    Query.append("(");
    values = enclosestrings(rowvalues[j]);
    for (int i = 0; i < values.count(); i++) {

      Query.append(values.at(i));
      if (i < values.count() - 1)
        Query.append(",");
      else
        Query.append(")");
    }
    if (j < rowvalues.count() - 1)
      Query.append(",");
  }
  Query.append(";");
  if (debugflag)
    emit logmessages(Query);
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  QSqlQuery query(db);
  if (!query.exec(Query)) {
    QSqlError Error = query.lastError();
    emit logmessagesR(Error.databaseText());
  }
}

void dbapi::addarow(QString tname, QStringList cnames, QStringList rowvalues) {

  if (cnames.count() != rowvalues.count()) {
    emit logmessagesR("when adding a row the columns names should be the same "
                      "size as the rowvalues");
  }
  QStringList values;
  QString Query;

  Query = "INSERT IGNORE INTO ";
  Query.append(tname);
  Query.append(" (");

  for (int i = 0; i < cnames.count(); i++) {
    Query.append(cnames.at(i));
    if (i < cnames.count() - 1)
      Query.append(",");
    else
      Query.append(")");
  }
  Query.append(" VALUES (");
  values = enclosestrings(rowvalues);
  for (int i = 0; i < values.count(); i++) {
    Query.append(values.at(i));
    if (i < values.count() - 1)
      Query.append(",");
    else
      Query.append(")");
  }
  Query.append(";");

  if (debugflag)
    emit logmessages(Query);
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  QSqlQuery query(db);
  if (!query.exec(Query)) {
    QSqlError Error = query.lastError();
    emit logmessagesR(Error.databaseText());
  }
}
void dbapi::fechvariantcells(QString tname, QString IDKey, QString IDkeyrow,
                             QStringList cnames, QList<QVariant> *col) {

  QString Query;

  Query = "SELECT ";
  for (int i = 0; i < cnames.count(); i++) {
    Query.append(cnames[i]);
    if (i < cnames.count() - 1)
      Query.append(",");
    else
      Query.append(" ");
  }
  Query.append(" FROM  ");
  Query.append(tname);
  Query.append(" WHERE ");
  Query.append(IDKey);
  Query.append(" ='");
  Query.append(IDkeyrow);
  Query.append("'");

  QSqlDatabase db = QSqlDatabase::database(connectionName);
  QSqlQuery query(db);
  if (!query.exec(Query)) {
    QSqlError Error = query.lastError();
    emit logmessages(Error.databaseText());
  }
  QSqlRecord rec = query.record();
  if (rec.count() != (cnames.count())) {
    emit logmessages("number of columns should be  " +
                     QString::number(cnames.count() + 1));

    return;
  }

  while (query.next()) {

    for (int i = 0; i < cnames.count(); i++) {
      *col << query.value(rec.indexOf(cnames.at(i)));
    }
  }
}

/**
 * this function fetchs  the cell value of string type in a table.
 *   example: the call
 *
 *   **fechastringcell**("instruments", "iname","CIMS", "ctype", &result)
 *
 * will perform the SQL query
 *
 * SELECT ctype FROM  instruments WHERE iname   ='CIMS'
 *
 * The result is stored in result QString
 *
 ******************************************************************************/

void dbapi::fechastringcell(QString tname, QString IDKey, QString IDkeyrow,
                            QString cname, QString *col) {

  QString Query;

  Query = "SELECT ";
  Query.append(cname);
  Query.append(" FROM  ");
  Query.append(tname);
  Query.append(" WHERE ");
  Query.append(IDKey);
  Query.append(" ='");
  Query.append(IDkeyrow);
  Query.append("'");

  QSqlDatabase db = QSqlDatabase::database(connectionName);
  QSqlQuery query(db);
  if (!query.exec(Query)) {
    QSqlError Error = query.lastError();
    emit logmessages(Error.databaseText());
  }

  QSqlRecord rec = query.record();
  if (rec.count() != 1) {
    emit logmessages("strange record count should be 1 unless thw key is not "
                     "primary so do not use this function");
    return;
  }

  while (query.next()) {
    *col = query.value(rec.indexOf(cname)).toString();
  }
}


void dbapi::droptable(QString tname) {

  QString Query;

  Query = "DROP  TABLE IF EXISTS ";
  Query.append(tname);
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  QSqlQuery query(db);
  if (!query.exec(Query)) {
    QSqlError Error = query.lastError();
    emit logmessagesR(Error.databaseText());
  }
}
/**
 * this function fetchs  cell values of string type in a table.
 *   when condition are met. when the input key as the value of the input keyvalue
 *
 *
 ******************************************************************************/
void  dbapi::fechnotimecolumns_string_filterstring(QString tname, QStringList cnames,QString key, QString keyvalue,QList<std::vector<QString> * >  *c )
{


    QString Query;

    Query="SELECT ";
    for(int i=0; i<cnames.count();i++)
    {
        Query.append(cnames[i]);
        if(i<cnames.count()-1) Query.append(","); else Query.append(" ");
    }
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append(" WHERE  ");
    Query.append(key);
    Query.append("='");
    Query.append(keyvalue);
    Query.append("'");

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=(cnames.count())){
        emit logmessages("number of columns should be  "+QString::number(cnames.count()));

        return;
    }

    while (query.next()){

        for(int i=0; i<cnames.count(); i++ )
        {
            c->at(i)->push_back(query.value(rec.indexOf(cnames.at(i))).toString());

        }

    }

}
/**
 * this function fetchs  cell values of string type in a table.
 *   when condition are met. when the input key as the value of the input keyvalue, oder by a given column value
 *
 *
 ******************************************************************************/

void  dbapi::fechnotimecolumns_string_filterstring_orderby(QString tname, QStringList cnames,QString key, QString keyvalue, QString order, QList<std::vector<QString> * >  *c )
{


    QString Query;

    Query="SELECT ";
    for(int i=0; i<cnames.count();i++)
    {
        Query.append(cnames[i]);
        if(i<cnames.count()-1) Query.append(","); else Query.append(" ");
    }
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append(" WHERE  ");
    Query.append(key);
    Query.append("='");
    Query.append(keyvalue);
    Query.append("'");
    Query.append( " ORDER BY ");
    Query.append(order);


    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=(cnames.count())){
        emit logmessages("number of columns should be  "+QString::number(cnames.count()));

        return;
    }

    while (query.next()){

        for(int i=0; i<cnames.count(); i++ )
        {
            c->at(i)->push_back(query.value(rec.indexOf(cnames.at(i))).toString());

        }

    }

}


/**
 * this function fetchs   cell values of string type in a table.
 *
 *
 ******************************************************************************/

void  dbapi::fechnotimecolumns_string(QString tname, QStringList cnames,QList<std::vector<QString> * >  *c )
{


    QString Query;

    Query="SELECT ";
    for(int i=0; i<cnames.count();i++)
    {
        Query.append(cnames[i]);
        if(i<cnames.count()-1) Query.append(","); else Query.append(" ");
    }
    Query.append(" FROM  ");
    Query.append(tname);

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=(cnames.count())){
        emit logmessages("number of columns should be  "+QString::number(cnames.count()));

        return;
    }

    while (query.next()){

        for(int i=0; i<cnames.count(); i++ )
        {
            c->at(i)->push_back(query.value(rec.indexOf(cnames.at(i))).toString());

        }

    }

}
/**
 * this function fetchs   several columns String values from a table orded some column value .
 *
 *
 ******************************************************************************/

void  dbapi::fechnotimecolumns_string_orderby(QString tname, QStringList cnames,QString order, QList<std::vector<QString> * >  *c )
{


    QString Query;

    Query="SELECT ";
    for(int i=0; i<cnames.count();i++)
    {
        Query.append(cnames[i]);
        if(i<cnames.count()-1) Query.append(","); else Query.append(" ");
    }
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append( " ORDER BY ");
    Query.append(order);
    Query.append(";");

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
      QSqlError Error= query.lastError();
      emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=(cnames.count())){
     emit logmessages("number of columns should be  "+QString::number(cnames.count()));

     return;
    }

    while (query.next()){

        for(int i=0; i<cnames.count(); i++ )
        {
           c->at(i)->push_back(query.value(rec.indexOf(cnames.at(i))).toString());

        }

    }

}
/**
 * this function fetchs  time and  several double columns  values from a table, between two time instants, orded by time.
 *
 *
 ******************************************************************************/

void dbapi::fechvariouscolumns_timeanddoubles(QString tname, QStringList cnames,long long itime,long long ltime,std::vector<long long >  *time,  QVector<std::vector<double> * >  *c )
{
    QString Query;

    Query="SELECT ";
    Query.append("time,");
    for(int i=0; i<cnames.count();i++)
    {
        Query.append(cnames[i]);
        if(i<cnames.count()-1) Query.append(","); else Query.append(" ");
    }
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append(" WHERE time >= ");
    Query.append(QString::number(itime));
    Query.append(" AND time <= ");
    Query.append(QString::number(ltime));
    Query.append(" ");
    Query.append(" ORDER BY time");

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=(cnames.count()+1)){
        emit logmessages("number of coluns should be  "+QString::number(cnames.count()+1));

        return;
    }

    while (query.next()){
        time->push_back(query.value(rec.indexOf("time")).toLongLong());
        for(int i=0; i<cnames.count(); i++ )
        {
            c->at(i)->push_back(query.value(rec.indexOf(cnames.at(i))).toDouble());
        }

    }

}

/**
 * this function fetchs   alll cell variant values from a table  .
 *
 *
 ******************************************************************************/
void dbapi::fechallrowsvariantcells(QString tname, QStringList  cnames, QList<std::vector<QVariant> * >  *c  )
{


    QString Query;

    Query="SELECT ";
    for(int i=0; i<cnames.count();i++)
    {
        Query.append(cnames[i]);
        if(i<cnames.count()-1) Query.append(","); else Query.append(" ");
    }
    Query.append(" FROM  ");
    Query.append(tname);



    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=(cnames.count())){
        emit logmessages("number of columns should be  "+QString::number(cnames.count()+1));

        return;
    }
    while (query.next()){

        for(int i=0; i<cnames.count(); i++ )
        {
            c->at(i)->push_back(query.value(rec.indexOf(cnames.at(i))));

        }

    }


}

/**
 * this function deletes all data from a given table.
 *
 *
 ******************************************************************************/

void  dbapi::emptytable(QString tname)
{

    QString Query;

    Query="TRUNCATE  TABLE  ";
    Query.append(tname);
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessagesR(Error.databaseText());
    }

}
/**
 * this function performs a generic sql instruction.
 *
 *
 ******************************************************************************/

void  dbapi::doaquery(QString Query ){
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessagesR(Error.databaseText());
        emit logmessagesR("*****the query was:******");
        emit logmessages(Query);
        emit logmessagesR("*********************");

    }

}
/**
 * this function fetchs the names and types of table columns
 *
 *
 ******************************************************************************/

void  dbapi::getallcolumnsnameandtype(QString tablename, QStringList *cnames,QStringList *ctypes)
{
    QString Query;
    Query=" SHOW COLUMNS FROM ";
    Query.append(tablename);
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessagesR(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    cnames->clear();
    ctypes->clear();

    while (query.next()){

        for(int i=0; i<rec.count(); i++ )
        {

            if(rec.fieldName(i)=="Field")cnames->append(query.value(rec.indexOf("Field")).toString());
            if(rec.fieldName(i)=="Type")ctypes->append(query.value(rec.indexOf("Type")).toString());

        }

    }


}
/**
 * this function adds  columns to a given table
 *
 *
 ******************************************************************************/

void  dbapi::addcolumns(QString tname, QStringList cname, QStringList ctype )
{

    QSqlDatabase db = QSqlDatabase::database(connectionName);


    QString Query;
    for(int i=0; i<cname.count();i++)
    {
        Query="ALTER TABLE ";
        Query.append(tname);
        Query.append(" ADD ");
        Query.append(cname[i]);
        Query.append(" ");
        Query.append(ctype[i]);
        Query.append(";");
        QSqlQuery query(db);
        if(!query.exec(Query)){
            QSqlError Error= query.lastError();
            emit logmessages(Error.databaseText());
        }
    }


}
/**
 * this function returns time values and a given double column values (ordered by time) betwen two time instants.
 *
 *
 ******************************************************************************/


void dbapi::fech2columns_timeanddouble(QString tname, QString cname,long long itime,long long ltime,std::vector<long long >  *time,  std::vector<double>  *c )
{

    QString Query;

    Query="SELECT ";
    Query.append("time,");
    Query.append(cname);
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append(" WHERE time >= ");
    Query.append(QString::number(itime));
    Query.append(" AND time <= ");
    Query.append(QString::number(ltime));
    Query.append(" AND ");
    Query.append(cname);
    Query.append(" IS NOT NULL");
    Query.append(" ORDER BY time;");


    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=2){
        emit logmessages("number of columns should be 2 ");
        return;
    }
    while (query.next()){
        time->push_back(query.value(rec.indexOf("time")).toLongLong());
        c->push_back(query.value(rec.indexOf(cname)).toDouble());

    }



}

/**
 * this function returns time values and a given double column values (ordered by time) betwen two time instants.
 * when condition are met. when the input clause as the value of the input cclause
 *
 ******************************************************************************/

void dbapi::fech2columns_timeanddoublefilter(QString tname, QString cname, QString cclause,QString clause,long long itime,long long ltime,std::vector<long long >  *time,  std::vector<double>  *c )
{


    QString Query;

    Query="SELECT ";
    Query.append("time,");
    Query.append(cname);
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append(" WHERE time >= ");
    Query.append(QString::number(itime));
    Query.append(" AND time <= ");
    Query.append(QString::number(ltime));
    Query.append(" AND   ");
    Query.append(cclause);
    Query.append("=");
    Query.append(clause);
    Query.append(" ORDER BY time");


    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=2){
        emit logmessages("number of coluns should be 2 ");
        return;
    }

    while (query.next()){


        time->push_back(query.value(rec.indexOf("time")).toLongLong());
        c->push_back(query.value(rec.indexOf(cname)).toDouble());



    }



}

/**
 * this function returns time values and a given string column values (ordered by time) betwen two time instants.
 * when condition are met. when the input clause as the value of the input cclause
 *
 ******************************************************************************/

void dbapi::fech2columns_timeandstringfilter(QString tname, QString cname,QString cclause,QString clause,long long itime, long long ltime,std::vector<long long >  *time,  std::vector<QString>  *c )
{

    QString Query;

    Query="SELECT ";
    Query.append("time,");
    Query.append(cname);
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append(" WHERE time >= ");
    Query.append(QString::number(itime));
    Query.append(" AND time <= ");
    Query.append(QString::number(ltime));
    Query.append(" AND   ");
    Query.append(cclause);
    Query.append("=");
    Query.append(clause);
    Query.append(" ORDER BY time");

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=2){
        emit logmessages("number of columns should be 2 ");
        return;
    }

    while (query.next()){


        time->push_back(query.value(rec.indexOf("time")).toLongLong());
        c->push_back(query.value(rec.indexOf(cname)).toString());



    }



}
/**
 * this function returns time values and a given text column values (ordered by time) betwen two time instants.
 *
 *
 ******************************************************************************/

void dbapi::fech2columns_timeandstring(QString tname, QString cname,long long itime, long long ltime,std::vector<long long >  *time,  std::vector<QString>  *c )
{

    QString Query;

    Query="SELECT ";
    Query.append("time,");
    Query.append(cname);
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append(" WHERE time >= ");
    Query.append(QString::number(itime));
    Query.append(" AND time <= ");
    Query.append(QString::number(ltime));
    Query.append(" ");
    Query.append("ORDER BY time");
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=2){
        emit logmessages("number of columns should be 2 ");
        return;
    }

    while (query.next()){

        time->push_back(query.value(rec.indexOf("time")).toLongLong());
        c->push_back(query.value(rec.indexOf(cname)).toString());



    }



}


/**
 * this function returns  column values (ordered by one of them ) betwen two instances of one of the columns.
 * This is useful  for a table when the time values are not named time as in case of runtable
 *
 ******************************************************************************/

void  dbapi::fechcolumns_string_orderby(QString tname, QStringList cnames,long long itime,long long ltime,QString order, QList<std::vector<QString> * >  *c )
{


    QString Query;

    Query="SELECT ";
    for(int i=0; i<cnames.count();i++)
    {
        Query.append(cnames[i]);
        if(i<cnames.count()-1) Query.append(","); else Query.append(" ");
    }
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append(" WHERE ");
    Query.append(order);
    Query.append(" >= ");
    Query.append(QString::number(itime));
    Query.append(" AND ");
    Query.append(order);
    Query.append(" <= ");
    Query.append(QString::number(ltime));
    Query.append( " ORDER BY ");
    Query.append(order);
    Query.append(";");

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }
    QSqlRecord rec = query.record();
    if(rec.count()!=(cnames.count())){
        emit logmessages("number of columns should be  "+QString::number(cnames.count()));

        return;
    }

    while (query.next()){

        for(int i=0; i<cnames.count(); i++ )
        {
            c->at(i)->push_back(query.value(rec.indexOf(cnames.at(i))).toString());

        }

    }

}
/**
 * this function return true if tabble exists
 *
 *
 ******************************************************************************/

bool  dbapi::checkiftableixist(QString tablename)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QStringList tlist;
    bool result;
    result=false;
    tlist = db.tables(QSql::Tables);
    for(int i=0; i<tlist.count(); i++)
    {
        if(tablename.simplified()==tlist.at(i).simplified()){
            result=true;
            break;
        }
    }
    return result;
}


/**
 * this function return true if a given entry exist in a table
 *
 *
 ******************************************************************************/

bool  dbapi::checkifentryexist(QString tablename, QString Column, QString Value)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QString Query;

    Query="SELECT EXISTS(SELECT * FROM ";
    Query.append(tablename);
    Query.append(" WHERE ");
    Query.append(Column);
    Query.append("=");
    Query.append(Value);
    Query.append(");");
    bool result;
    result=false;

    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }


    while (query.next()){
        result=query.value(0).toBool();

    }

    return result;
}

/**
 * this function return true if a given entry exist in a table
 * the condition are two coluns beegin equal one is string another a long long
 *
 ******************************************************************************/

bool  dbapi::checkifBIGINTentryexist(QString tablename, QString Column, long long Value, QString Colums, QString sValue)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QString Query;

    Query="SELECT EXISTS(SELECT * FROM ";
    Query.append(tablename);
    Query.append(" WHERE ");
    Query.append(Column);
    Query.append("=");
    Query.append(QString::number(Value));
    Query.append(" AND ");
    Query.append(Colums);
    Query.append("=");
    Query.append(sValue);

    Query.append(");");
    bool result;
    result=false;

    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }


    while (query.next()){
        result=query.value(0).toBool();

    }

    return result;
}





/**
 * this function deletes all rows in a table that match a given pattern in a column value.
 *
 *
 ******************************************************************************/

void  dbapi::Delrowsfrompatern(QString tname, QString  columnname, QString patern)
{


    QStringList cnames;
    QList<std::vector<QString> * >  rows;
    cnames.clear();
    rows.clear();
    cnames << columnname ;
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    for(int i=0; i<cnames.count(); i++) rows << new ( std::vector<QString>);
    fechnotimecolumns_string(tname,cnames,&rows);
    QString removerow;
    for(int i=0;i<(int)rows.at(0)->size(); i++){

        if(!rows.at(0)->at(i).contains(patern)) continue;
        removerow="delete from ";
        removerow.append(tname);
        removerow.append(" where ");
        removerow.append(columnname);
        removerow.append(" = '");
        removerow.append(rows.at(0)->at(i));
        removerow.append("'");
        //    qDebug() <<removerow;

        QSqlQuery queryd(db);
        if(!queryd.exec(removerow)){
            QSqlError Error= queryd.lastError();
            emit logmessagesR(Error.databaseText());
        }


    }
}
/**
 * this function   deletes all rows in a table that match a given  column value.
 *
 *
 ******************************************************************************/
void  dbapi::Delrowsmachingacolumnvalue(QString tname, QString  columnname, QString patern)
{


    QStringList cnames;
    QList<std::vector<QString> * >  rows;
    cnames.clear();
    rows.clear();
    cnames << columnname ;
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    for(int i=0; i<cnames.count(); i++) rows << new ( std::vector<QString>);
    fechnotimecolumns_string(tname,cnames,&rows);
    QString removerow;
    for(int i=0;i<(int)rows.at(0)->size(); i++){

        if(rows.at(0)->at(i).simplified()!=patern) continue;
        removerow="delete from ";
        removerow.append(tname);
        removerow.append(" where ");
        removerow.append(columnname);
        removerow.append(" = '");
        removerow.append(rows.at(0)->at(i));
        removerow.append("'");
        //    qDebug() <<removerow;

        QSqlQuery queryd(db);
        if(!queryd.exec(removerow)){
            QSqlError Error= queryd.lastError();
            emit logmessagesR(Error.databaseText());
        }


    }
}
/**
 * this function   selects two columns in a table an int column and a string column.
 *
 *
 ******************************************************************************/
void dbapi::fech2columns_stringsandints(QString tname, QString cstring ,  QString cint, QStringList *scol, QList <int> *icol)
{
    QString Query;

    Query="SELECT ";
    Query.append(cstring);
    Query.append(",");
    Query.append(cint);
    Query.append(" FROM  ");
    Query.append(tname);


    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }

    QSqlRecord rec = query.record();

    while (query.next()){

        scol->append(query.value(rec.indexOf(cstring)).toString());
        icol->append(query.value(rec.indexOf(cint)).toInt());

    }


}
/**
 * this function   selects two columns in a table an long column and a string column.
 *
 *
 ******************************************************************************/
void dbapi::fech2orderedcolumns_stringsandlonglong(QString tname, QString cstring ,  QString cint, QStringList *scol, QList <long long> *icol)
{
    QString Query;

    Query="SELECT ";
    Query.append(cstring);
    Query.append(",");
    Query.append(cint);
    Query.append(" FROM  ");
    Query.append(tname);
    Query.append( " ORDER BY ");
    Query.append(cint);
    Query.append(";");


    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    if(!query.exec(Query)){
        QSqlError Error= query.lastError();
        emit logmessages(Error.databaseText());
    }

    QSqlRecord rec = query.record();

    while (query.next()){

        scol->append(query.value(rec.indexOf(cstring)).toString());
        icol->append(query.value(rec.indexOf(cint)).toLongLong());

    }


}
