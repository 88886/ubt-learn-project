#include "workstationthread.h"
#include <agv.h>
#include <QMap>
#include <QtNetwork/QTcpSocket>
#include <QDebug>
#include <materialflowframe.h>
#include <database.h>
WorkStationThread::WorkStationThread(QString workArea ,QObject *parent) : QThread(parent),workArea(workArea)
{

    query = DataBase::getQuery(workArea+WORK_STATION_NAME);

}


//监听数据库,随时获取送货任务，一旦有agv可调，锁住
void WorkStationThread::getStartStationTask(){

    sql = "select start_point,flow_code from agv_task where agv_id is NULL and task_state='等待中' and  work_area_id = (select id from agv_work_area where name='"+workArea+"') and flow_code is not NULL limit 1";
    query.exec(sql);

    if(query.next()){
         emit startStationSignal(query.value(0).toString(),query.value(1).toString());

    }
    query.clear();


}

void WorkStationThread::run(){

    while(threadState){
        this->sleep(5);
        this->getStartStationTask();
    }

    DataBase::closeDataBase(workArea+WORK_STATION_NAME);


}
