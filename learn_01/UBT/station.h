#ifndef STATION_H
#define STATION_H
#include <QString>

class Station
{

public:
    Station(QString code,QString agvCode,int state,float x,float y,QString stationName );

private:
    QString code;
    QString agvCode;
    int state;
    float x;
    float y;
    int id;
    QString stationName;

public:
    int getId();
    void setId(int id);
    QString getAgvCode();
    QString getCode();
    int getState();
    float getX();
    float getY();
    QString getStationName();
    void setStationName(QString stationName);
    void setAgvCode(QString agvCode);
    void setState(int state);
    void setX(float x);
    void setY(float y);
    void setCode(QString code);


};

#endif // STATION_H
