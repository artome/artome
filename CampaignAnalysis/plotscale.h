#ifndef PLOTSCALE_H
#define PLOTSCALE_H

#include <QDialog>

namespace Ui {
class Plotscale;
}

class Plotscale : public QDialog
{
    Q_OBJECT
    
public:
    explicit Plotscale(QWidget *parent = 0);
    ~Plotscale();
    double xfactor,yfactor;

private slots:

    void changelink(int state);
    void readvalues();

private:
    Ui::Plotscale *ui;
};

#endif // PLOTSCALE_H
