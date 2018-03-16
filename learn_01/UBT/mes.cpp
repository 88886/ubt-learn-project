#include "mes.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonParseError>
#include <QJsonObject>
#include <mesdialog.h>
#include <QJsonArray>
Mes::Mes()
{

}

QString Mes::get(QString url){
    QString username = "admin";
    QString password = "apple.1234";

    QString up = username+":"+password;
    QByteArray ba = up.toUtf8();
    QByteArray base = ba.toBase64();
    QByteArray baseStr = "Basic "+base;


    //QString url = "http://192.168.245.128:8000/api/query_work_sheet/radio";
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    request.setRawHeader("Authorization",baseStr);

    QNetworkReply *reply = manager->get(request);
    QByteArray responseData;
    QEventLoop eventLoop;
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    QString stationCode = getBy(responseData = reply->readAll());
    return stationCode;

}

QString Mes::getBy(QByteArray array){
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(array, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {
        if (doucment.isObject()) {  // JSON 文档为对象
               QJsonObject object = doucment.object();  // 转化为对象

               //对应各个属性词'success'
               if(object.take("success").toVariant().toBool()){

                   //对应 data
                    if(object.contains("data")){
                        QJsonValue value = object.value("data");
                        if(value.isObject()){
                            QJsonObject obj = value.toObject();

                            //获取data如果是{}
                            if(obj.size()==0){
                                MesDialog *mes = new MesDialog();
                                mes->show();
                                return NULL;
                            }else{

                                //获取工位编号'code'属性
                                if(obj.contains("code")){

                                    return obj.take("code").toString();
                                }
                            }


                        }else{
                            MesDialog *mes = new MesDialog();
                            mes->show();
                            return NULL;
                        }
                    }


               }else{
                    MesDialog *mes = new MesDialog();
                    mes->show();
                    return NULL;
               }
        }
   }
}
