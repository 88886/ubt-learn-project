#ifndef WARNDIALOG_H
#define WARNDIALOG_H

#include <QDialog>
#include <QAbstractButton>
namespace Ui {
class WarnDialog;
}

class WarnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WarnDialog(QWidget *parent = 0);
    ~WarnDialog();
    void closeEvent(QCloseEvent *event);
signals:
    closeWindow();

private slots:


private:
    Ui::WarnDialog *ui;
};

#endif // WARNDIALOG_H
