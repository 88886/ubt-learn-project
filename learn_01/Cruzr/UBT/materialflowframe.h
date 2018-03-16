#ifndef MATERIALFLOWFRAME_H
#define MATERIALFLOWFRAME_H
#include <QLineEdit>
#include <QFrame>
#include <QTabWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QProgressBar>
#include <QStandardItemModel>
#include <QPushButton>
#include <QTableView>
namespace Ui {
class MaterialFlowFrame;
}

class MaterialFlowFrame : public QFrame
{
    Q_OBJECT

public:
    explicit MaterialFlowFrame(QWidget *parent = 0);
    ~MaterialFlowFrame();
    QLineEdit *flowNumber;
    QPushButton *stopTaskButton;
    QTableView *tableView;
    QPushButton *startOrStopButton;

    QTabWidget *threeWidget;
    QScrollArea *scrollArea;
    QGridLayout *layout;
    QWidget *scrollWidget;

    QLabel *agvCodeText;
    QLabel *agvStateText;
    QLabel *agvLocationText;
    QComboBox *startStation;
    QProgressBar *agvbatteryText;
    QStandardItemModel *taskModel;


protected:


private:
    Ui::MaterialFlowFrame *ui;
};

#endif // MATERIALFLOWFRAME_H
