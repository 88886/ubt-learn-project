#ifndef AGVFRAME_H
#define AGVFRAME_H
#include "addagv.h"
#include <QFrame>
#include <QList>
#include <QStandardItem>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <tablemodel.h>
#include <QPushButton>
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

    QSqlDatabase db;
    QSqlQuery query;

    QPushButton *startOrStopButton;


public:

    void connectDataBase();
    void queryArea();

signals:
    void startOrStopSignal(int row,QString code ,QString isStart,QString areaName,TableModel* agvModel);

private slots:
    void queryData();
    void on_addButton_pressed();
    void insertSave();
    void save();
    void cancel();
    void close();
    void on_updateButton_clicked();

    void on_deleteButton_clicked();

    void on_startOrStopButton_pressed();

    void on_renewButton_clicked();

    void on_queryButton_clicked();

private:
    Ui::AGVFrame *ui;
};

#endif // AGVFRAME_H
