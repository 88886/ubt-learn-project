#include "mflowframe.h"
#include "ui_mflowframe.h"

mFlowFrame::mFlowFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::mFlowFrame)
{
    ui->setupUi(this);
}

mFlowFrame::~mFlowFrame()
{
    delete ui;
}
