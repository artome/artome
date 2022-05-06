#ifndef XYZPROPRIETIES_H
#define XYZPROPRIETIES_H
#include <QString>


class xyzproprieties
{
public:
    xyzproprieties();
    bool logyevaluated;
    bool logzevaluated;
    double Ymin, Ymax,Ysmin, Ysmax,Ylmax,Ylmin,Zlmax,Zlmin,Zmax,Zmin,zlogmin;
    bool ylog,zlog;
    bool maxfreelimits,minfreelimits;
    QString surfacetype,colorscale,colorlimits;
    double xdist; //(half of the maximum allowed distance in time, delta time highter than this are considered lack of data
    double dcleg,celllabsep,dxvmax,dxvmin;
    QString Surfacecolor;
    QString Surfacelegendinside;
    QString yaxisLabel;
    QString plotidentifier;
    double factor;
    QString tableandcolumns;
    QString diameters;
};

#endif // XYZPROPRIETIES_H
