#include <QDateTime>
#include <cmath>

#include "plotting.h"
#include "dbapi.h"
#include "plplot.h"
#include "plplot_utils.h"


Plotting::Plotting(QObject *parent) : QObject(parent) {
 image = nullptr;
  pls = nullptr;
  // fontfamily.clear();fontstyle.clear(); fontweight.clear();
  // fontfamily<<"sans-serif"<<"serif"<<"monospace"<<"script"<<"symbol";
  // fontstyle<<"upright"<<"italic"<<"oblique";
  // fontweight<<"medium"<<"bold";

}
Plotting::~Plotting(void) {

}
void Plotting::Plot_page(int xsize, int ysize, QString device, QString filename,
                         int r, int g, int b, PLFLT alpha) {
    if (!pls) {
         pls = new plstream();
    }

  pls->scolbga(r, g, b, alpha);
  if (device == "memqt") {
      if (!image){
          image = new QImage(xsize, ysize, QImage::Format_ARGB32);}
     image->fill( QColor( 0, 0, 0, (int) ( 0.0 * 255 ) ) );
   pls->smema(xsize, ysize, image->scanLine(0));
  } else {
    pls->sfnam(filename.toLatin1().constData());
  }

  if(device.contains("psc")){
      pls->spage(0., 0., xsize, ysize, 0, -10);
  } else pls->spage(0., 0., xsize, ysize, 0, 0);

  pls->sdev(device.toLatin1().constData());

  pls->fontld (0);
  pls->init();
  pls->adv(0);

}
void Plotting::setfont(int fontset){
     pls->font(fontset);//default 1 ,2 é roman"
}
void Plotting::setcharheight(double charheight,double fontsize ){
 pls->schr(charheight,fontsize);//character height
}
void Plotting::deleteStream() {
if (pls) delete pls;
pls=nullptr;
}
void Plotting::deleteImage() {
    if (image) delete image;
    image=nullptr;
}
void Plotting::SetInicialColormap()
{

    PLINT r[8]={221,0,12,25,238,57,242,255};
    PLINT g[8]={221,0,19,181,16,238,242,255};
    PLINT b[8]={221,0,221,22,46,232,75,255};
    PLFLT a[8]={1.,1.,1.,1.,1.,1.,1.,1.};
    Plplot_utils::setcolormap0(8,r,g,b,a,pls);

}

void  Plotting::Multiaxis(long long itime,
                         long long ltime, QVector <xydata * > *xy,plotproprieties *p )
{
    // multiaxis
    int rc,lc;
    int r,b,g;
    PLINT dgmax,dg;
    PLFLT xmax,xmin,ymin, ymax;
    int leftaxis=0; int rightaxis=0;
    for(int i=0; i<xy->count();i++){if(xy->at(i)->p.leftaxis) leftaxis++; else  rightaxis++;}
    pls->col0(1);
    if(leftaxis)p->xvminp=p->xvmin+p->multiaxissep*(leftaxis-1);else p->xvminp=p->xvmin;
    if(rightaxis)p->xvmaxp=p->xvmax-p->multiaxissep*(rightaxis-1);else p->xvmaxp=p->xvmax;
    p->yvminp=p->yvmin;  p->yvmaxp=p->yvmax;

    PLFLT p_def,p_ht;
    pls->gchr(p_def,p_ht);
    PLFLT p_xmin,p_xmax,p_ymin,p_ymax;
    pls->gspa(p_xmin,p_xmax,p_ymin,p_ymax);
    double bottom;
    bottom=5.1*p_ht/(p_ymax-p_ymin);
    p->yvminp=p->yvmin+bottom;


    pls->vpor(p->xvminp,p->xvmaxp,p->yvminp,p->yvmaxp);
    pls->lab("","",p->plottitle.toLatin1().constData());
    xmin=(PLFLT) itime;
    xmax=(PLFLT) ltime;


    for(int i=0; i<xy->count(); i++){
        if(xy->at(i)->p.minfreelimits){ xy->at(i)->FindYmin(xmin,xmax); ;}
        ymin=xy->at(i)->p.Ymin;
        if(xy->at(i)->p.maxfreelimits){ xy->at(i)->FindYmax(xmin,xmax);}
        ymax=xy->at(i)->p.Ymax;

        if(xy->at(i)->p.ylog){
            xy->at(i)->Findlog(1);
            if(xy->at(i)->p.minfreelimits){xy->at(i)->FindYlmin(xmin,xmax);}else xy->at(i)->p.Ylmin=std::max(log10(xy->at(i)->p.Ymin),-30.);
            ymin=xy->at(i)->p.Ylmin;
            if(xy->at(i)->p.maxfreelimits){xy->at(i)->FindYlmax(xmin,xmax);}else xy->at(i)->p.Ylmax=std::min(log10(xy->at(i)->p.Ymax),30.);
            ymax=xy->at(i)->p.Ylmax;
        }
        if((xy->at(i)->p.smooth ||xy->at(i)->p.onlysmooth) && !(xy->at(i)->p.ylog) ){
            xy->at(i)->evaluatesmooth(xy->at(i)->p.smoothpoints,xy->at(i)->p.smoothalpha);
            if(xy->at(i)->p.onlysmooth  && (xy->at(i)->p.minfreelimits ||xy->at(i)->p.maxfreelimits) ){
                xy->at(i)->FindYsminmax(xmin,xmax);
                if(xy->at(i)->p.minfreelimits)ymin=xy->at(i)->p.Ysmin;
                if(xy->at(i)->p.maxfreelimits)ymax=xy->at(i)->p.Ysmax;

            }
        }
        if(xy->at(i)->p.derivative){
            xy->at(i)->derivativedouble(xy->at(i)->p.devnpoints);
            if(xy->at(i)->p.minfreelimits){xy->at(i)->FindYdmin(xmin,xmax);}
            ymin=xy->at(i)->p.Ydmin;
            if(xy->at(i)->p.maxfreelimits){xy->at(i)->FindYdmax(xmin,xmax);}
            ymax=xy->at(i)->p.Ydmax;
        }
        if(!(xy->at(i)->p.ylog) &&(xy->at(i)->p.minfreelimits || xy->at(i)->p.minfreelimits )){
            double diff=ymax-ymin;
            if(diff >.1e-29){
            ymax=ymax+.1*diff;
            ymin=ymin-.1*diff;
            }else{
                ymin=ymin-5.;
                ymax=ymax+5.;
            }
        }

        pls->wind(xmin,xmax,ymin,ymax);
        if(i==0){
        savedxmin=xmin;
        savedxmax=xmax;
        savedymin=ymin;
        savedymax=ymax;
        }//acrecentei isto par guardar os valore da janela de traçar curvas para acrescentar coisas.
        if(i==0) grid(p);

        Plplot_utils::rgbfromhex(&r,&g,&b,xy->at(i)->p.Color.toLatin1().constData());
        pls->scol0(2,r,g,b);
        pls->col0(2);
        pls->lsty(xy->at(i)->p.curvestyle);

        int size=(int) xy->at(i)->x.size();

        if(xy->at(i)->p.onlysmooth){
           if(!xy->at(i)->p.ylog) plcurve(size, &xy->at(i)->x[0], &xy->at(i)->ysmooth[0],xmin,ymin, xy->at(i)->p.linetype,xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale);
            else pllogsmoth(size, &xy->at(i)->x[0], &xy->at(i)->logy[0],xmin,ymin,xy->at(i)->p.linetype,xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                       xy->at(i)->p.marker, xy->at(i)->p.markersscale,xy->at(i)->p.smoothpoints);
            //só vou considerar o caso log para quando não há smoothing
        }else{
            if(!xy->at(i)->p.ylog){
                if(!xy->at(i)->p.derivative) plcurve(size,
                        &xy->at(i)->x[0], &xy->at(i)->y[0],xmin,ymin, xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale);
                else plcurve((int) xy->at(i)->xder.size(),
                            &xy->at(i)->xder[0], &xy->at(i)->yder[0],xmin,ymin, xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                            xy->at(i)->p.marker, xy->at(i)->p.markersscale);
            }
            else plcurve(size, &xy->at(i)->x[0], &xy->at(i)->logy[0],xmin,ymin,xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale);
            if(xy->at(i)->p.smooth){if(!xy->at(i)->p.ylog)plcurve(size, &xy->at(i)->x[0], &xy->at(i)->ysmooth[0],xmin,ymin,"line",xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale);
                        else pllogsmoth(size, &xy->at(i)->x[0], &xy->at(i)->logy[0],xmin,ymin,"line",xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale,xy->at(i)->p.smoothpoints);

            } //uma vez mais não considero log nos smoothing e considero neste caso sempre type igual a 1.
            //não me parece bem quando a linha e o smotthing são ambos traçados o smothing vai ser sempre em linhas.


        }


    }
    pls->width(p->defaultpwith);
    pls->col0(1);
    pls->lsty(1);
    //   fprintf(stderr,"lixo aqui\n");

    if(leftaxis==0){
        pls->box("",0.,0.,"bn",0,0);
    }
    if(rightaxis==0){
        pls->box("",0.,0.,"cm",0,0);
    }//Tirar a escala de um dos lados quando não existem escalas desse lado.
    if(!p->withoutxaxis){
    SetAxisTime(ltime,itime,p->Xtimeaxis,""); //atenção esta funçao chama um plbox logo só pode ser chamada depois da plwind
    if(p->firsttimelabel)Putfirsttimelabel(4.,0.,.5,itime);
    }
 //   emit logmessages("estou aqui");
 //    emit logmessages(QString::number(p_def)+"   --   "+ QString::number(p_ht));
    //if(Data.yaxistype[0]& CloudDax_enums::rightaxis){rc=1;lc=0;}else{rc=0;lc=1; }
    // isto era usado no programa anterior. Não entendo como
 //   emit logmessages(QString::number(p_ymin)+"   --   "+ QString::number(p_ymax));


    rc=0; lc=0;

    for(int i=0; i<xy->count(); i++){
        Plplot_utils::rgbfromhex(&r,&g,&b,xy->at(i)->p.Color.toLatin1().constData());
        pls->scol0(2,r,g,b);
        pls->col0(2);
        if(xy->at(i)->p.rightaxis){
            pls->vpor(p->xvminp,p->xvmaxp+p->multiaxissep*rc,p->yvminp,p->yvmaxp);
            rc++;
        }else{
            pls->vpor(p->xvminp-p->multiaxissep*lc,p->xvmaxp,p->yvminp,p->yvmaxp);
            lc++;
        }
        if(xy->at(i)->p.minfreelimits){xy->at(i)->FindYmin(xmin,xmax);}ymin=xy->at(i)->p.Ymin;
    if(xy->at(i)->p.maxfreelimits){xy->at(i)->FindYmax(xmin,xmax);}ymax=xy->at(i)->p.Ymax;

        if(xy->at(i)->p.ylog){
            ymin=xy->at(i)->p.Ylmin;ymax=xy->at(i)->p.Ylmax;
        }
        if((xy->at(i)->p.smooth ||xy->at(i)->p.onlysmooth) && !(xy->at(i)->p.ylog)){
            if(xy->at(i)->p.onlysmooth  && (xy->at(i)->p.minfreelimits ||xy->at(i)->p.maxfreelimits)){
                if(xy->at(i)->p.minfreelimits){ xy->at(i)->FindYsmin(xmin,xmax); ymin=xy->at(i)->p.Ysmin;}
                if(xy->at(i)->p.maxfreelimits) {xy->at(i)->FindYsmax(xmin,xmax); ymax=xy->at(i)->p.Ysmax;}
            }
        }
        if(xy->at(i)->p.derivative){
            xy->at(i)->derivativedouble(xy->at(i)->p.devnpoints);
            if(xy->at(i)->p.minfreelimits){xy->at(i)->FindYdmin(xmin,xmax);}
            ymin=xy->at(i)->p.Ydmin;
            if(xy->at(i)->p.maxfreelimits){xy->at(i)->FindYdmax(xmin,xmax);}
            ymax=xy->at(i)->p.Ydmax;
        }
        if(!(xy->at(i)->p.ylog)&&(xy->at(i)->p.minfreelimits || xy->at(i)->p.minfreelimits )){
            double diff=ymax-ymin;
            if(diff >.1e-29){
                ymax=ymax+.1*diff;
                ymin=ymin-.1*diff;
            }else{
                ymin=ymin-5.;
                ymax=ymax+5.;
            }
        }        pls->wind(xmin,xmax,ymin,ymax);

        if(xy->at(i)->p.rightaxis){
            if(xy->at(i)->p.ylog){
                pls->box("",0.,0.,p->MRightaxislog.toLatin1().constData(),0,0);

                //algo que o antónio pediu. Pensar melhor para aparecer a dimensão quando o plto naão apresenta nenhum label
                PLFLT p_xmin,p_xmax,p_ymin,p_ymax;
                pls->gvpw (p_xmin, p_xmax, p_ymin, p_ymax);
                if(((p_ymax-p_ymin)/std::max(1.,p_ymin))<1.1){
                    pls->gyax(dg,dgmax);
                    QString lixo;
                    lixo.clear();
                    lixo=lixo.number(std::pow(10,p_ymin),'g',4);
                    pls->mtex("rv",dgmax,0.,.5,lixo.toLatin1());
                }
                // ************************************************************

            }else pls->box("",0.,0.,p->MRightaxis.toLatin1().constData(),0,0);
            pls->gyax(dg,dgmax);
            pls->mtex("r",dgmax+p->labelsep,.5,0.5,xy->at(i)->p.yaxisLabel.toLatin1());
        }else{
            if(xy->at(i)->p.ylog){
                pls->box("",0.,0.,p->MLeftaxislog.toLatin1().constData(),0,0);
                //algo que o antónio pediu. Pensar melhor para aparecer a dimensão quando o plto naão apresenta nenhum label
                PLFLT p_xmin,p_xmax,p_ymin,p_ymax;
                pls->gvpw (p_xmin, p_xmax, p_ymin, p_ymax);
                if(((p_ymax-p_ymin)/std::max(1.,p_ymin))<1.1){
                    pls->gyax(dg,dgmax);
                    QString lixo;
                    lixo.clear();
                    lixo=lixo.number(std::pow(10,p_ymin),'g',4);
                    pls->mtex("lv",dgmax,0.,0.5,lixo.toLatin1());

                }
                // ************************************************************

            }else pls->box("",0.,0.,p->MLeftaxis.toLatin1().constData(),0,0);
            pls->gyax(dg,dgmax);
            pls->mtex("l",dgmax+p->labelsep,.5,0.5,xy->at(i)->p.yaxisLabel.toLatin1());


        }


    }


}
void Plotting::grid(plotproprieties *p){

    if(p->styley){
        pls->lsty(p->gridlinestyle);
        pls->box("",0.,0.,"g",0,0);
    }
    if(p->stylex){
        pls->lsty(p->gridlinestyle);
        pls->box("g",0.,0.,"",0,0);
    }

}
void Plotting::plcurve(PLINT n, const PLFLT *x, const PLFLT *y, PLFLT xmax, PLFLT ymin, QString type, int width,  int marker, double smarker)
{

    // type=1 linha, =2 markers =3 vertical bars =4 horizontal bars (só tem significado em funções biunivocas.

    if(type=="line" || type=="lineandmarkers"){
        pls->width(width);
        pls->line(n, x, y);
    }
    if(type=="markers" || type=="lineandmarkers"){
        pls->ssym (1, smarker);
        pls->sym (n, x, y, Plplot_utils::getMarker(marker) ); //este simbolos ficam centrados enquanto usar string pode não acontecer

    }else if(type=="Vbars"){
        bars(n,x,y,1, xmax,ymin);
    }else if(type=="Hbars"){
        bars(n,x,y,-1, xmax,ymin);
    }


}
void Plotting::pllogsmoth(PLINT n, const PLFLT *x, const PLFLT *y, PLFLT xmax, PLFLT ymin, QString type, int width,  int marker, double smarker,int np)
{

    // type=1 linha, =2 markers =3 vertical bars =4 horizontal bars (só tem significado em funções biunivocas.
    std::vector<double> xp,yp;
    int count;
    double sum;
    xp.clear(); yp.clear();
    for (int i = 0; i < n; i++) {
        count=0;
        sum=0.0;
        for(int j=-np; j<np+1; j++)
        {
            if(i+j >-1 && i+j < n)
            {
               if(y[(i+j)]>ymin){
                sum=sum+ y[(i+j)];
                count++;
               }
        }
        //if(count>(double)1.2*np){
            if(count>(double)np/2.){
            xp.push_back(x[i]);
            yp.push_back(sum/( (double)count));
        } // else   {
          //  xp.push_back(x[i]);
          //  yp.push_back(ymin-10.);}
     }


    }
      int size=(int)xp.size();
    if(type=="line" || type=="lineandmarkers"){
        pls->width(width);
        pls->line(size, &xp[0], &yp[0]);
    }
    if(type=="markers" || type=="lineandmarkers"){
        pls->ssym (1, smarker);
        pls->sym (size, &xp[0], &yp[0], Plplot_utils::getMarker(marker) ); //este simbolos ficam centrados enquanto usar string pode não acontecer

    }else if(type=="Vbars"){
        bars(size, &xp[0], &yp[0],1, xmax,ymin);
    }else if(type=="Hbars"){
        bars(size, &xp[0], &yp[0],-1, xmax,ymin);
    }


}
void Plotting::Putfirsttimelabel(PLFLT disp,PLFLT pos,PLFLT just,long long itime)

{
    QDateTime iitime;
    iitime.setTimeSpec(Qt::UTC);
    iitime.setSecsSinceEpoch(itime);

    pls->mtex("b",disp,pos,just,iitime.toString("yyyy/MM/dd hh:mm").toLatin1().constData());

}
void Plotting::bars(PLINT n, const PLFLT *xx,  const PLFLT *yy,int type,PLFLT xmin,PLFLT ymin)
{
    //type=1 vertical bars. type=-1 horizontal bars
    PLFLT x[4], y[4];

    if(type==1){
        for(int i=0; i<n;i++){
            if(i>0) x[0]= xx[i]-(xx[i]-xx[i-1])/2.; else  x[0]= xx[i];
            x[1]=x[0];
            if(i<n-1) x[2]=xx[i]+(xx[i+1]-xx[i])/2.; else  x[2]= xx[i];
            x[3]=x[2];
            y[0]=ymin;
            y[1]=yy[i];
            y[2]=y[1];
            y[3]=y[0];
            pls->line(4, x, y);
        }

    }
    if(type==-1){
        for(int i=0; i<n;i++){
            x[0]=xmin;
            x[1]=x[0];
            x[2]=xx[i];
            x[3]=x[2];
            if(i>0) y[0]= yy[i]-(yy[i]-yy[i-1])/2.; else  y[0]= yy[i];
            y[3]=y[0];
            if(i<n-1) y[2]=yy[i]+(yy[i+1]-yy[i])/2.; else  y[2]= yy[i];
            y[1]=y[2];
            pls->line(4, x, y);
        }


    }
}
void  Plotting::SetAxisTime(long long ltime,long long  itime,QString xaxis,QString yaxis)
{
    long long period=ltime-itime;
    QString timeformat,timeunits;
    long ticks,nticks;
    timeformat.clear();
    ticks=0;nticks=0;
    if(period<60){
        timeformat.clear();
        timeformat.append("%S");
        ticks=5.;
        nticks=5;
        timeunits="(seg)";
    }else if(period<60*60){
        int k;
        k=period/60;
        if(k<10){
            timeformat.clear();
            timeformat.append("%M");
            ticks=60.;
            nticks=6;
            timeunits="(min)";
        }else if(k<61){
            timeformat.clear();
            timeformat.append("%M");
            ticks=5*60.;
            nticks=5;
            timeunits="(min)";
        }
    }else if(period<60*60*5){
        int k;
        timeformat.clear();
        timeformat.append("%M");
        k=period/(60*60);
        if(k<2){
            timeformat.clear();
            timeformat.append("%k:%M");
            ticks=15*60.;
            nticks=3;
            timeunits="(hour:min)";
        }else if(k<6){
            timeformat.clear();
            timeformat.append("%k:%M");
            ticks=30.*60;
            nticks=2;
            timeunits="(hour:min)";
        }
    }else if(period<60*60*24){
        timeformat.clear();
        timeformat.append("%k");
        ticks=60.*60*2;
        nticks=4;
        timeunits="(hour)";
    }else if(period<60*60*24*30){
        int k;
        timeformat.clear();
        timeformat.append("%d");
        k=period/(60*60*24);
        ticks=60.*60.*24;
        if(k<2){
            ticks=60.*60.*6;
            nticks=2;
            timeformat.clear();
            timeformat.append("%k/%d");
            timeunits="(hour/day)";
        }else if(k<5){
            ticks=60.*60.*12;
            nticks=2;
            timeformat.clear();
            timeformat.append("%k/%d");
            timeunits="(hour/day)";
        }else if(k<10){
            ticks=60.*60.*24;
            nticks=2;
            timeunits="(day)";
        }else if(k<20){
            ticks=60.*60.*24*2;
            nticks=2;
            timeunits="(day)";
        }else if(k<30){
            ticks=60.*60.*24*3;
            nticks=3;
            timeunits="(day)";
            timeformat.clear();
        }
    }else{
        timeformat.clear();
        timeformat.append("%m/%d");
        ticks=0;
        nticks=0;
        timeunits="(month/day)";
    }
    pls->timefmt(timeformat.toLatin1().constData());
    pls->box(xaxis.toLatin1().constData(),ticks,nticks,yaxis.toLatin1().constData(),0,0);
    pls->mtex("b",4.,.5,.5,timeunits.toLatin1().constData());

}


void Plotting::Tile2d(int ylog, int clog, double xmin, double xmax, xyzdata *xyz)
{
    PLFLT x[4],y[4];
    unsigned long rows=xyz->y.size();  unsigned long columns=xyz->x.size();

/*
    double xdist=0;
    for(unsigned long j=0;j<rows-1;j++){
        xdist=xdist+xyz->x.at(j+1)-xyz->x.at(j);
    }

    xdist=xdist/rows;
*/
    double xdist=xyz->p.xdist;
    xdist=0;
    double cmin,cmax,csize;
    if(xyz->p.ylog)xyz->FindYlminmax();

    if(xyz->p.minfreelimits ){
        if(clog){
            xyz->FindlogZ(xyz->p.zlogmin);
            xyz->FindZlmin(xmin,xmax);
            cmin=xyz->p.Zlmin;

        }else{
            xyz->FindZmin(xmin,xmax);
            cmin=xyz->p.Zmin;
        }
    }else{
        if(clog){
            xyz->FindlogZ(xyz->p.zlogmin);
            cmin=log10(std::max(xyz->p.Zmin,xyz->p.zlogmin)) ;

        }else{
            cmin=xyz->p.Zmin;
        }
    }
    if(xyz->p.maxfreelimits ){
        if(clog){
            xyz->FindlogZ(xyz->p.zlogmin);
            xyz->FindZlmax(xmin,xmax);
            cmax=xyz->p.Zlmax;

        }else{
            xyz->FindZmax(xmin,xmax);
            cmax=xyz->p.Zmax;
        }
    }else{
        if(clog){
            xyz->FindlogZ(xyz->p.zlogmin);
            cmax=log10(std::max(xyz->p.Zmax,xyz->p.zlogmin)) ;

        }else{
            cmax=xyz->p.Zmax;
        }
    }


    for(unsigned long j=0;j<columns;j++){
        for(unsigned long i=0;i<rows;i++){
            if(j==0){
                x[0]=std::max(xyz->x.at(j)-xdist,xmin);
            }else{
               // x[0]=std::min(xyz->x.at(j)-xdist,(xyz->x.at(j)+xyz->x.at(j-1))/2.);
                x[0]=(xyz->x.at(j)+xyz->x.at(j-1))/2.;
            }
            if(j==columns-1){
                x[1]=std::min(xyz->x.at(j)+xdist,xmax);
            }else{
             //   x[1]=std::min(xyz->x.at(j)+xdist,(xyz->x.at(j+1)+xyz->x.at(j))/2.);
                x[1]=(xyz->x.at(j+1)+xyz->x.at(j))/2.;
            }
            x[2]=x[1];
            x[3]=x[0];
            if(i==0){
                if(!ylog){
                    y[0]=xyz->y.at(i);
                }else{
                    y[0]=xyz->logy.at(i);
                }
            }else{
                if(!ylog){
                    y[0]=(xyz->y.at(i)+xyz->y.at(i-1))/2.;
                }else{
                    y[0]=(xyz->logy.at(i)+xyz->logy.at(i-1))/2.;
                }
            }
            y[1]=y[0];
            if(i==rows-1){
                if(!ylog){
                    y[2]=xyz->y.at(i);
                }else{
                    y[2]=xyz->logy.at(i);
                }
            }else{
                if(!ylog){
                    y[2]=(xyz->y.at(i+1)+xyz->y.at(i))/2.;
                }else{
                    y[2]=(xyz->logy.at(i+1)+xyz->logy.at(i))/2.;
                }
            }
            y[3]=y[2];

            csize=cmax-cmin;
            if(csize==0)csize=.0000001;
            if(!clog){
                if(xyz->z.at(i).at(j)<cmin){if(xyz->p.colorlimits=="Bizarreway")pls->col1(0);else  pls->col0(0);}
                else if(xyz->z.at(i).at(j)>cmax){if(xyz->p.colorlimits=="Bizarreway") pls->col1(.9999999999999);else pls->col0(7);}
                else pls->col1((xyz->z.at(i).at(j)-cmin)/(1.000001*csize));
            }else{
                if(xyz->logz.at(i).at(j)<cmin){if(xyz->p.colorlimits=="Bizarreway")pls->col1(0);else  pls->col0(0);}
                else if(xyz->logz.at(i).at(j)>cmax){if(xyz->p.colorlimits=="Bizarreway")pls->col1(.9999999999999);else  pls->col0(7);}
                else pls->col1((xyz->logz.at(i).at(j)-cmin)/(1.000001*csize));

            }

            pls->fill(4,x,y);
        }
    }
    pls->col0(1);

}
void  Plotting::SurfacePlot( long long itime, long long ltime,xyzdata *xyz,plotproprieties *p, bool timeplot)
{


    p->xvminp=p->xvmin;
    p->yvminp=p->yvmin;  p->yvmaxp=p->yvmax;

    PLFLT p_def,p_ht;
    pls->gchr(p_def,p_ht);
    PLFLT p_xmin,p_xmax,p_ymin,p_ymax;
    pls->gspa(p_xmin,p_xmax,p_ymin,p_ymax);
    double bottom;
    bottom=5.1*p_ht/(p_ymax-p_ymin);
    p->yvminp=p->yvmin+bottom;

    if(xyz->p.colorscale=="rightcolorscale"){
        p->xvmaxp=p->xvmax-xyz->p.dcleg;
    } else p->xvmaxp=p->xvmax;


    pls->vpor(p->xvminp,p->xvmaxp,p->yvminp,p->yvmaxp);
    PLFLT xmax,xmin;
    PLINT dgmax,dg;
    int ylog=1;
    xmin=(PLFLT) itime;
    xmax=(PLFLT) ltime;



            if(xyz->p.ylog){
                xyz->Findlog(1);
                xyz->FindYlminmax();
                ylog=1;
                pls->wind(xmin,xmax,xyz->p.Ylmin,xyz->p.Ylmax);
            } else{
                xyz->FindYminmax();
                pls->wind(xmin,xmax,xyz->p.Ymin,xyz->p.Ymax);
                ylog=0;
            }



        //        QString Surfacecolor="rainbow_cloud#300";
        QStringList colors;
        int ncol;
        colors=xyz->p.Surfacecolor.split("#");
        if(colors[0].contains("rainbow_cloud")) ncol=270; else ncol=colors.at(1).toInt();

        Plplot_utils::Setcolormap(colors.at(0),colors.at(1).toInt(),pls); //não pode ser ncol porque no colormap o rainbow_cloud tem 300
        /*
         Due to the nature of data whenever one chooses log scale for surface the minimum value will be set to zero.
         Otherwise, as many present negative data or even zero values when there is no data using as minimum a smaller value,
         eg 1e-31 that is used for y log scale could lead to aditional 32 orders of meningless magnitude scale.
         */
        int clog;
        if(xyz->p.zlog) clog=1; else clog=0;

        if(xyz->p.surfacetype=="tiled2d") Tile2d(ylog,clog,xmin,xmax,xyz);
        else if(xyz->p.surfacetype=="contour")CountourShading(ylog,clog,xmin,xmax,ncol,xyz);


        pls->col0(1);
        if(p->withoutxaxis){
            if(xyz->p.ylog){
                pls->box("",0.,0.,p->MRightaxislog.toLatin1().constData(),0,0);
                pls->box("",0.,0.,p->MLeftaxislog.toLatin1().constData(),0,0);
            }else
                {
                    pls->box("",0.,0.,p->MRightaxis.toLatin1().constData(),0,0);
                    pls->box("",0.,0.,p->MLeftaxis.toLatin1().constData(),0,0);

                }
        }else{

        if(timeplot){
            if(!ylog) SetAxisTime(ltime,itime,p->Xtimeaxis,p->Ytimeaxis);
            else SetAxisTime(ltime,itime,p->Xtimeaxis,p->Ytimeaxislog);
            Putfirsttimelabel(4.,0.,.5,itime);
        }
        }
        grid(p);

        pls->lab("","",p->plottitle.toLatin1().constData());
        pls->gyax(dg,dgmax);
        pls->mtex("l",dgmax+xyz->p.celllabsep,.5,0.5,xyz->p.yaxisLabel.toLatin1().constData());
        QStringList legin;
        legin=xyz->p.Surfacelegendinside.split(";");
        double zmax,zmin;
        zmin=1.;zmax=10.;
        if(!(xyz->p.colorscale=="nocolorscale"))
        {

            if(xyz->p.minfreelimits ){
                if(clog){
                    xyz->FindlogZ(xyz->p.zlogmin);
                    xyz->FindZlmin(xmin,xmax);
                    zmin=xyz->p.Zlmin;

                }else{
                    xyz->FindZmin(xmin,xmax);
                    zmin=xyz->p.Zmin;
                }
            }else{
                if(clog){
                    xyz->FindlogZ(xyz->p.zlogmin);
                    zmin=log10(std::max(xyz->p.Zmin,xyz->p.zlogmin)) ;

                }else{
                    zmin=xyz->p.Zmin;
                }
            }
            if(xyz->p.maxfreelimits ){
                if(clog){
                    xyz->FindlogZ(xyz->p.zlogmin);
                    xyz->FindZlmax(xmin,xmax);
                    zmax=xyz->p.Zlmax;

                }else{
                    xyz->FindZmax(xmin,xmax);
                    zmax=xyz->p.Zmax;
                }
            }else{
                if(clog){
                    xyz->FindlogZ(xyz->p.zlogmin);
                    zmax=log10(std::max(xyz->p.Zmax,xyz->p.zlogmin)) ;

                }else{
                    zmax=xyz->p.Zmax;
                }
            }



        }
        if(xyz->p.colorscale=="rightcolorscale"){
            colorlegend(p->xvmaxp+xyz->p.dxvmin,p->xvmaxp+xyz->p.dcleg-xyz->p.dxvmax,p->yvminp,p->yvmaxp,zmin,zmax,clog,ncol,legin,1);
            pls->wind(0., 1., zmin, zmax);
        } else if (xyz->p.colorscale=="insidecolorscale"){
            colorlegend(p->xvmin+legin.at(0).toDouble()*(p->xvmax -p->xvmin),p->xvmax,
                        p->yvmax-legin.at(1).toDouble(),p->yvmax,zmin,zmax,clog, ncol,legin,2);
        }






}
void Plotting::CountourShading( int ylog, int clog, double xmin, double xmax,int ns,xyzdata *xyz)
{


    /*int rows=Data.xyz[k].nsize[0];int columns=Data.xyz[k].nsize[1];
 *
 * Esta routina dava resultados absurdios quando o ovector xyz tinha mais dados no incio do que aqueles que ias ser usados
 * Nunca acontecia com os plots da base de dados mas sim com os plots de tabelas em que os subplots mantém os dados completos.
 */
 /*
    int  columns= xyz->y.size(); //nas columns não há subsampling
    int rows,firstrow,lastrow;
    firstrow=(int) xyz->x.size()-1;
    lastrow=0;
    for(int i=0;i<(int) xyz->x.size(); i++){
        if(xyz->x.at(i) >=xmin) firstrow=std::min(firstrow,i);
        if(xyz->x.at(i) <=xmax) lastrow=std::max(lastrow,i);
    }
    rows=lastrow-firstrow+1;
  */
    int  columns= xyz->x.size();
    int rows= xyz->y.size();
    int firstrow=0;
    PLFLT      **c;
    PLcGrid2  cgrid2;
    PLFLT    *shedge;
    shedge = (PLFLT *) calloc( (size_t) ( ns + 1 ), sizeof ( PLFLT ) );
    double ymin,ymax;


    if(ylog){
        ymin=xyz->p.Ylmin; //mantenho contudos os limites dos dados globais
        ymax=xyz->p.Ylmax;

    }else{
        ymin=xyz->p.Ymin;
        ymax=xyz->p.Ymax;
    }

    pls->Alloc2dGrid( &c,  columns,rows );
    pls->Alloc2dGrid( &cgrid2.xg, columns,rows);
    pls->Alloc2dGrid( &cgrid2.yg,columns,rows );
    cgrid2.nx = columns;
    cgrid2.ny = rows;

    double zmax,zmin;

    if(xyz->p.minfreelimits ){
        if(clog){
            xyz->FindlogZ(xyz->p.zlogmin);
            xyz->FindZlmin(xmin,xmax);
            zmin=xyz->p.Zlmin;

        }else{
            xyz->FindZmin(xmin,xmax);
            zmin=xyz->p.Zmin;
        }
    }else{
        if(clog){
            xyz->FindlogZ(xyz->p.zlogmin);
            zmin=log10(std::max(xyz->p.Zmin,xyz->p.zlogmin)) ;

        }else{
            zmin=xyz->p.Zmin;
        }
    }
    if(xyz->p.maxfreelimits ){
        if(clog){
            xyz->FindlogZ(xyz->p.zlogmin);
            xyz->FindZlmax(xmin,xmax);
            zmax=xyz->p.Zlmax;

        }else{
            xyz->FindZmax(xmin,xmax);
            zmax=xyz->p.Zmax;
        }
    }else{
        if(clog){
            xyz->FindlogZ(xyz->p.zlogmin);
            zmax=log10(std::max(xyz->p.Zmax,xyz->p.zlogmin)) ;

        }else{
            zmax=xyz->p.Zmax;
        }
    }

    for(int  j=0;j<rows;j++){
        for(int i=0;i<columns;i++){
            if(j+firstrow <0 ||j+firstrow>rows-1 ) continue;
            if(clog)c[i][j]=xyz->logz.at(j+firstrow).at(i);
            else c[i][j]=xyz->z.at(j+firstrow).at(i);
            cgrid2.xg[i][j]=xyz->x.at(i+firstrow);
            if(ylog) cgrid2.yg[i][j]=xyz->logy.at(j);else cgrid2.yg[i][j]=xyz->y.at(j);
        }}

    for ( int i = 0; i < ns + 1; i++ )
        shedge[i] = zmin + ( zmax - zmin ) * (PLFLT) i / (PLFLT) ns;

    // plpsty (0);

    pls->shades ((const PLFLT**)c, columns,rows , NULL , xmin, xmax, ymin, ymax,shedge , ns+1, 2, 0, 0, plcallback::fill, false, plcallback::tr2, (void *) &cgrid2 );

    pls->Free2dGrid( c,  columns, rows );
    pls->Free2dGrid( cgrid2.xg,  columns, rows );
    pls->Free2dGrid( cgrid2.yg,  columns, rows);
    free( (void *) shedge );

}
void  Plotting::colorlegend(double xi,double xl,double yi,double yl, double cmin,double cmax,int clog, int ncol, QStringList legin, int legtype)
{

    PLFLT x[4],y[4];
    pls->vpor(xi,xl,yi,yl);
    if(legtype==2) {
        pls->wind(0.,1.,0.,1.);
        pls->col0(0);
        y[0]=0.;
        y[1]=1.;
        y[3]=y[0];
        y[2]=y[1];
        x[0]=0.;
        x[2]=1.;
        x[1]=x[0];
        x[3]=x[2];
        pls->fill(4, x, y);

    }

    if(legtype==2) {
        pls->vpor(xi+(xl-xi)/15.,xl-(xl-xi)/15.,yi+.7*(yl-yi),yl-(yl-yi)/5.);
        pls->wind(cmin,cmax,0.,1. );
        y[0]=0.;
        y[1]=1.;
        y[3]=y[0];
        y[2]=y[1];
        for (int k = 0; k < ncol; k++) {
            x[0] = cmin + k * (cmax-cmin) / (ncol);
            x[1] = x[0];
            x[2] =  cmin +(k + 1) * (cmax-cmin) / (ncol);
            x[3] = x[2];
            pls->col1((k / (float)ncol) + 1. / (2. * (ncol)));
            pls->fill(4, x, y);
        }

        pls->col0(1);
        if(!clog) pls->box("bcntv", 0., 0, "bcv", 0., 0);else
            pls->box("bcnstvl", 0., cmax / ncol, "bcv",0, 0);

    }else{
        pls->wind(0., 1.,cmin,cmax );
        x[0] = 0.;
        x[1] = 1.;
        x[2] = 1.;
        x[3] = 0.;
        for (int k = 0; k < ncol; k++) {
            y[0] = cmin + k * (cmax-cmin) / (ncol);
            y[1] = y[0];
            y[2] = cmin +(k + 1) * (cmax-cmin) / (ncol);
            y[3] = y[2];
            pls->col1((k / (float)ncol) + 1. / (2. * (ncol)));
            pls->fill(4, x, y);
        }
        pls->col0(1);
        if(!clog) pls->box("bcv", 0., 0, "bcmtv", 0., 0); else
            pls->box("bcv", 0., 0, "bcmstvl", cmax / ncol, 0);
    }

}
void Plotting::HistoPlot(QVector <histodata * > *histo,plotproprieties *p)
{
    double  ymax;
    int nbins=0;
    int maxnbins=0;
    int nofitens=0;
    //   bool timeinterval=true;
    ymax = -10.e33;
    for (int k = 0; k < histo->count();k++){
        int nsize=(int) histo->at(k)->x.size();
        histo->at(k)->FindYminmax();
        ymax=std::max(histo->at(k)->p.Ymax, ymax);
        nbins=nbins+nsize;
        maxnbins=std::max(maxnbins,nsize);
        nofitens++;
    }
    p->xvminp=p->xvmin;  p->xvmaxp=p->xvmax;
    p->yvminp=p->yvmin;  p->yvmaxp=p->yvmax;
    pls->vpor(p->xvminp,p->xvmaxp,p->yvminp,p->yvmaxp);
    pls->wind(0.,1.,0.,1.1*ymax); //histograms a começarem todos do 0
    if(histo->at(0)->p.htype!=10){
    int size;int r,b,g;int agre;int nb;
    nb=0;
    if(histo->at(0)->p.htype==0){ size=nbins; agre=0;} else {size=maxnbins; agre=nofitens; }
    PLINT ni=0;
    for (int k = 0; k < histo->count();k++) {
        Plplot_utils::rgbfromhex(&r,&g,&b,histo->at(k)->p.histocolor.toLatin1().constData());
        pls->scol0(2,r,g,b);
        pls->col0(2);
        if(histo->at(0)->p.htype) nb=0;
        int nsize=(int) histo->at(k)->x.size();
        for (int i = 0; i <std::min(nsize,size); i++) {
            if(histo->at(0)->p.htype!=2) plfbox((1. / (size + 1)) * (nb + 1), histo->at(k)->y.at(i), .7, size,ni,agre, 1, 2);
            else{
                if(i< std::min(nsize,size)-1){
                    pls->col0(2);
                    PLFLT x1,y1,x2,y2;
                    x1=(1. / (size + 1)) * (nb + 1);
                    x2=(1. / (size + 1)) * (nb + 2);
                    y1=histo->at(k)->y.at(i);
                    y2=histo->at(k)->y.at(i+1);
                    pls->join(x1,y1,x2,y2);
                }
            }

            if(agre==0 || ni==0)  pls->mtex("b", 2., (1. / (size + 1)) * (nb + 1), .5, histo->at(k)->x.at(i).toLatin1().constData());
            //Só põe a legenda dos  bins do primeiro histo sempre que houver agregação
            nb++;
        }
        ni++;
    }
    if(histo->at(0)->p.period.count()){

        pls->mtex("t", 5., .5, .5, p->plottitle.toLatin1().constData());
        pls->mtex("t", 2.5, .5, .5, histo->at(0)->p.period.toLatin1().constData());
    } else{pls->mtex("t", 2.5, .5, .5,p->plottitle.toLatin1().constData());}

    PLINT dgmax, dg;
    pls->gyax(dg, dgmax);
    ni=0;
    for(int k=0;k<histo->count();k++)
    {
        if(!ni)pls->mtex("l", dgmax + 3.5, .5, 0.5,histo->at(k)->p.ylabel.toLatin1() ); //optei por pôr apenas o yaxis label do primeiro histo. Deveriam ser sempre iguais
        if(!agre) pls->mtex("b", 4.5, (1. / (2. * nofitens)) + ni * (1. / ((float)nofitens)), .5, histo->at(k)->p.xlabel.toLatin1());
        ni++;

    }

    pls->col0(1);


    pls->box("bc", 1.0, 0, "bcnstv", .0, 0);
    }else{

    }

}
void Plotting::plfbox(PLFLT x0, PLFLT y0, float xinc, PLINT nbin, PLINT i, PLINT agbins, PLINT ncoll, PLINT ncolf)
{
    PLFLT x[4], y[4];
    double Dx;
    Dx= (0.5 / nbin) * xinc;

    if(!agbins)x[0] = x0 - (0.5 / nbin) * xinc; else x[0] = x0 - Dx+2*i*Dx/agbins;
    y[0] = 0.;
    x[1] = x[0];
    y[1] = y0;
    if(!agbins) x[2] = x0 + (0.5 / nbin) * xinc;else x[2] = x0 - Dx+2*(i+1)*Dx/agbins;
    y[2] = y[1];
    x[3] = x[2];
    y[3] = 0.;
    pls->col0(ncolf);
    pls->fill(4, x, y);
    pls->col0(ncoll);
    pls->lsty(1);
    pls->line(4, x, y);
}
QString  Plotting::getstartruns(long long timei,long long timef,QString databaseconnection)
{

    dbapi table;
    connect(&table, &dbapi::logmessages, this,
            &Plotting::LogMessages);
    connect(&table, &dbapi::logmessagesR, this,
            &Plotting::LogMessages);

    table.connectionName=databaseconnection;
    QStringList cnames;
    QList<std::vector<QString> * >  runrows;

    //

    cnames.clear();


    cnames.clear();
    cnames <<"RunNumber" <<"StartTime";
    QString rownames;

    for(int i=0; i<cnames.count(); i++) runrows << new ( std::vector<QString>);
    //    table.fechnotimecolumns_string("Runtable",cnames,&runrows);
    table.fechcolumns_string_orderby("Runtable",cnames,timei,timef,"StartTime",&runrows);
    QString StartingRuns;
    StartingRuns.clear();
    for(int i=0;i<(int)runrows.at(0)->size(); i++){
        rownames=runrows.at(0)->at(i);
        StartingRuns.append(QString::number(QDateTime::fromMSecsSinceEpoch(runrows.at(1)->at(i).toLongLong()).toUTC().toSecsSinceEpoch()));
        StartingRuns.append(":");
        StartingRuns.append(rownames);
        if(i<(int) runrows.at(0)->size()-1)StartingRuns.append(";");

    }
    for(int i=0; i<(int)runrows.count(); i++) delete runrows.at(i);
    return (StartingRuns);

}
void  Plotting::putrunlabels( long long itime, long long ltime, plotproprieties *p,QStringList text)
{

    PLFLT xmax,xmin;
    xmin=(PLFLT) itime;
    xmax=(PLFLT) ltime;
    int type=p->runstart;
    pls->col0(1);

    pls->vpor(p->xvminp,p->xvmaxp,p->yvminp,p->yvmaxp);
    pls->wind(xmin,xmax,0.,1.);

    PLFLT x,y,dx,dy,p_xmin, p_xmax, p_ymin, p_ymax;
    pls->gvpw (p_xmin, p_xmax, p_ymin, p_ymax);
    QString Lixo;

    QStringList runs;

    for(int k=0;k<text.count();k++)
    {
        runs=text.at(k).split(":");
        x=runs.at(0).toDouble();
        Lixo.clear();

            if((x<xmin+.01*(xmax-xmin) || x >xmax-.01*(xmax-xmin)) && type==1 ) continue;

        //    if(!screen) { Lixo.append("----- ");
        //        Lixo.append("----- ");
        //        Lixo.append("----- ");
        //        Lixo.append("----- ");
        //        Lixo.append("----- ");}
            Lixo.append("----- ");
            Lixo.append("----- ");
            Lixo.append(runs.at(1));
            Lixo.append("----- ");
;
        y=p_ymin+ p->runy*(p_ymax-p_ymin); // entre 0 e 1 dentro do viewport
        dx=std::cos(p->runangle);
        dy=std::sin(p->runangle);
        pls->ptex(x,y,dx,dy,0.5,Lixo.toLatin1().constData());

    }
}
void Plotting::LogMessages(QString mess){
    emit logmessages(mess);
}
void  Plotting::Twoaxis(long long itime, long long ltime,   QVector <xydata * > *xy ,plotproprieties *p)
{

    p->xvminp=p->xvmin;  p->xvmaxp=p->xvmax;
    p->yvminp=p->yvmin;  p->yvmaxp=p->yvmax;
    PLFLT p_def,p_ht;
    pls->gchr(p_def,p_ht);
    PLFLT p_xmin,p_xmax,p_ymin,p_ymax;
    pls->gspa(p_xmin,p_xmax,p_ymin,p_ymax);
    double bottom;
    bottom=5.1*p_ht/(p_ymax-p_ymin);
    p->yvminp=p->yvmin+bottom;

    pls->vpor(p->xvminp,p->xvmaxp,p->yvminp,p->yvmaxp);
    PLFLT xmax,xmin;
    PLINT dg,dgmax;
    double ymini,ymaxi;

    xmin=(PLFLT) itime;
    xmax=(PLFLT) ltime;

    double yminl=+10.e33;
    double ymaxl=-10.e33;
    double yminr=+10.e33;
    double ymaxr=-10.e33;
    bool logr=true;
    bool loge=true;
    QStringList ylimits;
    QStringList lAxisLimits,rAxisLimits;
    bool lgloballimits=true;
    bool rgloballimits=true;

    bool ok;

    int leftaxis,rightaxis;
    leftaxis=0;rightaxis=0;
    if(p->leftaxislimits.count()){
        lAxisLimits=p->leftaxislimits.split(";");
        if(lAxisLimits.count()==3){
            ylimits=lAxisLimits.at(1).split(":");
            if( ylimits.count()==2){
                yminl=ylimits.at(0).toDouble(&ok);
                if(!ok)  lgloballimits=false;
                ymaxl=ylimits.at(1).toDouble(&ok);
                if(!ok)  lgloballimits=false;
            }else lgloballimits=false;
            if(lAxisLimits.at(2).contains("log"))loge=true; else loge=false;
        }else lgloballimits=false;
    }else lgloballimits=false;

    if(p->rightaxislimits.count()){
        rAxisLimits=p->rightaxislimits.split(";");
        if(rAxisLimits.count()==3){
            ylimits=rAxisLimits.at(1).split(":");
            if( ylimits.count()==2){
                yminr=ylimits.at(0).toDouble(&ok);
                if(!ok)  lgloballimits=false;
                ymaxr=ylimits.at(1).toDouble(&ok);
                if(!ok)  rgloballimits=false;
            }else rgloballimits=false;
            if(rAxisLimits.at(2).contains("log"))logr=true; else logr=false;
        }else rgloballimits=false;
    }else rgloballimits=false;


    for(int i=0;i<xy->count();i++)
    {



        if(!rgloballimits){
            if( !(xy->at(i)->p.ylog)&& (xy->at(i)->p.rightaxis)){
                logr=false;
            }
        }
        if(!lgloballimits){
            if( !(xy->at(i)->p.ylog)&& !(xy->at(i)->p.rightaxis)){
                loge=false;
            }
        }

    }

    for(int i=0;i<xy->count();i++)
    {
        if(!(xy->at(i)->p.rightaxis)){
            leftaxis++;
            if(!lgloballimits){
                if(!loge){

                    if(xy->at(i)->p.minfreelimits ){xy->at(i)->FindYmin(xmin,xmax);}
                    ymini=xy->at(i)->p.Ymin;
                    if(xy->at(i)->p.maxfreelimits ){xy->at(i)->FindYmax(xmin,xmax);}
                    ymaxi=xy->at(i)->p.Ymax;
                    if((xy->at(i)->p.smooth) ||(xy->at(i)->p.onlysmooth) ){
                        xy->at(i)->evaluatesmooth(xy->at(i)->p.smoothpoints,xy->at(i)->p.smoothalpha);
                        if(xy->at(i)->p.onlysmooth &&(xy->at(i)->p.minfreelimits ||xy->at(i)->p.maxfreelimits) ){

                            if(xy->at(i)->p.minfreelimits ){ xy->at(i)->FindYsmin(xmin,xmax); ymini=xy->at(i)->p.Ysmin;}
                            if(xy->at(i)->p.maxfreelimits ) { xy->at(i)->FindYsmax(xmin,xmax); ymaxi=xy->at(i)->p.Ysmax;}
                        }
                    }
                    if(xy->at(i)->p.derivative){
                        xy->at(i)->derivativedouble(xy->at(i)->p.devnpoints);
                        if(xy->at(i)->p.minfreelimits){xy->at(i)->FindYdmin(xmin,xmax);}
                        ymini=xy->at(i)->p.Ydmin;
                        if(xy->at(i)->p.maxfreelimits){xy->at(i)->FindYdmax(xmin,xmax);}
                        ymaxi=xy->at(i)->p.Ydmax;
                    }

                } else{
                    xy->at(i)->Findlog(1);

                    if(xy->at(i)->p.minfreelimits ){xy->at(i)->FindYlmin(xmin,xmax);}else xy->at(i)->p.Ylmin=std::max(log10(xy->at(i)->p.Ymin),-30.);
                    ymini=xy->at(i)->p.Ylmin;
                    if(xy->at(i)->p.maxfreelimits ){xy->at(i)->FindYlmax(xmin,xmax);}else xy->at(i)->p.Ylmax=std::min(log10(xy->at(i)->p.Ymax),30.);
                    ymaxi=xy->at(i)->p.Ylmax;



                }


                yminl=std::min(yminl,ymini);
                ymaxl=std::max(ymaxl,ymaxi);

            }


        }else{
            rightaxis++;
            if(!rgloballimits){
                if(!logr){


                    if(xy->at(i)->p.minfreelimits ){xy->at(i)->FindYmin(xmin,xmax);}
                    ymini=xy->at(i)->p.Ymin;
                    if(xy->at(i)->p.maxfreelimits ){xy->at(i)->FindYmax(xmin,xmax);}
                    ymaxi=xy->at(i)->p.Ymax;
                    if((xy->at(i)->p.smooth) ||(xy->at(i)->p.onlysmooth) ){
                        xy->at(i)->evaluatesmooth(xy->at(i)->p.smoothpoints,xy->at(i)->p.smoothalpha);
                        if(xy->at(i)->p.onlysmooth  && (xy->at(i)->p.minfreelimits ||xy->at(i)->p.maxfreelimits)){
                            if(xy->at(i)->p.minfreelimits ) {xy->at(i)->FindYsmin(xmin,xmax);ymini=xy->at(i)->p.Ysmin;}
                            if(xy->at(i)->p.maxfreelimits ) {xy->at(i)->FindYsmax(xmin,xmax);ymaxi=xy->at(i)->p.Ysmax;}
                        }
                    }
                    if(xy->at(i)->p.derivative){
                        xy->at(i)->derivativedouble(xy->at(i)->p.devnpoints);
                        if(xy->at(i)->p.minfreelimits){xy->at(i)->FindYdmin(xmin,xmax);}
                        ymini=xy->at(i)->p.Ydmin;
                        if(xy->at(i)->p.maxfreelimits){xy->at(i)->FindYdmax(xmin,xmax);}
                        ymaxi=xy->at(i)->p.Ydmax;
                    }

                } else{
                    xy->at(i)->Findlog(1);

                    if(xy->at(i)->p.minfreelimits ){xy->at(i)->FindYlmin(xmin,xmax);}else xy->at(i)->p.Ylmin=std::max(log10(xy->at(i)->p.Ymin),-30.);
                    ymini=xy->at(i)->p.Ylmin;
                    if(xy->at(i)->p.maxfreelimits ){xy->at(i)->FindYlmax(xmin,xmax);}else xy->at(i)->p.Ylmax=std::min(log10(xy->at(i)->p.Ymax),30.);
                    ymaxi=xy->at(i)->p.Ylmax;



                }

                yminr=std::min(yminr,ymini);
                ymaxr=std::max(ymaxr,ymaxi);

            }

        }

    }
    if(lgloballimits){
        if(loge) {
            for(int i=0;i<xy->count();i++) {

                if(!(xy->at(i)->p.rightaxis))      xy->at(i)->Findlog(1);}
            yminl=log10(std::max(yminl, 1.e-30));
            ymaxl=log10(std::max(ymaxl, 1.e-30));
        }else{
            for(int i=0;i<xy->count();i++) {

                if(!(xy->at(i)->p.rightaxis)&&(xy->at(i)->p.smooth ||(xy->at(i)->p.onlysmooth))){
                    xy->at(i)->evaluatesmooth(xy->at(i)->p.smoothpoints,xy->at(i)->p.smoothalpha);}
            }
        }
    }
    if(rgloballimits){
        if(logr) {
            for(int i=0;i<xy->count();i++) {

                if(xy->at(i)->p.rightaxis)     xy->at(i)->Findlog(1);}
            yminr=log10(std::max(yminr, 1.e-30));
            ymaxr=log10(std::max(ymaxr, 1.e-30));
        }else{
            for(int i=0;i<xy->count();i++) {

                if(xy->at(i)->p.rightaxis&&(xy->at(i)->p.smooth ||(xy->at(i)->p.onlysmooth))){
                    xy->at(i)->evaluatesmooth(xy->at(i)->p.smoothpoints,xy->at(i)->p.smoothalpha);}
            }
        }
    }

    if(yminl > 1.e32) {yminl=1.;}
    if(ymaxl <-1.e32){ ymaxl=10.;}
     if(!loge){
        double diff=ymaxl-yminl;
        ymaxl=ymaxl+.1*diff;
        yminl=yminl-.1*diff;
        if(std::abs(yminl-ymaxl) <1.e-32) {ymaxl=yminl+10.; yminl=yminl-10.;}
    }

    if(yminr > 1.e32) {yminr=1.;}
    if(ymaxr <-1.e32){ ymaxr=10.;}
    if(!logr){
        double diff=ymaxr-yminr;
        ymaxr=ymaxr+.1*diff;
        yminr=yminr-.1*diff;
    }
    if(std::abs(yminr-ymaxr) <1.e-32) {ymaxr=yminr+10.; yminr=yminr-10.;}
    if(yminr > 1.e32) {yminr=1.;}
    if(ymaxr <-1.e32){ ymaxr=10.;}

    //left axis

    pls->wind(xmin,xmax,yminl,ymaxl);
    pls->col0(1);
    grid(p);

    int nc=0;
    for(int i=0;i<xy->count();i++)
    {
        if((xy->at(i)->p.rightaxis)) continue;
        int r,b,g;
        Plplot_utils::rgbfromhex(&r,&g,&b,xy->at(i)->p.Color.toLatin1().constData());
        pls->scol0(2,r,g,b);
        pls->col0(2);
        pls->lsty(xy->at(i)->p.curvestyle);

        int size=(int) xy->at(i)->x.size();
        if(loge){
            if(!xy->at(i)->p.onlysmooth)plcurve(size, &xy->at(i)->x[0], &xy->at(i)->logy[0],xmin,yminl,xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale);
            if(xy->at(i)->p.onlysmooth ||xy->at(i)->p.smooth )pllogsmoth(size, &xy->at(i)->x[0], &xy->at(i)->logy[0],xmin,yminl,xy->at(i)->p.linetype,xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale,xy->at(i)->p.smoothpoints);

        }else if(xy->at(i)->p.onlysmooth){
            plcurve(size, &xy->at(i)->x[0], &xy->at(i)->ysmooth[0],xmin,yminl,xy->at(i)->p.linetype,xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale);
        }else{
            if(!xy->at(i)->p.derivative)plcurve(size, &xy->at(i)->x[0], &xy->at(i)->y[0],xmin,yminl,xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale);
            else plcurve((int) xy->at(i)->xder.size(),
                        &xy->at(i)->xder[0], &xy->at(i)->yder[0],xmin,yminl, xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale);

            if(xy->at(i)->p.smooth)plcurve(size, &xy->at(i)->x[0], &xy->at(i)->ysmooth[0],xmin,yminl,"line",xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale);
        }
        pls->lsty(1);
        pls->mtex("t",-p->celldis-p->celllabsep*nc,p->cellpos,0.,xy->at(i)->p.yaxisLabel.toLatin1());
        nc++;
    }

    pls->wind(xmin,xmax,yminr,ymaxr);

    nc=0;
    for(int i=0;i<xy->count();i++)
    {
        if(!(xy->at(i)->p.rightaxis)) continue;
        int r,b,g;
        Plplot_utils::rgbfromhex(&r,&g,&b,xy->at(i)->p.Color.toLatin1().constData());
        pls->scol0(2,r,g,b);
        pls->col0(2);
          pls->lsty(xy->at(i)->p.curvestyle);
        int size=(int) xy->at(i)->x.size();
        if(logr){
            if(!xy->at(i)->p.onlysmooth)plcurve(size, &xy->at(i)->x[0], &xy->at(i)->logy[0],xmin,yminr,xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale);
            if(xy->at(i)->p.onlysmooth ||xy->at(i)->p.smooth )pllogsmoth(size, &xy->at(i)->x[0], &xy->at(i)->logy[0],xmin,yminr,xy->at(i)->p.linetype,xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale,xy->at(i)->p.smoothpoints);
        }else if(xy->at(i)->p.onlysmooth){
            plcurve(size, &xy->at(i)->x[0], &xy->at(i)->ysmooth[0],xmin,yminr,xy->at(i)->p.linetype,xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale);
        }else{
           if(!xy->at(i)->p.derivative) plcurve(size, &xy->at(i)->x[0], &xy->at(i)->y[0],xmin,yminr,xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale);
            else plcurve((int) xy->at(i)->xder.size(),
                        &xy->at(i)->xder[0], &xy->at(i)->yder[0],xmin,yminr, xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale);
            if(xy->at(i)->p.smooth)plcurve(size, &xy->at(i)->x[0], &xy->at(i)->ysmooth[0],xmin,yminr,"line",xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale);
        }
        pls->lsty(1);
        pls->mtex("t",-p->celldis-p->celllabsep*nc,1-p->cellpos,1.,xy->at(i)->p.yaxisLabel.toLatin1());
        nc++;
    }


    pls->col0(1);
    pls->lsty(1);
    pls->width(p->defaultpwith);
    if(!p->withoutxaxis){
        SetAxisTime(ltime,itime,p->Xtimeaxis,""); //atenção esta funçao chama um plbox logo só pode ser chamada depois da plwind
        if(p->firsttimelabel)Putfirsttimelabel(4.,0.,.5,itime);
    }

    pls->lab("","",p->plottitle.toLatin1().constData());
    if(leftaxis==0){
        pls->box("",0.,0.,"bn",0,0);
    }
    if(rightaxis==0){
        pls->box("",0.,0.,"cm",0,0);
    }//Tirar a escala de um dos lado squando não oexistem escalas desse lado.

    pls->wind(xmin,xmax,yminl,ymaxl);

    if(!loge){
        pls->box("",0.,0.,p->MLeftaxis.toLatin1().constData(),0,0);
    }else{
        pls->box("",0.,0.,p->MLeftaxislog.toLatin1().constData(),0,0);

    }
    pls->wind(xmin,xmax,yminr,ymaxr);
    if(!logr){
        pls->box("",0.,0.,p->MRightaxis.toLatin1().constData(),0,0);
    } else{
        pls->box("",0.,0.,p->MRightaxislog.toLatin1().constData(),0,0);

    }
    if(p->leftaxislimits.count()){

        pls->gyax(dg,dgmax);
        pls->mtex("l",dgmax+p->labelsep,.5,0.5,lAxisLimits.at(0).toLatin1());
    }
    if(p->rightaxislimits.count()){
        pls->gyax(dg,dgmax);
        pls->mtex("r",dgmax+p->labelsep,.5,0.5,rAxisLimits.at(0).toLatin1());
    }

}
void  Plotting::Singleaxis( long long itime, long long ltime,   QVector <xydata * > *xy ,plotproprieties *p)
{

    p->xvminp=p->xvmin;  p->xvmaxp=p->xvmax;
    p->yvminp=p->yvmin;  p->yvmaxp=p->yvmax;
    PLFLT p_def,p_ht;
    pls->gchr(p_def,p_ht);
    PLFLT p_xmin,p_xmax,p_ymin,p_ymax;
    pls->gspa(p_xmin,p_xmax,p_ymin,p_ymax);
    double bottom;
    bottom=5.1*p_ht/(p_ymax-p_ymin);
    p->yvminp=p->yvmin+bottom;

    pls->vpor(p->xvminp,p->xvmaxp,p->yvminp,p->yvmaxp);

    PLFLT xmax,xmin;
    PLINT dg,dgmax;


    xmin=(PLFLT) itime;
    xmax=(PLFLT) ltime;
    double ymini,ymaxi;

    double ymin=+10.e33;
    double ymax=-10.e33;
    bool log=true;
    QStringList ylimits;
    QStringList AxisLimits;
    bool globallimits=true;
    bool ok;


    if(p->leftaxislimits.count()){
        AxisLimits=p->leftaxislimits.split(";");
        if(AxisLimits.count()==3){
            ylimits=AxisLimits.at(1).split(":");
            if( ylimits.count()==2){
                ymin=ylimits.at(0).toDouble(&ok);
                if(!ok)  globallimits=false;
                ymax=ylimits.at(1).toDouble(&ok);
                if(!ok)  globallimits=false;
            }else globallimits=false;
            if(AxisLimits.at(2).contains("log"))log=true; else log=false;
        }else globallimits=false;
    }else globallimits=false;

    if(!globallimits){
        for(int i=0;i<xy->count();i++)
        {
            if( !(xy->at(i)->p.ylog)){
                log=false;
                break;
            }
        }
        for(int i=0;i<xy->count();i++)
        {
            if(!log){


                if(xy->at(i)->p.minfreelimits) {xy->at(i)->FindYmin(xmin,xmax);}
                ymini=xy->at(i)->p.Ymin;
                if(xy->at(i)->p.maxfreelimits) {xy->at(i)->FindYmax(xmin,xmax);}
                ymaxi=xy->at(i)->p.Ymax;

                if((xy->at(i)->p.smooth) ||(xy->at(i)->p.onlysmooth) ){
                    xy->at(i)->evaluatesmooth(xy->at(i)->p.smoothpoints,xy->at(i)->p.smoothalpha);
                    if(xy->at(i)->p.onlysmooth  &&(xy->at(i)->p.minfreelimits || xy->at(i)->p.maxfreelimits)){
                        xy->at(i)->FindYsminmax(xmin,xmax);
                        if(xy->at(i)->p.minfreelimits) ymini=xy->at(i)->p.Ysmin;
                        if(xy->at(i)->p.maxfreelimits) ymaxi=xy->at(i)->p.Ysmax;
                    }
                }

                if(xy->at(i)->p.derivative){
                    xy->at(i)->derivativedouble(xy->at(i)->p.devnpoints);
                    if(xy->at(i)->p.minfreelimits){xy->at(i)->FindYdmin(xmin,xmax);}
                    ymini=xy->at(i)->p.Ydmin;
                    if(xy->at(i)->p.maxfreelimits){xy->at(i)->FindYdmax(xmin,xmax);}
                    ymaxi=xy->at(i)->p.Ydmax;
                }
            } else{
                xy->at(i)->Findlog(1);
                if(xy->at(i)->p.minfreelimits || xy->at(i)->p.maxfreelimits)xy->at(i)->FindYlminmax(xmin,xmax);
                if(xy->at(i)->p.minfreelimits) ymini=xy->at(i)->p.Ylmin;else ymini=std::max(log10(xy->at(i)->p.Ymin),-30.);
                if(xy->at(i)->p.maxfreelimits)ymaxi=xy->at(i)->p.Ylmax;else ymaxi=std::min(log10(xy->at(i)->p.Ymax),-30.);



            }

            ymin=std::min(ymin,ymini);
            ymax=std::max(ymax,ymaxi);

        }


        if(ymin > 1.e32) {ymin=1.;}
        if(ymax <-1.e32){ ymax=10.;}
        if(log){

            if(ymax-ymin <1.){
                log=false;
                ymin=+10.e33;
                ymax=-10.e33;

                for(int i=0;i<xy->count();i++)
                {


                    if(xy->at(i)->p.minfreelimits ){ xy->at(i)->FindYmin(xmin,xmax);ymini=xy->at(i)->p.Ymin;}
                    if(xy->at(i)->p.maxfreelimits ){ xy->at(i)->FindYmax(xmin,xmax);ymaxi=xy->at(i)->p.Ymax;}

                    if((xy->at(i)->p.smooth) ||(xy->at(i)->p.onlysmooth) ){
                        xy->at(i)->evaluatesmooth(xy->at(i)->p.smoothpoints,xy->at(i)->p.smoothalpha);

                        if(xy->at(i)->p.onlysmooth  && (xy->at(i)->p.minfreelimits ||xy->at(i)->p.maxfreelimits)){

                            if(xy->at(i)->p.minfreelimits ) { xy->at(i)->FindYsmin(xmin,xmax);ymini=xy->at(i)->p.Ysmin;}
                            if(xy->at(i)->p.maxfreelimits ) {xy->at(i)->FindYsmax(xmin,xmax);ymaxi=xy->at(i)->p.Ysmax;}
                        }
                    }


                    ymin=std::min(ymin,ymini);
                    ymax=std::max(ymax,ymaxi);

                }
            }
        }
        if(ymin > 1.e32) {ymin=1.;}
        if(ymax <-1.e32){ ymax=10.;}

    }else{

        if(log) {
            for(int i=0;i<xy->count();i++) {

                xy->at(i)->Findlog(1);}
            ymin=log10(std::max(ymin, 1.e-30));
            ymax=log10(std::max(ymax, 1.e-30));
        }else{
            for(int i=0;i<xy->count();i++) {

                if(xy->at(i)->p.smooth ||(xy->at(i)->p.onlysmooth)){
                    xy->at(i)->evaluatesmooth(xy->at(i)->p.smoothpoints,xy->at(i)->p.smoothalpha);}
            }
        }
    }


    /*
    if(log){
        if(ymin <1.) ymin=10.;
        if(ymax <ymin) ymax=ymin+100.;
    }
*/
    //   if(!log){

    double diff=ymax-ymin;
    ymax=ymax+.1*diff;
    ymin=ymin-.1*diff;
    //    }

    if(std::abs(ymin-ymax) <1.e-32) {ymax=ymin+10.; ymin=ymin-10.;}

    if(ymin > 1.e32) {ymin=1.;}
    if(ymax <-1.e32){ ymax=10.;}

    pls->wind(xmin,xmax,ymin,ymax);
    pls->col0(1);
    grid(p);

    int nc=0;
    for(int i=0;i<xy->count();i++)
    {
        int r,b,g;
        Plplot_utils::rgbfromhex(&r,&g,&b,xy->at(i)->p.Color.toLatin1().constData());
        pls->scol0(2,r,g,b);
        pls->col0(2);

        pls->lsty(xy->at(i)->p.curvestyle);
        int size=(int) xy->at(i)->x.size();
        if(log) {
            if(!xy->at(i)->p.onlysmooth)plcurve(size, &xy->at(i)->x[0], &xy->at(i)->logy[0],xmin,ymin,xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale);
            if(xy->at(i)->p.onlysmooth ||xy->at(i)->p.smooth )pllogsmoth(size, &xy->at(i)->x[0], &xy->at(i)->logy[0],xmin,ymin,xy->at(i)->p.linetype,xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                    xy->at(i)->p.marker, xy->at(i)->p.markersscale,xy->at(i)->p.smoothpoints);

        }
        else {
            if(xy->at(i)->p.onlysmooth){
                plcurve(size, &xy->at(i)->x[0], &xy->at(i)->ysmooth[0],xmin,ymin,"line",xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale);                //só vou considerar o caso log para quando não há smoothing
            }else{

               if(!xy->at(i)->p.derivative)  plcurve(size,
                        &xy->at(i)->x[0], &xy->at(i)->y[0],xmin,ymin, xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale);
                else plcurve((int) xy->at(i)->xder.size(),
                            &xy->at(i)->xder[0], &xy->at(i)->yder[0],xmin,ymin, xy->at(i)->p.linetype,xy->at(i)->p.pwidth+p->hdpithickeness,
                            xy->at(i)->p.marker, xy->at(i)->p.markersscale);

                if(xy->at(i)->p.smooth)plcurve(size, &xy->at(i)->x[0], &xy->at(i)->ysmooth[0],xmin,ymin,"line",xy->at(i)->p.psmoothwidth+p->hdpithickeness,
                        xy->at(i)->p.marker, xy->at(i)->p.markersscale); //uma vez mais não considero log nos smoothing e considero neste caso sempre type igual a 1.
                //não me parece bem quando a linha e o smotthing são ambos traçados o smothing vai ser sempre em linhas.
            }

        }
        pls->mtex("t",-p->celldis-p->celllabsep*nc,p->cellpos,p->celljus,xy->at(i)->p.yaxisLabel.toLatin1());
        nc++;
    }
    pls->col0(1);
    pls->width(p->defaultpwith);
    pls->lsty(1);
if(!p->withoutxaxis){
    if(!log) SetAxisTime(ltime,itime,p->Xtimeaxis,p->Ytimeaxis);
    else {
        //      SetAxisTimeLog(ltime,itime,Data.Xtimeaxis,Data.Ytimeaxislog,4000.,1);
        SetAxisTime(ltime,itime,p->Xtimeaxis,p->Ytimeaxislog);
    }
   if(p->firsttimelabel) Putfirsttimelabel(4.,0.,.5,itime);
}     else{
    if(!log) pls->box("",0.,0.,p->Ytimeaxis.toLatin1().constData(),0,0);
    else pls->box("",0.,0.,p->Ytimeaxislog.toLatin1().constData(),0,0);
    }


    pls->lab("","",p->plottitle.toLatin1().constData());
    if(p->leftaxislimits.count()){

        pls->gyax(dg,dgmax);

        pls->mtex("l",dgmax+p->labelsep,.5,0.5,AxisLimits.at(0).toLatin1());
        pls->mtex("r",dgmax+p->labelsep,.5,0.5,AxisLimits.at(0).toLatin1());


    }

}


void Plotting::lifetime(std::vector<double> xxx,std::vector<double> yyy, double average, double slope,double y_intercept, double ymini,double xxi,double xvminp, double xvmaxp,double yvminp,double yvmaxp){

    pls->vpor(xvminp,xvmaxp,yvminp,yvmaxp);
    pls->wind(savedxmin,savedxmax,savedymin,savedymax);

    pls->col0(1);
    for(int i=0;i<(int) xxx.size(); i=i+2){
        pls->join((PLFLT) xxx.at(i),(PLFLT) yyy.at(i),(PLFLT) xxx.at(i+1),(PLFLT) yyy.at(i+1) );
   //     qDebug() << savedxmax-xxx.at(i) <<"  "<<  savedxmin-xxx.at(i)<< savedymax-yyy.at(i) <<"  "<<  savedymin-yyy.at(i);
    }
    pls->mtex("t", -2. - 2.5, .2, 0., "y=y0+Ae^(c*(x-xo))");
    QString legenda;
    legenda.clear();
    legenda.append("y0=");
    legenda.append(QString::number(ymini));
    pls->mtex("t", -4. - 2.5, .2, 0., legenda.toLatin1().constData());
    legenda.clear();
    legenda.append("A=");
    legenda.append(QString::number(y_intercept));
    pls->mtex("t", -6. - 2.5, .2, 0., legenda.toLatin1().constData());
    legenda.clear();
    legenda.append("x0=");
    legenda.append(QString::number(xxi));
    pls->mtex("t", -8. - 2.5, .2, 0., legenda.toLatin1().constData());
    legenda.clear();
    legenda.append("c=");
    legenda.append(QString::number(slope));
    pls->mtex("t", -2. - 2.5, .4, 0., legenda.toLatin1().constData());
    if (abs(slope) < .1e-23) {
        legenda.clear();
        legenda.append("1/c=");
        legenda.append(QString::number(1. / slope));
        pls->mtex("t", -4. - 2.5, .4, 0., legenda.toLatin1().constData());
    }
    legenda.clear();
    legenda.append("average=");
    legenda.append(QString::number(average, 'g', 6));
    pls->mtex("t", -2.5, .05, 0., legenda.toLatin1().constData());
    pls->mtex("t", -22.5, .05, 0., legenda.toLatin1().constData());


}

void  Plotting::getxlenylen(double *xl, double *yl)
{
PLFLT xp,yp;
PLINT xleng,yleng,xoff,yoff;
pls->gpage (xp, yp, xleng, yleng, xoff, yoff);
*xl=xleng;
*yl=yleng;
}
