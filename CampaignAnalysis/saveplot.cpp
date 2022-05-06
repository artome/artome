#include "saveplot.h"
#include "ui_saveplot.h"

SavePlot::SavePlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SavePlot)
{
    ui->setupUi(this);
    connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&SavePlot::setdevice);
}

SavePlot::~SavePlot()
{
    delete ui;
}
void SavePlot::setdevice()
{
    if (ui->pdf->isChecked()) device="pdfqt";
    if (ui->svg->isChecked()) device="svgqt";
    if (ui->png->isChecked()) device="pngqt";
    if (ui->ps->isChecked()) device="psc";
    if (ui->xfig->isChecked()) device="xfig";
}
