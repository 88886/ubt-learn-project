#ifndef MANUALTASKPOINT_H
#define MANUALTASKPOINT_H
#include <QComboBox>
#include <QDialog>

namespace Ui {
class ManualTaskPoint;
}

class ManualTaskPoint : public QDialog
{
    Q_OBJECT

public:
    explicit ManualTaskPoint(QWidget *parent = 0);
    ~ManualTaskPoint();

    QComboBox *goalPoint;
    QPushButton *comfrim;

private slots:
    void on_cancel_clicked();

private:
    Ui::ManualTaskPoint *ui;
};

#endif // MANUALTASKPOINT_H
