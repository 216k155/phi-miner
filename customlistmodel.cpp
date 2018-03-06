//
// Created by 216k155 on 2/26/18.
//

#include "customlistmodel.h"
#include <QColor>
#include <QTextStream>
#include <QFile>
#include <iostream>



CustomListModel::CustomListModel(QObject *parent):
        QStringListModel(parent){
}

CustomListModel::CustomListModel(const QStringList &strings, QObject *parent):
        QStringListModel(strings, parent){

}

int CustomListModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid() || (m_records->count() == 0)) {
        return 0;
    } else {
        return m_records->count() - 1;
    }
}

Qt::ItemFlags CustomListModel::flags (const QModelIndex & index) const {
    Qt::ItemFlags defaultFlags = QStringListModel::flags(index);
    if (index.isValid()){
        return defaultFlags | Qt::ItemIsUserCheckable;
    }
    return defaultFlags;
}

QVariant CustomListModel::data(const QModelIndex &index,
                               int role) const {
    if (!index.isValid())
        return QVariant();

    if(role == Qt::CheckStateRole)
        return checkedItems.contains(index) ?
               Qt::Checked : Qt::Unchecked;

    else if(role == Qt::BackgroundColorRole)
        return checkedItems.contains(index) ?
               QColor("#ffffb2") : QColor("#ffffff");

    return QStringListModel::data(index, role);
}

bool CustomListModel::setData(const QModelIndex &index,
                              const QVariant &value, int role) {

    if(!index.isValid() || role != Qt::CheckStateRole)
        return false;

    if(value == Qt::Checked)
        checkedItems.insert(index);
    else
        checkedItems.remove(index);

    emit dataChanged(index, index);
    return true;
}

void CustomListModel::save(){
    QFile file("required_components.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "Required components:" << "\n";
    foreach(QPersistentModelIndex index, checkedItems)
    out << index.data().toString() << "\n";
}

void CustomListModel::getCheckedRows(){

    foreach(QPersistentModelIndex index, checkedItems)
    std::cout << "- " << index.row() << "\n";
}

QVariant CustomListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(!m_records->count()) {
        return QVariant();
    }
    if(orientation == Qt::Horizontal) {
        if(role == Qt::DisplayRole) {
            return "FFFF";
        }
    }
    else if(orientation == Qt::Vertical) {
        if(role == Qt::DisplayRole) {
            return section+1;
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool CustomListModel::loadData(QList<QStringList>* newList) {
    m_records = newList;
}
