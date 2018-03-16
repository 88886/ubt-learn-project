#include "endstationthread.h"
#include <QSqlDatabase>
#include <QDebug>
#include <agv.h>
#include <QTcpSocket>
#include <QSqlQuery>
EndStationThread::EndStationThread(QObject *parent) : QThread(parent)
{




    db_C = QSqlDatabase::addDatabase("QMYSQL","C");
    db_C.setHostName("127.0.0.1");
    db_C.setPort(3306);
    db_C.setDatabaseName("ums");
    db_C.setUserName("root");
    db_C.setPassword("123456");
    db_C.open();




}

void EndStationThread::setStationMap(QMap<QString, QVector<Station *> *> stationMap){
    this->stationMap = stationMap;
}

void EndStationThread::getTask(){
    QMap<QString,QVector<Station*>*>::const_iterator j;
    QMap<QTcpSocket*,Agv*>::const_iterator s;
    QSqlQuery query(db_C);
    j = stationMap.find(START_STATION_NAME);

    sql = "select start_point,end_point,flow_code  from agv_task where start_point!='"+j.value()->first()->getCode()+"' and agv_id is NULL and flow_code is not NULL and task_state!='锁定中' order by new_time limit 1";
    query.exec(sql);
    if(query.next()){
        endTaskSignal(query.value(0).toString(),query.value(1).toString());
        sql = "update agv_task set task_state='锁定中' where flow_code='"+query.value(2).toString()+"'";
        query.exec(sql);

    }
}

void EndStationThread::run(){

    while(true){
        this->sleep(10);
        if(!isAccept){
            this->getTask();
        }
   }
}
