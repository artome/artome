#ifndef XYPROPRIETIES_H
#define XYPROPRIETIES_H
#include <QString>

class xyproprieties
{
public:
    xyproprieties();
    bool maxfreelimits,minfreelimits;
    int smoothpoints; double smoothalpha;
    int psmoothwidth,pwidth;
    int devnpoints;
    double Ymin, Ymax,Ysmin, Ysmax,Ylmax,Ylmin,Xlmax,Xlmin;
    double Ydmin, Ydmax;
    bool leftaxis,rightaxis,ylog,smooth,onlysmooth;
    bool logyevaluated;
    bool derivative;
    QString Color;
    QString yaxisLabel;
    int curvestyle;
    QString linetype;
    double markersscale;
    QString factor;
    int marker;
    QString cname,tname;


};

#endif // XYPROPRIETIES_H
