#ifndef WORKSTATIONTHREAD_H
#define WORKSTATIONTHREAD_H
#include <agv.h>
#include <QMap>
#include <QtNetwork/QTcpSocket>
#include <QTcpSocket>
#include <QThread>
#include <QQueue>
#include <QVector>
#include <materialflowframe.h>
#include <QSqlDatabase>
#include <QMap>
#include <QVector>
#include <station.h>
#include <QSqlQuery>
#include <QSqlDatabase>
class WorkStationThread : public QThread
{

    Q_OBJECT
public:
    WorkStationThread( QString workArea,QObject *parent);
//    explicit WorkStationThread(QObject *parent = 0);

    MaterialFlowFrame *frame;
    QString workArea;
    QString WORK_STATION_NAME = "WORK";

    QString flowCode;

    QSqlQuery query;
    QSqlDatabase db;
    QString sql;

    bool threadState = true;

    QMap<QString,QVector<Station*>*>::const_iterator j;
    QMap<QTcpSocket*,Agv*>::const_iterator s;

private:

public:
//    QMap<QString,QVector<Station*>*> stationMap;

//    void setStationMap(QMap<QString,QVector<Station*>*> stationMap);
//    QMap<QString,QVector<Station*>*> getStationMap();






protected:
    void run();

signals:


    void startStationSignal(QString endPoint,QString flowCode);


public slots:
    void getStartStationTask();
};

#endif // WORKSTATIONTHREAD_H
