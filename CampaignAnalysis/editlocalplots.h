#ifndef EDITLOCALPLOTS_H
#define EDITLOCALPLOTS_H

#include <QDialog>
#include <QStandardItemModel>
namespace Ui {
class Editlocalplots;
}

class Editlocalplots : public QDialog
{
    Q_OBJECT

public:
    explicit Editlocalplots(QWidget *parent = 0);
    ~Editlocalplots();
   void fillplots();
public slots:
    void plotsfilltree();
signals:
    void plots(QList<QVariant>,QString );

private slots:
    void filltyplots();
    void fillSplots();
    void fillHplots();
    void fillAplots();
    void commitplot();
    void sellect(QStandardItem *);
    void sellectd(QStandardItem *);
    void LogMessages(QString );
    void LogMessagesR(QString );
    void deleteplots();

private:
    Ui::Editlocalplots *ui;
    QStandardItemModel listplotmodel;
    QStandardItemModel dlistplotmodel;
    QStandardItem   *Graphitem  ;
    QStandardItem   *Bananaitem ;
    QStandardItem   *Histoitem  ;
    QStandardItem   *Screenitem  ;

};

#endif // EDITLOCALPLOTS_H
