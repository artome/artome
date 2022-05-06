#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include <QMimeData>
#include <QImage>


class TextEdit : public QTextEdit
{
    Q_OBJECT
  public:
    TextEdit(QWidget *parent);

    void        dropImage(const QImage& image, const QString& format);

  protected:
    bool        canInsertFromMimeData(const QMimeData *source) const;
    void        insertFromMimeData(const QMimeData *source);
    QMimeData  *createMimeDataFromSelection() const;

};

#endif // TEXTEDIT_H
