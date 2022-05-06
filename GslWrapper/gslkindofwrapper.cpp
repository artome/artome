#include "gslkindofwrapper.h"

GslKindofwrapper::GslKindofwrapper()
{
error.clear();
}

// stats package
double GslKindofwrapper::vector_mean(std::vector<double> *vec)
{
 size_t n=vec->size();
 if (n==0){
     error="sizeof vector was 0";
     return 0.0;
 }
 double mean=gsl_stats_mean(&vec->at(0),1,n);
 return mean;
}

double GslKindofwrapper::vector_median(std::vector<double> *vec)
{
 size_t n=vec->size();
 if (n==0){
     error="sizeof vector was 0";
     return 0.0;
 }

 gsl_sort (&vec->at(0), 1, n);

 double median=gsl_stats_median_from_sorted_data (&vec->at(0),1,n);
 return median;
}


double GslKindofwrapper::vector_std(std::vector<double> *vec,double mean)
{
 size_t n=vec->size();
 if (n==0){
     error="sizeof vector was 0";
     return 0.0;
 }
 double sd=gsl_stats_sd_m(&vec->at(0),1,n,mean);
 return sd;
}
double GslKindofwrapper::vector_max(std::vector<double> * vec)
{
    size_t n=vec->size();
    if (n==0){
        error="sizeof vector was 0";
        return 0.0;
    }

    double max=gsl_stats_max(&vec->at(0),1,n);
    return max;


}

double GslKindofwrapper::vector_min(std::vector<double> *vec)
{
    size_t n=vec->size();
    if (n==0){
        error="sizeof vector was 0";
        return 0.0;
    }
    double min=gsl_stats_min(&vec->at(0),1,n);
    return min;

}
void   GslKindofwrapper::vector_maxmin(std::vector<double> *vec, double *min, double *max)
{
    size_t n=vec->size();
    if (n==0){
        error="sizeof vector was 0";
        return ;
    }
    gsl_stats_minmax(min,max, &vec->at(0),1,n);

}
size_t    GslKindofwrapper::vector_max_index(std::vector<double> *vec )
{
    size_t n=vec->size();
    if (n==0){
        error="sizeof vector was 0";
        return 0.0;
    }
    size_t imax=gsl_stats_max_index(&vec->at(0),1,n);
    return imax;

}
size_t    GslKindofwrapper::vector_min_index(std::vector<double> *vec)
{
    size_t n=vec->size();
    if (n==0){
        error="sizeof vector was 0";
        return 0.0;
    }
    size_t imin=gsl_stats_min_index(&vec->at(0),1,n);
    return imin;


}
void   GslKindofwrapper::vector_maxmin_index(std::vector<double> *vec, size_t* imin, size_t* imax)
{
    size_t n=vec->size();
    if (n==0){
        error="sizeof vector was 0";
        return;
    }
     gsl_stats_minmax_index(imin,imax, &vec->at(0),1,n);
}



int GslKindofwrapper::Movingpolynomialfit(std::vector<double> *vecx,std::vector<double> *vecy,
                                           std::vector<double> *mvecx,std::vector<double> *mvecy,
                                           std::vector<double> *dvecx,std::vector<double> *dvecy,
                                           int order, size_t npoints)
{

    size_t size=vecx->size();
    if (size==0){
        error="sizeof vector was 0";
        return 1;
    }


    if ( npoints % 2 == 0 ) npoints=npoints+1;
    int nalf=npoints/2;

gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(npoints, order);

gsl_matrix *X   = gsl_matrix_alloc(npoints, order);
gsl_vector *y   = gsl_vector_alloc(npoints);
gsl_vector *c   = gsl_vector_alloc(order);
gsl_matrix *cov = gsl_matrix_alloc(order, order);


double chisq, xx;
float  yy;
mvecx->clear();mvecy->clear();dvecx->clear();dvecy->clear();
for (int m = nalf; m < (int) size - nalf ; m++) {
    for (int l = m - nalf; l < m + nalf + 1; l++) {
        xx = vecx->at(l) - vecx->at(m);
        for (int j = 0; j < order; j++) {
            gsl_matrix_set(X, l - m + nalf, j, pow(xx, j));
        }
        gsl_vector_set(y, l - m + nalf, vecy->at(l));
    }

    for (int kk = 1; kk < order; kk++) {
        gsl_vector_set(c, kk, -2.e+33); // porque me está a parecer que multifit nem sempre calcula todos os valores
    }
    gsl_multifit_linear(X, y, c, cov, &chisq, work);
    yy = gsl_vector_get(c, 0);


    if ((yy > -1.e+33) && gsl_finite(yy)) {
        mvecx->push_back(vecx->at(m));
        mvecy->push_back(yy);
    }
    yy = gsl_vector_get(c, 1);
    if (yy > -1.e+33) {
        dvecx->push_back(vecx->at(m));
        dvecy->push_back(yy);
    }
}
gsl_matrix_free(X);
gsl_vector_free(y);
gsl_vector_free(c);
gsl_matrix_free(cov);
gsl_multifit_linear_free(work);
return 0;
}
