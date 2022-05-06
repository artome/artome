#ifndef BUILTCOLORSCHEME_H
#define BUILTCOLORSCHEME_H

#include <QDialog>
#include <QTemporaryFile>

namespace Ui {
class builtcolorscheme;
}

class builtcolorscheme : public QDialog
{
    Q_OBJECT
    
public:
    explicit builtcolorscheme(QWidget *parent = 0);
    ~builtcolorscheme();
    QTemporaryFile tempfile; //legenda
    QString palete1;
    int ncol;

private slots:
    void testcolor(void);
    void pressedc1();
    void pressedc2();
    void pressedc3();
    void pressedc4();
    void pressedc5();
    void pressedc6();
    void pressedc7();
    void pressedc8();
    void pressedc9();
    void pressedc10();
    void pressedc11();
    void pressedc12();
    void pressedc13();
    void pressedc14();
    void pressedc15();
    void pressedc16();
    void pressedc17();
    void pressedc18();
    void pressedc19();
    void pressedc20();
    void showhelp();
private:
    Ui::builtcolorscheme *ui;
    QStringList PointColors;
    QPalette palette;
    QBrush brush;
    QColor color;
    QList <bool> pointcolorstatus;

};

#endif // BUILTCOLORSCHEME_H
