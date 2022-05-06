#ifndef LOADLOCALFILE_H
#define LOADLOCALFILE_H

#include <QDialog>

namespace Ui {
class loadLocalFile;
}

class loadLocalFile : public QDialog
{
    Q_OBJECT

public:
    explicit loadLocalFile(QWidget *parent = 0);
    ~loadLocalFile();
    QString fileName,timeString,separator;
    QString columnstoload,timecolumns;
    int igl;
    void specialbanana();
    void settitle(QString);
    bool formatted;
private slots:

    void doIt();
    void readvalues();
    void readvaluesF();
    void settimestrings(int);
    void savetimesettings();


private:
    Ui::loadLocalFile *ui;
    void filltimestrings();
    void readtimesettings();

};

#endif // LOADLOCALFILE_H
