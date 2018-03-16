#ifndef MESDIALOG_H
#define MESDIALOG_H

#include <QDialog>

namespace Ui {
class MesDialog;
}

class MesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MesDialog(QWidget *parent = 0);
    ~MesDialog();
    void closeEvent(QCloseEvent *event);
signals:


private:
    Ui::MesDialog *ui;
};

#endif // MESDIALOG_H
