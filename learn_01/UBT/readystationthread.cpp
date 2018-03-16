#include "readystationthread.h"
#include <QThread>
#include <QDebug>
#include <QWaitCondition>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <database.h>
ReadyStationThread::ReadyStationThread(QString workArea ,QObject *parent) : QThread(parent),workArea(workArea)
{

    query = DataBase::getQuery(workArea+READY_STATION_NAME);

}

//监听后备区工位占用，随时调度
void ReadyStationThread::getReadyStationState(){

    sql = "select code from agv_work_station where  state=0 and station_name='"+READY_STATION_NAME+"' and  work_area_id = (select id from agv_work_area where name='"+workArea+"') limit 1";
    query.exec(sql);

    if(query.next()){
        emit readyStationSignal(query.value(0).toString());

    }
    query.clear();

}

void ReadyStationThread::run(){

    while(threadState){
        this->sleep(5);
        getReadyStationState();
    }

     DataBase::closeDataBase(workArea+READY_STATION_NAME);
}
