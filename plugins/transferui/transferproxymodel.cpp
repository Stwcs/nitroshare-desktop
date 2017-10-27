/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <nitroshare/transfer.h>

#include "transferproxymodel.h"

void TransferProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, &QAbstractItemModel::dataChanged, this,
            [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &) {
        emit dataChanged(
            createIndex(topLeft.row(), 0),
            createIndex(bottomRight.row(), ColumnCount - 1)
        );
    });
}

QModelIndex TransferProxyModel::index(int row, int column, const QModelIndex &) const
{
    return createIndex(row, column);
}

QModelIndex TransferProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return createIndex(sourceIndex.row(), 0);
}

QModelIndex TransferProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return sourceModel()->index(proxyIndex.row(), 0);
}

QModelIndex TransferProxyModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int TransferProxyModel::rowCount(const QModelIndex &) const
{
    return sourceModel()->rowCount();
}

int TransferProxyModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant TransferProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid() || proxyIndex.row() < 0 || proxyIndex.row() >= rowCount()) {
        return QVariant();
    }

    Transfer *transfer = sourceRole(proxyIndex, Qt::UserRole).value<Transfer*>();

    switch (role) {
    case Qt::DisplayRole:
        switch (proxyIndex.column()) {
        case DeviceColumn:
            return transfer->deviceName();
        case ProgressColumn:
            return QString("%1%%").arg(transfer->progress());
        case StatusColumn:
            switch (transfer->state()) {
            case Transfer::Connecting:
                return tr("Connecting");
            case Transfer::InProgress:
                return tr("In Progress");
            case Transfer::Failed:
                return tr("Failed");
            case Transfer::Succeeded:
                return tr("Succeeded");
            default:
                return tr("Unknown");
            }
        }
        break;
    }

    return sourceRole(proxyIndex, role);
}

QVariant TransferProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }
    switch (section) {
    case DeviceColumn:
        return tr("Device");
    case ProgressColumn:
        return tr("Progress");
    case StatusColumn:
        return tr("Status");
    }
    return QVariant();
}

QVariant TransferProxyModel::sourceRole(const QModelIndex &proxyIndex, int role) const
{
    return sourceModel()->data(mapToSource(proxyIndex), role);
}