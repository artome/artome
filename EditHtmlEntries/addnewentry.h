#ifndef ADDNEWENTRY_H
#define ADDNEWENTRY_H

#include <QDialog>

namespace Ui {
class addnewentry;
}

class addnewentry : public QDialog
{
    Q_OBJECT

public:
    explicit addnewentry(QWidget *parent = nullptr);
    ~addnewentry();
     QString newentry;
private slots:
    void readvalues();

private:
    Ui::addnewentry *ui;
};

#endif // ADDNEWENTRY_H
