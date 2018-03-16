#include "stationframe.h"
#include "ui_stationframe.h"

StationFrame::StationFrame(QString title,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StationFrame)
{
    ui->setupUi(this);
    this->setWindowTitle(title);

    stationCode = ui->stationCode;
    stationX = ui->stationX;
    stationY = ui->stationY;
    stationType = ui->stationType;
    stationState = ui->stationState;

    saveButton = ui->saveButton;
    cancelButton = ui->cancelButton;
}

StationFrame::~StationFrame()
{
    delete ui;
}

void StationFrame::closeEvent(QCloseEvent *event){
    emit closeStation();
}
