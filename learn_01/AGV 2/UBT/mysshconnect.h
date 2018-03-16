#ifndef MYSSHCONNECT_H
#define MYSSHCONNECT_H

#include <QObject>

class MySshConnect:QObject
{
    Q_OBJECT
public:
    MySshConnect();

    void myConnect();

private slots:

    void sshCon();


};

#endif // MYSSHCONNECT_H
