#include "logframe.h"
#include "ui_logframe.h"
#include <QDebug>
#include <QEvent>
#include <QStandardItemModel>
#include <QSqlQuery>
#include <QDateTime>
#include <database.h>
#include <QAbstractItemView>
LogFrame::LogFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LogFrame)
{


    ui->setupUi(this);

    ui->flowNumberList->hide();
    ui->agvNumberList->hide();
    ui->workNumberList->hide();

    ui->flowNumberText->installEventFilter(this);
    ui->agvNumberText->installEventFilter(this);
    ui->workNumberText->installEventFilter(this);
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    logModel = new TableModel();
    logModel->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("序号")));
    logModel->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("配送流水号")));
    logModel->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("发货工位")));
    logModel->setHorizontalHeaderItem(3,new QStandardItem(QObject::tr("接收工位")));
    logModel->setHorizontalHeaderItem(4,new QStandardItem(QObject::tr("状态")));
    logModel->setHorizontalHeaderItem(5,new QStandardItem(QObject::tr("时间")));
    logModel->setHorizontalHeaderItem(6,new QStandardItem(QObject::tr("AGV")));
    logModel->setColumnCount(7);

    ui->logTable->setModel(logModel);
    ui->logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->logTable->setColumnWidth(0,150);
    ui->logTable->setColumnWidth(1,270);
    ui->logTable->setColumnWidth(2,265);
    ui->logTable->setColumnWidth(3,255);
    ui->logTable->setColumnWidth(4,285);
    ui->logTable->setColumnWidth(5,300);
    ui->logTable->setColumnWidth(6,255);

     query = getQuery("ui");

//     sql = "select agv_task.id,agv_task.flow_code,agv_task.start_point,agv_task.task_state,agv_task.new_time,agv_agv.code from agv_task,agv_agv  where agv_task.agv_id=agv_agv.id";
//     query.exec(sql);
//     while(query.next()){


//         QString id = query.value(0).toString();
//         item = new QStandardItem(id);
//         tableList.append(item);

//         QString flowCode = query.value(1).toString();
//         item = new QStandardItem(flowCode);
//         tableList.append(item);

//         QString startPoint = query.value(2).toString();
//         item = new QStandardItem(startPoint);
//         tableList.append(item);

//         QString state = query.value(3).toString();
//         item = new QStandardItem(state);
//         tableList.append(item);

//         QString newTime = query.value(4).toString();
//         item = new QStandardItem(newTime);
//         tableList.append(item);

//         QString agvCode = query.value(5).toString();
//         item = new QStandardItem(agvCode);
//         tableList.append(item);

//         logModel->appendRow(tableList);
//         tableList.clear();
//     }


}

LogFrame::~LogFrame()
{
    delete ui;
}

QSqlQuery LogFrame::getQuery(const QString &uiName){
    query = DataBase::getQuery(uiName);
    return query;
}

//日志面板按钮监听
bool LogFrame::eventFilter(QObject *obj, QEvent *e){

    if(obj==ui->agvNumberText){
        if(e->type()==QEvent::KeyRelease ){
            ui->agvNumberList->clear();
            sql = "select code  from agv_agv where code like '%"+ ui->agvNumberText->text()+"%' group by code";
            query.exec(sql);
            while(query.next()){
                ui->agvNumberList->addItem(query.value(0).toString());
            }
            ui->agvNumberList->show();
            ui->agvNumberList->raise();
        }

        if(e->type()==QEvent::FocusOut){
            ui->agvNumberList->hide();
            ui->agvNumberText->setText("");
        }
    }

    if(obj==ui->flowNumberText){
        if(e->type()==QEvent::KeyRelease ){
            ui->flowNumberList->clear();
            sql = "select flow_code  from agv_task where flow_code like '%"+ ui->flowNumberText->text()+"%' group by flow_code";
            query.exec(sql);
            while(query.next()){
                ui->flowNumberList->addItem(query.value(0).toString());
            }
            ui->flowNumberList->show();
            ui->flowNumberList->raise();
        }

        if(e->type()==QEvent::FocusOut){
            ui->flowNumberList->hide();
            ui->flowNumberText->setText("");
        }
    }

    if(obj==ui->workNumberText){
        if(e->type()==QEvent::KeyRelease ){
            ui->workNumberList->clear();
            sql = "select start_point  from agv_task where start_point like '%"+ ui->workNumberText->text()+"%' group by start_point";
            query.exec(sql);
            while(query.next()){
                ui->workNumberList->addItem(query.value(0).toString());
            }
            ui->workNumberList->show();
            ui->workNumberList->raise();
        }

        if(e->type()==QEvent::FocusOut){
            ui->workNumberList->hide();
            ui->workNumberText->setText("");
        }
    }

    return QFrame::eventFilter(obj, e);
}




void LogFrame::on_agvNumberList_itemPressed(QListWidgetItem *item)
{
    ui->agvNumberText->setText(item->text());
    ui->agvNumberList->hide();
}

void LogFrame::on_flowNumberList_itemPressed(QListWidgetItem *item)
{
    ui->flowNumberText->setText(item->text());
    ui->flowNumberList->hide();
}

void LogFrame::on_workNumberList_itemPressed(QListWidgetItem *item)
{
    ui->workNumberText->setText(item->text());
    ui->workNumberList->hide();
}

void LogFrame::on_pushButton_clicked()
{

    QString date = ui->dateTimeEdit->date().toString("yyyy-MM-dd");
    QString flowNumber = ui->flowNumberText->text();
    QString workNumber = ui->workNumberText->text();
    QString state = ui->state->currentText();
    QString agvNumber = ui->agvNumberText->text();

    int row = logModel->rowCount();
    logModel->removeRows(0,row);
    ui->logTable->reset();
    ui->logTable->setModel(logModel);

    if(flowNumber.trimmed()=="" && workNumber.trimmed()!="" && agvNumber.trimmed()!=""){
        sql = "select agv_task.id,agv_task.flow_code,agv_task.start_point,agv_task.end_point,agv_task.task_state,agv_task.new_time,agv_agv.code from agv_task,agv_agv "
             " where agv_task.agv_id in (select id from agv_agv where code like '%"+agvNumber+"%') and agv_task.start_point like '%"+workNumber+"%' "
             " and agv_task.task_state = '"+state+"' and new_time like '%"+date+"%' and agv_task.agv_id = agv_agv.id";

    }

    if(flowNumber.trimmed()!="" && workNumber.trimmed()=="" && agvNumber.trimmed()!=""){
        sql = "select agv_task.id,agv_task.flow_code,agv_task.start_point,agv_task.end_point,agv_task.task_state,agv_task.new_time,agv_agv.code from agv_task,agv_agv "
              " where agv_task.agv_id in (select id from agv_agv where code like '%"+agvNumber+"%') and agv_task.flow_code like '%"+flowNumber+"%' "
              " and agv_task.task_state = '"+state+"' and new_time like '%"+date+"%' and agv_task.agv_id = agv_agv.id";

    }

    if(flowNumber.trimmed()!="" && workNumber.trimmed()!="" && agvNumber.trimmed()==""){
        sql = "select agv_task.id,agv_task.flow_code,agv_task.start_point,agv_task.end_point,agv_task.task_state,agv_task.new_time,agv_agv.code from agv_task,agv_agv "
              " where agv_task.flow_code like '%"+flowNumber+"%' and agv_task.start_point like '%"+workNumber+"%' "
             + " and agv_task.task_state = '"+state+"' and new_time like '%"+date+"%' and agv_task.agv_id = agv_agv.id";

    }

    if(flowNumber.trimmed()=="" && workNumber.trimmed()=="" && agvNumber.trimmed()!=""){
        sql = "select agv_task.id,agv_task.flow_code,agv_task.start_point,agv_task.end_point,agv_task.task_state,agv_task.new_time,agv_agv.code from agv_task,agv_agv "
              " where agv_task.agv_id in (select id from agv_agv where code like '%"+agvNumber+"%') "
              + " and agv_task.task_state = '"+state+"' and new_time like '%"+date+"%' and agv_task.agv_id = agv_agv.id";

    }

    if(flowNumber.trimmed()==""&& workNumber.trimmed()!=""&&agvNumber.trimmed()==""){
        sql = "select agv_task.id,agv_task.flow_code,agv_task.start_point,agv_task.end_point,agv_task.task_state,agv_task.new_time,agv_agv.code from agv_task,agv_agv "
             " where  agv_task.start_point like '%"+workNumber+"%' "
             + " and agv_task.task_state = '"+state+"' and new_time like '%"+date+"%' and agv_task.agv_id = agv_agv.id";

    }

    if(flowNumber.trimmed()!="" && workNumber.trimmed()=="" &&agvNumber.trimmed()==""){
        sql = "select agv_task.id,agv_task.flow_code,agv_task.start_point,agv_task.end_point,agv_task.task_state,agv_task.new_time,agv_agv.code from agv_task,agv_agv "
               " where agv_task.flow_code like '%"+flowNumber+"%' and agv_task.task_state = '"+state+"' and new_time like '%"+date+"%' and agv_task.agv_id = agv_agv.id";


    }

    if(flowNumber.trimmed()=="" && workNumber.trimmed()=="" && agvNumber.trimmed()==""){
        sql = "select agv_task.id,agv_task.flow_code,agv_task.start_point,agv_task.end_point,agv_task.task_state,agv_task.new_time,agv_agv.code from agv_task,agv_agv "
              " where  agv_task.task_state = '"+state+"' and new_time like '%"+date+"%' and agv_task.agv_id = agv_agv.id";

    }


    query.exec(sql);
    while(query.next()){
        QString id = query.value(0).toString();
        item = new QStandardItem(id);
        tableList.append(item);

        QString flowCode = query.value(1).toString();
        item = new QStandardItem(flowCode);
        tableList.append(item);

        QString startPoint = query.value(2).toString();
        item = new QStandardItem(startPoint);
        tableList.append(item);

        QString endPoint = query.value(3).toString();
        item = new QStandardItem(endPoint);
        tableList.append(item);

        QString state = query.value(4).toString();
        item = new QStandardItem(state);
        tableList.append(item);

        QString newTime = query.value(5).toString();
        item = new QStandardItem(newTime);
        tableList.append(item);

        QString agvCode = query.value(6).toString();
        item = new QStandardItem(agvCode);
        tableList.append(item);

        logModel->appendRow(tableList);
        tableList.clear();
    }
}

void LogFrame::on_dateTimeEdit_dateChanged(const QDate &date)
{

}
