#ifndef MES_H
#define MES_H
#include <QString>

class Mes
{
public:
    Mes();

    QString get(QString url);
    QString getPoint(QByteArray array);
};

#endif // MES_H
