#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "logindialog.h"
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QDebug>

#include <QDateTime>

#include <agv.h>
#include <QRegExp>
#include <mes.h>


int main(int argc, char *argv[])
{
     QApplication a(argc, argv);
     MainWindow w;
     LoginDialog dialog;
     w.show();
     w.setWindowFlags(w.windowFlags());
     w.showMaximized();
     QTextCodec *codec = QTextCodec::codecForName("GB2312");
     QTextCodec::setCodecForLocale(codec);




    //if(dialog.exec() == QDialog::Accepted){
         return a.exec();
    //}else{
    //    return 0;
   // }


}
