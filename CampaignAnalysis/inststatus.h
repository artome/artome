#ifndef INSTSTATUS_H
#define INSTSTATUS_H

#include <QDialog>

namespace Ui {
class InstStatus;
}

class InstStatus : public QDialog
{
    Q_OBJECT

public:
    explicit InstStatus(QWidget *parent = nullptr);
    ~InstStatus();
    QString connectionName;

public slots:
   void check();
private slots:
   void recheck();

private:
    Ui::InstStatus *ui;
};

#endif // INSTSTATUS_H
