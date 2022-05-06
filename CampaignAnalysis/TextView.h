#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QTextCharFormat>
#include <QMainWindow>
#include <QSpinBox>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QColorDialog>
#include <QActionGroup>
#include <QMimeData>
#include <QStandardItemModel>

namespace Ui {
class TextView;
}

class TextView : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextView(QWidget *parent = 0);
    ~TextView();
    QString databaseconnection;
void    desactivateupload();
bool superuser;
bool instrument,tablecomment;
void settitle(QString);
void filltree();
QString Tableentry;

private slots:
    bool fileSave();
    bool fileSaveAs();
    void filePrint();
    void filePrintPdf();

     void textfind();
    void textfindnext();

    void clipboardDataChanged();

    void LogMessages(QString);
    void LogMessagesR(QString);
    void readEntry(QModelIndex);
    void insertEntry(QString);
    void upload();

private:
    Ui::TextView *ui;
    QString fileName;
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
   void alignmentChanged(Qt::Alignment a);
    void setCurrentFileName(const QString &fileName);
    QString toHtml() const;
    QString header;
    QStandardItemModel treemodel;
    void sortNonCaseSensitive(QStringList&);
    QTextCursor savedhighlightCursor;
    bool searchstatus;
    QString stringtosearch;
    QString entryname;


};

#endif // TEXTVIEW_H
