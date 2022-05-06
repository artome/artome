#include "xyproprieties.h"

xyproprieties::xyproprieties()
{
    leftaxis=true;ylog=false;smooth=false;onlysmooth=false;rightaxis=false;
    logyevaluated=false;
    derivative=false;
    smoothpoints=6;
    devnpoints=20;
    smoothalpha=0.1;
    Color="0000FF";
    curvestyle=1; //continuous
    linetype="line";
    psmoothwidth=2;
    pwidth=1;
    markersscale=2.5;
    marker=1;
    factor="1.0";
    maxfreelimits=true;
    minfreelimits=true;
    yaxisLabel.clear();
}
