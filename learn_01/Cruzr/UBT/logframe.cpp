#include "logframe.h"
#include "ui_logframe.h"
#include <QDebug>
#include <QEvent>
#include <QStandardItemModel>
#include <QSqlQuery>
LogFrame::LogFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LogFrame)
{
    ui->setupUi(this);
    connectDataBase();
    ui->flowNumberList->hide();
    ui->agvNumberList->hide();
    ui->workNumberList->hide();

    ui->flowNumberText->installEventFilter(this);
    ui->agvNumberText->installEventFilter(this);
    ui->workNumberText->installEventFilter(this);


    logModel = new TableModel();
    logModel->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("序号")));
    logModel->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("配送流水号")));
    logModel->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("目标工位")));
    logModel->setHorizontalHeaderItem(3,new QStandardItem(QObject::tr("状态")));
    logModel->setHorizontalHeaderItem(4,new QStandardItem(QObject::tr("AGV")));
    logModel->setHorizontalHeaderItem(5,new QStandardItem(QObject::tr("时间")));
    logModel->setColumnCount(6);

    ui->logTable->setModel(logModel);
    ui->logTable->setColumnWidth(0,150);
    ui->logTable->setColumnWidth(1,330);
    ui->logTable->setColumnWidth(2,325);
    ui->logTable->setColumnWidth(3,320);
    ui->logTable->setColumnWidth(4,330);
    ui->logTable->setColumnWidth(5,330);


     QSqlQuery query(db_B);
     sql = "select * from agv_task ";
     query.exec(sql);
     while(query.next()){
         QString id = query.value(0).toString();
         item = new QStandardItem(id);
         tableList.append(item);

         QString flowCode = query.value(1).toString();
         item = new QStandardItem(flowCode);
         tableList.append(item);

         QString endPoint = query.value(8).toString();
         item = new QStandardItem(endPoint);
         tableList.append(item);

         QString state = query.value(3).toString();
         item = new QStandardItem(state);
         tableList.append(item);

         QString agv = query.value(6).toString();
         item = new QStandardItem(agv);
         tableList.append(item);

         logModel->appendRow(tableList);
         tableList.clear();
     }


}

LogFrame::~LogFrame()
{
    delete ui;
}

void LogFrame::connectDataBase(){
    db_B = QSqlDatabase::addDatabase("QMYSQL","A");
    db_B.setHostName("127.0.0.1");
    db_B.setPort(3306);
    db_B.setDatabaseName("ums");
    db_B.setUserName("root");
    db_B.setPassword("123456");
    db_B.open();
}

bool LogFrame::eventFilter(QObject *obj, QEvent *e){
    if(obj==ui->agvNumberText){
        if(e->type()==QEvent::KeyRelease ){
            ui->agvNumberList->clear();
            ui->agvNumberList->addItem("1");
            ui->agvNumberList->addItem("2");
            ui->agvNumberList->addItem("3");
            ui->agvNumberList->show();
            ui->agvNumberList->raise();
        }

        if(e->type()==QEvent::FocusOut){
            ui->agvNumberList->hide();
        }
    }

    if(obj==ui->flowNumberText){
        if(e->type()==QEvent::KeyRelease ){
            ui->flowNumberList->addItem("1");
            ui->flowNumberList->addItem("2");
            ui->flowNumberList->addItem("3");
        }
    }

    if(obj==ui->workNumberText){
        if(e->type()==QEvent::KeyRelease ){
            ui->workNumberList->addItem("1");
            ui->workNumberList->addItem("2");
            ui->workNumberList->addItem("3");
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

}

void LogFrame::on_workNumberList_itemPressed(QListWidgetItem *item)
{

}
