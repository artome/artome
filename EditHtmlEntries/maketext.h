#ifndef MAKETEXT_H
#define MAKETEXT_H

#include <QButtonGroup>
#include <QColorDialog>
#include <QDateTime>
#include <QFile>
#include <QFontComboBox>
#include <QMimeData>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QTextStream>
#include <QWidget>

namespace Ui {
class MakeText;
}

class MakeText : public QWidget {
  Q_OBJECT

public:
  explicit MakeText(QWidget *parent = 0);
  ~MakeText();
  QString folder;
  QString ext, extb, fileName, rootname,sfolder;
  QString header, databaseconnection, type;
  bool Texthaschanged;
  QDateTime *Interfacetime;
  bool remote;
  void fillcanvas(QString);
  QString listtable,keyname;
  void Fieldchange(QString);
  void hideloadfile();
  bool htmledit;
  void writetextname(QString);
  void hidenewentry();
  void reportmode();
  void insertImageFile(QString );

signals:
  void emiterrorR(QString);
  void emiterror(QString);

public slots:
  void changeuploadicon(int );
  void LogMessages(QString);
  void LogMessagesR(QString);

private slots:
  void loadfile();
  bool fileSave();
  bool fileSaveAs();
  void filePrint();
  //   void filePrintPreview();
  void filePrintPdf();

  void textBold();
  void textUnderline();
  void textItalic();
  void textStrike();
  void textFamily(const QString &f);
  void textSize(const QString &p);
  void textStyle(int styleIndex);
  void textColor();
  void textAlignLeft();
  void textAlignRight();
  void textAlignCenter();

  void backColor();

  void currentCharFormatChanged(const QTextCharFormat &format);
  void cursorPositionChanged();

  void clipboardDataChanged();
  void fontsize(int);
  void textedited();
  void addentry();

  void addpresenttime();


  //

  void upload();

  void insertImage();

private:
  Ui::MakeText *ui;
  void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
  void mergeFormatOnWordOrSelection_I(const QTextCharFormat &format);
  //  void colorChanged(const QColor &c);
  void alignmentChanged(Qt::Alignment a);
  void setCurrentFileName(const QString &fileName);
  QString toHtml() const;
  QButtonGroup *alignmentGroup;


};

#endif // MAKETEXT_H
