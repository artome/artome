#ifndef HISTODATA_H
#define HISTODATA_H
#include <vector>
#include <QString>
#include "histoproprieties.h"

class histodata
{
public:
    histodata();
    histoproprieties p;
    std::vector<double>  y;
    std::vector<QString>  x;
    void FindYminmax();

};

#endif // HISTODATA_H
