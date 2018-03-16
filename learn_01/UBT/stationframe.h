#ifndef STATIONFRAME_H
#define STATIONFRAME_H
#include <QLineEdit>
#include <QDialog>
#include <QComboBox>
namespace Ui {
class StationFrame;
}

class StationFrame : public QDialog
{
    Q_OBJECT

public:
    StationFrame(QString tile,QWidget *parent = 0);
    explicit StationFrame(QWidget *parent = 0);
    ~StationFrame();
    QLineEdit *stationCode,*stationX,*stationY,*stationType;
    QComboBox *stationState;

    QPushButton *saveButton;
    QPushButton *cancelButton;

    void closeEvent(QCloseEvent *event);

signals:
    closeStation();

private:
    Ui::StationFrame *ui;
};

#endif // STATIONFRAME_H
