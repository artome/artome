#ifndef GSLWRAPPER_H
#define GSLWRAPPER_H
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_test.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_ieee_utils.h>
#include <gsl/gsl_sort.h>
#include <QObject>

class GslWrapper
{
public:
    GslWrapper();

    QString error;

    double vector_mean(std::vector<double> *);
    double vector_median(std::vector<double> *);
    double vector_std(std::vector<double> *, double);
    double vector_max(std::vector<double> *);
    double vector_min(std::vector<double> *);
    void   vector_maxmin(std::vector<double> *, double*, double*);
    static double vector_trimMean(std::vector<double> *,double);
    size_t    vector_max_index(std::vector<double> *);
    size_t    vector_min_index(std::vector<double> *);
    void   vector_maxmin_index(std::vector<double> *, size_t*, size_t*);
    int Movingpolynomialfit(std::vector<double> *,std::vector<double> *, std::vector<double> *,std::vector<double> *,
                            std::vector<double> *,std::vector<double> *,int , size_t );


};

#endif // GSLWRAPPER_H
