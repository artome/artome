/*!
 * \file dbapi.h
 *   <summary>
 *   include file for <B> dbapi</B> class
 *   </summary>
 *

*
*
*/#ifndef DBAPI_H
#define DBAPI_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>

class dbapi : public QObject
{
    Q_OBJECT
public:
    explicit dbapi(QObject *parent = nullptr);
    QString connectionName;
    bool debugflag;

//public functions

     void createtable_onekey(QString /**< [in] table to create */,
                             QString /**< [in] column primary key name */,
                             QStringList /**< [in] list of the column names  */,
                             QStringList /**< [in] list of the column types  */); ///< This function creates a table with one primary key

     void createtable_twokeys(QString /**< [in] table to create */,
                              QString /**< [in] column primary key name */,
                              QString /**< [in] column primary key name */,
                              QStringList /**< [in] list of the column names  */,
                              QStringList /**< [in] list of the column types  */);///< This function creates a table with two primary key

     void bindquery(QString , QStringList * );

     void addarow(QString,  QStringList ,QStringList);
     void addseveralrows(QString, QStringList,QList<QStringList>);

     void fechvariantcells(QString , QString ,QString ,QStringList, QList<QVariant> *);

     void droptable(QString/**< [in] table to delete */);///< This function deletes a table from database

     void fechastringcell(QString /**< [in] table  name  */,
                          QString /**< [in]  column name that has the value.. */,
                          QString /**< [in]  equal to this argument value  */,
                          QString /**< [in]  column name to retrive */,
                          QString */**< [in,out] Qstring poinert to store the cell value. */);

     void preparebind(QString /**< [in] table  name  */, QStringList * /**< [in] table  name  */);

     void fechnotimecolumns_string(QString /**< [in] table  name  */,
                                   QStringList /**< [in]  columns names to retrive */,
                                   QList<std::vector<QString> *  >*/**< [in,out] list of Qstring poinert to store the cell values. */);

     void fechnotimecolumns_string_filterstring(QString /**< [in] table  name  */ ,
                                                QStringList /**< [in]  columns names to retrive */,
                                                QString/**< [in]  column name that has the value.. */,
                                                QString/**< [in]  equal to this argument value  */,
                                                QList<std::vector<QString> *  >* /**< [in,out] list of Qstring poinert to store the cell values. */);


     void fechnotimecolumns_string_orderby(QString/**< [in] table  name  */,
                                           QStringList /**< [in]  columns names to retrive */,
                                           QString/**< [in]  ordered by this column value  */,
                                           QList<std::vector<QString> *  >* /**< [in,out] list of Qstring poinert to store the cell values. */);

     void fechallrowsvariantcells(QString /**< [in] table  name  */,
                                  QStringList/**< [in]  columns names to retrive */ ,
                                  QList<std::vector<QVariant> *  >* /**< [in,out] list of Qstring poinert to store the cell values. */);


     bool checkiftableixist(QString/**< [in] table  name  */);
     bool checkifentryexist(QString/**< [in] table  name  */, QString/**< [in] column  name  */, QString/**< [in] column cell value to test  */);
     bool checkifBIGINTentryexist(QString/**< [in] table  name  */, QString/**< [in] column  name  */,
                                  long long/**< [in] column cell value to test  */,
                                  QString/**< [in] second column  name  */,
                                  QString/**< [in] column cell value to test*/);


     void emptytable(QString /**< [in] table  name  */ );
     void doaquery(QString /**< [in] sql sentence  */ );
     void getallcolumnsnameandtype(QString /**< [in] sql sentence  */
                                   ,QStringList */**< [out]  List of column names  */
                                   ,QStringList * /**< [out]  List of column types  */);
     void addcolumns(QString /**< [in] table  name  */
                     ,QStringList /**< [in]  columns names to add */
                     ,QStringList  /**< [in]  types of columns names to add */ );

     void fech2columns_timeanddouble(QString  /**< [in] table  name  */
                                     ,QString /**< [in]  column name of doubles  to fectch  */
                                     ,long long/**< [in]  initial time  miliSecsSinceEpoch */
                                     ,long long /**< [in]  final time  miliSecsSinceEpoch */
                                     , std::vector<long long> * /**< [out] time values */
                                     , std::vector<double> * /**< [out] column of double values */);

     void fech2columns_timeanddoublefilter(QString /**< [in] table  name  */
                                           ,QString /**< [in]  column name of doubles  to fectch  */
                                           ,QString /**< [in]  column name that has the value.. */
                                           ,QString /**< [in]  equal to this argument value  */
                                           ,long long /**< [in]  initial time  miliSecsSinceEpoch */
                                           ,long long /**< [in]  final time  miliSecsSinceEpoch */
                                           ,std::vector<long long >  * /**< [out] time values */,
                                           std::vector<double> * /**< [out] column of double values */);

      void fech2columns_timeandstringfilter(QString /**< [in] table  name  */
                                           ,QString /**< [in]  column name of doubles  to fectch  */
                                           ,QString /**< [in]  column name of doubles  to fectch  */
                                           ,QString /**< [in]  column name that has the value.. */
                                           ,long long /**< [in]  initial time  miliSecsSinceEpoch */
                                           ,long long /**< [in]  final time  miliSecsSinceEpoch */
                                           ,std::vector<long long> * /**< [out] time values */
                                           ,std::vector<QString> * /**< [out] column of string values */);

     void fech2columns_timeandstring(QString  /**< [in] table  name  */
                                     ,QString /**< [in]  column name of doubles  to fectch  */
                                     ,long long/**< [in]  initial time  miliSecsSinceEpoch */
                                     ,long long /**< [in]  final time  miliSecsSinceEpoch */
                                     , std::vector<long long> * /**< [out] time values */
                                     , std::vector<QString> * /**< [out] column String values */);


     void fechvariouscolumns_timeanddoubles(QString /**< [in] table  name  */
                              ,QStringList /**< [in]  list of column names of doubles  to fectch  */
                              ,long long/**< [in]  initial time  miliSecsSinceEpoch */
                              ,long long/**< [in]  final time  miliSecsSinceEpoch */
                              ,std::vector<long long> * /**< [out] time values */
                              ,QVector<std::vector<double> *  >*/**< [out] list (QVector= of std::vector of columns double values */ );

     void  fechcolumns_string_orderby(QString  /**< [in] table  name  */
                                     ,QStringList  /**< [in]  list of column names of doubles  to fectch  */
                                     ,long long /**< [in]  initial time  miliSecsSinceEpoch */
                                     ,long long /**< [in]  final time  miliSecsSinceEpoch */,
                                     QString order /**<  [in] colunm to order values */,
                                     QList<std::vector<QString> * >  * /**< [out] list of Qstring poinert to store the cell values. */);

     void  fechnotimecolumns_string_filterstring_orderby(QString /**< [in] table  name  */ ,
                                                QStringList /**< [in]  columns names to retrive */,
                                                QString/**< [in]  column name that has the value.. */,
                                                QString/**< [in]  equal to this argument value  */,
                                                QString order /**<  [in] colunm to order values */,
                                                QList<std::vector<QString> *  >* /**< [in,out] list of Qstring poinert to store the cell values. */);

     void fech2columns_stringsandints(QString /**< [in] table  name  */
                                      , QString /**< [in]  string column names to retrive */
                                      , QString /**< [in]  string column names to retrive */
                                      , QStringList * /**<  [out] colunm strings values retrived */
                                      , QList <int> * /**<  [out] colunm ints values retrived */);

     void fech2orderedcolumns_stringsandlonglong(QString /**< [in] table  name  */
                                      , QString /**< [in]  string column names to retrive */
                                      , QString /**< [in]  string column names to retrive */
                                      , QStringList * /**<  [out] colunm strings values retrived */
                                      , QList <long long> * /**<  [out] colunm long values retrived */);

     void Delrowsfrompatern(QString /**< [in] table  name  */ ,
                            QString /**< [in]  column name that has the value.. */ ,
                            QString /**< [in]  containing this pattern*/);

     void Delrowsmachingacolumnvalue(QString /**< [in] table  name  */  ,
                            QString /**< [in]  column name that has the value.. */ ,
                            QString /**< [in]  equal to this*/);


 signals:
    void logmessages(QString);
    void logmessagesR(QString);

public slots:


private:
     QStringList enclosestrings(QStringList );

};

#endif // DBAPI_H
