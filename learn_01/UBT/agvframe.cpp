#include "agvframe.h"
#include "ui_agvframe.h"
#include <QStandardItemModel>
#include <workthread.h>
#include <tablemodel.h>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTimer>
#include <database.h>
#include <QAbstractItemView>
AGVFrame::AGVFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AGVFrame)
{

    connectDataBase();

    ui->setupUi(this);
    agvModel = new TableModel();
    agvModel->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("序号")));
    agvModel->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("编号")));
    agvModel->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("机器ID")));
    agvModel->setHorizontalHeaderItem(3,new QStandardItem(QObject::tr("类型")));
    agvModel->setHorizontalHeaderItem(4,new QStandardItem(QObject::tr("IP")));
    agvModel->setHorizontalHeaderItem(5,new QStandardItem(QObject::tr("端口")));
    agvModel->setHorizontalHeaderItem(6,new QStandardItem(QObject::tr("工作区域")));
    agvModel->setHorizontalHeaderItem(7,new QStandardItem(QObject::tr("状态")));
    agvModel->setHorizontalHeaderItem(8,new QStandardItem(QObject::tr("是否启用")));
    agvModel->setHorizontalHeaderItem(9,new QStandardItem(QObject::tr("操作")));


    ui->agvTable->setModel(agvModel);
    ui->agvTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->agvTable->setColumnWidth(0,100);
    ui->agvTable->setColumnWidth(1,155);
    ui->agvTable->setColumnWidth(2,155);
    ui->agvTable->setColumnWidth(3,155);
    ui->agvTable->setColumnWidth(4,155);
    ui->agvTable->setColumnWidth(5,155);
    ui->agvTable->setColumnWidth(6,155);
    ui->agvTable->setColumnWidth(7,155);
    ui->agvTable->setColumnWidth(8,155);
    ui->agvTable->setColumnWidth(9,155);

    ui->agvTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    startOrStopButton = ui->startOrStopButton;

    queryData();
    queryArea();
    QTimer *timer = new QTimer(this);
    timer->start(15000);
    connect(timer,SIGNAL(timeout()),this,SLOT(queryData()));
}

void AGVFrame::queryArea(){

     sql = "select name from agv_work_area";
     query.exec(sql);
     while(query.next()){
         ui->workAreaSelect->addItem(query.value(0).toString());
         ui->queryWorkArea->addItem(query.value(0).toString());
     }
}

void AGVFrame::queryData(){

    int row = agvModel->rowCount();
    agvModel->removeRows(0,row);
    ui->agvTable->reset();
    ui->agvTable->setModel(agvModel);
    sql = "select agv_agv.id,agv_agv.code,agv_agv.robot_id,agv_agv.type,agv_agv.address_ip,agv_agv.port,agv_work_area.name,agv_agv.state,agv_agv.isStart from agv_agv,agv_work_area   where agv_agv.work_area_id = agv_work_area.id;";
    query.exec(sql);

    while(query.next()){
        QString id = query.value(0).toString();
        item = new QStandardItem(id);
        list.append(item);

        QString code = query.value(1).toString();
        item = new QStandardItem(code);
        list.append(item);

        QString robotId = query.value(2).toString();
        item = new QStandardItem(robotId);
        list.append(item);

        QString type = query.value(3).toString();
        item = new QStandardItem(type);
        list.append(item);

        QString address_ip = query.value(4).toString();
        item = new QStandardItem(address_ip);
        list.append(item);

        QString port = query.value(5).toString();
        item = new QStandardItem(port);
        list.append(item);

        QString area = query.value(6).toString();
        item = new QStandardItem(area);
        list.append(item);

        QString state = query.value(7).toString();
        item = new QStandardItem(state);
        list.append(item);

        QString isStart = query.value(8).toString();
        item = new QStandardItem(isStart);
        list.append(item);

        agvModel->appendRow(list);
        list.clear();
    }
}

AGVFrame::~AGVFrame()
{
    delete ui;
}

void AGVFrame::connectDataBase(){

    query = DataBase::getQuery("Agv");
}

void AGVFrame::insertSave(){

}

void AGVFrame::on_addButton_pressed()
{


    if(addAGV!=NULL){
        addAGV->activateWindow();
        addAGV->raise();

    }else{
        addAGV = new AddAGV(this);
        connect(addAGV->saveButton,SIGNAL(released()),this,SLOT(save()));
        connect(addAGV->cancelButton,SIGNAL(released()),this,SLOT(cancel()));
        connect(addAGV,SIGNAL(closeWindow()),this,SLOT(close()));
        addAGV->show();
    }
}

void AGVFrame::save(){

    if(addAGV->agvCode->text().trimmed()=="" || addAGV->agvIP->text().trimmed()=="" || addAGV->agvPort->text().trimmed()==""){
        QMessageBox::warning(this, tr("Waring"), tr("编号或ip或端口不能为空！"),QMessageBox::Yes);
    }else{

        sql = "replace into agv_agv(code,robot_id,type,address_ip,port,work_area_id,state,X,Y,isStart) values(:code,:robot_id,:type,:address_ip,:port,:work_area_id,:state,:X,:Y,:isStart)";
        query.prepare(sql);
        query.bindValue(":code",addAGV->agvCode->text());
        query.bindValue(":robot_id",addAGV->agvID->text());
        query.bindValue(":type",addAGV->type->currentText() );
        query.bindValue(":address_ip",addAGV->agvIP->text());
        query.bindValue(":port",addAGV->agvPort->text().toInt());
        query.bindValue(":work_area_id",addAGV->getWorkAreaId());
        query.bindValue(":state",addAGV->state->currentText());
        query.bindValue(":X",0);
        query.bindValue(":Y",0);
        query.bindValue(":isStart","否");

        if(query.exec()){
            QMessageBox::warning(this, tr("Waring"), tr("保存成功"),QMessageBox::Yes);
            queryData();


        }else{
            QMessageBox::warning(this, tr("Waring"), tr("保存失败"),QMessageBox::Yes);
        }
    }

    if(addAGV!=NULL){
        addAGV->deleteLater();
        addAGV->close();
        addAGV=NULL;
    }
}

void AGVFrame::cancel(){
    addAGV->deleteLater();
    addAGV->close();

    addAGV=NULL;
}

void AGVFrame::close(){
    addAGV->deleteLater();
    addAGV->close();   
    addAGV=NULL;
}

void AGVFrame::on_updateButton_clicked()
{
     int row = ui->agvTable->currentIndex().row();

     if(row==-1){
         QMessageBox::warning(this, tr("Waring"), tr("请选中agv信息！"),QMessageBox::Yes);
     }

     QString isStart = agvModel->data(agvModel->index(row,8)).toString();

     if(isStart=="是"){
         QMessageBox::warning(this, tr("Waring"), tr("机器在使用中不能修改信息！请先停用"),QMessageBox::Yes);
     }else if(isStart=="否"){
        QString code = this->agvModel->data(this->agvModel->index(row,1)).toString();
        QString agvID = this->agvModel->data(this->agvModel->index(row,2)).toString();
        QString type = this->agvModel->data(this->agvModel->index(row,3)).toString();
        QString agvIP = this->agvModel->data(this->agvModel->index(row,4)).toString();
        QString agvPort = this->agvModel->data(this->agvModel->index(row,5)).toString();
        QString area = this->agvModel->data(this->agvModel->index(row,6)).toString();
        QString state = this->agvModel->data(this->agvModel->index(row,7)).toString();

        addAGV = new AddAGV(this);
        addAGV->setWindowTitle("修改AGV");

        connect(addAGV->saveButton,SIGNAL(released()),this,SLOT(save()));
        connect(addAGV->cancelButton,SIGNAL(released()),this,SLOT(cancel()));
        connect(addAGV,SIGNAL(closeWindow()),this,SLOT(close()));
        addAGV->show();

        addAGV->agvCode->setText(code);
        addAGV->agvID->setText(agvID);
        addAGV->agvIP->setText(agvIP);
        addAGV->agvPort->setText(agvPort);
        addAGV->type->setCurrentText(type);
        addAGV->workArea->setCurrentText(area);
        addAGV->state->setCurrentText(state);

     }

}

void AGVFrame::on_deleteButton_clicked()
{
     int row = ui->agvTable->currentIndex().row();

     if(row==-1){
         QMessageBox::warning(this, tr("Waring"), tr("请选中agv信息！"),QMessageBox::Yes);
     }

     QString isStart = agvModel->data(agvModel->index(row,8)).toString();

     if(isStart=="是"){
         QMessageBox::warning(this, tr("Waring"), tr("机器在使用中不能删除信息！请先停用"),QMessageBox::Yes);
     }else if(isStart=="否"){
         QMessageBox::StandardButton button;
         button = QMessageBox::question(this, tr("删除信息"), QString(tr("确认删除该AGV?")),QMessageBox::Yes | QMessageBox::No);
         if (button == QMessageBox::Yes) {
            QString id = agvModel->data(this->agvModel->index(row,0)).toString();
            sql = "delete from agv_agv where id="+id;

            bool mes = query.exec(sql);
            if(!mes){
                 QMessageBox::warning(this, tr("Waring"), tr("该agv有送货记录，无法删除！"),QMessageBox::Yes);
            }else{

                queryData();
            }

         }
     }
}

//停用或启用
void AGVFrame::on_startOrStopButton_pressed()
{

    int row = ui->agvTable->currentIndex().row();

    QString code = agvModel->data(agvModel->index(row,1)).toString();
    QString areaName = agvModel->data(agvModel->index(row,6)).toString();

    if(row==-1){
        QMessageBox::warning(this, tr("Waring"), tr("请选中agv信息！"),QMessageBox::Yes);
    }else{

        sql = "select state,isStart from agv_agv where code='"+code+"'";
        query.exec(sql);
        if(query.next()){
            if(query.value(0).toString()=="送货中" || query.value(0).toString()=="准备送货中"){
                QMessageBox::warning(this, tr("Waring"), tr("agv有配送任务无法停用！"),QMessageBox::Yes);
            }else{
                startOrStopSignal(row,code,query.value(1).toString(),areaName,agvModel);
            }
        }
    }

}

//重置
void AGVFrame::on_renewButton_clicked()
{

    sql = "update agv_agv set state='闲置中'  where work_area_id in (select id from agv_work_area where name='"+ui->workAreaSelect->currentText()+"')";
    query.exec(sql);
    sql = "update agv_work_station set state=0,agv_code=NULL where work_area_id in (select id from agv_work_area where name='"+ui->workAreaSelect->currentText()+"')";
    query.exec(sql);
    queryData();
}

//查询
void AGVFrame::on_queryButton_clicked()
{

    int row = agvModel->rowCount();
    agvModel->removeRows(0,row);
    ui->agvTable->reset();
    ui->agvTable->setModel(agvModel);
    if(ui->queryrobotId->text().trimmed()!="" ){
        sql = "select agv_agv.id,agv_agv.code,agv_agv.robot_id,agv_agv.type,agv_agv.address_ip,agv_agv.port,agv_work_area.name,agv_agv.state,agv_agv.isStart from agv_agv,agv_work_area   where  agv_agv.robot_id like '%"+ui->queryrobotId->text()+"%' and agv_work_area.name='"+ui->queryWorkArea->currentText()+"'";
        query.exec(sql);
    }else{
        sql = "select agv_agv.id,agv_agv.code,agv_agv.robot_id,agv_agv.type,agv_agv.address_ip,agv_agv.port,agv_work_area.name,agv_agv.state,agv_agv.isStart from agv_agv,agv_work_area   where   agv_agv.work_area_id =agv_work_area.id and agv_work_area.name='"+ui->queryWorkArea->currentText()+"'";
        query.exec(sql);
    }

    while(query.next()){
        QString id = query.value(0).toString();
        item = new QStandardItem(id);
        list.append(item);

        QString code = query.value(1).toString();
        item = new QStandardItem(code);
        list.append(item);

        QString robotId = query.value(2).toString();
        item = new QStandardItem(robotId);
        list.append(item);

        QString type = query.value(3).toString();
        item = new QStandardItem(type);
        list.append(item);

        QString address_ip = query.value(4).toString();
        item = new QStandardItem(address_ip);
        list.append(item);

        QString port = query.value(5).toString();
        item = new QStandardItem(port);
        list.append(item);

        QString area = query.value(6).toString();
        item = new QStandardItem(area);
        list.append(item);

        QString state = query.value(7).toString();
        item = new QStandardItem(state);
        list.append(item);

        QString isStart = query.value(8).toString();
        item = new QStandardItem(isStart);
        list.append(item);

        agvModel->appendRow(list);
        list.clear();
    }
}
