#ifndef DEFINERUNLAYOUTS_H
#define DEFINERUNLAYOUTS_H

#include <QDialog>
#include <QStandardItemModel>
namespace Ui {
class DefineRunLayouts;
}

class DefineRunLayouts : public QDialog
{
    Q_OBJECT

public:
    explicit DefineRunLayouts(QWidget *parent = nullptr);
    ~DefineRunLayouts();
    void filllayouts();

  private slots:

   void sellect(QStandardItem *);
   void savelayouts();
   void readlayout(QModelIndex);
   void deletelayout();

private:
    Ui::DefineRunLayouts *ui;
    QStandardItemModel listrunmodel;

};

#endif // DEFINERUNLAYOUTS_H
