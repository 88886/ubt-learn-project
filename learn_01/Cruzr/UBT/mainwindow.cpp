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
WorkThread *workthread;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->materialFlow->installEventFilter(this);
    ui->agvManage->installEventFilter(this);
    ui->log->installEventFilter(this);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(ui->mainContainer);
    this->setLayout(mainLayout);
    ui->listArea->setStyleSheet("QListWidget::Item{padding-bottom:5px;padding-top:5px;}");
    ui->listArea->item(0)->setSelected(true);


    frame2 = new MaterialFlowFrame(this);
    frame1 = new MaterialFlowFrame(this);
    frame5 = new MaterialFlowFrame(this);

    frame1->flowNumber->setEnabled(false);
    frame2->flowNumber->setEnabled(false);
    frame5->flowNumber->setEnabled(false);

    agvFrame = new AGVFrame(this);




    log = new LogFrame(this);

    ui->container->addWidget(frame2);
    ui->container->addWidget(frame1);
    ui->container->addWidget(frame5);
    ui->container->addWidget(agvFrame);
    ui->mainContainer->addWidget(log);
    ui->container->setCurrentWidget(frame2);



}

MainWindow::~MainWindow()
{

}



bool MainWindow::eventFilter(QObject *target, QEvent *e){

    if(target==ui->materialFlow){
       if(e->type()==QEvent::MouseButtonPress || e->type()==QEvent::MouseButtonRelease ){
           ui->mainContainer->setCurrentWidget(ui->flowAndAgvFrame);
           ui->container->show();
           ui->container->setCurrentWidget(frame2);
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





void MainWindow::on_listArea_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if("一楼仓库"==current->text()){
        ui->container->setCurrentWidget(frame1);
        ui->buttonFrame->show();

    }

    if("五楼仓库"==current->text()){
        ui->container->setCurrentWidget(frame5);
        ui->buttonFrame->show();
    }

    if("二楼车间"==current->text()){
        ui->container->setCurrentWidget(frame2);
        ui->buttonFrame->show();
    }
}

void MainWindow::on_action_triggered()
{

}

void MainWindow::on_onlineDispatcherButton_pressed()
{

    if(ui->listArea->currentItem()->text()=="一楼仓库"){
        frame1->flowNumber->setEnabled(true);



    }

    if(ui->listArea->currentItem()->text()=="二楼车间"){
        frame2->flowNumber->setEnabled(true);
        thread = new WorkThread(ui->listArea->currentItem()->text(),frame2,this);
        agvFrame->queryData();
        thread->start();

    }

    if(ui->listArea->currentItem()->text()=="五楼仓库"){
        frame5->flowNumber->setEnabled(true);
    }
}

void MainWindow::on_stopButton_pressed()
{

    if(ui->listArea->currentItem()->text()=="一楼仓库"){
        frame1->flowNumber->setEnabled(false);
    }

    if(ui->listArea->currentItem()->text()=="二楼车间"){
        frame2->flowNumber->setEnabled(false);
        thread->destroyed();
    }

    if(ui->listArea->currentItem()->text()=="五楼仓库"){
        frame5->flowNumber->setEnabled(false);
    }
}


