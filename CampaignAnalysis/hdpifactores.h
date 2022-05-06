#ifndef HDPIFACTORES_H
#define HDPIFACTORES_H

#include <QDialog>

namespace Ui {
class Hdpifactores;
}

class Hdpifactores : public QDialog
{
    Q_OBJECT

public:
    explicit Hdpifactores(QWidget *parent = nullptr);
    ~Hdpifactores();
    double fontfactor,thickfactor,windowsfactor,charheight,dpi,wfontsize;
    void fillvalues(double,double,double,double,double,double);
private slots:
    void readvalues();
    void storevalues();
    void setdefault();
private:
    Ui::Hdpifactores *ui;
};

#endif // HDPIFACTORES_H
