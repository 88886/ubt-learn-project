#include "materialflowframe.h"
#include "ui_materialflowframe.h"
#include <QStandardItemModel>
#include <QPainter>
#include <QDebug>
#include <tablemodel.h>
MaterialFlowFrame::MaterialFlowFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MaterialFlowFrame)
{
    ui->setupUi(this);
    flowNumber = ui->flowNumber;
    threeWidget = ui->threeWidget;
    scrollArea = ui->scrollArea;
    scrollArea->setWidgetResizable(true);
    scrollWidget = ui->scrollWidget;

    layout = ui->gridLayout;
    agvbatteryText = ui->agvBatteryText;
    agvCodeText = ui->agvCodeText;
    agvStateText = ui->agvStateText;
    agvLocationText = ui->agvLocationText;
    startStation = ui->startStation;
    stopTaskButton = ui->stopTask;
    tableView = ui->taskTable;
    startOrStopButton = ui->startOrStopButton;

    taskModel = new TableModel();

    taskModel->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("配送流水号")));
    taskModel->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("发货工位")));
    taskModel->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("目标工位")));
    taskModel->setHorizontalHeaderItem(3,new QStandardItem(QObject::tr("状态")));
    taskModel->setHorizontalHeaderItem(4,new QStandardItem(QObject::tr("AGV")));
    taskModel->setHorizontalHeaderItem(5,new QStandardItem(QObject::tr("操作")));


    ui->taskTable->setModel(taskModel);

    ui->taskTable->setColumnWidth(0,245);
    ui->taskTable->setColumnWidth(1,245);
    ui->taskTable->setColumnWidth(2,245);
    ui->taskTable->setColumnWidth(3,247);
    ui->taskTable->setColumnWidth(4,247);
    ui->taskTable->setColumnWidth(5,229);
    ui->taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);


}

MaterialFlowFrame::~MaterialFlowFrame()
{
    delete ui;
}



