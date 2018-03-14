//
// Created by 216k155 on 2/26/18.
//

#include "customlistmodel.h"
#include <QColor>
#include <QTextStream>
#include <QFile>
#include <QSize>
#include <iostream>



CustomListModel::CustomListModel(QObject *parent):
        QAbstractTableModel(parent){
    m_records = new QList<QStringList>;
}

/*CustomListModel::CustomListModel(const QStringList &strings, QObject *parent):
        QStringListModel(strings, parent){

}*/

int CustomListModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid() || (m_records->count() == 0)) {
        return 0;
    } else {
        return m_records->count() -1;
    }
}

int CustomListModel::columnCount(const QModelIndex &parent) const {
    if(m_records->count()) {
        return m_records->at(0).count();
    } else {
        return 0;
    }
}

Qt::ItemFlags CustomListModel::flags (const QModelIndex & index) const {
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    /*if (index.isValid()){
        return defaultFlags | Qt::ItemIsUserCheckable;
    }
    return defaultFlags;*/
    //printf("MY ROWS SELECT %d\n",selectableRows.size());

    if(std::find(selectableRows.begin(), selectableRows.end(), index.row()) != selectableRows.end()){

        return defaultFlags | Qt::ItemIsUserCheckable;
    }
    else
    {
        return Qt::NoItemFlags;
    }
}

void CustomListModel::setSelectableRows(std::vector<int> newSelectableRows) {
    selectableRows = newSelectableRows;
}

QVariant CustomListModel::data(const QModelIndex &index,
                               int role) const {

    QStringList record = m_records->at(index.row()+1);
    if (!index.isValid() || !m_records->count())
        return QVariant();

    if(role == Qt::CheckStateRole && index.column()==0)
        return checkedItems.contains(index) ?
               Qt::Checked : Qt::Unchecked;

    /*else if(role == Qt::BackgroundColorRole)
    {
        /*foreach(QPersistentModelIndex curIndex, checkedItems)
                if(curIndex.row() == index.row()) return QColor("#aaffb2");
        return QColor("#ffffff");*/
       /* return checkedItems.contains(index)  ?
               QColor("#aaffb2") : QColor("#ffffff");
    }*/

    if(role == Qt::SizeHintRole) {
        return QSize(30, 12);
    }





    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        return record.at(index.column());
    }

    return QVariant();//QStringListModel::data(index, role);
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

std::string CustomListModel::getCheckedRows(){

    std::string str = "";
    foreach(QPersistentModelIndex index, checkedItems){
            str += (std::to_string(index.row()) + ",");
        }
    if(str.size()) {
        str.erase(str.size()-1, 1);
    }
    return str;
}

QVariant CustomListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(!m_records->count()) {
        return QVariant();
    }
    if(orientation == Qt::Horizontal) {
        if(role == Qt::DisplayRole) {
            return m_records->at(0).at(section);
        }
    }
    else if(orientation == Qt::Vertical) {
        if(role == Qt::DisplayRole) {
            return section+1;
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool CustomListModel::loadData(QList<QStringList> newList) {

    m_records = new QList<QStringList>(newList);
    return true;
}
