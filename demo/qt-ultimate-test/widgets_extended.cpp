#include "extended_tests.h"

#include <QSortFilterProxyModel>
#include <QStandardItemModel>

bool ultimateWidgetsExtended(QString &detail)
{
    QStandardItemModel source;
    source.setHorizontalHeaderLabels({QStringLiteral("name"), QStringLiteral("value")});
    source.appendRow({new QStandardItem(QStringLiteral("switch")), new QStandardItem(QStringLiteral("42"))});
    source.appendRow({new QStandardItem(QStringLiteral("desktop")), new QStandardItem(QStringLiteral("7"))});

    QSortFilterProxyModel proxy;
    proxy.setSourceModel(&source);
    proxy.setFilterKeyColumn(0);
    proxy.setFilterFixedString(QStringLiteral("switch"));
    const QModelIndex index = proxy.index(0, 1);
    const bool ok = source.rowCount() == 2 && proxy.rowCount() == 1
            && index.data().toString() == QStringLiteral("42")
            && proxy.mapToSource(index).row() == 0;
    detail = ok ? QStringLiteral("QStandardItemModel + proxy filtering + index mapping")
                : QStringLiteral("model/view invariant failed");
    return ok;
}

