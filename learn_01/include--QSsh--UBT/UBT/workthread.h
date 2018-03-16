#ifndef WORKTHREAD_H
#define WORKTHREAD_H
#include <QThread>
#include <QObject>
#include <QString>
#include <agv.h>
#include <QMap>
#include <QtNetwork/QTcpSocket>
#include <materialflowframe.h>
#include <workstationthread.h>
#include <station.h>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QTimer>
#include <QList>
#include <QStandardItem>

#include <tablemodel.h>
#include <QStringList>
#include <manualtaskpoint.h>
#include <readystationthread.h>
#include <mes.h>
class WorkThread : public QThread
{
    Q_OBJECT
public:
    WorkThread(QString workArea,MaterialFlowFrame *frame, QObject *parent);
//    explicit WorkThread(QObject *parent = 0);


    QString WAIT_STATION_NAME = "WAIT";//等候区命名
    QString RECHARGE_STATION_NAME = "RECHARGE";//充电区命名
    QString READY_STATION_NAME = "READY";//后备区命名

    Mes *MES;

    QString location;
    int battery;

    QString NAVIGATION = "NAVIGATION|";//送货起始指令
    QString BACKTOSTATION = "BACKTOSTATION|";//回等候区指令
    QString TRANSACTION = "TRANSACTION|";//调度起始指令

    QString workArea;
    WorkStationThread *workStationThread = NULL; //获取任务线程
    ReadyStationThread *readyStationThread = NULL;//后备区调度AGV线程

    QMap<QTcpSocket*,Agv*> agvMap,socketAgv,socketAgvMap;
    MaterialFlowFrame *frame;
    QTcpSocket *socket,*agvSocket;
    QList<Agv*> list;
    Agv *agv;
    Station *station;

    QSqlQuery query;
    QSqlDatabase db;

    QString sql;
    QString message;
    QTimer repeatTimer;
    QString agv_code;

    QString startPoint;
    QString endPoint;


    bool mes =false;

    int ngTimes = 5;
    bool isConnect = false;

    bool manualDisconnect = false;

    bool threadState = true;

    int id = 0;

    float x = 0.0,x0 = 0.0;
    float y = 0.0,y0 = 0.0;
    float min = 0.0,s = 0.0;

    QTcpSocket* socketArr[1];

    QList<QStandardItem*> tableList;
    QStandardItem *item;

    QVector<Station*> startStationVector;
    QVector<Agv*> vector;

    QMap<QString,QVector<Station*>*> stationMap;
    QVector<Station*> *stationVector = new QVector<Station*>;
    ManualTaskPoint *manualTaskPoint;
    QString goalPoint;

    int row = 5;
    int colum = 5;

    int getStaionLocationTimes =3 ;
    int reSendTimes = 5;

    QList<QString> stationList;
    bool isInitStation = false;

    QMap<QString,QVector<Station*>*>::const_iterator i;
    QMap<QTcpSocket*,Agv*>::const_iterator a;


public:
    QSqlQuery getQuery();

    void addAgv(QTcpSocket *socket,Agv *agv);
    void drawAgv();
    void drawRemoveAgv(Agv *agv);
    void removeAgv(QTcpSocket *socket,Agv *agv);
    void queryStation(QString workArea);
    bool isContainReadyStation(QString code);

    void writeData(QString startPoint,QString endPoint,QString agvCode,QString data,QString flowCode);

    void updateAgv(Agv *agv,QString message);
    void navigation(QTcpSocket *socket,Agv *agv,QString read);
    void backtostation(QTcpSocket *socket,Agv *agv,QString read);   
    void transaction(QTcpSocket *socket, Agv *agv, QString read);
    void getWorkStationLocation( Agv *agv, QString read);

    void initTask();
    void initAgv(QString workArea);

    QTcpSocket* getSocket(QString code);    
    QMap<QTcpSocket*,Agv*> getSocketAndAgv(QString agvCode);

signals:
    void startTimes(QString timerName);
    void send(QString args);
    void sendStation();
    void back();
    void isHasTask();
    void finishThread();
    void finishDispatcherSignal();

protected:
    void run();


public slots:
    void reSend();
    void receiveData();
    void sendData(QString args);
    void dispatcherTask();
    void showAgv();

    void dispatcherStartStation(QString endPoint,QString flowCode);    
    void dispatcherReadyStation(QString endPoint);
    void repeatConnect();

    void isConnected();

    void stopTask();
    void startOrStopTask();

    void startOrStopAgv(int row,QString code ,QString isStart,QString areaName,TableModel* agvModel);
    void manualControl();
    void manualControlPoint();

    bool closeThread();

};

#endif // WORKTHREAD_H
