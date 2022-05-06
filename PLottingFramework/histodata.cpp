#include "histodata.h"

histodata::histodata()
{

}
void histodata::FindYminmax()
{
    double ymin, ymax;
    ymin = +10.e33;
    ymax = -10.e33;
    unsigned long size=y.size();
    for (unsigned long i = 0; i < size; i++) {
        ymin = std::min(ymin, y.at(i));
        ymax = std::max(ymax, y.at(i));

    }
    p.Ymin=ymin;
    p.Ymax=ymax;
    if(ymin > 1.e32) {p.Ymin=1.;}
    if(ymax <-1.e32){ p.Ymax=10.;}
    if(std::abs(p.Ymax-p.Ymin) <.1e-31){p.Ymin=p.Ymin-5.; p.Ymax=p.Ymax+5.;}

}
