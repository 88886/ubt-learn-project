#include "mesdialog.h"
#include "ui_mesdialog.h"

MesDialog::MesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MesDialog)
{
    ui->setupUi(this);
}

MesDialog::~MesDialog()
{
    delete ui;
}

void MesDialog::closeEvent(QCloseEvent *event){

    delete this;

}
