#ifndef ENDSTATIONTHREAD_H
#define ENDSTATIONTHREAD_H
#include <QSqlDatabase>
#include <QThread>
#include <QMap>
#include <QVector>
#include <station.h>
#include <workstationthread.h>
#include <readystationthread.h>
class EndStationThread:public QThread
{
    Q_OBJECT
public:
    EndStationThread (WorkStationThread *workStationThread,ReadyStationThread *readyStationThread, QObject *parent);

private:

public:
    WorkStationThread *workStationThread = NULL;
    ReadyStationThread *readyStationThread = NULL;


public slots:
    void stopThread();

signals:

protected:
    void run();

};

#endif // ENDSTATIONTHREAD_H
