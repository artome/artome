#include "timefromruns.h"
#include "ui_timefromruns.h"
#include <QScrollBar>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFile>

#include "help.h"

TimeFromRuns::TimeFromRuns(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimeFromRuns)
{
    ui->setupUi(this);
    Itime.setTimeSpec(Qt::UTC);
    Ltime.setTimeSpec(Qt::UTC);

    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &TimeFromRuns::selectrun);
    connect(ui->ok,&QAbstractButton::pressed,this,&QDialog::accept);
    connect(ui->cancel,&QAbstractButton::pressed,this, &QDialog::reject);
    connect(ui->help,&QAbstractButton::pressed,this, &TimeFromRuns::showhelp);
    connect(ui->Save, &QAbstractButton::pressed,this,&TimeFromRuns::save);
    ui->ok->setDisabled(true);

}

TimeFromRuns::~TimeFromRuns()
{
    delete ui;
}
void TimeFromRuns::fill()
{
  ui->tableView->setModel(model);
  if(model->vscrollpos==-1){
          ui->tableView->scrollToBottom();
          int smin,smax,s;
          smin=ui->tableView->verticalScrollBar()->minimum();
          smax=ui->tableView->verticalScrollBar()->maximum();
          s=smax -(smax-smin)/10;
          ui->tableView->verticalScrollBar()->setSliderPosition (s);

  }else{
      ui->tableView->verticalScrollBar()->setValue(model->vscrollpos);
  }
  clearcheck();
  ui->tableView->resizeColumnToContents(0);
  ui->tableView->resizeColumnToContents(1);
  ui->tableView->resizeColumnToContents(2);
  ui->tableView->show();


}
void TimeFromRuns::clearcheck()
{
    for(int i=0; i<model->rowCount(); i++){
        if(model->startc.at(i) ) {model->startc.replace(i,0);}
        if(model->endc.at(i)  ) {model->endc.replace(i,0);}
    }

}

void TimeFromRuns::check(int r,int c, bool event)
{
   if(c >1) return;
   int icheck=0;

   for(int i=0; i<model->rowCount(); i++){
        if(model->startc.at(i)) icheck++;
        if(model->endc.at(i)) icheck++;

    }

    if(icheck >2){

    for(int i=0; i<model->rowCount(); i++){
        if(model->startc.at(i) && !(i==r && c==0)) {model->startc.replace(i,0);  icheck--;}
        if(icheck <3) break;
        if(model->endc.at(i)  && !(i==r && c==1)) {model->endc.replace(i,0); icheck--;}
        if(icheck <3) break;
    }

    }

    icheck=0;
    Sstart.clear(); Send.clear();
    for(int i=0; i<model->rowCount(); i++){
        if(model->startc.at(i)) {icheck++;
                    if(icheck==1) {
                        Sstart.append(" from start of  ");
                        Sstart.append(model->rownames.at(i));
                        Itime.setSecsSinceEpoch(model->startrun.at(i));
                        xrunnumber=(int)model->rownames.at(i).toDouble();
                    }else{
                        Send.append(" till the start of  ");
                        Send.append(model->rownames.at(i));
                        Ltime.setSecsSinceEpoch(model->startrun.at(i));
                    }
        }
        if(model->endc.at(i)) {icheck++;
            if(icheck==1) {
                Sstart.append(" from end of  ");
                Sstart.append(model->rownames.at(i));
                xrunnumber=(int)model->rownames.at(i).toDouble();
                Itime.setSecsSinceEpoch(model->endrun.at(i));
            }else{
                Send.append(" till the end of ");
                Send.append(model->rownames.at(i));
                Ltime.setSecsSinceEpoch(model->endrun.at(i));

            }
        }

    }
    if(icheck<2){
        Send.append("  End time still not set ");

    }else{ ui->ok->setEnabled(true);}
    ui->timetalklabel->setText(Sstart+ " " + Send);

    //

    int secinterval=Itime.secsTo(Ltime);
    int minutes= secinterval/60;
    int hours=minutes/60;
        minutes=minutes%60;
    int seconds=secinterval%60;
    QString lixo;
    lixo.clear();
    lixo.append("lenght ");
    lixo.append(QString::number(hours));
    lixo.append("h:");
    lixo.append(QString::number(minutes));
    lixo.append("min:");
    lixo.append(QString::number(seconds));
    lixo.append("s");
    ui->TimeInterval->setText(lixo);


    //
}
void TimeFromRuns::checkevent(int r,int c, bool state)
{
   if(c >1) return;
   bool set;
   set=false;
if(c==0){
    if(model->startc.at(r)) {set=true;model->endc.replace(r,1);ui->ok->setEnabled(true);}
    else {model->endc.replace(r,0);ui->ok->setEnabled(false);}
 }else{
    if(model->endc.at(r)) {set=true; model->startc.replace(r,1);ui->ok->setEnabled(true);}
    else   {model->startc.replace(r,0);ui->ok->setEnabled(false);}
 }
model->refresh(); //fundamental para fazer o refresh da tábua

if(!set) return;
    for(int i=0; i<model->rowCount(); i++){
        if(i!=r) {model->startc.replace(i,0); model->endc.replace(i,0); }

    }



    Sstart.clear(); Send.clear();
    Sstart.append(" from start of  ");
    Sstart.append(model->rownames.at(r));
    Itime.setSecsSinceEpoch(model->startrun.at(r));
    xrunnumber=(int)model->rownames.at(r).toDouble();
    Send.append(" till the end of ");
    Send.append(model->rownames.at(r));
    Ltime.setSecsSinceEpoch(model->endrun.at(r));


    ui->ok->setEnabled(true);
    ui->timetalklabel->setText(Sstart+ " " + Send);

    //

    int secinterval=Itime.secsTo(Ltime);
    int minutes= secinterval/60;
    int hours=minutes/60;
        minutes=minutes%60;
    int seconds=secinterval%60;
    QString lixo;
    lixo.clear();
    lixo.append("lenght ");
    lixo.append(QString::number(hours));
    lixo.append("h:");
    lixo.append(QString::number(minutes));
    lixo.append("min:");
    lixo.append(QString::number(seconds));
    lixo.append("s");
    ui->TimeInterval->setText(lixo);


    //
}

void TimeFromRuns::selectrun(const QModelIndex &index)
{
    int row = index.row();

    for(int i=0; i<model->rowCount(); i++){

        if(model->startc.at(i)) {model->startc.replace(i,0); }
        if(model->endc.at(i)) {model->endc.replace(i,0); }
    }

     model->startc.replace(row,1);
     model->endc.replace(row,1);
     model->refresh(); //fundamental para fazer o refresh da tábua

     int icheck=0;
     Sstart.clear(); Send.clear();
     for(int i=0; i<model->rowCount(); i++){
         if(model->startc.at(i)) {icheck++;
                     if(icheck==1) {
                         Sstart.append(" from start of  ");
                         Sstart.append(model->rownames.at(i));
                         Itime.setSecsSinceEpoch(model->startrun.at(i));
                         xrunnumber=model->rownames.at(i).toInt();
                     }else{
                         Send.append(" till the start of  ");
                         Send.append(model->rownames.at(i));
                         Ltime.setSecsSinceEpoch(model->startrun.at(i));
                     }
         }
         if(model->endc.at(i)) {icheck++;
             if(icheck==1) {
                 Sstart.append(" from end of  ");
                 Sstart.append(model->rownames.at(i));
                 Itime.setSecsSinceEpoch(model->endrun.at(i));
                 xrunnumber=(int)model->rownames.at(i).toDouble();
             }else{
                 Send.append(" till the end of  ");
                 Send.append(model->rownames.at(i));
                 Ltime.setSecsSinceEpoch(model->endrun.at(i));

             }
         }

     }
     ui->timetalklabel->setText(Sstart+ " " + Send);
    ui->ok->setEnabled(true);
    int secinterval=Itime.secsTo(Ltime);
    int minutes= secinterval/60;
    int hours=minutes/60;
        minutes=minutes%60;
    int seconds=secinterval%60;
    QString lixo;
    lixo.clear();
    lixo.append("lenght ");
    lixo.append(QString::number(hours));
    lixo.append("h:");
    lixo.append(QString::number(minutes));
    lixo.append("min:");
    lixo.append(QString::number(seconds));
    lixo.append("s");
    ui->TimeInterval->setText(lixo);

}

void TimeFromRuns::save()
{


    QString fileName = QFileDialog::getSaveFileName(this, tr("Save data as"), "runtable.txt", tr("ascii text files (*.csv *.txt *.dat);;All files (*)"));

    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot write file %1:\n%2.")
                                .arg(fileName)
                                .arg(file.errorString()));
        return;
    }

    QTextStream out(&file);

    out.setRealNumberNotation(QTextStream::SmartNotation);
    out.setRealNumberPrecision(10);
    QString lixo;
    lixo.clear();
    QString sep="\t";

    lixo.append(model->headernames[0]);lixo.append(sep);
    lixo.append("Run number "); lixo.append(sep);
    lixo.append(model->headernames[2]);lixo.append(sep);
    lixo.append(model->headernames[3]);lixo.append(sep);
    lixo.append(model->headernames[1]);lixo.append(sep);
    lixo.append(model->headernames[4]);lixo.append(sep);

    out << lixo.toLatin1().constData();
    out << Qt::endl;
    QString timestringformat="yyyy/MM/dd hh:mm:ss";

    for (int i = 0; i < model->rowCount(); i++) {
          lixo.clear();
          lixo.append(QDateTime::fromSecsSinceEpoch(model->startrun.at(i)).toUTC().toString(timestringformat));lixo.append(sep);
          lixo.append(model->rownames[i]); lixo.append(sep);
          lixo.append(model->type[i]); lixo.append(sep);
          lixo.append(model->description[i]); lixo.append(sep);
          lixo.append(QDateTime::fromSecsSinceEpoch(model->endrun.at(i)).toUTC().toString(timestringformat));lixo.append(sep);
          lixo.append(model->commentaries[i]);
          out << lixo.toLatin1().constData()<< Qt::endl;

        }


}

int TimeFromRuns::Getvscrollpos(){
    return ui->tableView->verticalScrollBar()->value();
//return(ui->tableView->verticalScrollBar());
    return 1;
}

void TimeFromRuns::showhelp(){

       Help dialog;
       dialog.Text("<html><head/><body><p>This dialog displays\302\240the experiment runtable\302\240as it is been\302\240saved in cloud-control. One can individually select the start time as the begin or the end\302\240\302\240of any run or stage. </p><p>As only two\302\240ticked \302\240boxes can be ticked at one time if one ticks a third box one of the previous\302\240two ticks is erased, the automatic process not always guess what one wants. Playing save by unchecking the box you do not want to remain checked before checking a third one. </p><p><span style=\" color:#ff0000;\">For convenience double clicking in any cell of a given line will set up that run/stage as initial and final time.</span></p><p>As one can have various opened databases the windows title will remind to which database the runtable belongs. Pressing the ok button will close the windows dialog and will send the information of the time interval selected to every plotwidget belonging to the same database. No need to press ok button in the managing time window "
                                                     "dialog, that is only needed for manual settings of the time interval. Of course future calls for plots or data will use the last chosen time interval. </p><p>If one wants to save a copy of this runtable in the local hard drive just needs to press the button save.\302\240\302\240 </p><p/></body></html>");
       int result;
        result = dialog.exec();

        if(result){

        }


}
void TimeFromRuns::hidesave(){
    ui->Save->setHidden(true);
}
