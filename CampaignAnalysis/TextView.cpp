#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QSettings>
#include <QBuffer>
#include<QImageReader>
#include <QDebug>
//#include <QTextDocumentFragment>
#include <QMessageBox>

#include "TextView.h"
#include "ui_TextView.h"
#include "dbapi.h"
#include "searchtext.h"

#include <QDebug>
TextView::TextView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextView)
{
    ui->setupUi(this);
  //  ui->mainToolBar->addWidget(&chosefonte);
  //    ui->mainToolBar->insertWidget(ui->actionsave,&chosefonte);
      connect(ui->textEdit->document(), &QTextDocument::modificationChanged,
              ui->actionsave, &QAction::setEnabled);
      connect(ui->textEdit->document(), &QTextDocument::modificationChanged,
              this, &QWidget::setWindowModified);
      connect((ui->actionExport_pdf),&QAction::triggered,this,&TextView::filePrintPdf);
      connect((ui->actionprint),&QAction::triggered,this,&TextView::filePrint);
      connect((ui->actionsave),&QAction::triggered,this,&TextView::fileSaveAs);
      connect(ui->actionsearch,&QAction::triggered,this,&TextView::textfind);
      connect(ui->actionsearchnext,&QAction::triggered,this,&TextView::textfindnext);

      connect(ui->treeView,&QAbstractItemView::doubleClicked,this,&TextView::readEntry);

      header="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">***************** CLOUD Run.Stage=0000.00 *****************</span></p>\n<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Shift team leader:</span><span style=\" font-size:12pt;\"> AAAAAAA BBBBBB </span><span style=\" font-size:12pt; color:#0000ff;\">email:</span><span style=\" font-size:12pt;\"> GGGGGG@HHHHHH</span></p>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Team member: </span><span style=\" font-size:12pt;\">DDDDDD DDDDDD </span><span style=\" font-size:12pt; color:#0000ff;\">email: </span><span style=\" font-size:12pt; color:#000000;\">IIIIII@IIIIII</span></p>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">********************************</span></p></body></html>";
 //     setWindowTitle(tr("LogBook[*]"));
      instrument=false;
      tablecomment=false;
      searchstatus=false;
      stringtosearch.clear();
}

TextView::~TextView()
{
    delete ui;
}
void  TextView::sortNonCaseSensitive(QStringList& sList)
{
    ///  Sorts the passed sList non-case-sensitively.
    ///  (Preserves the cases! Just doesn't use them
    ///  while sorting.)
    QMap<QString, QString> strMap;
    foreach(QString str, sList) {
        strMap.insert(str.toLower(), str);
    }
    sList = strMap.values();
}

void TextView::settitle(QString title){
     setWindowTitle(title);
}
void TextView::textfind(){
    searchtext dialog;
    dialog.settext(stringtosearch);
    if(dialog.exec()){
      stringtosearch=dialog.stringtosearch;
     bool found = false;

     if (stringtosearch.isEmpty()) {
         QMessageBox::information(this, tr("Empty Search Field"),
                                  tr("The search field is empty. "
                                     "Please enter a word and click Find."));
     } else {
         searchstatus=false;
    QTextCursor highlightCursor( ui->textEdit->document());
    QTextCursor cursor (ui->textEdit->document());

    cursor.beginEditBlock();

    QTextCharFormat plainFormat(highlightCursor.charFormat());
    QTextCharFormat colorFormat = plainFormat;
    colorFormat.setForeground(Qt::red);
    while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
        highlightCursor = ui->textEdit->document()->find(stringtosearch, highlightCursor,
                                         QTextDocument::FindWholeWords);

        if (!highlightCursor.isNull()) {
            found = true;
            highlightCursor.movePosition(QTextCursor::WordRight,
                                         QTextCursor::KeepAnchor);
            highlightCursor.mergeCharFormat(colorFormat);
            ui->textEdit->setTextCursor(highlightCursor);
            savedhighlightCursor=highlightCursor;
            searchstatus=true;
            break;

        }

    }
     cursor.endEditBlock();
}
     if (found == false) {
                 QMessageBox::information(this, tr("Word Not Found"),
                                          tr("Sorry, the word cannot be found."));
             }

}
}
void TextView::textfindnext(){

     bool found = false;

  restart:   if (stringtosearch.isEmpty()) {
         QMessageBox::information(this, tr("Empty Search Field"),
                                  tr("The search field is empty. "
                                     "Please enter a word and click Find."));
     } else {
    QTextCursor highlightCursor( ui->textEdit->document());
    QTextCursor cursor (ui->textEdit->document());

    cursor.beginEditBlock();

    QTextCharFormat plainFormat(highlightCursor.charFormat());
    QTextCharFormat colorFormat = plainFormat;
    colorFormat.setForeground(Qt::red);
    if(searchstatus) highlightCursor=savedhighlightCursor;
    while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
        highlightCursor = ui->textEdit->document()->find(stringtosearch, highlightCursor,
                                         QTextDocument::FindWholeWords);

        if (!highlightCursor.isNull()) {
            found = true;
            highlightCursor.movePosition(QTextCursor::WordRight,
                                         QTextCursor::KeepAnchor);
            highlightCursor.mergeCharFormat(colorFormat);
            ui->textEdit->setTextCursor(highlightCursor);
            savedhighlightCursor=highlightCursor;
            searchstatus=true;
            break;

        }

    }
      cursor.endEditBlock();
}
     if (found == false) {
         QMessageBox msgBox;
         msgBox.setText("Word Not Found");
         msgBox.setInformativeText("Do you want to start from beginning?");
         msgBox.setStandardButtons(QMessageBox::Yes |  QMessageBox::Cancel);
         msgBox.setDefaultButton(QMessageBox::Yes);
         int ret = msgBox.exec();
         switch (ret) {
           case QMessageBox::Yes:
               searchstatus=false;
               goto restart;
               break;
           case QMessageBox::Cancel:
               // Cancel was clicked
               break;
           default:
               // should never be reached
               break;
         }
             }


}
void TextView::filltree(){

    treemodel.clear();

    dbapi table;
    table.connectionName=databaseconnection;
    //connect (&newtable,SIGNAL(logmessages(QString)),this,SLOT(LogMessages(QString)));
    //connect (&newtable,SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesR(QString)));
    QStringList rootData;
    QString     ttip;
    QList<QVariant> grafid;
    QStandardItem *parentItem = treemodel.invisibleRootItem();
    QDateTime time;
    QString Stime;

    if(instrument) rootData << "Instruments";  else if(tablecomment) rootData << "Data table comments";
    else rootData << "Run/stages";
    treemodel.setHorizontalHeaderLabels(rootData);
 /*   if(!(instrument || tablecomment)){
            grafid.clear();
             grafid <<-1<< "empty entry";
             QStandardItem *item = new QStandardItem("all entries");
             item->setData(grafid);
             item->setEditable(false);
             parentItem->appendRow(item);

        }*/ //desactivei a junção de todas as entradas.
    QList<std::vector<QString> * >  itens;

      QStringList cnames;
      cnames.clear();
      if (!(instrument || tablecomment)) cnames <<"RunNumber" <<"type" <<"comments" <<"StartTime";
      else cnames <<"NAMEID" <<"NAME";

      for(int i=0; i<cnames.count(); i++) itens << new ( std::vector<QString>);
      //if (!instrument) table.fechnotimecolumns_string("Runtable",cnames,&itens);
      if (!(instrument || tablecomment)) table.fechnotimecolumns_string_orderby("Runtable",cnames,"RunNumber",&itens);

      else table.fechnotimecolumns_string(Tableentry,cnames,&itens);

      if(instrument || tablecomment ){
          QStringList insts;
          insts.clear();
          for(int i=0; i<(int )itens.at(0)->size(); i++) insts <<itens.at(0)->at(i);
          sortNonCaseSensitive(insts);

      for(int i=0; i<insts.count(); i++){
          grafid.clear();
          QStandardItem *item = new QStandardItem(insts.at(i));
          grafid << insts.at(i);
          item->setData(grafid);
          item->setEditable(false);
          ttip=insts.at(i);
          item->setToolTip(ttip);
          parentItem->appendRow(item);
      }}else{

          QStringList run, runroot;
          QList <bool> runalready;
          runalready.clear();
          bool erun;
          for(int i=0; i<(int )itens.at(0)->size(); i++){
          run=itens.at(0)->at(i).split(".");
          erun=false;

          for(int j=0; j<runroot.count();j++ )
          {
              if (run.at(0)==runroot.at(j)){erun=true; break;}
          }
          if(!erun) runroot << run.at(0);
          }
          for(int j=0; j<runroot.count();j++ )
          {
               grafid.clear();
             grafid <<0<< runroot.at(j);
              QStandardItem *item = new QStandardItem(runroot.at(j));
              item->setData(grafid);
              item->setEditable(false);
              parentItem->appendRow(item);
          }

           for(int i=0; i<(int )itens.at(0)->size(); i++){
             grafid.clear();
          QStandardItem *item = new QStandardItem(itens.at(0)->at(i));
             time=time.toUTC().fromMSecsSinceEpoch(itens.at(3)->at(i).toLongLong() );
             Stime=time.toUTC().toString("yyyy/MM/dd hh:mm:ss");
 //         QStandardItem *item = new QStandardItem(Stime);

          grafid << 1<< itens.at(0)->at(i);
          item->setData(grafid);
          item->setEditable(false);
          ttip=itens.at(0)->at(i)+" type:"+itens.at(1)->at(i)+"->"+Stime;
          item->setToolTip(ttip);
          run=itens.at(0)->at(i).split(".");
          for(int j=0; j<runroot.count();j++ )
          {
 //             if (run.at(0)==runroot.at(j)){parentItem->child(j+1)->appendRow(item); //resultou de desactivar as all entries
              if (run.at(0)==runroot.at(j)){parentItem->child(j)->appendRow(item);
              //    if(runalready.count()<j+1){//resultou de desactivar as all entries
                if(runalready.count()<=j){
                  Stime=Stime.prepend("->");
               //   Stime=Stime.prepend(parentItem->child(j+1)->text());//resultou de desactivar as all entries
                  Stime=Stime.prepend(parentItem->child(j)->text());
              //    parentItem->child(j+1)->setText(Stime);//resultou de desactivar as all entries
                  parentItem->child(j)->setText(Stime);
                  runalready <<true;}
                  break;}
          }


          }

}

      ui->treeView->setModel(&treemodel);
      ui->treeView->show();




}
void TextView::readEntry(QModelIndex index){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    ui->textEdit->setReadOnly(false);
    dbapi table;
    table.connectionName=databaseconnection;
    //connect (&newtable,SIGNAL(logmessages(QString)),this,SLOT(LogMessages(QString)));
    //connect (&newtable,SIGNAL(logmessagesR(QString)),this,SLOT(LogMessagesR(QString)));
    QStringList cnames;
    cnames.clear();
    cnames <<"ENTRY";
    QList <std::vector<QString> * > c;

    for(int i=0; i<cnames.count(); i++) c << new ( std::vector<QString>);

    QList<QVariant> list,list1;
    list = treemodel.itemFromIndex(index)->data().toList();

    if(instrument||tablecomment){table.fechnotimecolumns_string_filterstring(Tableentry, cnames,"NAMEID",list[0].toString() , &c );
        ui->textEdit->clear();
       if (c.at(0)->size()){
            ui->textEdit->setHtml(c.at(0)->at(0));
         }
       entryname=list[0].toString();
    }
    else {
        QStringList cdnames;
        cdnames.clear();
        cdnames <<"Type" <<"Description"<<"comments";

        QList <std::vector<QString> * > d;
        for(int i=0; i<cdnames.count(); i++) d << new ( std::vector<QString>);

        if(list[0].toInt()==0){

             ui->textEdit->clear();
            for(int j=0; j<treemodel.itemFromIndex(index)->rowCount(); j++){
              list1 = treemodel.itemFromIndex(index)->child(j)->data().toList();
              for(int k=0; k<c.count();k++) c.at(k)->clear();
        table.fechnotimecolumns_string_filterstring("LOGTABLE", cnames,"RunNumber",list1[1].toString(), &c );


         if (c.at(0)->size())
         {
  //         QString plainText = QTextDocumentFragment::fromHtml(c.at(0)->at(0)).toPlainText();
            for(int k=0; k<d.count();k++) d.at(k)->clear();
          table.fechnotimecolumns_string_filterstring("Runtable", cdnames,"RunNumber",list1[1].toString() , &d );
          ui->textEdit->setTextColor(Qt::blue);
          ui->textEdit->append("************"+list1[1].toString()+"************");
           ui->textEdit->setAlignment(Qt::AlignLeft);
           if(d.at(0)->size()) ui->textEdit->append("Type: "+d.at(0)->at(0));
            if(d.at(1)->size()) ui->textEdit->append("Description: "+d.at(1)->at(0));
           if(d.at(2)->size()) ui->textEdit->append("Comments:"+d.at(2)->at(0));
           ui->textEdit->setTextColor(Qt::black);
            ui->textEdit->append(c.at(0)->at(0));


 //for(int i=0; i<cdnames.count(); i++) delete d.at(i);

         }

            }
             entryname="NOTHINGTOBESAVED";
           // ui->treeView->collapse(index);
        }    else  if(list[0].toInt()==-1){

            ui->textEdit->clear();
         for(int k=0;k<treemodel.rowCount();k++){
             list=treemodel.itemFromIndex(treemodel.index(k,0))->data().toList();
             if(list[0].toInt()==-1) continue;


           for(int j=0; j<treemodel.itemFromIndex(treemodel.index(k,0))->rowCount(); j++){
             list1 = treemodel.itemFromIndex(treemodel.index(k,0))->child(j)->data().toList();
             for(int kk=0; kk<c.count();kk++) c.at(kk)->clear();
       table.fechnotimecolumns_string_filterstring("LOGTABLE", cnames,"RunNumber",list1[1].toString(), &c );


        if (c.at(0)->size())
        {
 //         QString plainText = QTextDocumentFragment::fromHtml(c.at(0)->at(0)).toPlainText();
             for(int k=0; k<d.count();k++) d.at(k)->clear();
            table.fechnotimecolumns_string_filterstring("Runtable", cdnames,"RunNumber",list1[1].toString() , &d );
            ui->textEdit->setTextColor(Qt::blue);
            ui->textEdit->append("************"+list1[1].toString()+"************");
            ui->textEdit->setAlignment(Qt::AlignLeft);
             if(d.at(0)->size()) ui->textEdit->append("Type: "+d.at(0)->at(0));
              if(d.at(1)->size()) ui->textEdit->append("Description: "+d.at(1)->at(0));
             if(d.at(2)->size()) ui->textEdit->append("Comments:"+d.at(2)->at(0));
             ui->textEdit->setTextColor(Qt::black);

           ui->textEdit->append(c.at(0)->at(0));



        }
           }

       }
 entryname="NOTHINGTOBESAVED";
      }else{
        table.fechnotimecolumns_string_filterstring("LOGTABLE", cnames,"RunNumber",list[1].toString(), &c );
        ui->textEdit->clear();
       if (c.at(0)->size()){

           for(int k=0; k<d.count();k++) d.at(k)->clear();
          table.fechnotimecolumns_string_filterstring("Runtable", cdnames,"RunNumber",list[1].toString() , &d );
          ui->textEdit->setTextColor(Qt::blue);
          ui->textEdit->append("************"+list[1].toString()+"************");
          ui->textEdit->setAlignment(Qt::AlignLeft);
           if(d.at(0)->size()) ui->textEdit->append("Type: "+d.at(0)->at(0));
            if(d.at(1)->size()) ui->textEdit->append("Description: "+d.at(1)->at(0));
           if(d.at(2)->size()) ui->textEdit->append("Comments:"+d.at(2)->at(0));
           ui->textEdit->setTextColor(Qt::black);
            ui->textEdit->append(c.at(0)->at(0));
       }
        entryname=list[1].toString();
        }
    }
   // if (!c.at(0)->size()) return;
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    ui->actionsave->setEnabled(true);

    ui->textEdit->setReadOnly(true);

}





void TextView::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}








void TextView::clipboardDataChanged()
{
/*
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        ui->actionPaste->setEnabled(md->hasText());
#endif
*/
}
/*
void MainWindow::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    ui->actiontext_color->setIcon(pix);
}
*/

bool TextView::fileSaveAs()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "application/vnd.oasis.opendocument.text" << "text/html" << "text/plain";
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.setDefaultSuffix("odt");
    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString fn = fileDialog.selectedFiles().first();
    setCurrentFileName(fn);
    return fileSave();
}
void TextView::filePrint()
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

void TextView::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    ui->textEdit->document()->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));
//! [0]
#endif
}
void TextView::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    ui->textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}
bool TextView::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();
    if (fileName.startsWith(QStringLiteral(":/")))
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    bool success = writer.write(ui->textEdit->document());
    if (success) {
        ui->textEdit->document()->setModified(false);
        statusBar()->showMessage(tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName)));
    } else {
        statusBar()->showMessage(tr("Could not write to file \"%1\"")
                                 .arg(QDir::toNativeSeparators(fileName)));
    }
    return success;
}
QString TextView::toHtml() const {


    QString s = ui->textEdit->toHtml();
    // convert emails to links
    s = s.replace(QRegularExpression("(<[^a][^>]+>(?:<span[^>]+>)?|\\s)([a-zA-Z\\d]+@[a-zA-Z\\d]+\\.[a-zA-Z]+)"), "\\1<a href=\"mailto:\\2\">\\2</a>");
    // convert links
   s = s.replace(QRegularExpression("(<[^a][^>]+>(?:<span[^>]+>)?|\\s)((?:https?|ftp|file)://[^\\s'\"<>]+)"), "\\1<a href=\"\\2\">\\2</a>");
    // see also: Utils::linkify()
    return s;
}
void TextView::upload()
{
  if(entryname.contains("NOTHINGTOBESAVED")) return;
  insertEntry(entryname);
ui->textEdit->document()->setModified(false);
}

void TextView::insertEntry(QString text){

 dbapi  table;
 table.connectionName=databaseconnection;
 connect (&table,&dbapi::logmessages,this,&TextView::LogMessages);
 connect (&table,&dbapi::logmessagesR,this,&TextView::LogMessagesR);
 QString prepare;
 QStringList binds;
 binds.clear();
 if(!(instrument || tablecomment)) prepare="INSERT INTO LOGTABLE (RunNumber,ENTRY) VALUES (?,?) ";
 else  {
     prepare="INSERT INTO ";
     prepare.append(Tableentry);
     prepare.append(" (NAMEID,ENTRY) VALUES (?,?) ");
 }
 prepare.append(" ON DUPLICATE KEY UPDATE ENTRY=VALUES(ENTRY)");
 binds<<text<<toHtml();
 table.preparebind(prepare,&binds);


}

void TextView::LogMessagesR(QString mess){
    qDebug() << mess;

}
void TextView::LogMessages(QString mess){
    qDebug() << mess;

}
