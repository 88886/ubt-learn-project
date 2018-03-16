#ifndef ADDAREA_H
#define ADDAREA_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
namespace Ui {
class AddArea;
}

class AddArea : public QDialog
{
    Q_OBJECT

public:
    explicit AddArea(QWidget *parent = 0);
    ~AddArea();


public:
    QPushButton *saveButton,*cancelButton;
    QLineEdit *workAreaEdit;
public:
    void closeEvent(QCloseEvent *event);

signals:
    closeSignal();

private:
    Ui::AddArea *ui;
};

#endif // ADDAREA_H
