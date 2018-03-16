#include "endstationthread.h"
#include <QSqlDatabase>
#include <QDebug>
#include <agv.h>
#include <QTcpSocket>
#include <QSqlQuery>
EndStationThread::EndStationThread(WorkStationThread *workStationThread,ReadyStationThread *readyStationThread,QObject *parent) : QThread(parent),workStationThread(workStationThread),readyStationThread(readyStationThread)
{

}

void EndStationThread::stopThread(){
    if(workStationThread!=NULL){
        workStationThread->threadState = false;
        workStationThread->quit();
        workStationThread->wait();

        delete workStationThread;
        workStationThread = NULL;
    }

    if(readyStationThread!=NULL){
        readyStationThread->threadState = false;
        readyStationThread->quit();
        readyStationThread->wait();

        delete readyStationThread;
        readyStationThread = NULL;
    }
}

void EndStationThread::run(){
    stopThread();
}
