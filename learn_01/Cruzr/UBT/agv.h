#ifndef AGV_H
#define AGV_H
#include <QString>
#include <QFrame>
#include <QAbstractButton>
#include <QMouseEvent>
#include <QTimer>
class Agv:public QFrame
{
    Q_OBJECT
public:
    Agv(int id,QString code,QString robot_id,QString state,QString address_ip,int port,QString Battery,QString type,float x,float y,QWidget *parent);
    explicit Agv(QWidget *parent = 0);
private:
    int id;
    QString code;
    QString robot_id;
    QString state;
    QString address_ip;
    int port;
    QString battery;
    QString type;
    float x;
    float y;
    int times = 0;
    int ngTimes = 0;
    QString data;
    QString reSendData = "no";
    QTimer *reSendTimer ;
    QString flowCode;
    int connectTimes = 0;
    int disConnectTimes = 0;
    QString startPoint;
    QString endPoint;

public:
    QString getEndPoint();
    void setEndPoint(QString endPoint);
    QString getStartPoint();
    void setStartPoint(QString startPoitn);
    void addNGTimes(int times);
    void addDisConnectTimes(int times);
    void setDisConnectTimes(int times);
    int getDisConnectTimes();
    int getId();
    QString getCode();
    QString getRobotId();
    QString getState();
    QString getAddressIp();
    int getPort();
    QString getBattery();
    QString getType();
    float getX();
    float getY();
    int getTimes();
    void addTimes(int times);
    void setReSendTimer(QTimer *timer);
    QTimer* getReSendTimer();
    QString getFlowCode();
    void setNGTimes(int times);
    void setReSendData(QString data);
    void addConnectTimes(int times);
    int getConnectTimes();
    void setConnectTimes(int times);
    QString getReSendData();
    int getNGTimes();
    void setFlowCode(QString flwoCode);
    QString getSendData();
    void setBattery(QString battery);
    void setTimes(int times);
    void setSendData(QString data);
    void setX(float x);
    void setY(float y);
    void setId(int id);
    void setState(QString state);
    void setClick(QString state);
    void setUnClick(QString state);
    void mouseReleaseEvent(QMouseEvent *);
signals:
    void clicked();

};

#endif // AGV_H
