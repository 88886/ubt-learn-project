#ifndef MES_H
#define MES_H
#include <QString>
#include <QByteArray>
class Mes
{
public:
    Mes();

    QString get(QString url);
    QString getBy(QByteArray array);
};

#endif // MES_H
