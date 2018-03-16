#ifndef AGVFRAME_H
#define AGVFRAME_H
#include "addagv.h"
#include <QFrame>
#include <QList>
#include <QStandardItem>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <tablemodel.h>
namespace Ui {
class AGVFrame;
}

class AGVFrame : public QFrame
{
    Q_OBJECT

public:
    explicit AGVFrame(QWidget *parent = 0);
    ~AGVFrame();
    AddAGV *addAGV = NULL;

    TableModel *agvModel;

    QList<QStandardItem*> list;
    QStandardItem *item;
    QString sql;

    QString code;
    QString type;
    QString robotId;
    QString addressIp;
    QString port;
    int workAreaId;
    QString isStart;
    QSqlDatabase db_A;


public:
    void queryData();
    void connectDataBase();

private slots:
    void on_addButton_pressed();
    void insertSave();
    void save();
    void cancel();
    void close();
    void on_updateButton_clicked();

    void on_deleteButton_clicked();

private:
    Ui::AGVFrame *ui;
};

#endif // AGVFRAME_H
