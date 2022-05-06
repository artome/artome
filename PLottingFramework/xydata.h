#ifndef XYDATA_H
#define XYDATA_H
#include <vector>
#include <QString>
#include "xyproprieties.h"

class xydata
{
public:
    xydata();
    xyproprieties p;
    std::vector<double>x,y,logy,logx,ysmooth,yder, xder;
    int setarrayvalue(double , int , unsigned long); //value, dimension(0 or 1), array pos.
    void FindYminmax(double , double );
    void FindYlminmax(double , double );
    void FindYsminmax(double , double );
    void FindYmin(double , double );
    void FindYlmin(double , double );
    void FindYsmin(double , double );
    void FindYmax(double , double );
    void FindYlmax(double , double );
    void FindYsmax(double , double );
    void Findlog(int );
    void evaluatesmooth(int,double );
    void FindYdmin(double , double );
    void FindYdmax(double , double );

    long derivativedouble(int );

};

#endif // XYDATA_H
