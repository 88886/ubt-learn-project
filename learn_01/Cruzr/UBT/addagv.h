#ifndef ADDAGV_H
#define ADDAGV_H
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include <QMap>
namespace Ui {
class AddAGV;
}

class AddAGV : public QDialog
{
    Q_OBJECT

public:
    explicit AddAGV(QWidget *parent = 0);
    ~AddAGV();
    QPushButton *saveButton,*cancelButton;

    QLineEdit *agvCode, *agvID,*agvIP,*agvPort;
    QComboBox *type,*isStart,*workArea,*state;
    QMap<QString,int> workAreaMap;
    int workAreaId;

public:
    void closeEvent(QCloseEvent *event);

    void setWorkAreaId(int workAreaId);
    void setIsStart(QString isStart);

    int getWorkAreaId();
    QString getIsStart();




signals:
    void closeWindow();

private slots:




    void on_workArea_currentTextChanged(const QString &arg1);

private:
    Ui::AddAGV *ui;
};

#endif // ADDAGV_H
