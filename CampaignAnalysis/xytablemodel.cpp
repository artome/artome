#include "xytablemodel.h"

#include <QBrush>

xytablemodel::xytablemodel(QObject *parent) :
    QAbstractTableModel(parent)
{
    ctype.clear();
    cindex.clear();
    m_realCount=0;
    m_timeCount=0;
    m_stringCount=0;
    headernames.clear();
    rownames.clear();
    timestringformat="dd/MM/yyyy hh:mm:ss.zzz";
    m_rowCount=0;
    m_columnCount=0;



}
int xytablemodel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_rowCount ;
}

int xytablemodel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_columnCount;
}
void xytablemodel::refresh()
{
 //   this->dataChanged();
 //   this->layoutChanged();
    emit layoutChanged();

}
QVariant xytablemodel::headerData(int section, Qt::Orientation orientation, int role) const
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
            if(rownames.count())return rownames[section];else   return QString::number(section);

        }

    }
    return QVariant();
}

void xytablemodel::addcolv(int coltype, std::vector<double> *data ){

     m_rowCount=std::max(m_rowCount, (int)data->size() );
     m_columnCount++;
     beginInsertColumns(QModelIndex(),m_columnCount,m_columnCount);
     this->insertColumn(m_columnCount);
     endInsertColumns();
            switch(coltype){
              case 0:
            {
                std::vector<double>* dataVec = new std::vector<double>(data->size());
                for (int k = 0; k < (int )data->size(); k++) {
                     dataVec->at(k)=data->at(k);
                }
                 m_datatime.append(dataVec);
                 cindex <<m_timeCount;
                  m_timeCount++;
            }
              break;

              case 1:
             {
              std::vector<double>* dataVec = new std::vector<double>(data->size());
                for (int k = 0; k < (int )data->size(); k++) {
                    dataVec->at(k)=data->at(k);
                }
                m_datareal.append(dataVec);
                cindex << m_realCount;
                m_realCount++;
             }
              break;

            }

}
void xytablemodel::addcollonglongv(int coltype, std::vector<long long> *data ){

     m_rowCount=std::max(m_rowCount, (int)data->size() );
     m_columnCount++;
     beginInsertColumns(QModelIndex(),m_columnCount,m_columnCount);
     this->insertColumn(m_columnCount);
     endInsertColumns();
            switch(coltype){
              case 0:
            {
                std::vector<double>* dataVec = new std::vector<double>(data->size());
                for (int k = 0; k < (int )data->size(); k++) {
                     dataVec->at(k)=(double)data->at(k);
                }
                 m_datatime.append(dataVec);
                 cindex <<m_timeCount;
                  m_timeCount++;
            }
              break;

              case 1:
             {
              std::vector<double>* dataVec = new std::vector<double>(data->size());
                for (int k = 0; k < (int)data->size(); k++) {
                    dataVec->at(k)=(double)data->at(k);
                }
                m_datareal.append(dataVec);
                cindex << m_realCount;
                m_realCount++;
             }
              break;

            }

}
void xytablemodel::addcolstringv(int coltype, std::vector<QString> *data ){

     m_rowCount=std::max(m_rowCount, (int)data->size() );
     m_columnCount++;
     beginInsertColumns(QModelIndex(),m_columnCount,m_columnCount);
     this->insertColumn(m_columnCount);
     endInsertColumns();
            switch(coltype){
              case 2:
            {
                std::vector<std::string>* dataVec = new std::vector<std::string>(data->size());
                for (int k = 0; k < (int )data->size(); k++) {
                     dataVec->at(k)=data->at(k).toStdString();
                }
                m_datastring.append(dataVec);
                cindex << m_stringCount;
                m_stringCount++;
             }
              break;



            }


}

void xytablemodel::addcoldatetimev(int coltype, std::vector<QDateTime> *data ){

     m_rowCount=std::max(m_rowCount, (int)data->size() );
     m_columnCount++;
     beginInsertColumns(QModelIndex(),m_columnCount,m_columnCount);
     this->insertColumn(m_columnCount);
     endInsertColumns();
            switch(coltype){
              case 0:
            {
                std::vector<double>* dataVec = new std::vector<double>(data->size());
                for (int k = 0; k < (int )data->size(); k++) {
                     dataVec->at(k)=(double)data->at(k). toSecsSinceEpoch();

                }
                 m_datatime.append(dataVec);
                 cindex <<m_timeCount;
                  m_timeCount++;
            }
              break;

              case 1:
             {
              std::vector<double>* dataVec = new std::vector<double>(data->size());
                for (int k = 0; k < (int )data->size(); k++) {
                    dataVec->at(k)=(double)data->at(k). toSecsSinceEpoch();
                }
                m_datareal.append(dataVec);
                cindex << m_realCount;
                m_realCount++;
             }
              break;

            }

}

void xytablemodel::addcoldoublearray(int coltype, double *data, int size ){
     m_rowCount=std::max(m_rowCount, size );
     m_columnCount++;
     beginInsertColumns(QModelIndex(),m_columnCount,m_columnCount);
     this->insertColumn(m_columnCount);
     endInsertColumns();
            switch(coltype){
              case 0:
            {
                std::vector<double>* dataVec = new std::vector<double>(size);
                for (int k = 0; k < size; k++) {
                     dataVec->at(k)= data[k];
                }
                 m_datatime.append(dataVec);
                 cindex <<m_timeCount;
                  m_timeCount++;
            }
              break;

              case 1:
             {
                std::vector<double>* dataVec = new std::vector<double>(size);
                for (int k = 0; k < size; k++) {
                     dataVec->at(k)=data[k];
                }
                m_datareal.append(dataVec);
                cindex << m_realCount;
                m_realCount++;
             }
              break;

            }

}
void xytablemodel::addcoldatetimearray(int coltype, QDateTime *data, int size ){
     m_rowCount=std::max(m_rowCount, size );
     m_columnCount++;
     beginInsertColumns(QModelIndex(),m_columnCount,m_columnCount);
     this->insertColumn(m_columnCount);
     endInsertColumns();
            switch(coltype){
              case 0:
            {
                std::vector<double>* dataVec = new std::vector<double>(size);
                for (int k = 0; k < size; k++) {
                     dataVec->at(k)= (double)data[k]. toSecsSinceEpoch();
                }
                 m_datatime.append(dataVec);
                 cindex <<m_timeCount;
                  m_timeCount++;
            }
              break;

              case 1:
             {
                std::vector<double>* dataVec = new std::vector<double>(size);
                for (int k = 0; k < size; k++) {
                     dataVec->at(k)=(double)data[k].toSecsSinceEpoch();
                }
                m_datareal.append(dataVec);
                cindex << m_realCount;
                m_realCount++;
             }
              break;

            }

}

void xytablemodel::addcolempty(int type, int size )
{
    m_rowCount=std::max(m_rowCount, size );
    m_columnCount++;
    beginInsertColumns(QModelIndex(),m_columnCount,m_columnCount);
    this->insertColumn(m_columnCount);
    endInsertColumns();
    switch(type){
      case 0:
    {
        std::vector<double>* dataVec = new std::vector<double>(size);
          m_datatime.append(dataVec);
         cindex <<m_timeCount;
          m_timeCount++;
    }
      break;

      case 1:
     {
        std::vector<double>* dataVec = new std::vector<double>(size);
        m_datareal.append(dataVec);
        cindex << m_realCount;
        m_realCount++;
     }
      break;
    case 2:
   {
      std::vector<std::string>* dataVec = new std::vector<std::string>(size);
      m_datastring.append(dataVec);
      cindex << m_stringCount;
      m_stringCount++;
   }
    break;
    }
}
void xytablemodel::fillemptycol(int type, int k ,double value) // a ser chamada logo imeidatamente depois de addcolempty
{
    switch(type){
      case 0:

             m_datatime.last()->at(k)=(double)value;

      break;

      case 1:
           m_datareal.last()->at(k)=(double)value;
      break;

    }
}

void xytablemodel::fillStringemptycol(int type,int k,const char *s) // a ser chamada logo imidatamente depois de addcolempty
{
    switch(type){
      case 2:

             m_datastring.last()->at(k)=s;

      break;



    }
}
void xytablemodel::addcolstring(int coltype, std::string *data, int size ){
    m_rowCount=std::max(m_rowCount, size);
     m_columnCount++;
     beginInsertColumns(QModelIndex(),m_columnCount,m_columnCount);
     this->insertColumn(m_columnCount);
     endInsertColumns();
            switch(coltype){


              case 2:
             {
                std::vector<std::string>* dataVec = new std::vector<std::string>(size);
                for (int k = 0; k < size; k++) {
                     dataVec->at(k)=data->at(k);
                }
                m_datastring.append(dataVec);
                cindex << m_realCount;
                m_realCount++;
             }
              break;

            }

}

void xytablemodel::addcollonglongarray(int coltype, long long *data, int size ){
    m_rowCount=std::max(m_rowCount, size);
     m_columnCount++;
     beginInsertColumns(QModelIndex(),m_columnCount,m_columnCount);
     this->insertColumn(m_columnCount);
     endInsertColumns();
            switch(coltype){
              case 0:
            {
                std::vector<double>* dataVec = new std::vector<double>(size);
                for (int k = 0; k < size; k++) {
                     dataVec->at(k)=(double)data[k];
                }
                 m_datatime.append(dataVec);
                 cindex <<m_timeCount;
                  m_timeCount++;
            }
              break;

              case 1:
             {
                std::vector<double>* dataVec = new std::vector<double>(size);
                for (int k = 0; k < size; k++) {
                     dataVec->at(k)=(double) data[k];
                }
                m_datareal.append(dataVec);
                cindex << m_realCount;
                m_realCount++;
             }
              break;

            }

}
QVariant xytablemodel::data(const QModelIndex &index, int role) const
{
    int col = index.column();
    int row = index.row();
    switch(role){
    case Qt::EditRole:
        switch(ctype[col]){
          case 0:
        if((int )m_datatime.at(cindex[col])->size() >row) return  m_datatime.at(cindex[col])->at(row);
           break;
        case 1:
           if((int )m_datareal.at(cindex[col])->size() >row)  return  m_datareal.at(cindex[col])->at(row);
            break;
        case 2:
           if((int )m_datastring.at(cindex[col])->size() >row)  return  QString::fromStdString(m_datastring.at(cindex[col])->at(row));
            break;
         }
    case Qt::DisplayRole:
        switch(ctype[col]){
          case 0:
       if((int )m_datatime.at(cindex[col])->size() >row){

           if(timestringformat.contains("igor"))
               return  (QString::number((long long)(m_datatime.at(cindex[col])->at(row))/1000.+ 2082844800,'f',0));
           else if (timestringformat.contains("unixtime") )
               return QString::number(m_datatime.at(cindex[col])->at(row)/1000.,'f',0);
           else return QDateTime::fromMSecsSinceEpoch((long long)m_datatime.at(cindex[col])->at(row)).toUTC().toString(timestringformat);
       }
           break;
        case 1:
        if((int )m_datareal.at(cindex[col])->size() >row)      return  m_datareal.at(cindex[col])->at(row);
            break;

        case 2:
          if((int )m_datastring.at(cindex[col])->size() >row)      return  m_datastring.at(cindex[col])->at(row).c_str();
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
        if(ctype[col]==0)
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
 /*
        if (row == 1 && col == 0) //add a checkbox to cell(1,0)
        {
            return Qt::Checked;

        }
    */
        break;
    }

    return QVariant();

}

bool xytablemodel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        int row = index.row();
        int col = index.column();

        switch(ctype[col]){
          case 0:
           m_datatime.at(cindex[col])->at(row)=value.toDouble();
           break;
        case 1:
            m_datareal.at(cindex[col])->at(row)=value.toDouble();
            break;
        case 2:
            m_datastring.at(cindex[col])->at(row)=value.toChar().toLatin1();
            break;
         }
        QString result;
        result="Qualquer coisa de útil";
         emit editCompleted( result );
    }
    return true;
}


Qt::ItemFlags xytablemodel::flags(const QModelIndex &index) const
{
   return Qt::ItemIsEditable | QAbstractTableModel::flags(index);

}
