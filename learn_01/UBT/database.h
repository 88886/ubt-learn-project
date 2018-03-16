#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QSqlQuery>

class DataBase
{
public:
    DataBase();
    static QSqlDatabase db;
    static QSqlQuery query;


static void creatConnection(const QString &connectionName);
static QSqlDatabase getDataBase(const QString &connectionName);

static QSqlQuery getQuery(const QString &connectionName);
static void closeDataBase(QString connectionName);

};

#endif // DATABASE_H
