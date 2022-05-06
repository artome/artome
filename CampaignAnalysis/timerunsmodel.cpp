#include "timerunsmodel.h"
#include <QBrush>
#include <QFile>
#include <QTextStream>

TimeRunsModel::TimeRunsModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    headernames.clear();
    rownames.clear();
    timestringformat="dd/MM/yyyy hh:mm:ss";
    m_columnCount=5;
    startrun.clear();
    endrun.clear();
    startc.clear();
    endc.clear();
    description.clear(); type.clear(); commentaries.clear(); runnumber.clear();
    vscrollpos=-1;
    headernames <<"Start"<< "End" <<"type" << "Description" <<"comments" ;

}
int TimeRunsModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_rowCount ;
}

int TimeRunsModel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_columnCount;
}
void TimeRunsModel::refresh()
{
 //   this->dataChanged();
 //   this->layoutChanged();
    emit layoutChanged();

}
QVariant TimeRunsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            if(headernames.count())
              {
               if(section <headernames.count()) return headernames[section];
            } else   return QString::number(section);

        }
        if (orientation == Qt::Vertical) {
            return rownames[section];
        }

    }
    return QVariant();
}



QVariant TimeRunsModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    switch(role){
    case Qt::EditRole:

        switch(col){
          case 0:
        return  startrun.at(row);
           break;
        case 1:
            return  endrun.at(row);
            break;
        case 2:
            return  type.at(row);
            break;
      case 3:
          return  description.at(row);
          break;
      case 4:
          return  commentaries.at(row);
          break;
         }
        break;
    case Qt::DisplayRole:
        switch(col){
          case 0:
        return  QDateTime:: fromSecsSinceEpoch(startrun.at(row)).toUTC().toString(timestringformat);
           break;
        case 1:
            return  QDateTime:: fromSecsSinceEpoch(endrun.at(row)).toUTC().toString(timestringformat);
            break;
        case 2:
            return type.at(row);
            break;
        case 3:
            return  description.at(row);
            break;
        case 4:
            return commentaries.at(row);
            break;
         }
        break;
    case Qt::FontRole:
    /*    if (row == 0 && col == 0) //change font only for cell(0,0)
        {
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
   */
        break;
    case Qt::BackgroundRole:
        if(col==2)
        {
            QBrush colorBackground( QColor::fromRgb(191,255,191,50));
            return colorBackground;
        }
 /*
        if (row == 1 && col == 2)  //change background only for cell(1,2)
        {
            QBrush redBackground(Qt::red);
            return redBackground;
        }
   */
        break;
    case Qt::TextAlignmentRole:
 /*
        if (row == 1 && col == 1) //change text alignment only for cell(1,1)
        {
            return Qt::AlignRight + Qt::AlignVCenter;
        }
   */
        break;
    case Qt::CheckStateRole:

        if (col ==0 )
        {
            if(startc.at(row))return Qt::Checked; else return Qt::Unchecked;
        }
        if (col ==1 )
        {
            if(endc.at(row))return Qt::Checked; else return Qt::Unchecked;
        }

        break;
    }

    return QVariant();
}

bool TimeRunsModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    /*
    if (role == Qt::EditRole)
    {
        int row = index.row();
        int col = index.column();

        switch(ctype[col]){
          case 0:
           m_datatime.at(cindex[col])->replace(row,value.toDateTime());
           break;
        case 1:
            m_dataint.at(cindex[col])->replace(row,value.toLongLong());
            break;
        case 2:
            m_datareal.at(cindex[col])->replace(row,value.toDouble());
            break;
         }
        QString result;
        result="Qualquer coisa de útil";
         emit editCompleted( result );
    } */
    if (role == Qt::CheckStateRole)
    {
        int col = index.column();
        int row = index.row();
        if (col ==0 )
        {
            if(startc.at(row)){
                startc.replace(row,0);
                emit checkchanged(row,col,false);
            }else  {
                startc.replace(row,1);
                emit checkchanged(row,col,true);
           }
        }
        if (col ==1 )
        {
            if(endc.at(row)){
                endc.replace(row,0);
                emit checkchanged(row,col,false);
            }else  {
                endc.replace(row,1);
                emit checkchanged(row,col,true);
           }
        }



    }
    emit layoutChanged();
    return true;
}


Qt::ItemFlags TimeRunsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags result =  QAbstractTableModel::flags(index);
        if (index.column() <2) result |= Qt::ItemIsUserCheckable;
        return result;

}

