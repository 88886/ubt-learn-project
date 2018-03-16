#include "station.h"
#include <QString>
Station::Station(QString code,QString agvCode,int state,float x,float y,QString stationName )
{
    this->code = code;
    this->agvCode = agvCode;
    this->state = state;
    this->x = x;
    this->y = y;
    this->stationName = stationName;
}

void Station::setId(int id){
    this->id = id;
}

int Station::getId(){
    return this->id;
}

QString Station::getStationName(){
    return this->stationName;
}

void Station::setStationName(QString stationName){
    this->stationName = stationName;
}

QString Station::getCode(){
    return this->code;
}

void Station::setCode(QString code){
    this->code = code;
}

void Station::setAgvCode(QString agvCode){
    this->agvCode = agvCode;
}

QString Station::getAgvCode(){
    return this->agvCode;
}

void Station::setState(int state){
    this->state = state;
}

int Station::getState(){
    return this->state;
}

void Station::setX(float x){
    this->x = x;
}

float Station::getX(){
    return this->x;
}

void Station::setY(float y){
    this->y = y;
}

float Station::getY(){
    return this->y;
}
