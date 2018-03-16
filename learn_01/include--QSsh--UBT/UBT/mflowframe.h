#ifndef MFLOWFRAME_H
#define MFLOWFRAME_H

#include <QFrame>

namespace Ui {
class mFlowFrame;
}

class mFlowFrame : public QFrame
{
    Q_OBJECT

public:
    explicit mFlowFrame(QWidget *parent = 0);
    ~mFlowFrame();

private:
    Ui::mFlowFrame *ui;
};

#endif // MFLOWFRAME_H
