#ifndef PREVIOUSLOGINS_H
#define PREVIOUSLOGINS_H

#include <QDialog>

#include <QStandardItemModel>

namespace Ui {
class PreviousLogins;
}

class PreviousLogins : public QDialog
{
    Q_OBJECT

public:
    explicit PreviousLogins(QWidget *parent = nullptr);
    ~PreviousLogins();
    void fillListView(QStringList);
    int ch;

private slots:
    void chosen(QModelIndex );


private:
    Ui::PreviousLogins *ui;
     QStandardItemModel model;
};

#endif // PREVIOUSLOGINS_H
