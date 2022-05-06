#include "xyzdata.h"
#include <cmath>
xyzdata::xyzdata() {



}
void xyzdata::Findlog(int i) {
  unsigned long size;
  switch (i) {
  case 1:
    size = y.size();
    logy.resize(size);
    for (unsigned long j = 0; j < size; j++) {

      logy[j] = log10(std::max(y.at(j), 1.e-30));
    }
    break;

  default:

    break;
  }
  return;
}

void xyzdata::FindlogZ(double valuemin) {
  unsigned long size0, size1;
  size0 = y.size();
  size1 = x.size();
  logz.resize(size0);
  for (unsigned long i = 0; i < size0; i++) {
    logz[i].resize(size1);
    for (unsigned long k = 0; k < size1; k++) {
      //      logz[i][k]=log10(std::max(z[i][k], 1.e-30));
      //        A linha anterior seria a linha mais natural.
      // contudo nos banans que sopostamente são contagens logo positiva a linha
      // anterir daria valores mínimos de -32 e como em geral as contagens não
      // passam dos 300 cujo logaritmos é ~3 estariamos a fazer um gráfico só de
      // lixo. no anterior cloud dax eu estava a fazer qualquer coisa como
      //          logz[i][k]=log10(std::max(z[i][k], 1.));
      // mas paar salvaguardarmos a hipótes de um dia termos necessidade de
      // fazer realmente uma superfície log com valores muito pequenos entre
      // zero e um vou por o argumento do mínimo na função.

      logz[i][k] = log10(std::max(z.at(i).at(k), valuemin));
    }
  }
  return;
}

void xyzdata::FindYminmax() {
  double ymin, ymax;
  ymin = +10.e33;
  ymax = -10.e33;
  unsigned long size = y.size();
  for (unsigned long i = 0; i < size; i++) {

    ymin = std::min(ymin, y.at(i));
    ymax = std::max(ymax, y.at(i));
  }
  p.Ymin = ymin;
  p.Ymax = ymax;
  if (ymin > 1.e32) {
    p.Ymin = 1.;
  }
  if (ymax < -1.e32) {
    p.Ymax = 10.;
  }
  if (std::abs(p.Ymax - p.Ymin) < .1e-31) {
    p.Ymin = p.Ymin - 5.;
    p.Ymax = p.Ymax + 5.;
  }
}
void xyzdata::FindYlminmax() {
  double ymin, ymax;
  ymin = +10.e33;
  ymax = -10.e33;
  unsigned long size = y.size();
  for (unsigned long i = 0; i < size; i++) {

    ymin = std::min(ymin, logy.at(i));
    ymax = std::max(ymax, logy.at(i));
  }
  p.Ylmin = ymin;
  p.Ylmax = ymax;
  if (ymin > 1.e32) {
    p.Ylmin = 1.;
  }
  if (ymax < -1.e32) {
    p.Ylmax = 10.;
  }
  if (std::abs(p.Ylmax - p.Ylmin) < .1e-31) {
    p.Ylmin = std::max(p.Ylmin - 1., 0.);
    p.Ylmax = p.Ylmax + 1.;
  }
}

void xyzdata::FindZminmax(double xi, double xf) {
  double ymin, ymax;
  ymin = +10.e33;
  ymax = -10.e33;
  unsigned long size0, size1;
  size0 = y.size();
  size1 = x.size();

  for (unsigned long i = 0; i < size0; i++) {
    for (unsigned long k = 0; k < size1; k++) {
      if ((x.at(k) < xi) || (x.at(k) > xf)) continue;
      ymin = std::min(ymin, z.at(i).at(k));
      ymax = std::max(ymax, z.at(i).at(k));
    }
  }
  p.Zmin = ymin;
  p.Zmax = ymax;
  if (ymin > 1.e32) {
    p.Zmin = 1.;
  }
  if (ymax < -1.e32) {
    p.Zmax = 10.;
  }
  if (std::abs(p.Zmax - p.Zmin) < .1e-31) {
    p.Zmin = p.Zmin - 5.;
    p.Zmax = p.Zmax + 5.;
  }
}
void xyzdata::FindZlminmax(double xi, double xf) {
  double ymin, ymax;
  ymin = 33;
  ymax = -33;
  unsigned long size0, size1;
  size0 = y.size();
  size1 = x.size();

  for (unsigned long i = 0; i < size0; i++) {
    for (unsigned long k = 0; k < size1; k++) {
      if ((x.at(k) < xi) || (x.at(k) > xf)) continue;
      ymin = std::min(ymin, logz.at(i).at(k));
      ymax = std::max(ymax, logz.at(i).at(k));
    }
  }
  p.Zlmin = ymin;
  p.Zlmax = ymax;
  if (ymin > 32) {
    p.Zlmin = 1.;
  }
  if (ymax < -32) {
    p.Zlmax = 10.;
  }
  if (std::abs(p.Zlmax - p.Zlmin) < 1.) {
    p.Zlmin = std::max(p.Zlmin - 1., 0.);
    p.Zlmax = p.Zlmax + 1.;
  }
}

void xyzdata::FindZmin(double xi, double xf) {
  double ymin;
  ymin = +10.e33;
  unsigned long size0, size1;
  size0 = y.size();
  size1 = x.size();

  for (unsigned long i = 0; i < size0; i++) {
    for (unsigned long k = 0; k < size1; k++) {
        if ((x.at(k) < xi) || (x.at(k) > xf)) continue;

      ymin = std::min(ymin, z.at(i).at(k));
    }
  }
  p.Zmin = ymin;
  if (ymin > 1.e32) {
    p.Zmin = 1.;
  }
}
void xyzdata::FindZlmin(double xi, double xf) {

    //tudo mal feito teria que ser nos limites do plot
  double ymin;
  ymin = 33;
  unsigned long size0, size1;
  size0 = y.size();
  size1 = x.size();

  for (unsigned long i = 0; i < size0; i++) {
    for (unsigned long k = 0; k < size1; k++) {
        if ((x.at(k) < xi) || (x.at(k) > xf)) continue;

      ymin = std::min(ymin, logz.at(i).at(k));
    }
  }
  p.Zlmin = ymin;

  if (ymin > 32) {
    p.Zlmin = 1.;
  }
}

void xyzdata::FindZmax(double xi, double xf) {
  double ymax;
  ymax = -10.e33;
  unsigned long size0, size1;
  size0 = y.size();
  size1 = x.size();

  for (unsigned long i = 0; i < size0; i++) {
    for (unsigned long k = 0; k < size1; k++) {
      if ((x.at(k) < xi) || (x.at(k) > xf)) continue;
      ymax = std::max(ymax, z.at(i).at(k));
    }
  }

  p.Zmax = ymax;
  if (ymax < -1.e32) {
    p.Zmax = 10.;
  }
}
void xyzdata::FindZlmax(double xi, double xf) {
  double ymax;
  ymax = -33;
  unsigned long size0, size1;
  size0 = y.size();
  size1 = x.size();

  for (unsigned long i = 0; i < size0; i++) {
    for (unsigned long k = 0; k < size1; k++) {
      if ((x.at(k) < xi) || (x.at(k) > xf)) continue;
      ymax = std::max(ymax, logz.at(i).at(k));
    }
  }
  p.Zlmax = ymax;
  if (ymax < -32) {
    p.Zlmax = 10.;
  }
}
