#include "agv.h"
#include <QString>
#include <QDebug>
#include <QFrame>
#include <QMouseEvent>
#include <QTimer>
Agv::Agv(int id,QString code,QString robot_id,QString state,QString address_ip,int port,QString battery,QString type,float x,float y,QWidget *parent): QFrame(parent)
{
    this->id = id;
    this->code = code;
    this->robot_id = robot_id;
    this->state = state;
    this->address_ip = address_ip;
    this->port = port;
    this->battery = battery;
    this->type = type;
    this->x = x;
    this->y = y;
    this->setFixedHeight(120);
    this->setFixedWidth(111);
    if(state=="闲置中"){
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px solid blue;};");
    }else if(state=="故障中"){
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px solid red;};");
    }else {
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px solid yellow;};");
    }
}

QString Agv::getStationName(){
    return this->stationName;
}

void Agv::setStationName(QString stationName){
    this->stationName = stationName;
}

void Agv::setIsDispatcher(bool isDispatcher){
    this->isDispatcher = isDispatcher;
}

bool Agv::getIsDispatcher(){
    return this->isDispatcher;
}

QString Agv::getEndPoint(){
    return this->endPoint;
}

void Agv::setEndPoint(QString endPoint){
    this->endPoint = endPoint;
}

QString Agv::getStartPoint(){
    return this->startPoint;
}

void Agv::setStartPoint(QString startPoint){
    this->startPoint = startPoint;
}

void Agv::addNGTimes(int times){
    this->ngTimes=this->ngTimes+times;
}

void Agv::setDisConnectTimes(int times){
    this->disConnectTimes = times;
}

int Agv::getDisConnectTimes(){
    return this->disConnectTimes;
}

void Agv::addDisConnectTimes(int times){
    this->disConnectTimes = this->disConnectTimes + times;
}

void Agv::setConnectTimes(int times){
    this->connectTimes = times;
}

int Agv::getConnectTimes(){
    return this->connectTimes;
}

void Agv::addConnectTimes(int times){
    this->connectTimes = this->connectTimes + times;
}

void Agv::setReSendData(QString data){
    this->reSendData = data;
}

QString Agv::getReSendData(){
    return this->reSendData;
}

int Agv::getNGTimes(){
    return this->ngTimes;
}

void Agv::setNGTimes(int times){
    this->ngTimes=times;
}

QString Agv::getFlowCode(){
    return this->flowCode;
}

void Agv::setFlowCode(QString flwoCode){
    this->flowCode = flwoCode;
}

void Agv::setReSendTimer( QTimer *timer){
    this->reSendTimer = &*timer;

}

QTimer* Agv::getReSendTimer(){
    return &*reSendTimer;
}

void Agv::addTimes(int times){
    this->times = this->times + times;
}

void Agv::setTimes(int times){
    this->times = times;
}

int Agv::getTimes(){
    return this->times;
}

void Agv::setSendData(QString data){
    this->data = data;
}

QString Agv::getSendData(){
    return this->data;
}

void Agv::setId(int id){
    this->id = id;
}

int Agv::getId(){
   return id;
}

void Agv::setClick(QString state){
    if(state=="闲置中"){
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px dashed blue;};");
    }else if(state=="故障中"){
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px dashed red;};");
    }else {
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px dashed yellow;};");
    }

}

void Agv::setUnClick(QString state){
    if(state=="闲置中"){
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px solid blue;};");
    }else if(state=="故障中"){
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px solid red;};");
    }else {
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px solid yellow;};");
    }
}

QString Agv::getCode(){
    return code;
}

void Agv::mouseReleaseEvent(QMouseEvent *ev)
{

    if (ev->button() == Qt::LeftButton)
        emit clicked();
}

QString Agv::getRobotId(){
    return this->robot_id;
}

QString Agv::getState(){


    return this->state;
}

void Agv::setState(QString state){
    if(state=="闲置中"){
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px solid blue;};");
    }else if(state=="故障中"){
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px solid red;};");
    }else {
        this->setStyleSheet("QFrame{background-image: url(:/Icon/images/agv-1.jpg);border:4px solid yellow;};");
    }
    this->state = state;
}

QString Agv::getAddressIp(){
    return this->address_ip;
}

int Agv::getPort(){
    return this->port;
}

void Agv::setBattery(QString battery){
    this->battery = battery;
}

QString Agv::getBattery(){
    return this->battery;
}

QString Agv::getType(){
    return type;
}

float Agv::getX(){
    return this->x;
}

float Agv::getY(){
    return this->y;
}

void Agv::setX(float x){
    this->x = x;
}

void Agv::setY(float y){
    this->y = y;
}


