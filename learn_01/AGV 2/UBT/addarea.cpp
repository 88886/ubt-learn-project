#include "addarea.h"
#include "ui_addarea.h"

AddArea::AddArea(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddArea)
{
    ui->setupUi(this);
    saveButton = ui->saveButton;
    cancelButton = ui->cancelButton;
    workAreaEdit = ui->workAreaEdit;
}

AddArea::~AddArea()
{
    delete ui;
}

void AddArea::closeEvent(QCloseEvent *event){
    emit closeSignal();
}
