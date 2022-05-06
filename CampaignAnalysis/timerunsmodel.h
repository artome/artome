#ifndef TIMERUNSMODEL_H
#define TIMERUNSMODEL_H


#include <QAbstractTableModel>
#include <QDateTime>
#include <QStringList>

class TimeRunsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TimeRunsModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const ;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QStringList headernames,rownames;
    QString timestringformat;
    int m_columnCount;
    int m_rowCount;
    QVector<QString>  description, type, commentaries, runnumber;
    QVector<qlonglong> startrun;
    QVector<qlonglong> endrun;
    QVector<int> startc, endc;
    void refresh();
    int vscrollpos;


signals:
    void editCompleted(const QString &);
    void checkchanged(int, int, bool);
  //  void dataChanged();

public slots:
private:



};

#endif // TIMERUNSMODEL_H
