#ifndef MATEIALFLOWFRAME_H
#define MATEIALFLOWFRAME_H

#include <QFrame>

namespace Ui {
class MateialFlowFrame;
}

class MateialFlowFrame : public QFrame
{
    Q_OBJECT

public:
    explicit MateialFlowFrame(QWidget *parent = 0);
    ~MateialFlowFrame();

private:
    Ui::MateialFlowFrame *ui;
};

#endif // MATEIALFLOWFRAME_H
