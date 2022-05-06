#ifndef SAVETABLE_H
#define SAVETABLE_H

#include <QDialog>

namespace Ui {
class SaveTable;
}

class SaveTable : public QDialog
{
    Q_OBJECT
    
public:
    explicit SaveTable(QWidget *parent = nullptr);
    ~SaveTable();
    QString fileName,timeString,separator;
    QString suggestname;
    void hidetimeoptions();

private slots:
    void doIt();
    void readvalues();
    void settimestrings(int);
    void savetimesettings();

private:
    Ui::SaveTable *ui;
    void filltimestrings();
    void readtimesettings();

};

#endif // SAVETABLE_H
