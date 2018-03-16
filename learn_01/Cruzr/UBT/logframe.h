#ifndef LOGFRAME_H
#define LOGFRAME_H
#include <QListWidgetItem>
#include <QFrame>
#include <QList>
#include <QStandardItem>
#include <tablemodel.h>
#include <QSqlDatabase>
namespace Ui {
class LogFrame;
}

class LogFrame : public QFrame
{
    Q_OBJECT

public:
    explicit LogFrame(QWidget *parent = 0);
    ~LogFrame();
    QList<QStandardItem*> tableList;
    QStandardItem *item;
    QString sql;
    TableModel *logModel;
    QSqlDatabase db_B;
    void connectDataBase();

protected:
    bool eventFilter(QObject *obj, QEvent *e);




private slots:
    void on_agvNumberList_itemPressed(QListWidgetItem *item);

    void on_flowNumberList_itemPressed(QListWidgetItem *item);

    void on_workNumberList_itemPressed(QListWidgetItem *item);

private:
    Ui::LogFrame *ui;
};

#endif // LOGFRAME_H
