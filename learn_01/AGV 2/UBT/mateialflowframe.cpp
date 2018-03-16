#include "mateialflowframe.h"
#include "ui_mateialflowframe.h"

MateialFlowFrame::MateialFlowFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MateialFlowFrame)
{
    ui->setupUi(this);
}

MateialFlowFrame::~MateialFlowFrame()
{
    delete ui;
}
