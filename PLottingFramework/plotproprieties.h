#ifndef PLOTPROPRIETIES_H
#define PLOTPROPRIETIES_H
#include <QString>
#include "plstream.h"

class plotproprieties
{
public:
    plotproprieties();
    int gridlinestyle;
    bool stylex,styley;
    int defaultpwith;
    QString MRightaxis, MLeftaxis, MRightaxislog, MLeftaxislog;
    QString Xtimeaxis,Ytimeaxis,Ytimeaxislog;
    QString Plotname;
    double hdpithickeness;
    bool firsttimelabel;
    PLINT fontset;
    PLFLT fontsize;
    PLFLT labelsep;
    PLFLT xvmin,xvmax,yvmin, yvmax;
    double xvminp,xvmaxp,yvminp,yvmaxp;
    double celldis,celllabsep,cellpos,celljus;
    double multiaxissep;
    bool withoutxaxis;
    double runangle,runy;
    int Multixais;
    QString leftaxislimits;
    QString rightaxislimits;
    int plottype;
    int runstart;
    int bred,bgreen,bblue;
    double balpha;
    double hdpifont,charheight;
    QString plottitle;

};

#endif // PLOTPROPRIETIES_H
