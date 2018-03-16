#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton_clicked()
{
    if(ui->username->text().trimmed()==tr("1")&&ui->password->text().trimmed()==tr("1")){
        accept();
    }else{
        QMessageBox::warning(this, tr("Waring"),
                                    tr("用户名或者密码错误！"),QMessageBox::Yes
                                    );
    }
}
