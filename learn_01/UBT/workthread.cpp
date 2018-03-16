#include "workthread.h"
#include <QThread>
#include <QSqlQuery>
#include <QDebug>
#include <QString>
#include <QObject>
#include <agv.h>
#include <workstationthread.h>
#include <QtNetwork/QTcpSocket>
#include <QTextCodec>
#include <QGridLayout>
#include <materialflowframe.h>
#include <QList>
#include <QEvent>
#include <QTimer>
#include <QDateTime>
#include <QtCore>
#include <QException>
#include <QMessageBox>
#include <QColor>
#include <warndialog.h>
#include <QSqlDatabase>
#include <database.h>
#include <endstationthread.h>
#include <mesdialog.h>

WorkThread::WorkThread(QString workArea ,MaterialFlowFrame *frame,QObject *parent) : QThread(parent),workArea(workArea),frame(frame)
{

    MES = new Mes();

    query = DataBase::getQuery(workArea);

    initAgv(workArea);

    drawAgv();

    if(list.size()!=0){
        list.at(0)->setClick(list.at(0)->getState());
        vector.append(list.at(0));
    }

    queryStation(workArea);

    initTask();

    frame->startStation->clear();
    for(int i=0;i<stationList.size();i++){
        frame->startStation->addItem(stationList.at(i));
    }


    connect(this,SIGNAL(send(QString)),this,SLOT(sendData(QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(finishThread()),this,SLOT(closeThread()));

    connect(frame->flowNumber,SIGNAL(returnPressed()),this,SLOT(dispatcherTask()));
    connect(frame->stopTaskButton,SIGNAL(clicked(bool)),this,SLOT(stopTask()));
    connect(frame->startOrStopButton,SIGNAL(clicked(bool)),this,SLOT(startOrStopTask()));
    connect(frame->manualControlButton,SIGNAL(clicked(bool)),this,SLOT(manualControl()));
    connect(this,SIGNAL(finishDispatcherSignal()),frame,SLOT(finishDispatcherWarning()));

    //任务调度线程
    workStationThread = new WorkStationThread(workArea,this);
    connect(workStationThread,SIGNAL(startStationSignal(QString,QString)),this,SLOT(dispatcherStartStation(QString,QString)));
    workStationThread->start();

    //houbei
    readyStationThread = new ReadyStationThread(workArea,this);
    connect(readyStationThread,SIGNAL(readyStationSignal(QString)),this,SLOT(dispatcherReadyStation(QString)));
    readyStationThread->start();

}


//关闭
bool WorkThread::closeThread(){
    if(agvMap.size()!=0){
        for(a=agvMap.constBegin();a!=agvMap.end();a++){
            if(a.value()->getState()!="闲置中" && a.value()->getState()!="故障中"){
                emit finishDispatcherSignal();
                return false;
            }
        }
    }

   if(workStationThread!=NULL&&readyStationThread!=NULL){
       sql = "update agv_agv set isStart='否'  where work_area_id in (select id from agv_work_area where name='"+workArea+"')";
       query.exec(sql);

//       EndStationThread *endThread = new EndStationThread(workStationThread,readyStationThread,this);
//       endThread->start();

       workStationThread->threadState = false;
       workStationThread->terminate();
       workStationThread->wait();
//       workStationThread->deleteLater();
       workStationThread = NULL;

       readyStationThread->threadState = false;
//       readyStationThread->deleteLater();
       readyStationThread->terminate();
       readyStationThread->wait();
       readyStationThread = NULL;

       if(agvMap.size()!=0){
           for(a=agvMap.constBegin();a!=agvMap.end();a++){
               a.key()->close();
               a.key()->deleteLater();
               list.removeOne(a.value());
               drawRemoveAgv(a.value());
           }
       }

       agvMap.clear();

       return true;
   }

}

//手动指定工位
void WorkThread::manualControlPoint(){
    QMap<QTcpSocket*,Agv*>::const_iterator a;
    //QMap<QString,QVector<Station*>*>::const_iterator j;
    if(vector.size()!=0){

        //j = stationMap.find(START_STATION_NAME);
        message = TRANSACTION+manualTaskPoint->goalPoint->currentText();

        for(a=agvMap.constBegin();a!=agvMap.end();a++){
            if(a.value()->getCode()==vector.first()->getCode()){
                if(a.value()->getState()!="送货中" && a.value()->getState()!="准备送货中" && a.value()->getState()!="故障中"){
                     if(a.value()->getState()=="准备调度中" || a.value()->getState()=="调度中"){
                         if(isContainReadyStation(a.value()->getEndPoint())){
                             sql = "update agv_work_station set state=0 where code='"+a.value()->getEndPoint()+"'";
                             query.exec(sql);

                         }else{
                             sql = "update agv_task set task_state='等待中' where flow_code='"+a.value()->getFlowCode()+"'";
                             query.exec(sql);

                         }
                     }

                     if(a.value()->getState()=="返回中" || a.value()->getState()=="准备返回中"){
                         sql = "update agv_work_station set state=0 where code='"+a.value()->getEndPoint()+"'";
                         query.exec(sql);
                     }

                     a.value()->setSendData(message);
                     a.value()->setEndPoint(manualTaskPoint->goalPoint->currentText());
                     a.key()->write(message.toUtf8());
                     a.key()->waitForBytesWritten();
                     a.key()->flush();
                     a.value()->setState("手动控制");
                }

                manualTaskPoint->close();
                delete manualTaskPoint;
                return;
            }
        }
    }
}

void WorkThread::manualControl(){

    if(vector.size()!=0){
        manualTaskPoint = new ManualTaskPoint(frame);
        for(int i=0;i<stationList.size();i++){
            manualTaskPoint->goalPoint->addItem(stationList.at(i));
        }
        manualTaskPoint->show();
        connect(manualTaskPoint->comfrim,SIGNAL(clicked()),this,SLOT(manualControlPoint()));
    }
}

//停止或启用AGV
void WorkThread::startOrStopAgv(int row,QString code ,QString isStart,QString areaName,TableModel* agvModel){
     QMap<QTcpSocket*,Agv*>::const_iterator a;
     QMap<QString,QVector<Station*>*>::const_iterator j;

     if(workArea==areaName){

         if(agvMap.size()!=0){
            for(a=agvMap.constBegin();a!=agvMap.end();a++){
                if(code==a.value()->getCode()){
                    if(isStart=="是"){

                        if(a.value()->getState()=="准备调度中" || a.value()->getState()=="调度中"){
                            if(isContainReadyStation(a.value()->getEndPoint())){
                                sql = "update agv_work_station set state=0 where code='"+agv->getEndPoint()+"'";
                                query.exec(sql);

                            }else{
                                sql = "update agv_task set task_state='等待中' where flow_code='"+a.value()->getFlowCode()+"'";
                                query.exec(sql);

                            }
                        }

                        if(a.value()->getState()=="返回中" || a.value()->getState()=="准备返回中"){
                            sql = "update agv_work_station set state=0 where code='"+agv->getEndPoint()+"'";
                            query.exec(sql);
                        }

                        sql = "update agv_agv set isStart='否' where id="+QString::number(a.value()->getId());
                        query.exec(sql);

                        agvModel->setItem(row,8,new QStandardItem("否"));                        
                        a.key()->disconnectFromHost();
                        a.key()->close();
                        frame->layout->removeWidget(a.value());
                        removeAgv(a.key(),a.value());
                        drawAgv();

                        return;

                    }else{
                        sql = "update agv_agv set isStart='是' where code='"+agv->getCode()+"'";
                        mes = query.exec(sql);
                        agvModel->setItem(row,8,new QStandardItem("是"));
                        a.value()->setState("闲置中");
                        a.value()->setIsDispatcher(false);
                        return;
                    }
                }
            }
         }

         if(isStart=="否"){
             sql = "select * from agv_agv where code='"+code+"'";
             query.exec(sql);
             if(query.next()){
                 int id = query.value(0).toInt();
                 QString code = query.value(1).toString();
                 QString robot_id = query.value(2).toString();
                 QString type = query.value(3).toString();
                 QString address_ip= query.value(4).toString();
                 int port = query.value(5).toString().toInt();

                 QString state = query.value(6).toString();
                 if(state!="故障中"){
                     state="闲置中";
                 }
                 QString electricity = query.value(7).toString();
                 float x = query.value(8).toString().toFloat();
                 float y = query.value(9).toFloat();

                 agv = new Agv(id,code,robot_id,state,address_ip,port,electricity,type,x,y,frame);
             }

             sql = "update agv_agv set state='闲置中',isStart='是' where code='"+code+"'";
             query.exec(sql);

             agvModel->setItem(row,8,new QStandardItem("是"));
             QTcpSocket *socket = new QTcpSocket(this);
             socket->setObjectName(code);
             socket->connectToHost(agv->getAddressIp(),agv->getPort());

             connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));
             connect(socket,SIGNAL(disconnected()),this,SLOT(repeatConnect()));
             connect(socket,SIGNAL(connected()),this,SLOT(isConnected()));
             connect(agv,SIGNAL(clicked()),this,SLOT(showAgv()));

             addAgv(socket,agv);
             drawAgv();
         }
     }
}

//手动终止AGV任务
void WorkThread::startOrStopTask(){
    QMap<QTcpSocket*,Agv*>::const_iterator a;
    QMap<QString,QVector<Station*>*>::const_iterator j;
   // j = stationMap.find(START_STATION_NAME);

    if(vector.size()!=0){
        Agv *agv = vector.first();

        if(agv->getState()=="故障中"){

            sql = "update agv_agv set isStart='是' where code='"+agv->getCode()+"'";
            query.exec(sql);

            if(agvMap.size()!=0){
                for(a=agvMap.constBegin();a!=agvMap.end();a++){
                    if(agv->getCode()==a.value()->getCode()){
                        a.value()->setState("闲置中");
                        a.value()->setIsDispatcher(false);
                        if(a.key()->state()==QAbstractSocket::ConnectedState){
                            a.key()->abort();

                            a.key()->connectToHost(a.value()->getAddressIp(),a.value()->getPort());
                            updateAgv(agv,"no data");
                            return;
                        }

                    }
                }
            }

            QTcpSocket *socket = new QTcpSocket(this);
            socket->setObjectName(agv->getCode());
            socket->connectToHost(agv->getAddressIp(),agv->getPort());

            connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));
            connect(socket,SIGNAL(disconnected()),this,SLOT(repeatConnect()));
            connect(socket,SIGNAL(connected()),this,SLOT(isConnected()));
            connect(agv,SIGNAL(clicked()),this,SLOT(showAgv()));

            addAgv(socket,agv);
            drawAgv();

        }else if(agv->getState()!="送货中" && agv->getState()!="准备送货中" && agv->getState()!="故障中"){

           if(agvMap.size()!=0){
               for(a=agvMap.constBegin();a!=agvMap.end();a++){
                   if(agv->getCode()==a.value()->getCode()){
                       a.value()->setIsDispatcher(false);
                       a.key()->disconnectFromHost();
                       a.key()->close();
                       a.key()->deleteLater();
                       agvMap.remove(a.key());

                       break;
                   }
               }
           }

            if(agv->getState()=="返回中" || agv->getState()=="准备返回中"){
                sql = "update agv_work_station set state=0 where code='"+agv->getEndPoint()+"'";
                query.exec(sql);
            }

            if(agv->getState()=="调度中" || agv->getState()=="准备调度中"){
                if(isContainReadyStation(agv->getEndPoint())){
                    sql = "update agv_work_station set state=0 where code='"+agv->getEndPoint()+"'";
                    query.exec(sql);


                }else{
                    sql = "update agv_task set task_state='等待中' where code='"+agv->getFlowCode()+"'";
                    query.exec(sql);

                }
            }

           agv->setState("故障中");
           sql = "update agv_agv set isStart='否' where code='"+agv->getCode()+"'";
           query.exec(sql);

           updateAgv(agv,"no data");

        }
    }
}

//终止任务修改对应表格
void WorkThread::stopTask(){

    int row = frame->tableView->currentIndex().row();
    QString flowCode = frame->taskModel->data(frame->taskModel->index(row,0)).toString();
    QString agvId = frame->taskModel->data(frame->taskModel->index(row,4)).toString();
    if(agvId!=""){
        if(flowCode!=""){
            sql = "update agv_task set task_state='完成', finish_time='"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"', agv_id="+agvId+"  where flow_code='"+flowCode+"'";
            query.exec(sql);

        }
    }else{
        if(flowCode!=""){
            sql = "update agv_task set task_state='完成', finish_time='"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"' where flow_code='"+flowCode+"'";
            query.exec(sql);

        }
    }

    frame->taskModel->removeRow(row);
    frame->tableView->selectRow(-1);
    frame->tableView->reset();


}

//
QSqlQuery WorkThread::getQuery(){
    return QSqlQuery(db);
}


//初始化AGV
void WorkThread::initAgv(QString workArea){
    sql = "select * from agv_agv as a where a.work_area_id = (select id from agv_work_area as area where area.name ='"+workArea+"')";

    query.exec(sql);
    while(query.next()){
        int id = query.value(0).toInt();
        QString code = query.value(1).toString();
        QString robot_id = query.value(2).toString();
        QString type = query.value(3).toString();
        QString address_ip= query.value(4).toString();
        int port = query.value(5).toString().toInt();
        QString state = query.value(6).toString();
        QString electricity = query.value(7).toString();
        float x = query.value(8).toString().toFloat();
        float y = query.value(9).toFloat();
        agv = new Agv(id,code,robot_id,state,address_ip,port,electricity,type,x,y,frame);

        socket = new QTcpSocket(this);
        socket->setObjectName(agv->getCode());
        socket->connectToHost(agv->getAddressIp(),agv->getPort());
        addAgv(socket,agv);

        connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));
        connect(socket,SIGNAL(disconnected()),this,SLOT(repeatConnect()),Qt::QueuedConnection);
        connect(socket,SIGNAL(connected()),this,SLOT(isConnected()));
        connect(agv,SIGNAL(clicked()),this,SLOT(showAgv()));
    }

    sql = "update agv_agv set isStart='是' where agv_agv.work_area_id = (select id from agv_work_area as area where area.name ='"+workArea+"')";
    query.exec(sql);
}

//表格初始化显示为完成任务
void WorkThread::initTask(){
    sql = "select * from agv_task where agv_id is NULL and task_state ='等待中' and work_area_id in (select id from agv_work_area where name='"+workArea+"')";
    query.exec(sql);
    while(query.next()){

        QString flowCode = query.value(1).toString();
        item = new QStandardItem(flowCode);

        tableList.append(item);

        QString startPoint = query.value(6).toString();
        item = new QStandardItem(startPoint);
        tableList.append(item);

        QString endPoint = query.value(7).toString();
        item = new QStandardItem(endPoint);
        tableList.append(item);

        QString state = query.value(2).toString();
        item = new QStandardItem(state);
        tableList.append(item);

        frame->taskModel->appendRow(tableList);

        tableList.clear();
    }
}

//获取工作区域工位信息
void WorkThread::queryStation(QString workArea){
    if(stationVector->size()!=0){
        stationVector->clear();
    }

    sql = "select station_name  from agv_work_station where work_area_id in (select id from agv_work_area where name='"+workArea+"') group by station_name";
    query.exec(sql);
    while(query.next()){
        QString stationName = query.value(0).toString();

        stationVector= new QVector<Station*>;
        stationMap.insert(stationName,stationVector);
    }

    QMap<QString,QVector<Station*>*>::const_iterator i;

    query.exec("select * from agv_work_station as a where a.work_area_id = (select id from agv_work_area as area where area.name ='"+workArea+"');");
    while(query.next()){
        int id = query.value(0).toInt();
        QString code = query.value(1).toString();
        QString agvCode = query.value(2).toString();
        int state = query.value(5).toInt();
        float x = query.value(3).toFloat();
        float y = query.value(4).toFloat();
        QString stationName = query.value(6).toString();
        station = new Station(code,agvCode,state,x,y,stationName);
        if(!isInitStation){
            stationList.append(code);
        }
        station->setId(id);
        i = stationMap.find(stationName);
        if(i!=stationMap.end()){
            i.value()->append(station);
        }

    }

    isInitStation = true;


//     for(i=stationMap.constBegin();i!=stationMap.constEnd();i++){
//             qDebug()<<i.key()<<i.value();
//         for(int j=0;j<i.value()->size();j++){
//             qDebug()<<i.value()->at(j)->getCode()<<i.value()->at(j)->getState();
//         }
//     }
}

//重新连接
void WorkThread::repeatConnect(){
    QMap<QTcpSocket*,Agv*>::const_iterator i;
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

    if(agvMap.size()!=0){
        i = agvMap.find(socket);
        if(i!=agvMap.end()){
            i.key()->connectToHost(i.value()->getAddressIp(),i.value()->getPort());
        }
    }
}

void WorkThread::removeAgv(QTcpSocket *socket,Agv *agv){
    agv->setParent(NULL);
    frame->layout->removeWidget(agv);

    list.removeOne(agv);
    agvMap.remove(socket);
    socket->deleteLater();

}

//从面板删除AGV
void WorkThread::drawRemoveAgv(Agv *agv){
    agv->setParent(NULL);
    frame->layout->removeWidget(agv);
}

//面板初始，画出所有AGV
void WorkThread::drawAgv(){

    int a=0;    
    for(int i=0;i<row;i++){
        for(int j=0;j<colum;j++){
            if(a<list.size()){
                frame->layout->addWidget(list.at(a),i,j,10,-10,Qt::AlignTop);
                a++;
            } else {
               break;
            }
        }
    }
}

//增加AGV
void WorkThread::addAgv(QTcpSocket *socket, Agv *agv){
    agvMap.insert(socket,agv);
    if(!list.contains(agv)){
         list.append(agv);
    }
}

//判断是否是候备区
bool WorkThread::isContainReadyStation(QString code){

    i = stationMap.find(READY_STATION_NAME);
    bool isContain = false;
    QVector<Station*> *findStationVector = NULL;
    if(i!=stationMap.end()){
        findStationVector = i.value();
        if(findStationVector!=NULL){
            for(int n=0;n<findStationVector->size();n++){

                if(code == findStationVector->at(n)->getCode()){
                    isContain = true;
                    break;
                }
            }
        }

    }

    findStationVector = NULL;
    return isContain;
}

//候备区自动调度AGV
void WorkThread::dispatcherReadyStation(QString endPoint){
    QVector<Station*> *findStationVector = NULL;
    i = stationMap.find(READY_STATION_NAME);
    if(i!=stationMap.end()){
        findStationVector = i.value();
        if(findStationVector!=NULL){
            for(int n=0;n<i.value()->size();n++){
                if(findStationVector->at(n)->getCode()==endPoint){
                    x = i.value()->at(n)->getX();
                    y = i.value()->at(n)->getY();
                    break;
                }
            }
        }

    }
    findStationVector = NULL;

    message = TRANSACTION+endPoint;

    //等候区掉
    this->queryStation(workArea);
    i = stationMap.find(WAIT_STATION_NAME);
    if(i!=stationMap.end()){
        findStationVector = i.value();
        if(findStationVector!=NULL){
            for(int j=0;j<findStationVector->size();j++){
                if(findStationVector->at(j)->getState()==1){
                    socketAgvMap = getSocketAndAgv(findStationVector->at(j)->getAgvCode());

                    if(socketAgvMap.size()>0){
                        startPoint = findStationVector->at(j)->getCode();

                        sql = "update agv_work_station set state=2 where code='"+endPoint+"'";
                        query.exec(sql);


                        writeData(startPoint,endPoint,socketAgvMap.first()->getCode(),message,socketAgvMap.first()->getFlowCode());
                        socketAgvMap.first()->setIsDispatcher(true);
                        findStationVector = NULL;
                        return;
                    }
                }
            }
        }

    }

    //从最近调
    if(agvMap.size()!=0){

        min =0.0;
        for(a=agvMap.constBegin();a!=agvMap.end();a++){

            if(a.value()->getState()=="闲置中" && a.key()->state()==QAbstractSocket::ConnectedState && a.value()->getIsDispatcher()==false && this->isContainReadyStation(a.value()->getStartPoint())==false){

                x0 = a.value()->getX()-x;
                y0 = a.value()->getY()-y;


                s = x0*x0+y0*y0;

                if(min==0.0){
                    min = s;
                    socketArr[0]=a.key();
                }

                if(s<=min){
                    min = s;
                    socketArr[0]= a.key();
                }
            }
        }

         a = agvMap.find(socketArr[0]);
         if(a!=agvMap.end()){

             sql = "update agv_work_station set state=2 where code='"+endPoint+"'";
             query.exec(sql);

             a.value()->setIsDispatcher(true);
             writeData(a.value()->getStartPoint(),endPoint,a.value()->getCode(),message,agv->getFlowCode());
        }

        socketArr[0]=NULL;
        findStationVector = NULL;

    }else{


    }
}

//任务调度AGV
void WorkThread::dispatcherStartStation(QString endPoint,QString flowCode){
    QVector<Station*> *findStationVector = NULL;
   if(stationMap.size()>0){
       for(i=stationMap.constBegin();i!=stationMap.end();i++){
           if(i.value()->first()->getCode()==endPoint){
               x = i.value()->first()->getX();
               y = i.value()->first()->getY();
               break;
           }
       }
   }

    message = TRANSACTION+endPoint;

    if(agvMap.size()!=0){
        min = 0.0;
        for(a=agvMap.constBegin();a!=agvMap.end();a++){

            if((a.value()->getState()=="闲置中" || a.value()->getState()=="返回中") && a.key()->state()==QAbstractSocket::ConnectedState && a.value()->getIsDispatcher()==false ){
                x0 = a.value()->getX()-x;
                y0 = a.value()->getY()-y;
                s = x0*x0+y0*y0;
                if(min==0.0){
                    min = s;
                    socketArr[0]=a.key();
                }

                if(s<=min){
                    min = s;
                    socketArr[0]= a.key();
                }
            }
        }



        a = agvMap.find(socketArr[0]);
        if(a!=agvMap.end()){

            i = stationMap.find(READY_STATION_NAME);
            if(i!=stationMap.end()){
                findStationVector = i.value();
                if(findStationVector!=NULL){
                    for(int n=0;n<findStationVector->size();n++){
                        if(a.key()->objectName()==findStationVector->at(n)->getAgvCode()){
                            a.value()->setStartPoint(findStationVector->at(n)->getCode());
                            break;
                        }
                    }
                }

            }

            findStationVector = NULL;

            i = stationMap.find(WAIT_STATION_NAME);
            if(i!=stationMap.end()){
                findStationVector = i.value();
                if(findStationVector!=NULL){
                    for(int n=0;n<findStationVector->size();n++){
                        if(a.key()->objectName()==findStationVector->at(n)->getAgvCode()){
                            a.value()->setStartPoint(findStationVector->at(n)->getCode());
                            break;
                        }
                    }
                }
            }


            if(a.value()->getState()=="返回中"){
                a.value()->setStartPoint(a.value()->getEndPoint());
            }


            sql = "update agv_task set task_state='锁定中' where flow_code='"+flowCode+"'";
            query.exec(sql);


            writeData(a.value()->getStartPoint(),endPoint,a.value()->getCode(),message,flowCode);
            a.value()->setIsDispatcher(true);
            socketArr[0]=NULL;
            findStationVector = NULL;
        }
    }else{

    }

}

void WorkThread::showAgv(){
    vector.clear();
    Agv *agv = qobject_cast<Agv*>(sender());
    for(int i=0;i<list.size();i++){
        if(list.at(i)==agv){
            agv->setClick(agv->getState());
        }else{
            list.at(i)->setUnClick(list.at(i)->getState());
        }
    }

    vector.append(agv);

    updateAgv(agv,"no data");
}


//连接mes获取送货任务，存入数据库，
//从MES类调用get方法获取工位编号
void WorkThread::dispatcherTask(){
    QMap<QString,QVector<Station*>*>::const_iterator j;
    QString stationName = frame->startStation->currentText();
    query.clear();
    sql = "select id from agv_work_area where name='"+workArea+"'";
    query.exec(sql);
    query.next();
    int workAreaId = query.value(0).toInt();

    //从mes获取目标工位
    QString url = "mes.ubtrobot.com";
    QString end_point = MES->get(url);

    if(end_point!=NULL){
        QString sql = "insert ignore into agv_task(flow_code,task_state,new_time,start_point,end_point,work_area_id) values(:flow_code,:task_state,:new_time,:start_point,:end_point,:work_area_id)" ;
        query.prepare(sql);
        query.bindValue(":flow_code",frame->flowNumber->text());
        query.bindValue(":task_state","等待中");
        query.bindValue(":new_time",QDateTime::currentDateTime());
        query.bindValue(":start_point",stationName);
        query.bindValue(":end_point",end_point);
        query.bindValue(":work_area_id",workAreaId);
        query.exec();

        item = new QStandardItem(frame->flowNumber->text());
        tableList.append(item);
        item = new QStandardItem(stationName);
        tableList.append(item);

        item = new QStandardItem(end_point);
        tableList.append(item);
        item = new QStandardItem("等待中");
        tableList.append(item);
        item = new QStandardItem();
        tableList.append(item);
        frame->taskModel->appendRow(tableList);
        tableList.clear();

        frame->flowNumber->clear();

    }else{
        MesDialog *mesDialog = new MesDialog();
        mesDialog->show();
    }





}

//更新AGV面板信息
void WorkThread::updateAgv(Agv *agv,QString read){
    QMap<QString,QVector<Station*>*>::const_iterator i;

    QRegExp patternLocation("GETLOCATION\\|[-]*\\d");
    QRegExp patternBattery("GETBATTERY\\|\\d");
    QRegExp patternStatus("GETAGVSTATUS");

    frame->agvCodeText->setText(agv->getCode());
    frame->agvStateText->setText(agv->getState());

    sql = "update agv_agv set state='"+agv->getState()+"' where id="+QString::number(agv->getId());
    query.exec(sql);

    if(patternLocation.indexIn(read)!=-1){
        read = read.section('|',1,1);
        float x = read.section(',',0,0).toFloat();
        float y = read.section(',',1,1).toFloat();

        sql = "update agv_agv set X="+QString("%1").arg(x)+",Y="+QString("%1").arg(y)+" where id="+QString::number(agv->getId());
        query.exec(sql);


        location = QString("%1").arg(x)+","+QString("%1").arg(y);
        frame->agvLocationText->setText(location);

    }

    if(patternBattery.indexIn(read)!=-1){
        battery = read.section('|',1,1).toInt();
        agv->setBattery(read);
        frame->agvbatteryText->setValue(battery);

//        if(battery<10){
//              socket->write(message.toUtf8());
//              agv->setSendData(message);
//        }

//        sql = "update agv_agv set electricity="+QString::number(battery)+" where id="+agv->getId();
//        query.exec(sql);
    }

    if(patternStatus.indexIn(read)!=-1){
        read = read.section('|',1,1);
        if(read=="NG"){
            agv->setState("异常");
            frame->agvStateText->setText("异常");
        }else{
            agv->setState("正常");
        }
    }
}

//获取socket,跟对应AGV
QMap<QTcpSocket*,Agv*> WorkThread::getSocketAndAgv(QString agvCode){
    socketAgv.clear();
    if(agvMap.size()!=0){
        for(a=agvMap.constBegin();a!=agvMap.end();a++){

            if(agvCode==a.value()->getCode() && a.value()->getState()=="闲置中" && a.key()->state()!=QAbstractSocket::ConnectedState && a.value()->getIsDispatcher()==false){
                socketAgv.insert(a.key(),a.value());
                break;
            }
        }
    }

    return socketAgv;
}


//获取agv对应socket
QTcpSocket* WorkThread::getSocket(QString code){
    QMap<QTcpSocket*,Agv*>::const_iterator j;
    QTcpSocket *socket = NULL;
    for (j = agvMap.constBegin(); j != agvMap.constEnd(); ++j) {
        if(j.value()->getCode()== code){
            socket = j.key();
            break;
        }
    }

    return socket;
}

//重发指令
void WorkThread::reSend(){
    QTimer *timer = qobject_cast<QTimer*>(sender());
    if(agvMap.size()!=0){
        for(a=agvMap.constBegin();a!=agvMap.end();a++){
            if(a.value()->getCode()==timer->objectName()){
                if(a.value()->getTimes()<reSendTimes){
                     if(a.value()->getState()!="故障中"){
                         a.key()->write(a.value()->getSendData().toUtf8());
                         a.key()->waitForBytesWritten();
                         a.key()->flush();
                         a.value()->addTimes(1);
                     }
                      return;
                }else{

                    if(a.value()->getReSendTimer()!=NULL){
                        if(a.value()->getReSendTimer()->isActive()){
                            a.value()->getReSendTimer()->stop();
                            delete a.value()->getReSendTimer();
                            a.value()->setReSendTimer(NULL);
                        }
                     }


                    if(!isContainReadyStation(a.value()->getEndPoint())){
                        if(a.value()->getSendData().startsWith(TRANSACTION)){
                            sql = "update agv_task set task_state='等待中' where flow_code='"+a.value()->getFlowCode()+"'";
                            query.exec(sql);

                        }

                        if(a.value()->getSendData().startsWith(NAVIGATION)){
                            sql = "update agv_task set task_state='等待中' where flow_code='"+a.value()->getFlowCode()+"'";
                            query.exec(sql);

                        }

                        if(a.value()->getSendData().startsWith(BACKTOSTATION)){
                            sql = "update agv_work_station set agv_code=NULL,state=0 where code='"+a.value()->getStartPoint()+"'";
                            query.exec(sql);

                        }
                    }else{
                        sql = "update agv_work_station set state=0 where code='"+agv->getEndPoint()+"'";
                        query.exec(sql);

                    }

                    sql = "update agv_agv set state='故障中' where id="+QString::number(a.value()->getId());
                    query.exec(sql);
                    a.value()->setState("故障中");

                    return;
                }

            }
        }
    }

}

void WorkThread::isConnected(){
//    QMap<QTcpSocket*,Agv*>::const_iterator i;
//    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
//    i = agvMap.find(socket);

//    if(!manualDisconnect){
//        socket->connectToHost(i.value()->getAddressIp(),i.value()->getPort());
//    }

//    manualDisconnect = false;
}

//槽函数监听AGV反馈信息
void WorkThread::receiveData(){

    QMap<QTcpSocket*,Agv*>::const_iterator i;
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if(agvMap.size()!=0){
        i = agvMap.find(socket);
        if(i!=agvMap.end()){
            QString read = socket->readLine();
             read = read.left(read.length()-1);


    //获取工站位置
            getWorkStationLocation(i.value(),read);

    //start_station_dispatcher
            transaction(i.key(),i.value(),read);

    //到达返回----------------------
            backtostation(i.key(),i.value(),read);

    //工位送货------------------
            navigation(i.key(),i.value(),read);
    //面板信息显示-------------------------------
            if(vector.size()!=0){
                if(i.value()==vector.first()){
                    updateAgv(i.value(),read);
                }
            }
        }
    }


}

//调度指令处理
void WorkThread::transaction(QTcpSocket *socket, Agv *agv, QString read){

    i = stationMap.find(READY_STATION_NAME);

    //调度反馈
    if(read=="TRANSACTION|RSUCCESS"){

       agv->setTimes(0);

       if(agv->getReSendTimer()!=NULL){
           if(agv->getReSendTimer()->isActive()){
               agv->getReSendTimer()->stop();
               delete agv->getReSendTimer();
               agv->setReSendTimer(NULL);
           }
        }

        if(agv->getState()=="返回中"){
            sql = "update agv_work_station set state=0 where code='"+agv->getStartPoint()+"'";
            query.exec(sql);

        }

        //工站任务调度
//        if(i!=stationMap.end()){

//            if(!isContainReadyStation(agv->getEndPoint())){

//                if(agv->getState()!="手动控制"){

//                }
//            }


//        }

        sql = "update agv_agv set state='准备调度中' where id="+QString::number(agv->getId());
        query.exec(sql);


        if(agv->getState()!="手动控制"){
            agv->setState("准备调度中");
        }
    }

    //开始跑动

    if(read=="MOVING"){

            if(agv->getState()=="手动控制"){
                sql = "update agv_work_station set state=0,agv_code=NULL where code='"+agv->getStartPoint()+"'";
                query.exec(sql);

            }

            if(agv->getState()=="准备返回中"){
                sql = "update agv_agv set state='返回中' where id="+QString::number(agv->getId());

                query.exec(sql);
                agv->setState("返回中");
            }


            if(agv->getState()=="准备调度中"){
                sql = "update agv_agv set state='调度中' where id="+QString::number(agv->getId());

                query.exec(sql);
                agv->setState("调度中");

            }

            if(agv->getState()=="准备送货中"){
                sql = "update agv_agv set state='送货中' where id="+QString::number(agv->getId());

                query.exec(sql);
                agv->setState("送货中");

                sql = "update agv_task set state='送货中' where flow_code='"+agv->getFlowCode()+"'";
                query.exec(sql);

                if(frame->taskModel->findItems(agv->getFlowCode(),Qt::MatchExactly,0).size()!=0){
                    int row = frame->taskModel->findItems(agv->getFlowCode(),Qt::MatchExactly,0).first()->row();
                    frame->taskModel->setItem(row,3,new QStandardItem("送货中"));
                }

            }

        sql = "update agv_work_station set agv_code=NULL,state=0 where code='"+agv->getStartPoint()+"'";

        query.exec(sql);
    }

    //调度异常
    if(read=="TRANSACTION|NG"){

        if(agv->getNGTimes()<ngTimes){
            agv->addNGTimes(1);
            socket->write(agv->getSendData().toUtf8());
        }else{
            //机器故障

            //添加断开连接操作

            //
            agv->setNGTimes(0);
            agv->setSendData("");

            if(isContainReadyStation(agv->getEndPoint())){
                if(agv->getState()=="准备调度中"){
                    sql = "update agv_work_station set state=0,agv_code=NULL where code='"+agv->getStartPoint()+"'";
                    query.exec(sql);


                    sql = "update agv_work_station set state=0 where code='"+agv->getEndPoint()+"'";
                    query.exec(sql);


                }

                if(agv->getState()=="调度中"){
                    sql = "update agv_work_station set state=0 where code='"+agv->getEndPoint()+"'";
                    query.exec(sql);

                }

            }else{
                if(agv->getState()=="准备调度中"){
                    sql = "update agv_work_station set state=0,agv_code=NULL where code='"+agv->getStartPoint()+"'";
                    query.exec(sql);


                    sql = "update agv_task set task_state='等待中' where flow_code='"+a.value()->getFlowCode()+"'";
                    query.exec(sql);

                }

                if(agv->getState()=="调度中"){
                    sql = "update agv_task set task_state='等待中' where flow_code='"+a.value()->getFlowCode()+"'";
                    query.exec(sql);

                }
            }

            agv->setState("故障中");
            sql = "update agv_agv set state='故障中' where id="+QString::number(agv->getId());
            query.exec(sql);

        }
    }

    //调度成功
    if(read=="TRANSACTION|OK"){
        agv->setIsDispatcher(false);
        agv->setNGTimes(0);
        query.clear();
        sql = "update agv_work_station set state=1,agv_code='"+agv->getCode()+"' where code='"+agv->getEndPoint()+"'";
        query.exec(sql);


        if(isContainReadyStation(agv->getEndPoint())==true){
            agv->setState("闲置中");
            agv->setIsDispatcher(false);
            agv->setStartPoint(agv->getEndPoint());
        }else{

            if(agv->getState()!="手动控制"){
                sql = "select end_point,flow_code from agv_task where start_point='"+agv->getEndPoint()+"' and agv_id is NULL and task_state='锁定中' and flow_code is not NULL  limit 1";
                query.exec(sql);


                if(query.next()){
                    writeData(agv->getEndPoint(),query.value(0).toString(),agv->getCode(),NAVIGATION+query.value(0).toString(),query.value(1).toString());
                }

                 agv->setState("等待中");

            }else{

                agv->setState("闲置中");
                agv->setStartPoint(agv->getEndPoint());
            }
        }



        sql = "update agv_agv set state="+agv->getState()+" where id="+QString::number(agv->getId());
        query.exec(sql);


    }

}

//返回指令处理
void WorkThread::backtostation(QTcpSocket *socket, Agv *agv, QString read){


    // //锁定返回工位
    if(read=="BACKTOSTATION|RSUCCESS"){
        agv->setTimes(0);

        sql = "update agv_work_station set state=2 where code='"+agv->getEndPoint()+"'";
        query.exec(sql);


        sql = "update agv_agv set state='准备返回中' where id="+QString::number(agv->getId());
        agv->setState("准备返回中");
        query.exec(sql);


        if(agv->getReSendTimer()!=NULL){
            if(agv->getReSendTimer()->isActive()){
                agv->getReSendTimer()->stop();
                delete agv->getReSendTimer();
                agv->setReSendTimer(NULL);
            }
        }

     }

        //到达返回工位
    if(read == "BACKTOSTATION|OK"){
        agv->setNGTimes(0);
        agv->setIsDispatcher(false);

        sql = "update agv_work_station set agv_code='"+agv->getCode()+"',state=1 where code='"+agv->getEndPoint()+"'";
        query.exec(sql);


        sql = "update agv_agv set state='闲置中' where id="+QString::number(agv->getId());
        query.exec(sql);
        agv->setState("闲置中");


        agv->setStartPoint(agv->getEndPoint());
        queryStation(workArea);
    }


    //返回工位失败判断
    if(read=="BACKTOSTATION|NG"){
        if(agv->getNGTimes()<ngTimes){
            socket->write(agv->getSendData().toUtf8());
            agv->addNGTimes(1);
        }else{
            agv->setNGTimes(0);

            if(agv->getState()=="准备返回中"){
                sql = "update agv_work_station set state=0,agv_code=NULL where code='"+agv->getStartPoint()+"'";
                query.exec(sql);
            }

            sql = "update agv_work_station set state=0 where code='"+agv->getEndPoint()+"'";
            query.exec(sql);

            sql = "update agv_agv set state='故障中' where id="+QString::number(agv->getId());
            agv->setState("故障中");
            query.exec(sql);
        }
    }
}

//送货指令处理
void WorkThread::navigation(QTcpSocket *socket, Agv *agv,QString read){

    //i = stationMap.find(START_STATION_NAME);

     //机器送货指令接收
    if(read=="NAVIGATION|RSUCCESS"){
        agv->setTimes(0);

        if(agv->getReSendTimer()!=NULL){
            if(agv->getReSendTimer()->isActive()){
                agv->getReSendTimer()->stop();
                delete agv->getReSendTimer();
                agv->setReSendTimer(NULL);
            }
        }


        sql = "update agv_task set state = '准备送货中',agv_id="+QString::number(agv->getId())+" where flow_code='"+agv->getFlowCode()+"'";
        query.exec(sql);



        sql = "update agv_agv set state='准备送货中' where id="+QString::number(agv->getId());
        query.exec(sql);


        agv->setState("准备送货中");

        if(frame->taskModel->findItems(agv->getFlowCode(),Qt::MatchExactly,0).size()!=0){
            int row = frame->taskModel->findItems(agv->getFlowCode(),Qt::MatchExactly,0).first()->row();
            frame->taskModel->setItem(row,3,new QStandardItem("准备送货中"));
            frame->taskModel->setItem(row,4,new QStandardItem(agv->getCode()));
        }
     }

   //机器送货NG指令
    if(read=="NAVIGATION|NG"){
        if(agv->getNGTimes()<ngTimes){
              socket->write(agv->getSendData().toUtf8());
              agv->addNGTimes(1);
         }else{

              if(agv->getState()=="准备送货中"){                    
                    sql = "update agv_work_station set agv_code=NULL,state=0 where code='"+agv->getStartPoint()+"'";
                    query.exec(sql);                  


                    sql="update agv_task set task_state='失败',agv_id=NULL,where flow_code='"+agv->getFlowCode()+"'";
                    query.exec(sql);


              }else{
                   sql = "update agv_task set task_state='失败' where flow_code='"+agv->getFlowCode()+"'";
                   mes = query.exec(sql);

              }

                sql = "update agv_agv set state='故障中' where id="+QString::number(agv->getId());
                agv->setState("故障中");


                mes = query.exec(sql);

                if(frame->taskModel->findItems(agv->getFlowCode(),Qt::MatchExactly,0).size()!=0){
                    int row = frame->taskModel->findItems(agv->getFlowCode(),Qt::MatchExactly,0).first()->row();
                    frame->taskModel->setItem(row,3,new QStandardItem("任务失败"));
                    frame->taskModel->findItems(agv->getFlowCode(),Qt::MatchExactly,3).first()->setBackground(QColor(Qt::red));

                }

                agv->setNGTimes(0);
                agv->setSendData("");
                agv->setSendData("");
                agv->setFlowCode("");

          }
      }

        //完成发送返回指令
    if(read=="NAVIGATION|OK"){
        agv->setIsDispatcher(false);
        //更新目标工站
        sql = "update agv_work_station set agv_code='"+agv->getCode()+"',state=1 where code='"+agv->getEndPoint()+"'";
        mes = query.exec(sql);


        //更新任务表
        sql = "update agv_task set task_state='完成', agv_id="+QString::number(agv->getId())+",finish_time='"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"' where flow_code='"+agv->getFlowCode()+"'";
        query.exec(sql);


        if(frame->taskModel->findItems(agv->getFlowCode(),Qt::MatchExactly,0).size()!=0){
            int row = frame->taskModel->findItems(agv->getFlowCode(),Qt::MatchExactly,0).at(0)->row();
             frame->taskModel->removeRow(row);
        }


        //更新agv表
        sql = "update agv_agv set state='已送达' where id="+QString::number(agv->getId());
        query.exec(sql);

        agv->setState("已送达");

        agv->setNGTimes(0);
        agv->setSendData("");
        agv->setFlowCode("");

        this->queryStation(workArea);
        //等候区
        i = stationMap.find(WAIT_STATION_NAME);
        for(int n=0;n<i.value()->size();n++){
            if(i.value()->at(n)->getState()==0){
                message = BACKTOSTATION+i.value()->at(n)->getCode();
                agv->setStartPoint(agv->getEndPoint());
                agv->setEndPoint(i.value()->at(n)->getCode());
                writeData(agv->getStartPoint(),agv->getEndPoint(),agv->getCode(),message,agv->getFlowCode());
                return;
            }
        }
    }

 }

//获取AGV位置信息
void WorkThread::getWorkStationLocation(Agv *agv, QString read){
     QRegExp patternStationLocation("GETSTATIONLOCATION\\|[-]*\\d");

     if(patternStationLocation.indexIn(read)!=-1){
         read = read.section('|',1,1);
         float x = read.section(',',0,0).toFloat();
         float y = read.section(',',1,1).toFloat();

         sql = "update agv_work_station set X="+QString("%1").arg(x)+",Y="+QString("%1").arg(y)+" where code='"+agv->getStationName()+"'";
         query.exec(sql);
     }
}


//发送agv基本指令
void WorkThread::sendData(QString args){

    QMap<QTcpSocket*,Agv*>::const_iterator j;
    if(agvMap.size()!=0){

        if(args.startsWith("S0")){
            for (j = agvMap.constBegin(); j != agvMap.constEnd(); j++) {
                if(j.value()->getState()!="故障中" && j.key()->state()==QAbstractSocket::ConnectedState  ){
                        message = "GETSTATIONLOCATION|"+args;
                        j.value()->setStationName(stationList.at(x));
                        j.key()->flush();
                        j.key()->write(message.toUtf8());
                        j.key()->waitForBytesWritten();
                        j.key()->flush();
                }

                return;
            }
        }

        if(args == "location"){
            for (j = agvMap.constBegin(); j != agvMap.constEnd(); j++) {
                QTextCodec *codec = QTextCodec::codecForName("GBK");
                QString *message2 = new QString("GETLOCATION");
                if(j.key()->state()==QAbstractSocket::ConnectedState){

                    if(j.value()->getState()!="故障中" && j.key()->state()==QAbstractSocket::ConnectedState  ){
                        j.key()->write(message2->toUtf8());
                        j.key()->waitForBytesWritten();
                        j.key()->flush();
                    }
                }else{
                    j.value()->setState("故障中");

                }

            }
        }

        if(args == "state"){
            for (j = agvMap.constBegin(); j != agvMap.constEnd(); j++) {
                QTextCodec *codec = QTextCodec::codecForName("GBK");
                QString *message2 = new QString("GETSTATUS");
                 if(j.value()->getState()!="故障中" && j.key()->state()==QAbstractSocket::ConnectedState){
                    j.key()->write(message2->toUtf8());
                    j.key()->waitForBytesWritten();
                    j.key()->flush();
                }
            }
        }

        if(args == "battery"){
            for (j = agvMap.constBegin(); j != agvMap.constEnd(); j++) {
                QTextCodec *codec = QTextCodec::codecForName("GBK");
                QString *message2 = new QString("GETBATTERY");
                 if(j.value()->getState()!="故障中" && j.key()->state()==QAbstractSocket::ConnectedState){
                    j.key()->write(message2->toUtf8());
                    j.key()->waitForBytesWritten();
                    j.key()->flush();
                }
            }
        }
    }

}

//发送指令
void WorkThread::writeData(QString startPoint,QString endPoint,QString agvCode, QString data,QString flowCode){

        QTimer *repeatTimer = new QTimer(this);
        repeatTimer->setObjectName(agvCode);
        connect(repeatTimer,SIGNAL(timeout()),this,SLOT(reSend()));


        for (a = agvMap.constBegin(); a != agvMap.constEnd(); a++) {
            if(a.value()->getCode()==agvCode){
                a.value()->setStartPoint(startPoint);
                a.value()->setEndPoint(endPoint);
                a.value()->setSendData(data);
                a.value()->setReSendTimer(NULL);
                a.value()->setReSendTimer(repeatTimer);
                a.value()->setFlowCode(flowCode);
                a.key()->flush();
                a.key()->write(data.toUtf8());
                a.key()->waitForBytesWritten();
                a.key()->flush();
                break;
            }
        }
        repeatTimer->start(15000);

}


//线程run方法
void WorkThread::run(){
    int n=0;
     while(threadState){

         queryStation(workArea);
         this->sleep(3);

         if(n<getStaionLocationTimes){
             for(int x=0;x<stationList.size();x++){
                 this->send(stationList.at(x));
                 this->msleep(5000);
             }
             n++;
         }

        this->sleep(5);
        this->send("location");
        this->msleep(5000);
        this->send("state");
        this->msleep(5000);
        this->send("battery");
        this->msleep(5000);
    }


     DataBase::closeDataBase(workArea);
}
