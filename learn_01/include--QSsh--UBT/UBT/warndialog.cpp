#include "warndialog.h"
#include "ui_warndialog.h"

WarnDialog::WarnDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WarnDialog)
{
    ui->setupUi(this);
}

WarnDialog::~WarnDialog()
{
    delete ui;
}

void WarnDialog::closeEvent(QCloseEvent *event){

    delete this;
    emit this->closeWindow();
}

