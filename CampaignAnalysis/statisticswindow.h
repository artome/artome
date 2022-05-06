#ifndef STATISTICSWINDOW_H
#define STATISTICSWINDOW_H

#include <QDialog>

namespace Ui {
class StatisticsWindow;
}

class StatisticsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsWindow(QWidget *parent = 0);
    ~StatisticsWindow();
    void inserttext(QString, QColor);

private:
    Ui::StatisticsWindow *ui;
};

#endif // STATISTICSWINDOW_H
