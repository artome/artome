#include "xydata.h"
#include "gslwrapper.h"

xydata::xydata()
{
    x.clear();y.clear();logy.clear();logx.clear();ysmooth.clear();

}
int xydata::setarrayvalue(double value, int i, unsigned long j)
{

    switch ( i ) {
    case 0:
        if(j >x.size()-1) return -1;
        x[j]=value;

        break;
    case 1:
        if(j >y.size()-1) return -1;
        y[j]=value;

        break;

    default:
        return(-1);
    }
    return 1;

}
void xydata::FindYminmax(double xi, double xf)
{
    double ymin, ymax;
    ymin = +10.e33;
    ymax = -10.e33;
    for (unsigned long i = 0; i < x.size(); i++) {
        if ((x.at(i) >= xi) && (x.at(i) <= xf)) {
            ymin = std::min(ymin, y.at(i));
            ymax = std::max(ymax, y.at(i));
        }
    }
    p.Ymin=ymin;
    p.Ymax=ymax;
    if(ymin > 1.e32) {p.Ymin=1.;}
    if(ymax <-1.e32){ p.Ymax=10.;}
    if(std::abs(p.Ymax-p.Ymin) <.1e-31){p.Ymin=p.Ymin-5.; p.Ymax=p.Ymax+5.;}

}
void xydata::FindYlminmax(double xi, double xf)
{
    double ymin, ymax;
    ymin = 31.;
    ymax = -31.;
    for (unsigned long i = 0; i < x.size(); i++) {
        if ((x.at(i) >= xi) && (x.at(i) <= xf)) {
            ymin = std::min(ymin, logy.at(i));
            ymax = std::max(ymax, logy.at(i));
        }
    }
    p.Ylmin=ymin;
    p.Ylmax=ymax;
    if(std::abs(p.Ylmax-p.Ylmin) <.1e-10){p.Ylmax=p.Ylmin+1.;}
    if(ymin > 30 && ymax <-30){p.Ylmin=0.; p.Ylmax=2.;} else {
        if(ymin > 30) {p.Ylmin=-30.;}
        if(ymax <-30){p.Ylmax=30.;}
    }
}

void xydata::FindYsminmax(double xi, double xf)
{
    double ymin, ymax;
    ymin = +10.e33;
    ymax = -10.e33;
    for (unsigned long i = 0; i < x.size(); i++) {
        if ((x.at(i) >= xi) && (x.at(i) <= xf)) {
            ymin = std::min(ymin, ysmooth.at(i));
            ymax = std::max(ymax, ysmooth.at(i));
        }
    }
    p.Ysmin=ymin;
    p.Ysmax=ymax;
    if(ymin > 1.e32) {p.Ysmin=1.;}
    if(ymax <-1.e32){p.Ysmax=10.;}
    if(std::abs(p.Ysmax-p.Ysmin) <.1e-31){p.Ysmin=std::max(p.Ysmin-1.,0.); p.Ysmax=p.Ysmax+1.;}

}
void xydata::FindYmin(double xi, double xf)
{
    double ymin;
    ymin = +10.e33;
    for (unsigned long i = 0; i < x.size(); i++) {
        if ((x.at(i) >= xi) && (x.at(i) <= xf)) {
            ymin = std::min(ymin, y.at(i));
        }
    }
    p.Ymin=ymin;
    if(ymin > 1.e32) {p.Ymin=1.;}

}
void xydata::FindYlmin(double xi, double xf)
{
    double ymin;
    ymin = 31.;
    for (unsigned long i = 0; i < x.size(); i++) {
        if ((x.at(i) >= xi) && (x.at(i) <= xf)) {
            ymin = std::min(ymin, logy[i]);
        }
    }
    p.Ylmin=ymin;
    if(ymin > 30) {p.Ylmin=-30.;}
}


void xydata::FindYsmin(double xi, double xf)
{
    double ymin;
    ymin = +10.e33;
    for (unsigned long i = 0; i < x.size(); i++) {
        if ((x.at(i) >= xi) && (x.at(i) <= xf)) {
            ymin = std::min(ymin, ysmooth.at(i));
        }
    }
    p.Ysmin=ymin;
    if(ymin > 1.e32) {p.Ysmin=1.;}

}
void xydata::FindYdmin(double xi, double xf)
{
    double ymin;
    ymin = +10.e33;
    for (unsigned long i = 0; i < xder.size(); i++) {
        if ((xder.at(i) >= xi) && (xder.at(i) <= xf)) {
            ymin = std::min(ymin, yder.at(i));
        }
    }
    p.Ydmin=ymin;
    if(ymin > 1.e32) {p.Ydmin=1.;}

}
void xydata::FindYmax(double xi, double xf)
{
    double ymax;
    ymax = -10.e33;
    for (unsigned long i = 0; i < x.size(); i++) {
        if ((x.at(i) >= xi) && (x.at(i) <= xf)) {
            ymax = std::max(ymax, y.at(i));
        }
    }
    p.Ymax=ymax;
    if(ymax <-1.e32){ p.Ymax=10.;}

}
void xydata::FindYlmax(double xi, double xf)
{
    double ymax;
    ymax = -31.;
    for (unsigned long i = 0; i < x.size(); i++) {
        if ((x.at(i) >= xi) && (x.at(i) <= xf)) {
            ymax = std::max(ymax, logy[(unsigned long)i]);
        }
    }
    p.Ylmax=ymax;
    if(ymax <-30){p.Ylmax=30.;}
}


void xydata::FindYsmax(double xi, double xf)
{
    double ymax;
    ymax = -10.e33;
    for (unsigned long i = 0; i < x.size(); i++) {
        if ((x.at(i) >= xi) && (x.at(i) <= xf)) {
            ymax = std::max(ymax, ysmooth.at(i));
        }
    }
    p.Ysmax=ymax;
    if(ymax <-1.e32){p.Ysmax=10.;}

}
void xydata::FindYdmax(double xi, double xf)
{
    double ymax;
    ymax = -10.e33;
    for (unsigned long i = 0; i < xder.size(); i++) {
        if ((xder.at(i) >= xi) && (x.at(i) <= xf)) {
            ymax = std::max(ymax, yder.at(i));
        }
    }
    p.Ydmax=ymax;
    if(ymax <-1.e32){p.Ydmax=10.;}

}
void xydata::evaluatesmooth(int npoints, double alpha)
{
    std::vector<double> p;
    /*
     * adjacent averages  of the n point y values using all the y values  in
     * x-npoints<x<x+npoints interval. Ignore %alfa smaller valueus and laege values to deal with outliners
     */
    int   i,j, count;
    int size= (int) y.size();
    ysmooth.resize(size);

    for (i = 0; i <  size; i++) {
        p.clear();
        count=0;
        for(j=-npoints; j<npoints+1; j++)
        {
            if(i+j >-1 && i+j < size)
            {
                p.push_back(y.at((unsigned long)(i+j)));
                count++;
            }
        }
        if(count>int(alpha*2*npoints)) ysmooth[(unsigned long)i]=GslWrapper::vector_trimMean(&p,alpha);
        else ysmooth[(unsigned long)i]=y.at((unsigned long)i);
    }

}
void xydata::Findlog(int i )
{

    switch ( i ) {
    case 0:
    {unsigned long size= x.size();
        logx.resize(size);
        for (unsigned long j=0; j<size; j++){

            logx[j]=log10(std::max(x.at(j), 1.e-30));

        }
    }
        break;
    case 1:
    {
        unsigned long size= y.size();
        logy.resize(size);
        for (unsigned long j=0; j<size; j++){

            logy[j]=log10(std::max(y.at(j), 1.e-30));

        }
        p.logyevaluated=true;
    }
        break;

    default:

        break;

    }
    return;
}
long xydata::derivativedouble(int nd)
{

    int n=(int) x.size();
    double sx, sxy, xm, ym;
    int    i, j, k;

    if (!(nd % 2)) {
        nd = nd + 1;
    }
    k  = (nd - 1) / 2;
    yder.clear();
    xder.clear();
    yder.resize(n - 2 * k);
    xder.resize(n - 2 * k);
    for (i = k; i < n - k; i++) {
        sx  = 0.;
        sxy = 0.;
        xm  = 0.;
        ym  = .0;
        for (j = -k; j <= k; j++) {
            xm = xm + x.at(i + j);
            ym = ym + y.at(i + j);
        }
        xm = xm / (2. * k + 1.);
        ym = ym / (2. * k + 1.);
        for (j = -k; j <= k; j++) {
            sx  = sx + (x.at(i + j) - xm) * (x.at(i + j) - xm);
            sxy = sxy + (x.at(i + j) - xm) * (y.at(i + j) - ym);
        }
        yder[i - k] = sxy / sx;
        xder[i - k] = x.at(i);

    }

    return(n - 2 * k);
}
