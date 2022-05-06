#include "plotproprieties.h"

plotproprieties::plotproprieties()
{
    gridlinestyle=2;
    defaultpwith=1;
    hdpithickeness=0.0;
    hdpifont=1.;
    charheight=0.;
    stylex=false;styley=false;
    MRightaxis= "cmstv";
    MLeftaxis="bnstv";
    MRightaxislog="cmstlv";
    MLeftaxislog="bnstlv";
    Xtimeaxis="dcbnstv";
    Ytimeaxis="cbmnstv";
    Ytimeaxislog="cbmnstlv";
    firsttimelabel=true;
    fontsize=0.6;
    fontset=1;
    labelsep=2.5;
    multiaxissep=.08;
    xvmin=0.1;xvmax=0.9;yvmin=0.1;yvmax=0.85;
    withoutxaxis=false;
    runangle=1.570796;
    runy=0.5;
    Multixais=1;
    celldis=2.;
    celllabsep=2.5;
    cellpos=.1;
    celljus=0.;

    bred=255; bblue=255; bgreen=255; balpha=1.0;

    leftaxislimits.clear();
    rightaxislimits.clear();

    plottype=0; //0 typlot 1 surface 2 histo
    runstart=1; //0 every runstart (even at borders) 1 do not plot runs at borderds -1 do not plot runstarts
}
