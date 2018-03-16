#ifndef ENDSTATIONTHREAD_H
#define ENDSTATIONTHREAD_H
#include <QSqlDatabase>
#include <QThread>
#include <QMap>
#include <QVector>
#include <station.h>
class EndStationThread:public QThread
{
    Q_OBJECT
public:
    EndStationThread( QObject *parent);

    QMap<QString,QVector<Station*>*> stationMap;
    QString sql;
    QSqlDatabase db_C;
    QString START_STATION_NAME = "START";
    bool isAccept = false;
    bool reSendEndSignal = false;

public:
    void setStationMap(QMap<QString,QVector<Station*>*> stationMap);

public slots:
     void getTask();

signals:
    void getTaskSignal( );
    void endTaskSignal(QString startPoint,QString endPoint);
protected:
    void run();

};

#endif // ENDSTATIONTHREAD_H
