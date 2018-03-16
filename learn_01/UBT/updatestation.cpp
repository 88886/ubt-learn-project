#include "updatestation.h"
#include "ui_updatestation.h"
#include <database.h>
#include <QDebug>
#include <tablemodel.h>
#include <QStandardItem>
#include <QMessageBox>
UpdateStation::UpdateStation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateStation)
{
    ui->setupUi(this);
    query = DataBase::getQuery("ui");

    sql = "select name from agv_work_area";
    query.exec(sql);
    while(query.next()){
        ui->workArea->addItem(query.value(0).toString());
    }

    stationModel = new TableModel();
    stationModel->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("ID编号")));
    stationModel->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("工位编号")));
    stationModel->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("坐标X")));
    stationModel->setHorizontalHeaderItem(3,new QStandardItem(QObject::tr("坐标Y")));
    stationModel->setHorizontalHeaderItem(4,new QStandardItem(QObject::tr("是否占用")));
    stationModel->setHorizontalHeaderItem(5,new QStandardItem(QObject::tr("工位类型")));



    ui->stationTable->setModel(stationModel);
    ui->stationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->stationTable->setColumnWidth(0,70);
    ui->stationTable->setColumnWidth(1,100);
    ui->stationTable->setColumnWidth(2,100);
    ui->stationTable->setColumnWidth(3,100);
    ui->stationTable->setColumnWidth(4,100);
    ui->stationTable->setColumnWidth(5,150);

    ui->stationTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    addButton = ui->addButton;
    updateButton = ui->updateButton;
    deleteButton = ui->deleteButton;

}

UpdateStation::~UpdateStation()
{
    delete ui;
}

//更新表格查询
void UpdateStation::resetTable(){
    int row = stationModel->rowCount();
    stationModel->removeRows(0,row);
    ui->stationTable->reset();
    ui->stationTable->setModel(stationModel);

    QString name = ui->workArea->currentText();
    sql = "select s.id, s.code,s.X,s.Y,s.state,s.station_name from agv_work_station as s INNER JOIN agv_work_area as a on s.work_area_id = a.id and s.work_area_id = (select id FROM agv_work_area WHERE name='"+name+"')";
    query.exec(sql);
    while(query.next()){
        item = new QStandardItem(query.value(0).toString());
        tableList.append(item);

        item = new QStandardItem(query.value(1).toString());
        tableList.append(item);

        item = new QStandardItem(query.value(2).toString());
        tableList.append(item);

        item = new QStandardItem(query.value(3).toString());
        tableList.append(item);

        item = new QStandardItem(query.value(4).toString());
        tableList.append(item);

        item = new QStandardItem(query.value(5).toString());
        tableList.append(item);

        stationModel->appendRow(tableList);
        tableList.clear();
    }
}

//获取工作区域的工位
void UpdateStation::on_workArea_currentTextChanged(const QString &arg1)
{
    resetTable();
}

//关闭窗口
void UpdateStation::closeEvent(QCloseEvent *event){
    emit closeStationSignal();
}

//新增工位按钮
void UpdateStation::on_addButton_clicked()
{
    QString name = ui->workArea->currentText();
    if(name=="请选择工作区域"){
        QMessageBox::warning(this, tr("Waring"), tr("请选择工作区域"),QMessageBox::Yes);
    }else{
         if(stationFrame == NULL){
             stationFrame = new StationFrame("新增工位",this);
             connect(stationFrame,SIGNAL(closeStation()),this,SLOT(closeStation()));
             connect(stationFrame->saveButton,SIGNAL(clicked(bool)),this,SLOT(saveStation(bool)));
             connect(stationFrame->cancelButton,SIGNAL(clicked(bool)),this,SLOT(cancelStation()));
             stationFrame->show();
         }else{
             stationFrame->activateWindow();
             stationFrame->raise();
         }
    }
}

void UpdateStation::closeStation(){
    if(stationFrame != NULL){
        stationFrame->deleteLater();
        stationFrame = NULL;
    }

    if(flag==true){
        flag = false;
    }
}

//新增或者修改工位保存
void UpdateStation::saveStation(bool f){

    QString workAreaName = ui->workArea->currentText();

    QString type = stationFrame->stationType->text().trimmed();
    QString code = stationFrame->stationCode->text().trimmed();
    QString x = stationFrame->stationX->text().trimmed();
    QString y = stationFrame->stationY->text().trimmed();
    QString stationState = stationFrame->stationState->currentText();

    if(type=="" || code=="" || x =="" || y ==""){
        QMessageBox::warning(this, tr("Waring"), tr("请填写完整信息！"),QMessageBox::Yes);
    }else{

            QString state = "0";
            if(stationState =="是"){
                state = "1";
            }

           if(flag==false){

                QString sql = "select count(*) from agv_work_station where code='"+code+"' and station_name='"+type+"' and work_area_id in (select id from agv_work_area where name='"+workAreaName+"')";
                query.exec(sql);
                query.next();
                if(query.value(0).toInt()>0){
                     QMessageBox::warning(this, tr("Waring"), tr("工位命名重复！"),QMessageBox::Yes);
                     return;
                }

                QString sql_2 = "select id from agv_work_area where name ='"+workAreaName+"'";
                query.exec(sql_2);
                query.next();
                int work_area_id = query.value(0).toInt();


                QString str = "insert into agv_work_station(code,X,Y,state,station_name,work_area_id) values(:code,:X,:Y,:state,:station_name,:work_area_id)";
                query.prepare(str);
                query.bindValue(":code",code);
                query.bindValue(":X",x.toInt());
                query.bindValue(":Y",y.toInt());
                query.bindValue(":state",state);
                query.bindValue(":station_name",type);
                query.bindValue(":work_area_id",work_area_id);

                if(query.exec()){

                    resetTable();

                    if(stationFrame != NULL){
                        stationFrame->deleteLater();
                        stationFrame = NULL;
                    }

                    if(flag==true){
                        flag = false;
                    }

                }else{
                    QMessageBox::warning(this, tr("Waring"), tr("操作失败！"),QMessageBox::Yes);
                }

           }else if(flag == true){
                int row = ui->stationTable->currentIndex().row();
                QString id = this->stationModel->data(this->stationModel->index(row,0)).toString();

                QString str = "update agv_work_station set code='"+code+"',X="+x+",Y="+y+",state="+state+",station_name='"+type+"' where id="+id;

                if(query.exec(str)){
                    resetTable();

                    if(stationFrame != NULL){
                        stationFrame->deleteLater();
                        stationFrame = NULL;
                    }

                    if(flag==true){
                        flag = false;
                    }
                }else{
                     QMessageBox::warning(this, tr("Waring"), tr("操作失败！"),QMessageBox::Yes);
                }

           }

    }

}


void UpdateStation::cancelStation(){
    if(stationFrame != NULL){
        stationFrame->deleteLater();
        stationFrame = NULL;
    }

    if(flag==true){
        flag = false;
    }
}

//修改工位
void UpdateStation::on_updateButton_clicked()
{
    QString name = ui->workArea->currentText();
    if(name=="请选择工作区域"){
        QMessageBox::warning(this, tr("Waring"), tr("请选择工作区域"),QMessageBox::Yes);
    }else{

        int row = ui->stationTable->currentIndex().row();
        if(row==-1){
            QMessageBox::warning(this, tr("Waring"), tr("请选中工位信息信息！"),QMessageBox::Yes);
        }else{
            if(stationFrame == NULL){
                stationFrame = new StationFrame("修改工位",this);
                flag = true;

                QString code = this->stationModel->data(this->stationModel->index(row,1)).toString();
                QString x = this->stationModel->data(this->stationModel->index(row,2)).toString();
                QString y = this->stationModel->data(this->stationModel->index(row,3)).toString();
                QString state = this->stationModel->data(this->stationModel->index(row,4)).toString();
                QString type = this->stationModel->data(this->stationModel->index(row,5)).toString();

                stationFrame->stationCode->setText(code);
                stationFrame->stationX->setText(x);
                stationFrame->stationY->setText(y);
                stationFrame->stationType->setText(type);

                if(state=="0"){
                    stationFrame->stationState->setCurrentIndex(0);
                }else{
                    stationFrame->stationState->setCurrentIndex(1);
                }

                connect(stationFrame,SIGNAL(closeStation()),this,SLOT(closeStation()));
                connect(stationFrame->saveButton,SIGNAL(clicked(bool)),this,SLOT(saveStation(bool)));
                connect(stationFrame->cancelButton,SIGNAL(clicked(bool)),this,SLOT(cancelStation()));
                stationFrame->show();
            }else{
                stationFrame->activateWindow();
                stationFrame->raise();
            }
        }

    }
}

//删除工位
void UpdateStation::on_deleteButton_clicked()
{
    int row = ui->stationTable->currentIndex().row();
    QString id = this->stationModel->data(this->stationModel->index(row,0)).toString();
    QString sql = "delete from agv_work_station where id="+id;
    if(query.exec(sql)){
        resetTable();
        QMessageBox::warning(this, tr("Waring"), tr("删除成功！"),QMessageBox::Yes);
    }else{
        QMessageBox::warning(this, tr("Waring"), tr("删除失败！"),QMessageBox::Yes);
    }
}
