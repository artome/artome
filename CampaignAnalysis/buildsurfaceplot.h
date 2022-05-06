#ifndef BUILDSURFACEPLOT_H
#define BUILDSURFACEPLOT_H

#include <QDialog>

namespace Ui {
class buildsurfaceplot;
}

class buildsurfaceplot : public QDialog
{
    Q_OBJECT

public:
    explicit buildsurfaceplot(QWidget *parent = 0);
    ~buildsurfaceplot();
    void writeinformation(QString);
    QString channelstring;
    QString yscale;
    QString itens;
    int ndiameters;

private slots:
    void readvalues();
    void evaluate();
private:
    Ui::buildsurfaceplot *ui;
};

#endif // BUILDSURFACEPLOT_H
