#ifndef READYSTATIONTHREAD_H
#define READYSTATIONTHREAD_H
#include <QMap>
#include <QVector>
#include <station.h>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSqlQuery>
#include <QSqlDatabase>

class ReadyStationThread:public QThread
{
    Q_OBJECT
public:

    ReadyStationThread(QString workArea,QObject *parent);

    bool threadState=true;
    QString workArea;
    QString READY_STATION_NAME = "READY";
    QString sql;

private:

public:

    QSqlQuery query;
    QSqlDatabase db;

    void getReadyStationState();


signals:
    void readyStationSignal(QString endPoint);
protected:
    void run();
};

#endif // READYSTATIONTHREAD_H
