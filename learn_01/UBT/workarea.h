#ifndef WORKAREA_H
#define WORKAREA_H

#include <QDialog>

namespace Ui {
class WorkArea;
}

class WorkArea : public QDialog
{
    Q_OBJECT

public:
    explicit WorkArea(QWidget *parent = 0);
    ~WorkArea();

private:
    Ui::WorkArea *ui;
};

#endif // WORKAREA_H
