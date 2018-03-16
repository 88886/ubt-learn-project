#include "addagv.h"
#include "ui_addagv.h"
#include <QDebug>
#include <QSqlQuery>

#include <database.h>
AddAGV::AddAGV(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAGV)
{
    ui->setupUi(this);

    QSqlQuery query = DataBase::getQuery("ui");

    query.exec("select id,name from agv_work_area; ");
    while(query.next()){
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        ui->workArea->addItem(name);
        workAreaMap.insert(name,id);
    }

    this->setWorkAreaId(workAreaMap.find(ui->workArea->currentText()).value());

    saveButton = ui->addAGVButton;
    cancelButton = ui->cancelAddAGVButton;

    this->agvCode = ui->agvCode;
    this->agvID = ui->agvID;
    this->agvIP = ui->agvIP;
    this->agvPort = ui->agvPort;
    this->type = ui->type;

    this->state = ui->state;
    this->workArea = ui->workArea;
    this->setWorkAreaId(workAreaMap.find(ui->workArea->currentText()).value());
}

AddAGV::~AddAGV()
{
    delete ui;
}

void AddAGV::closeEvent(QCloseEvent *event){

    emit closeWindow();
}


void AddAGV::setWorkAreaId(int id){
    this->workAreaId = id;
}

int AddAGV::getWorkAreaId(){
    return this->workAreaId;
}



void AddAGV::on_workArea_currentTextChanged(const QString &arg1)
{
    QString area = ui->workArea->currentText();
    int id = workAreaMap.find(area).value();
    this->setWorkAreaId(id);
}


