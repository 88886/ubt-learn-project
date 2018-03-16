#include "database.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
QSqlDatabase DataBase::db;
QSqlQuery DataBase::query;
#include <QSqlQueryModel>
#include <QString>
#include <QMessageBox>



DataBase::DataBase()
{


}

void DataBase::creatConnection(const QString &connectionName){


     db=QSqlDatabase::addDatabase("QMYSQL",connectionName);
     db.setHostName("127.0.0.1");
     //db.setHostName("210.75.21.106");
     db.setPort(3306);
     QSqlDriver *driver ;

     db.setDatabaseName("ums");
     db.setUserName("root");
    // db.setPassword("Xrllwzz..&%22)");

    db.setPassword("123456");
    //SshConnectionParameters s = new SshConnectionParameters(this);

    if(db.open())
    {


    }else{

        QMessageBox::warning(NULL, QObject::tr("Waring"), QObject::tr("数据库连接失败，请联系后台管理人员！"),QMessageBox::Yes);

    }
}

void DataBase::closeDataBase(QString connectionName){
    QString name ;
    if(db.contains(connectionName)){
        db.close();
        if(connectionName.endsWith("READY")){
            db = QSqlDatabase::database(connectionName);
            name = db.connectionName();
            QSqlDatabase::removeDatabase(name);
        }
         QSqlDatabase::removeDatabase(connectionName);

    }
  //  QSqlDatabase db = QSqlDatabase::database(connectionName);//获得实例。
   //  QString name = db.connectionName();//获得默认连接名。
   //  QSqlDatabase::removeDatabase(name);//删除默认连接。
   //  qDebug()<<name;
}

QSqlDatabase DataBase::getDataBase(const QString &connectionName){
    return QSqlDatabase::database(connectionName);
}

QSqlQuery DataBase::getQuery(const QString &connectionName){


    if(QSqlDatabase::contains(connectionName)){
        db = QSqlDatabase::database(connectionName);

    }else{
        creatConnection(connectionName);
    }

    return (QSqlQuery)db;
}
