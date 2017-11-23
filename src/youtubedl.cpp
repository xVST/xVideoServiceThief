#include "youtubedl.h"

YoutubeDL::YoutubeDL(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant YoutubeDL::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex YoutubeDL::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex YoutubeDL::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int YoutubeDL::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int YoutubeDL::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant YoutubeDL::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
