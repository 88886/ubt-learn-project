#ifndef UPDATESTATION_H
#define UPDATESTATION_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <tablemodel.h>
#include <QList>
#include <QStandardItem>
#include <stationframe.h>
namespace Ui {
class UpdateStation;
}

class UpdateStation : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateStation(QWidget *parent = 0);
    ~UpdateStation();


    QSqlQuery query;
    QString sql;
    TableModel *stationModel;
    QList<QStandardItem*> tableList;
    QStandardItem *item;
    QPushButton *addButton,*updateButton,*deleteButton,*saveButton,*cancelButton;
    StationFrame *stationFrame = NULL;
    bool flag = false;

    void closeEvent(QCloseEvent *event);
    void resetTable();

private slots:
    void on_workArea_currentTextChanged(const QString &arg1);
    void on_addButton_clicked();
    void closeStation();
    void saveStation(bool flag);
    void cancelStation();

    void on_updateButton_clicked();

    void on_deleteButton_clicked();

signals:
    closeStationSignal();
private:
    Ui::UpdateStation *ui;
};

#endif // UPDATESTATION_H
