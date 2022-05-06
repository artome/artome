#ifndef HELP_H
#define HELP_H

#include <QDialog>

namespace Ui {
class Help;
}

class Help : public QDialog
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = nullptr);
    ~Help();
    void fill(QString);
    void Text(QString );

private:
    Ui::Help *ui;
};

#endif // HELP_H
