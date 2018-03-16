#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "materialflowframe.h"
#include "agvframe.h"
#include "logframe.h"
#include <QDebug>
#include <QEvent>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QtNetwork/QTcpSocket>
#include <QTextCodec>
#include <workthread.h>
#include <QSqlQuery>
#include <database.h>
#include <QMessageBox>
#include <QFont>
#include <database.h>
#include <updatestation.h>

WorkThread *workthread;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->materialFlow->installEventFilter(this);
    ui->materialFlow->addSeparator();
    ui->agvManage->installEventFilter(this);

    ui->log->installEventFilter(this);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(ui->mainContainer);
    this->setLayout(mainLayout);
    ui->listArea->setStyleSheet("QListWidget::Item{padding-bottom:5px;padding-top:5px;}");
//    ui->listArea->item(0)->setSelected(true);


//    frame2 = new MaterialFlowFrame(this);
//    frame1 = new MaterialFlowFrame(this);
//    frame5 = new MaterialFlowFrame(this);
//    frame1->flowNumber->setEnabled(false);
//    frame2->flowNumber->setEnabled(false);
//    frame5->flowNumber->setEnabled(false);

//    QMap<MaterialFlowFrame*,WorkThread*> *inMap1 = new QMap<MaterialFlowFrame*,WorkThread*>;
//    inMap1->insert(frame1,thread);
//    outMap.insert("一楼仓库",inMap1);

//    QMap<MaterialFlowFrame*,WorkThread*> *inMap2 = new QMap<MaterialFlowFrame*,WorkThread*>;
//    inMap2->insert(frame2,thread);
//    outMap.insert("二楼车间",inMap2);

//    QMap<MaterialFlowFrame*,WorkThread*> *inMap5 = new QMap<MaterialFlowFrame*,WorkThread*>;
//    inMap5->insert(frame5,thread);
//    outMap.insert("五楼仓库",inMap5);
//    inMap5->insert(frame5,thread);

    agvFrame = new AGVFrame(this);
    log = new LogFrame(this);

//    ui->container->addWidget(frame2);
//    ui->container->addWidget(frame1);
//    ui->container->addWidget(frame5);

    ui->container->addWidget(agvFrame);
    ui->mainContainer->addWidget(log);
//    ui->container->setCurrentWidget(frame2);

    query = getQuery("ui");
    initFrame();

    if(ui->listArea->count()>0){
        ui->listArea->item(0)->setSelected(true);
    }

}

MainWindow::~MainWindow()
{

}

void MainWindow::initFrame(){
    QString sql = "select name from agv_work_area";
    query.exec(sql);
    while(query.next()){
        QString workAreaName = query.value(0).toString();
        frame = new MaterialFlowFrame(this);
        frame->flowNumber->setEnabled(false);
        ui->container->addWidget(frame);

        QMap<MaterialFlowFrame*,WorkThread*> *inMap = new QMap<MaterialFlowFrame*,WorkThread*>;
        thread = NULL;
        inMap->insert(frame,thread);
        outMap.insert(workAreaName,inMap);

        QListWidgetItem *item = new  QListWidgetItem(workAreaName);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFont(QFont("Arial",18,QFont::Bold));
        ui->listArea->addItem(item);
    }

}


QSqlQuery MainWindow::getQuery(const QString &uiName){
    query = DataBase::getQuery(uiName);
    return query;
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(outMap.size()!=0){
        for(out = outMap.constBegin();out != outMap.end();out++){
            if(out.value()->size()!=0){
               in = out.value()->begin();
               if(in.value()!=NULL){
                   QMessageBox::warning(this, tr("Waring"), tr("请先停止楼层的在线调度功能！"),QMessageBox::Yes);
                   event->ignore();
                   break;
               }
            }
        }
    }
}

//菜单栏按钮动作事件
bool MainWindow::eventFilter(QObject *target, QEvent *e){

    if(target==ui->materialFlow){
       if(e->type()==QEvent::MouseButtonPress || e->type()==QEvent::MouseButtonRelease ){
           ui->mainContainer->setCurrentWidget(ui->flowAndAgvFrame);
           ui->container->show();
           if(outMap.size()!=0){
               out = outMap.begin();
               in = out.value()->begin();
               ui->container->setCurrentWidget(in.key());
           }
           ui->buttonFrame->show();
       }
    }

    if(target==ui->agvManage){
       if(e->type()==QEvent::MouseButtonPress || e->type()==QEvent::MouseButtonRelease ){
           ui->mainContainer->setCurrentWidget(ui->flowAndAgvFrame);
           ui->container->show();
           ui->container->setCurrentWidget(agvFrame);
           ui->buttonFrame->hide();
       }
    }

    if(target==ui->log){
       if(e->type()==QEvent::MouseButtonPress || e->type()==QEvent::MouseButtonRelease ){
            ui->mainContainer->setCurrentWidget(log);
       }
    }



    return QMainWindow::eventFilter(target, e);
}

//工作区域信息切换
void MainWindow::on_listArea_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(outMap.contains(current->text())){

        out = outMap.find(current->text());

        in = out.value()->begin();

        ui->container->setCurrentWidget(in.key());
        ui->buttonFrame->show();

        if(in.key()->flowNumber->isEnabled()){
            ui->onlineDispatcherButton->setStyleSheet("QPushButton{background-color: rgb(85, 0, 255);border-color: rgb(170, 255, 255);}");
        }else{
            ui->onlineDispatcherButton->setStyleSheet("QPushButton{background-color:rgb(64, 66, 68);border-color: rgb(170, 255, 255);}");
        }
    }
}

void MainWindow::on_action_triggered()
{

}

//调度按钮信息切换
void MainWindow::on_onlineDispatcherButton_pressed()
{
    if(outMap.size()!=0){

        if(outMap.contains(ui->listArea->currentItem()->text())){

            out = outMap.find(ui->listArea->currentItem()->text());
            in = out.value()->begin();

            //in.key()获取MaterialFlowFrame,in.value()获取workThread;
            if(in.value()==NULL){
                frame = in.key();
                frame->flowNumber->setEnabled(true);
                thread = new WorkThread(ui->listArea->currentItem()->text(),in.key(),this);

                out.value()->clear();
                out.value()->insert(frame,thread);

                in = out.value()->begin();


                connect(agvFrame,SIGNAL(startOrStopSignal(int,QString,QString,QString,TableModel*)),in.value(),SLOT(startOrStopAgv( int,QString,QString,QString,TableModel*)));
                in.value()->start();
                ui->onlineDispatcherButton->setStyleSheet("QPushButton{background-color: rgb(85, 0, 255);border-color: rgb(170, 255, 255);}");

            }
        }
    }

}

//终止按钮信息切换
void MainWindow::on_stopButton_pressed()
{ 
    if(outMap.size()!=0){

        if(outMap.contains(ui->listArea->currentItem()->text())){

            out = outMap.find(ui->listArea->currentItem()->text());
            in = out.value()->begin();

            //in.key()获取MaterialFlowFrame,in.value()获取workThread;
            if(in.value()!=NULL){
                if(in.value()->closeThread()){
                    frame = in.key();
                    frame->flowNumber->setEnabled(false);
                    in.value()->threadState = false;
                    in.value()->terminate();
                    in.value()->wait();
                    in.value()->deleteLater();
                    out.value()->clear();
                    out.value()->insert(frame,NULL);
                    ui->onlineDispatcherButton->setStyleSheet("QPushButton{background-color:rgb(64, 66, 68);border-color: rgb(170, 255, 255);}");
                }
            }
        }
    }

}

void MainWindow::on_addWorkArea_triggered()
{

    if(addArea==NULL){
        addArea = new AddArea(this);
        addArea->show();
        connect(addArea->saveButton,SIGNAL(clicked(bool)),this,SLOT(addWorkArea()));
        connect(addArea->cancelButton,SIGNAL(clicked(bool)),this,SLOT(closeWorkArea()));
        connect(addArea,SIGNAL(closeSignal()),this,SLOT(closeWorkArea()));
    }else{
        addArea->activateWindow();
        addArea->raise();
    }
}

//增加工作区域
void MainWindow::addWorkArea(){
    if(addArea!=NULL){
        QString workAreaName = addArea->workAreaEdit->text();
        if(workAreaName.trimmed()!=""){
            frame = new MaterialFlowFrame(this);
            frame->flowNumber->setEnabled(false);
            ui->container->addWidget(frame);

            sql ="select count(*) from agv_work_area where name='"+workAreaName+"'";
            query.exec(sql);
            query.next();
            if(query.value(0).toInt()>0){
                QMessageBox::warning(this, tr("Waring"), tr("已有相同工作区域名称"),QMessageBox::Yes);
                return;
            }

            sql = "insert into agv_work_area(name) values('"+workAreaName+"')";
            query.exec(sql);

            QMap<MaterialFlowFrame*,WorkThread*> *inMap = new QMap<MaterialFlowFrame*,WorkThread*>;
            WorkThread *addThread = NULL;
            inMap->insert(frame,addThread);
            outMap.insert(workAreaName,inMap);

            QListWidgetItem *item = new  QListWidgetItem(workAreaName);
            item->setTextAlignment(Qt::AlignCenter);
            item->setFont(QFont("Arial",18,QFont::Bold));
            ui->listArea->addItem(item);
            addArea->close();
            addArea->deleteLater();
            addArea = NULL;
        }
    }
}

//删除工作区域
void MainWindow::on_deleteWorkArea_triggered()
{
    QString name = ui->listArea->currentItem()->text();
    QString es = "确认删除该"+name;

    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("删除信息"),es,QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::Yes){
        sql = "delete from agv_work_area where name='"+name+"'";
        if(query.exec(sql)){
            ui->listArea->takeItem(ui->listArea->currentRow());
            ui->listArea->removeItemWidget(ui->listArea->currentItem());
            DataBase::closeDataBase(name+WORK_STATION_NAME);
            DataBase::closeDataBase(name);
            DataBase::closeDataBase(name+READY_STATION_NAME);
        }else {
            QMessageBox::warning(this, tr("Waring"), tr("无法删除该工作区域，该工作区域与存入相关数据信息"),QMessageBox::Yes);
        }
    }
}

//关闭工作区域
void MainWindow::closeWorkArea(){
    if(addArea!=NULL){
        addArea->close();
        addArea->deleteLater();
        addArea = NULL;
    }

}

void MainWindow::on_updateWorkStation_triggered()
{
    if(updateStation == NULL){
        updateStation = new UpdateStation(this);
        connect(updateStation,SIGNAL(closeStationSignal()),this,SLOT(closeStation()));

        updateStation->show();
    }else{
        updateStation->activateWindow();
        updateStation->raise();
    }
}

void MainWindow::closeStation(){
    if(updateStation!=NULL){
        updateStation->deleteLater();
        updateStation = NULL;
    }
}
