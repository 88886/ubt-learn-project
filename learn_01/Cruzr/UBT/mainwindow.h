 #ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QAction>
#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QListWidgetItem>
#include <materialflowframe.h>
#include <QtNetwork/QTcpSocket>
#include <workthread.h>
#include <workstationthread.h>
#include <QVector>
#include <agv.h>
#include <agvframe.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


protected:
    bool eventFilter(QObject *obj, QEvent *e);

public:
    MaterialFlowFrame *frame1,*frame2,*frame5;
    AGVFrame *agvFrame;
    QFrame *log;
    QTcpSocket *socket,*socket1;
    WorkThread *thread=NULL;
    WorkStationThread *workStationThread;
    QVector<Agv*> vector;


    QList<QStandardItem*> list;
    QSqlQuery *query;
    QString sql;
    QStandardItem *item;

private slots:
    void on_listArea_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_action_triggered();

    void on_onlineDispatcherButton_pressed();

    void on_stopButton_pressed();



private:
    Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
