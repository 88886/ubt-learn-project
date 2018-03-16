#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "logindialog.h"
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QtSql/QSqlError>
#include <QDateTime>
#include <QSqlQuery>
#include <agv.h>
#include <QRegExp>

int main(int argc, char *argv[])
{
     QApplication a(argc, argv);
     MainWindow w;
     LoginDialog dialog;
     w.show();
     w.setWindowFlags(w.windowFlags());
     w.showMaximized();

     QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL");
     QSqlDatabase::addDatabase("QMYSQL","1");
     db.setHostName("127.0.0.1");
     db.setPort(3306);
     db.setDatabaseName("ums");
     db.setUserName("root");
     db.setPassword("123456");

     if(db.open())
     {
        qDebug()<<"database:success!";

     }else{
         qDebug()<<"failure";
         qDebug()<<db.lastError();

     }


    if(dialog.exec() == QDialog::Accepted){
         return a.exec();
    }else{
        return 0;
    }



}
