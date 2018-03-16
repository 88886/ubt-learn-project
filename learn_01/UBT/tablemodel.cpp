#include "tablemodel.h"
#include <QStandardItemModel>

TableModel::TableModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

//QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    // FIXME: Implement me!
//}

//QModelIndex TableModel::index(int row, int column, const QModelIndex &parent) const
//{
//    // FIXME: Implement me!
//}

//QModelIndex TableModel::parent(const QModelIndex &index) const
//{
//    // FIXME: Implement me!
//}

//int TableModel::rowCount(const QModelIndex &parent) const
//{
//    if (!parent.isValid())
//        return 0;

//    // FIXME: Implement me!
//}

//int TableModel::columnCount(const QModelIndex &parent) const
//{
//    if (!parent.isValid())
//        return 0;

//    // FIXME: Implement me!
//}

QVariant TableModel::data(const QModelIndex &index,int role)const{

    QVariant value = QStandardItemModel::data(index,role);
    if(role==Qt::TextAlignmentRole){
        value = int(Qt::AlignCenter);
        return value;
    }else if(role==Qt::DisplayRole){

    }
    return value;
}
