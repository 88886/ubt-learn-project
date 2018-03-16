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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <addarea.h>
#include <updatestation.h>
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

    MaterialFlowFrame *frame1,*frame2,*frame5,*frame;
    AGVFrame *agvFrame;
    QFrame *log;
    QTcpSocket *socket,*socket1;
    AddArea * addArea = NULL;

    WorkThread *thread2=NULL,*thread=NULL;

    WorkStationThread *workStationThread;
    QVector<Agv*> vector;

    QList<QStandardItem*> list;

    QSqlQuery query ;
    QSqlDatabase db;
    QString sql;

    QStandardItem *item;

    QMap<QString,QMap<MaterialFlowFrame*,WorkThread*>*>outMap;

    QMap<QString,QMap<MaterialFlowFrame*,WorkThread*>*>::const_iterator out;
    QMap<MaterialFlowFrame*,WorkThread*>::const_iterator in;

    QString WORK_STATION_NAME = "WORK";
    QString READY_STATION_NAME = "READY";

    UpdateStation *updateStation = NULL;

public:
    void closeEvent(QCloseEvent *event);
    void initFrame();
    QSqlQuery getQuery(const QString &uiName);



signals:
    void closeWindow();
private slots:
    void closeWorkArea();

    void on_listArea_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_action_triggered();

    void on_onlineDispatcherButton_pressed();

    void on_stopButton_pressed();

    void on_addWorkArea_triggered();

    void addWorkArea();

    void on_deleteWorkArea_triggered();

    void on_updateWorkStation_triggered();

    void closeStation();



private:
    Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
