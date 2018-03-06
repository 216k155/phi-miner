//
// Created by 216k155 on 2/26/18.
//

#ifndef CMAKE_AND_CUDA_CUSTOMLISTMODEL_H
#define CMAKE_AND_CUDA_CUSTOMLISTMODEL_H

#include <QStringListModel>
#include <QObject>
#include <QStringList>
#include <QModelIndex>
#include <QVariant>
#include <QSet>
#include <QPersistentModelIndex>

class CustomListModel : public QAbstractListModel
{
public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    CustomListModel(QObject* parent = 0);
    CustomListModel(const QStringList & strings, QObject* parent = 0);
    Qt::ItemFlags flags (const QModelIndex& index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role);
    void save();
    void getCheckedRows();
    QVariant headerData(int section,
                            Qt::Orientation orientation,
                            int role=Qt::DisplayRole) const;
    bool loadData(QList<QStringList>* newList) const;
private:
    QSet<QPersistentModelIndex> checkedItems;
    QList<QStringList> *m_records;
};

#endif //CMAKE_AND_CUDA_CUSTOMLISTMODEL_H
