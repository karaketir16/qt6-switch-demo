#include "extended_tests.h"

#include <QBuffer>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QHash>
#include <QItemSelectionModel>
#include <QSaveFile>
#include <QScopedPointer>
#include <QTemporaryFile>
#include <QTimer>
#include <QUrlQuery>

bool ultimateCoreExtended(QString &detail)
{
    QByteArray payload("qt-switch-coverage");
    const QByteArray digest = QCryptographicHash::hash(payload, QCryptographicHash::Sha256);
    QBuffer buffer;
    buffer.setData(payload);
    if (!buffer.open(QIODevice::ReadOnly) || buffer.readAll() != payload)
        return detail = QStringLiteral("QBuffer round-trip failed"), false;

    QHash<QString, int> counts{{QStringLiteral("core"), 1}, {QStringLiteral("switch"), 2}};
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("module"), QStringLiteral("QtCore"));
    QTemporaryFile file(QDir::tempPath() + QStringLiteral("/qt-ultimate-XXXXXX"));
    if (!file.open())
        return detail = QStringLiteral("QTemporaryFile could not open"), false;
    const QByteArray stamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toUtf8();
    if (file.write(stamp) != stamp.size() || !file.flush())
        return detail = QStringLiteral("temporary file write failed"), false;

    QTimer timer;
    bool fired = false;
    QObject::connect(&timer, &QTimer::timeout, [&fired] { fired = true; });
    timer.setSingleShot(true);
    timer.start(0);
    while (timer.isActive())
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    const bool ok = digest.size() == QCryptographicHash::hashLength(QCryptographicHash::Sha256)
            && counts.value(QStringLiteral("switch")) == 2
            && query.queryItemValue(QStringLiteral("module")) == QStringLiteral("QtCore")
            && fired;
    detail = ok ? QStringLiteral("hash + buffer + temp file + URL query + timer signal")
                : QStringLiteral("extended QtCore invariant failed");
    return ok;
}
