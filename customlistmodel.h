//
// Created by 216k155
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

class CustomListModel : public QStringListModel
{
public:
    CustomListModel(QObject* parent = 0);
    CustomListModel(const QStringList & strings, QObject* parent = 0);
    Qt::ItemFlags flags (const QModelIndex& index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role);
    void save();
private:
    QSet<QPersistentModelIndex> checkedItems;
};

#endif //CMAKE_AND_CUDA_CUSTOMLISTMODEL_H
