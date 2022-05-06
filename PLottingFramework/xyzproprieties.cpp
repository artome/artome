#include "xyzproprieties.h"

xyzproprieties::xyzproprieties()
{
  logyevaluated=false;
  logzevaluated=false;
  ylog=false;
  zlog=false;
  maxfreelimits=true;
  minfreelimits=true;
  surfacetype="tiled2d";
  colorlimits="Bizarreway";
  xdist=1.0e+20; //larger values will imply no lack od data will be noticed,
  colorscale="rightcolorscale";
  dcleg=.10;
  Surfacecolor="rainbow_cloud#300";
  Surfacelegendinside = ".8;.15;.067;0.07;.7;.2";
  celllabsep=2.5;
  dxvmin=.05;
  dxvmax=.00;
  Ymin=.0;
  Ymax=.0;
  Ysmin=.0;
  Ysmax=.0;
  Ylmax=10.;
  Ylmin=1.;
  Zmin=.0;
  Zmax=.0;
  Zlmax=10.;
  Zlmin=1.;
  zlogmin=1.;
  factor=1.;
}
