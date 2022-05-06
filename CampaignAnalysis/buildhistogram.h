#ifndef BUILDHISTOGRAM_H
#define BUILDHISTOGRAM_H

#include <QDialog>

namespace Ui {
class BuildHistogram;
}

class BuildHistogram : public QDialog
{
    Q_OBJECT

public:
    explicit BuildHistogram(QWidget *parent = 0);
    ~BuildHistogram();
    QString binlabels;
    QStringList bins;
    void setnofbin(int);

private slots:
    void readvalues();
    void freemanual(bool);
    void evaluatenofbins();
private:
    Ui::BuildHistogram *ui;
    int nofbins;
    int sampling;
    double initialvalue,finalvalue;

};

#endif // BUILDHISTOGRAM_H
