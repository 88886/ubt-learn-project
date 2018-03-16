#include "workarea.h"
#include "ui_workarea.h"

WorkArea::WorkArea(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WorkArea)
{
    ui->setupUi(this);
}

WorkArea::~WorkArea()
{
    delete ui;
}
