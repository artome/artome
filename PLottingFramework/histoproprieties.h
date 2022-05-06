#ifndef HISTOPROPRIETIES_H
#define HISTOPROPRIETIES_H
#include <QString>

class histoproprieties
{
public:
    histoproprieties();
    QString period;
    double Ymin,Ymax;
    QString histocolor, xlabel,ylabel;
    int htype;
    QString plotidentifier;
    double factor;
    QString closanstables;
    QString Bins;

};

#endif // HISTOPROPRIETIES_H
