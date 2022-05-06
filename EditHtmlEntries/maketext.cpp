#include "maketext.h"
#include "qpagesize.h"
#include "ui_maketext.h"
#include "Textedit.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QSettings>
#include <QBuffer>
#include <QImageReader>
#include <QDebug>
#include <QDateTime>
#include <QRegularExpression>
#include "dbapi.h"
#include "addnewentry.h"


MakeText::MakeText(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MakeText)
{
    ui->setupUi(this);

    connect((ui->Export_pdf),&QAbstractButton::clicked,this,&MakeText::filePrintPdf);
    connect((ui->print),&QAbstractButton::clicked,this,&MakeText::filePrint);
    connect((ui->save),&QAbstractButton::clicked,this,&MakeText::fileSaveAs);
    connect(ui->chosesize,&QSpinBox::valueChanged,this,&MakeText::fontsize);
    connect(ui->upload,&QAbstractButton::clicked,this,&MakeText::upload);
    connect(ui->textcolor,&QAbstractButton::clicked,this,&MakeText::textColor);
    connect(ui->backcolor,&QAbstractButton::clicked,this,&MakeText::backColor);
    connect(ui->InsertPicture,&QAbstractButton::clicked,this,&MakeText::insertImage);
    connect(ui->loadfile, &QAbstractButton::clicked, this, &MakeText::loadfile);
    connect(ui->newentry, &QAbstractButton::clicked, this, &MakeText::addentry);
    connect(ui->presenttime, &QAbstractButton::clicked, this, &MakeText::addpresenttime);
    remote=false;


    connect(ui->textEdit, &QTextEdit::currentCharFormatChanged,
            this, &MakeText::currentCharFormatChanged);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged,
            this, &MakeText::cursorPositionChanged);

    connect(ui->textEdit->document(), &QTextDocument::modificationChanged,
            ui->save, &QPushButton::setEnabled);
    connect(ui->textEdit->document(), &QTextDocument::modificationChanged,
            this, &QWidget::setWindowModified);
    connect(ui->textEdit->document(), &QTextDocument::undoAvailable,
            ui->undo, &QPushButton::setEnabled);
    connect(ui->textEdit->document(), &QTextDocument::redoAvailable,
            ui->redo, &QPushButton::setEnabled);
  //  setWindowModified(ui->textEdit->document()->isModified());
    ui->undo->setEnabled(ui->textEdit->document()->isUndoAvailable());
    ui->redo->setEnabled(ui->textEdit->document()->isRedoAvailable());

    connect(ui->TextBold,&QAbstractButton::clicked,this,&MakeText::textBold);
    connect(ui->textunderline,&QAbstractButton::clicked,this,&MakeText::textUnderline);
    connect(ui->TeXtItalic,&QAbstractButton::clicked,this,&MakeText::textItalic);
    connect(ui->textStrike,&QAbstractButton::clicked,this,&MakeText::textStrike);

    connect(ui->Textalignleft,&QAbstractButton::clicked,this,&MakeText::textAlignLeft);
    connect(ui->textalignright,&QAbstractButton::clicked,this,&MakeText::textAlignRight);
    connect(ui->textcenter,&QAbstractButton::clicked,this,&MakeText::textAlignCenter);
    alignmentGroup=new QButtonGroup(this);
    alignmentGroup->addButton(ui->Textalignleft);
    alignmentGroup->addButton(ui->textalignright);
    alignmentGroup->addButton(ui->textcenter);
    ui->Textalignleft->setChecked(true);

    header="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">***************** CLOUD Run.Stage=0000.00 *****************</span></p>\n<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Shift team leader:</span><span style=\" font-size:12pt;\">          </span><span style=\" font-size:12pt; color:#0000ff;\">email:</span><span style=\" font-size:12pt;\">      @      </span></p>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Team member: </span><span style=\" font-size:12pt;\">            </span><span style=\" font-size:12pt; color:#0000ff;\">email: </span><span style=\" font-size:12pt; color:#000000;\">     @   </span></p>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">********************************</span></p></body></html>";

    connect(ui->textEdit, &QTextEdit::textChanged,this,&MakeText::textedited);
    Texthaschanged=false;

}

MakeText::~MakeText()
{
    delete ui;
}

void MakeText:: textedited(){
    Texthaschanged=true;
}
void MakeText::insertImage() {
    QSettings s;
    QString attdir = s.value("logBook/imagepath").toString();
    QString file = QFileDialog::getOpenFileName(this,
                                    tr("Select an image"),
                                    attdir,
                                    tr("PNG (*.png);; JPEG (*.jpg);; GIF (*.gif);;  BMP (*.bmp);; All (*)"));
    QImage image = QImageReader(file).read();

    ui->textEdit->dropImage(image, QFileInfo(file).suffix().toUpper().toLocal8Bit().data() );

}
void MakeText::insertImageFile(QString file) {

    QImage image = QImageReader(file).read();

    ui->textEdit->append(" \n *******************Add plot name and other text ****************\n");
    ui->textEdit->dropImage(image, QFileInfo(file).suffix().toUpper().toLocal8Bit().data() );

}
void MakeText::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}

void MakeText::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->TextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void MakeText::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->textunderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}


void MakeText::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->TeXtItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MakeText::textStrike()
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(ui->textStrike->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}


void MakeText::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}



void MakeText::fontsize(int size)
{
    textSize(QString::number(size));
}



void MakeText::textSize(const QString &p)
{

    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}



void MakeText::textStyle(int styleIndex)
{
    QTextCursor cursor = ui->textEdit->textCursor();

    if (styleIndex != 0) {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex) {
            default:
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}



void MakeText::textColor()
{
    QColor col = QColorDialog::getColor(ui->textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
   // colorChanged(col);
}



void MakeText::backColor()
{
    QColor col = QColorDialog::getColor(ui->textEdit->textBackgroundColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setBackground(col);
    mergeFormatOnWordOrSelection(fmt);
   // colorChanged(col);
}


void MakeText::textAlignLeft()
{
    ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
}

void MakeText::textAlignCenter()
{
        ui->textEdit->setAlignment(Qt::AlignHCenter);

}

void MakeText::textAlignRight()
{

        ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);

}

void MakeText::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        ui->Textalignleft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        ui->textcenter->setChecked(true);
    else if (a & Qt::AlignRight)
        ui->textalignright->setChecked(true);

}

void MakeText::currentCharFormatChanged(const QTextCharFormat &format)
{
 //   fontChanged(format.font());
  //  colorChanged(format.foreground().color());
}

void MakeText::cursorPositionChanged()
{
    alignmentChanged(ui->textEdit->alignment());
}

void MakeText::clipboardDataChanged()
{
/*
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        ui->actionPaste->setEnabled(md->hasText());
#endif
*/
}
/*
void MakeText::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    ui->actiontext_color->setIcon(pix);
}
*/

void MakeText::loadfile()
{
    if (Texthaschanged) {
        QMessageBox msgBox;
        msgBox.setText("The Entry has been modified.");
        msgBox.setInformativeText("Do you want to ignore the changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Yes:
            // Save was clicked
            break;

        case QMessageBox::Cancel:
            // Cancel was clicked
            return;
            break;
        default:
            // should never be reached
            break;
        }
    }
    disconnect(ui->textEdit, &QTextEdit::textChanged, this, &MakeText::textedited);
    QString  filename;
    QStringList filesnames;

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDirectory(folder);
    if (dialog.exec())
        filesnames = dialog.selectedFiles();
    if (filesnames.count())
        filename = filesnames.at(0);
    else
        return;

    QFile file;
    file.setFileName(filename);
   // qDebug() << "lixo";

    if (!file.open(QFile::ReadOnly)) {
        QMessageBox msgBox;
        msgBox.setText("Unable to open file.");
        msgBox.exec();
        return;
    }

        QTextStream outfile(&file);
        ui->textEdit->clear();
        ui->textEdit->setHtml(outfile.readAll());
        file.close();
        Texthaschanged = false;

        writetextname(filename);
    connect(ui->textEdit, &QTextEdit::textChanged, this, &MakeText::textedited);
}


bool MakeText::fileSaveAs()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "application/vnd.oasis.opendocument.text" << "text/html" << "text/plain";
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.setDefaultSuffix("html");
    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString fn = fileDialog.selectedFiles().constFirst();
    setCurrentFileName(fn);
    return fileSave();
}


void MakeText::filePrint()
{
#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (ui->textEdit->textCursor().hasSelection())
        dlg->setOption(QAbstractPrintDialog::PrintSelection,true);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        ui->textEdit->print(&printer);
    delete dlg;
#endif
}

void MakeText::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().constFirst();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A3);
    ui->textEdit->document()->print(&printer);
  //  statusBar()->showMessage(tr("Exported \"%1\"")
  //                           .arg(QDir::toNativeSeparators(fileName)));
//! [0]
#endif
}


void MakeText::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    ui->textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
   // setWindowModified(false);
    if(!htmledit) writetextname(fileName);
}

bool MakeText::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();
    if (fileName.startsWith(QStringLiteral(":/")))
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    bool success = writer.write(ui->textEdit->document());
    if (success) {
        ui->textEdit->document()->setModified(false);

     //   statusBar()->showMessage(tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName)));
    } else {
       // statusBar()->showMessage(tr("Could not write to file \"%1\"")
                              //   .arg(QDir::toNativeSeparators(fileName)));
    }
    return success;
}


QString MakeText::toHtml() const {


    QString s = ui->textEdit->toHtml();
    // convert emails to links
    s = s.replace(QRegularExpression("(<[^a][^>]+>(?:<span[^>]+>)?|\\s)([a-zA-Z\\d]+@[a-zA-Z\\d]+\\.[a-zA-Z]+)"), "\\1<a href=\"mailto:\\2\">\\2</a>");
    // convert links
   s = s.replace(QRegularExpression("(<[^a][^>]+>(?:<span[^>]+>)?|\\s)((?:https?|ftp|file)://[^\\s'\"<>]+)"), "\\1<a href=\"\\2\">\\2</a>");
    // see also: Utils::linkify()
    return s;
}
void MakeText::upload()
{
    QFile Fileout;

    if(!htmledit)
    {
       if(Texthaschanged){
        Fileout.setFileName(ui->filename->text());
        if(!Fileout.open(QFile::WriteOnly)){
        emit    emiterrorR("It was not possible to open the file to save\n");
            return;
        }
        QTextStream outfile(&Fileout);

        outfile <<toHtml();

        Fileout.close();

       }
       Texthaschanged=false;
        return;

}
    QString name;
    if(Texthaschanged){
        QDateTime Interfacetime;
        Interfacetime = Interfacetime.currentDateTimeUtc();
        Interfacetime.setTimeSpec(Qt::UTC);

        name=rootname;
        name.append(ext);
        Fileout.setFileName(folder+name);
        if(Fileout.exists()){

            QString file1=folder+name;
            QString file2=folder+sfolder+"/"+rootname+"-"+Interfacetime.toString("yyyy_MM_dd_hh_mm_ss")+extb;
            QFile::copy(file1,file2);

        }

    }


    name=rootname;
    name.append(ext);

    Fileout.setFileName(folder+name);
    if(!Fileout.open(QFile::WriteOnly)){
    emit    emiterrorR("It was not possible to open the file to save\n");
        return;
    }
    QTextStream outfile(&Fileout);

    outfile <<toHtml();

    Fileout.close();
    if(remote) return;

              dbapi table;
              table.connectionName=databaseconnection;
              connect (&table,&dbapi::logmessages,this,&MakeText::LogMessages);
              connect (&table,&dbapi::logmessagesR,this,&MakeText::LogMessagesR);


              QString prepare;
              QStringList binds;
              binds.clear();
               prepare="INSERT INTO ";
               prepare.append(listtable);
               prepare.append("  (");
               prepare.append(keyname);
               prepare.append(",ENTRY) VALUES (?,?) ");
              prepare.append(" ON DUPLICATE KEY UPDATE ENTRY=VALUES(ENTRY)");
              binds<<rootname<<toHtml();
              table.preparebind(prepare,&binds);
              Texthaschanged = false;

}

void MakeText::fillcanvas(QString text)
{
    disconnect(ui->textEdit, &QTextEdit::textChanged, this, &MakeText::textedited);
    ui->textEdit->clear();
    ui->textEdit->setHtml(text);
    Texthaschanged = false;
    connect(ui->textEdit, &QTextEdit::textChanged, this, &MakeText::textedited);

}
void MakeText::addentry()
{
    addnewentry dialog;
    if(dialog.exec()){
        keyname=dialog.newentry;
        writetextname(keyname);
        rootname=keyname;
    }
}

void MakeText::changeuploadicon(int i)
{
    QIcon icon2;
    if (i == 3) {
        icon2.addFile(QString::fromUtf8(":/Icons/Icons/floppy1.svg"),
                      QSize(),
                      QIcon::Normal,
                      QIcon::Off);

    }  else {

        icon2.addFile(QString::fromUtf8(":/Icons/Icons/uploadhtml.svg"),
                      QSize(),
                      QIcon::Normal,
                      QIcon::Off);

    }
        ui->upload->setIcon(icon2);
}

void MakeText::Fieldchange(QString field){
    disconnect(ui->textEdit, &QTextEdit::textChanged,this,&MakeText::textedited);
    QFile Fileout;
    QString name;
    if(Texthaschanged){
 /*       name=rootname;
        name.append(ext);
        Fileout.setFileName(folder+name);
        if(Fileout.exists()){

            QString file1=folder+name;
            QString file2=folder+sfolder+"/"+rootname+"-"+Interfacetime->toString("yyyy_MM_dd_hh_mm_ss")+extb;
            QFile::copy(file1,file2);

        }
*/  //it as comented as upload also do the backup thing. So it was producing duplicated backups
        upload();
    }
    if(field=="CloseEvent") return;
    writetextname(field);
    rootname=field;
    name=rootname;
    name.append(ext);
    Fileout.setFileName(folder+name);
    if(Fileout.exists()){
        if (!Fileout.open(QFile::ReadOnly))
            return;
        QTextStream outfile(&Fileout);
        ui->textEdit->clear();
        ui->textEdit->setHtml(outfile.readAll());
        Fileout.close();
        Texthaschanged=false;
    }else{
        if(listtable=="InstDescription"){
        ui->textEdit->clear();
        dbapi table;
        QString text;
        table.connectionName=databaseconnection;
        table.fechastringcell(listtable,"NAMEID",rootname,"ENTRY",&text);
        ui->textEdit->setHtml(text);
        }
        if(listtable=="LOGTABLE"){
            ui->textEdit->clear();
            QString lixo;
            lixo=header;
            lixo=lixo.replace("0000.00",rootname);
            ui->textEdit->append(lixo);
        }
        Texthaschanged=false;
    }
    // aqui o upload do último ficheiro

    connect(ui->textEdit, &QTextEdit::textChanged,this,&MakeText::textedited);


}

void MakeText::addpresenttime()
{
    QTextCursor cursor(ui->textEdit->textCursor());
    int pos=cursor.position();
    cursor.setPosition(pos,cursor.MoveAnchor);
    ui->textEdit->setTextCursor(cursor);

    ui->textEdit->insertPlainText(QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss"));
}
void MakeText::hideloadfile(){
    ui->loadfile->setHidden(true);
}
void MakeText::hidenewentry()
{
    ui->newentry->setHidden(true);
}
void MakeText::reportmode()
{
    ui->newentry->setHidden(true);
    ui->upload->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/floppy1.svg")));
    ui->upload->setToolTip("save the file into hard drive");
}

void MakeText::writetextname(QString name){
    ui->filename->setText(name);
}
void MakeText::LogMessages(QString mess){
    emit emiterror(mess);
}void MakeText::LogMessagesR(QString mess){
    emit emiterrorR(mess);
}
