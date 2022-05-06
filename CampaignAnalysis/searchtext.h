#ifndef SEARCHTEXT_H
#define SEARCHTEXT_H

#include <QDialog>

namespace Ui {
class searchtext;
}

class searchtext : public QDialog
{
    Q_OBJECT

public:
    explicit searchtext(QWidget *parent = nullptr);
    ~searchtext();
    QString stringtosearch;
    void settext(QString);
private slots:
   void acceptc();
private:
    Ui::searchtext *ui;

};

#endif // SEARCHTEXT_H
