#ifndef XYZDATA_H
#define XYZDATA_H

#include <vector>
#include <QString>
#include <QVector>
#include "xyzproprieties.h"
class xyzdata
{
public:
    xyzdata();
    xyzproprieties p;
    std::vector<double>  x,y,logy;
    std::vector<std::vector<double> >  z,logz;

    void FindYminmax( );
    void FindYlminmax();
    void FindZminmax(double , double );
    void FindZlminmax(double , double );

    void FindZmin(double , double );
    void FindZlmin(double , double );
    void FindZmax(double , double );
    void FindZlmax(double , double );


    void Findlog(int );
    void FindlogZ(double );

};

#endif // XYZDATA_H
