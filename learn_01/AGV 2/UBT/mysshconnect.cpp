#include "mysshconnect.h"
#include <QSsh/include/QSsh>

MySshConnect::MySshConnect()
{

}

void MySshConnect::myConnect(){
    QSsh::SshConnectionParameters *p = new QSsh::SshConnectionParameters();
    p->host = "210.75.21.106";
    p->port = 8888;
    p->userName = "root";
    p->password = "Xrllwzz..&%22)";
    p->authenticationType = QSsh::SshConnectionParameters::AuthenticationTypePassword;

    QSsh::SshConnection *con = new QSsh::SshConnection(*p,NULL);

}

void MySshConnect::sshCon(){
    qDebug()<<"true connect";
}
