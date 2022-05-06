#include "plotscale.h"
#include "ui_plotscale.h"

Plotscale::Plotscale(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plotscale)
{
    ui->setupUi(this);

    connect(ui->xscale, &QDoubleSpinBox::valueChanged, ui->yscale, &QDoubleSpinBox::setValue);
    connect(ui->yscale, &QDoubleSpinBox::valueChanged, ui->xscale, &QDoubleSpinBox::setValue);
    connect(ui->keepratio, &QCheckBox::stateChanged, this, &Plotscale::changelink);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &Plotscale::readvalues);


}

Plotscale::~Plotscale()
{
    delete ui;
}
void Plotscale::changelink(int state)
{
    if (state == Qt::Checked) {
        connect(ui->xscale, &QDoubleSpinBox::valueChanged, ui->yscale, &QDoubleSpinBox::setValue);
        connect(ui->yscale, &QDoubleSpinBox::valueChanged, ui->xscale, &QDoubleSpinBox::setValue);
    } else {
        disconnect(ui->xscale, &QDoubleSpinBox::valueChanged, ui->yscale, &QDoubleSpinBox::setValue);
        disconnect(ui->yscale, &QDoubleSpinBox::valueChanged, ui->xscale, &QDoubleSpinBox::setValue);
    }
}
void Plotscale::readvalues()
{
 xfactor=ui->xscale->value();
 yfactor=ui->yscale->value();

}
