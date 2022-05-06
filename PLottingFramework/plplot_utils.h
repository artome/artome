#ifndef PLPLOT_UTILS_H
#define PLPLOT_UTILS_H
#include "plstream.h"
#include <QString>

class Plplot_utils
{
public:
    Plplot_utils();
    static void setcolormap0(int, int* , int* , int *, PLFLT* ,plstream *);
    static void rgbfromhex(int*, int*, int*, const char*);
    static int Testcolormap(int , int , QString , QString, int);
    static int Setcolormap(QString, int,  plstream *);
    static int getMarker(int );

private:



};

#endif // PLPLOT_UTILS_H
