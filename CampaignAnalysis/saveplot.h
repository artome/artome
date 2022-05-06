#ifndef SAVEPLOT_H
#define SAVEPLOT_H

#include <QDialog>

namespace Ui {
class SavePlot;
}

class SavePlot : public QDialog
{
    Q_OBJECT
    
public:
    explicit SavePlot(QWidget *parent = 0);
    ~SavePlot();
    QString device;
private slots:
    void setdevice();
    
private:
    Ui::SavePlot *ui;
};

#endif // SAVEPLOT_H
