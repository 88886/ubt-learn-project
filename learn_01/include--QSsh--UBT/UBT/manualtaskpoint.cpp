#include "manualtaskpoint.h"
#include "ui_manualtaskpoint.h"

ManualTaskPoint::ManualTaskPoint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManualTaskPoint)
{
    ui->setupUi(this);
    goalPoint = ui->goalPoint;
    comfrim = ui->commit;
}

ManualTaskPoint::~ManualTaskPoint()
{
    delete ui;
}

void ManualTaskPoint::on_cancel_clicked()
{
    this->close();
    delete this;
}
