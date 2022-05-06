#ifndef XYTABLEMODEL_H
#define XYTABLEMODEL_H
#include <QAbstractTableModel>
#include <QDateTime>
#include <vector>
#include <cstring>


class xytablemodel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit xytablemodel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const ;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void refresh();

    QList<int> ctype; //0 -datatime, 1,double, 2 string
    void fill(int); //ctype tem que ser preenchido antes de se chamar esta função
    QStringList headernames, rownames;
    QString timestringformat;
    int m_columnCount;
    int m_rowCount;
    QList<int> cindex;
    QList<std::vector<double> * > m_datareal;
    QList<std::vector<double> * > m_datatime;
    QList<std::vector<std::string> * > m_datastring;

    void addcolv(int,std::vector<double> *);
    void addcollonglongv(int,std::vector<long long> *);
    void addcolstringv(int,std::vector<QString> *);
    void addcoldatetimev(int , std::vector<QDateTime> * );
    void addcoldoublearray(int, double *,int);
    void addcoldatetimearray(int, QDateTime *, int);
    void addcollonglongarray(int,long long *,int);
    void addcolempty(int , int  );
    void fillemptycol(int , int  ,double );
    void fillStringemptycol(int , int, const char * );
    void addcolstring(int , std::string *, int  );

signals:
    void editCompleted(const QString &);
  //  void dataChanged();

public slots:
private:
    int m_realCount, m_timeCount,m_stringCount;

};

#endif // XYTABLEMODEL_H
