#ifndef PLOTTING_H
#define PLOTTING_H

#include <QObject>
#include "plstream.h"
#include <QImage>

#include <QVector>
#include "xydata.h"
#include "plotproprieties.h"
#include "xyzdata.h"
#include "histodata.h"

class Plotting : public QObject
{
    Q_OBJECT
public:
    explicit Plotting(QObject *parent = nullptr);
    ~Plotting();

    PLFLT dpi;
    void Plot_page(int ,int, QString,QString,int,int,int,PLFLT);

    void deleteStream();
    void deleteImage();

    QImage *image;
 //   QImage image = QImage(10,10, QImage::Format_ARGB32);
    void SetInicialColormap();
    void setfont(int);
    void setcharheight(double ,double );

    void Multiaxis(long long, long long,   QVector <xydata * > * ,plotproprieties * );
    void Twoaxis( long long, long long,   QVector <xydata * > * ,plotproprieties * );
    void Singleaxis( long long, long long,   QVector <xydata * > * ,plotproprieties * );

    void Tile2d(int,int , int, double, double);
    void SurfacePlot( long long , long long,xyzdata *,plotproprieties * , bool=true );
    void HistoPlot(QVector<histodata *> *,plotproprieties *);
    QString getstartruns(long long,long long, QString);
    void putrunlabels( long long , long long,plotproprieties *,QStringList );
    void getxlenylen(double *, double *);
    void lifetime(std::vector<double> ,std::vector<double> , double , double ,double, double, double, double ,double, double, double );
public slots:
    void LogMessages(QString);
 signals:
    void logmessages(QString);
private:
    plstream *pls;
 //   QStringList fontfamily,fontstyle, fontweight;
    void plcurve(PLINT , const PLFLT *, const PLFLT *, PLFLT , PLFLT , QString , int , int ,  double );
    void pllogsmoth(PLINT , const PLFLT *, const PLFLT *, PLFLT , PLFLT , QString , int , int ,  double,int );
    void Putfirsttimelabel(PLFLT , PLFLT , PLFLT  , long long);
    void grid(plotproprieties *);
    void bars(PLINT , const PLFLT *,  const PLFLT *,int, PLFLT, PLFLT );
    void SetAxisTime(long long, long long, QString, QString );
    void Tile2d(int , int, double, double,xyzdata *);
    void CountourShading(int , int, double, double , int,xyzdata * );
    void colorlegend(double, double, double, double, double, double,int, int, QStringList , int);
    void plfbox(PLFLT , PLFLT , float , PLINT , PLINT, PLINT, PLINT , PLINT );
    PLFLT savedxmin,savedxmax,savedymin,savedymax;

};

#endif // PLOTTING_H
