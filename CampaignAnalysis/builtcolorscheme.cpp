
#include <QColorDialog>
#include <QMessageBox>
#include "builtcolorscheme.h"
#include "ui_builtcolorscheme.h"
#include "help.h"
#include "plplot_utils.h"

builtcolorscheme::builtcolorscheme(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::builtcolorscheme)
{
    /// <summary>
    /// This will allow setting new colors schemes using linear interpolation between some previous stablished
    /// color points (colors will be equidistant).
    /// </summary>
    ui->setupUi(this);
    tempfile.open();
    PointColors.clear();
    for(int i=0;i<20;i++){
        PointColors <<"#####";
        pointcolorstatus <<false;
    }

    connect(ui->test, &QAbstractButton::pressed,this, &builtcolorscheme::testcolor);
    connect(ui->c1, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc1);
    connect(ui->c2, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc2);
    connect(ui->c3, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc3);
    connect(ui->c4, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc4);
    connect(ui->c5, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc5);
    connect(ui->c6, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc6);
    connect(ui->c7, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc7);
    connect(ui->c8, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc8);
    connect(ui->c9, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc9);
    connect(ui->c10, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc10);
    connect(ui->c11, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc11);
    connect(ui->c12, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc12);
    connect(ui->c13, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc13);
    connect(ui->c14, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc14);
    connect(ui->c15, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc15);
    connect(ui->c16, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc16);
    connect(ui->c17, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc17);
    connect(ui->c18, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc18);
    connect(ui->c19, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc19);
    connect(ui->c20, &QAbstractButton::pressed,this, &builtcolorscheme::pressedc20);
    connect(ui->buttonBox, &QDialogButtonBox::accepted,this, &builtcolorscheme::testcolor);
    connect(ui->buttonBox, &QDialogButtonBox::helpRequested,this, &builtcolorscheme::showhelp);

}

builtcolorscheme::~builtcolorscheme()
{
    delete ui;
}

void builtcolorscheme::testcolor()
{
    ncol=ui->ncol->value();
    palete1.clear();
    int r,b,g;
    int n=0;
    for(int i=0; i<20; i++){
      if(pointcolorstatus[i]){
        Plplot_utils::rgbfromhex(&r,&g,&b,PointColors[i].toLatin1().constData());
        palete1.append(QString::number((double)r/255.));
        palete1.append(",");
        palete1.append(QString::number((double)g/255.));
        palete1.append(",");
        palete1.append(QString::number((double)b/255.));
        palete1.append(";");
        n++;
       }
      }
    if(n<2){palete1="error"; ui->Result->setText("you have to choose a minimum of two colors");return;}
    Plplot_utils::Testcolormap(450,30,tempfile.fileName(),palete1,ncol);
    QPixmap renderer = QPixmap(tempfile.fileName().toLatin1().constData());
    ui->Result->setPixmap(renderer);

}
void builtcolorscheme::pressedc1()
{
    int i=0;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c1->setIcon(icon1);
    ui->c1->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c1->setIcon(QIcon());
   ui->c1->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc2()
{
    int i=1;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c2->setIcon(icon1);
    ui->c2->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c2->setIcon(QIcon());
   ui->c2->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;


}
void builtcolorscheme::pressedc3()
{
    int i=2;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c3->setIcon(icon1);
    ui->c3->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c3->setIcon(QIcon());
   ui->c3->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc4()
{
    int i=3;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c4->setIcon(icon1);
    ui->c4->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c4->setIcon(QIcon());
   ui->c4->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc5()
{
    int i=4;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c5->setIcon(icon1);
    ui->c5->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c5->setIcon(QIcon());
   ui->c5->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc6()
{
    int i=5;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c6->setIcon(icon1);
    ui->c6->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c6->setIcon(QIcon());
   ui->c6->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc7()
{
    int i=6;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c7->setIcon(icon1);
    ui->c7->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c7->setIcon(QIcon());
   ui->c7->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc8()
{
    int i=7;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c8->setIcon(icon1);
    ui->c8->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c8->setIcon(QIcon());
   ui->c8->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc9()
{
    int i=8;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c9->setIcon(icon1);
    ui->c9->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c9->setIcon(QIcon());
   ui->c9->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc10()
{
    int i=9;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c10->setIcon(icon1);
    ui->c10->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c10->setIcon(QIcon());
   ui->c10->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc11()
{
    int i=10;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c11->setIcon(icon1);
    ui->c11->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c11->setIcon(QIcon());
   ui->c11->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc12()
{
    int i=11;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c12->setIcon(icon1);
    ui->c12->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c12->setIcon(QIcon());
   ui->c12->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc13()
{
    int i=12;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c13->setIcon(icon1);
    ui->c13->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c13->setIcon(QIcon());
   ui->c13->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc14()
{
    int i=13;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c14->setIcon(icon1);
    ui->c14->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c14->setIcon(QIcon());
   ui->c14->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc15()
{
    int i=14;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c15->setIcon(icon1);
    ui->c15->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c15->setIcon(QIcon());
   ui->c15->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc16()
{
    int i=15;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c16->setIcon(icon1);
    ui->c16->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c16->setIcon(QIcon());
   ui->c16->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc17()
{
    int i=16;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c17->setIcon(icon1);
    ui->c17->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c17->setIcon(QIcon());
   ui->c17->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc18()
{
    int i=17;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c18->setIcon(icon1);
    ui->c18->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c18->setIcon(QIcon());
   ui->c18->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}
void builtcolorscheme::pressedc19()
{
    int i=18;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c19->setIcon(icon1);
    ui->c19->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c19->setIcon(QIcon());
   ui->c19->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}

void builtcolorscheme::pressedc20()
{
    int i=19;
    if(pointcolorstatus[i]){
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/Icons/Icons/nocolorbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->c20->setIcon(icon1);
    ui->c20->setPalette(QApplication::palette(this));
    pointcolorstatus[i]=false;
    return;
    }
   color = QColorDialog::getColor(Qt::black, this, "Point Color", QColorDialog::DontUseNativeDialog);
   if(!color.isValid())
   {
    QMessageBox::information(this, "Point color", "Not a valid color");
    return;
   }
   brush.setColor(color);
   brush.setStyle(Qt::SolidPattern);
   palette.setBrush(QPalette::Active, QPalette::Button, brush);
   palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
   palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
   ui->c20->setIcon(QIcon());
   ui->c20->setPalette(palette);
   QString lixo;
   lixo=color.name();
   lixo.remove("#");
   PointColors[i]=lixo;
   pointcolorstatus[i]=true;

}


void builtcolorscheme::showhelp(){

       Help dialog;
       dialog.Text("<html><head/><body><p align=\"justify\">This dialog allows to built color schemes by linear interpolation in color space of equidistant color nodes. One just needs to choose the colors of the nodes (up to a maximum of 20) for that just mouse left click over a pushup button with a X and you will get a nice color dialog to choose the color for that node. If you left click in a node already set it will be unset. The order of the nodes are horizontal but you can let unset nodes between set nodes that they will be ignored.  The larger the number you choose for the color grid value the more continuous your color scheme will look. 300 is a kind of standard value.  If you want a precisely defined discrete color palette you just need to choose for color grid number the number of nodes.</p><p><br/></p><p><span style=\" color:#0000ff;\">Press the </span><span style=\" font-weight:600; color:#0000ff;\">Test </span><span style=\" color:#0000ff;\">button to see the result. </span></p><p><br/></p><p align=\"justify\">The scr"
                   "eenshot below  shows the palette built by three colors with a color grid equal to 300.</p><p align=\"center\"><img src=\":/ScreenShots/Icons/builtcolor0.png\"/></p><p><br/></p><p>The screenshot below  shows the palette built by 6 colors with a color grid exactly equal to 6.</p><p><br/></p><p align=\"center\"><img src=\":/ScreenShots/Icons/builtcolor1.png\"/></p><p><br/></p><p>For this to work you, obviously, need at least two nodes. </p></body></html>"
                   );
       int result;
        result = dialog.exec();
        if(result){

        }


}


