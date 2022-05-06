#ifndef MOVIE_H
#define MOVIE_H

#include <QDialog>

namespace Ui {
class Movie;
}

class Movie : public QDialog
{
    Q_OBJECT

public:
    explicit Movie(QWidget *parent = 0);
    ~Movie();
    void setinitialtime(long long, long long);
    long long itimemovie,ltimemovie,stepmovie, refreshmovie, interval;
private slots:
   void getvalues();

private:
    Ui::Movie *ui;
};

#endif // MOVIE_H
