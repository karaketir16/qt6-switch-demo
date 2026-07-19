#include "extended_tests.h"

#include <QColor>
#include <QDataStream>
#include <QDate>
#include <QDateTime>
#include <QHash>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QRegularExpression>
#include <QStringList>
#include <QUrl>
#include <QUrlQuery>
#include <QVariant>

bool ultimateBulkCase(int n, QString &detail)
{
    const int normalized = n % 200;
    const int group = normalized / 20;
    const int variant = normalized % 20;
    bool ok = false;
    switch (group) {
    case 0: { // QByteArray and QString conversions.
        const QByteArray value = QByteArray("switch-") + QByteArray::number(variant);
        ok = value.startsWith("switch-") && value.endsWith(QByteArray::number(variant))
                && QString::fromUtf8(value).contains(QStringLiteral("switch"));
        break;
    }
    case 1: { // Numeric and date/value conversions.
        const QString text = QString::number(variant * variant);
        bool converted = false;
        const int number = text.toInt(&converted);
        const QDate date(2024, 1, variant + 1);
        ok = converted && number == variant * variant && date.isValid()
                && QDateTime(date, QTime(12, variant, 0)).date() == date;
        break;
    }
    case 2: { // Containers and QVariant detachment/value semantics.
        QList<int> values{variant, variant + 1, variant + 2};
        QHash<QString, QVariant> map;
        map.insert(QStringLiteral("value"), values.at(1));
        const QVariant copy = map.value(QStringLiteral("value"));
        ok = values.size() == 3 && values.at(0) == variant && copy.toInt() == variant + 1;
        break;
    }
    case 3: { // JSON round trips with changing keys.
        QJsonObject object;
        object.insert(QStringLiteral("case"), variant);
        object.insert(QStringLiteral("enabled"), (variant % 2) == 0);
        const QJsonObject roundTrip = QJsonDocument(object).toJson(QJsonDocument::Compact).isEmpty()
                ? QJsonObject() : QJsonDocument::fromJson(QJsonDocument(object).toJson()).object();
        ok = roundTrip.value(QStringLiteral("case")).toInt() == variant
                && roundTrip.value(QStringLiteral("enabled")).toBool() == ((variant % 2) == 0);
        break;
    }
    case 4: { // Regular expressions and URL query encoding.
        const QString token = QStringLiteral("qt-switch-%1").arg(variant);
        const QRegularExpression expression(QStringLiteral("^qt-switch-(\\d+)$"));
        const auto match = expression.match(token);
        QUrl url(QStringLiteral("https://example.invalid/test"));
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("token"), token);
        url.setQuery(query);
        ok = match.hasMatch() && match.captured(1).toInt() == variant
                && QUrlQuery(url).queryItemValue(QStringLiteral("token")) == token;
        break;
    }
    case 5: { // Color parsing and channel arithmetic.
        const QColor color = QColor::fromHsv((variant * 17) % 360, 200, 240, 255);
        const QColor copy(color.name(QColor::HexArgb));
        ok = color.isValid() && color.alpha() == 255 && copy.isValid()
                && copy.red() == color.red() && copy.green() == color.green();
        break;
    }
    case 6: { // QDataStream primitive serialization.
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << qint32(variant) << QStringLiteral("case-%1").arg(variant);
        QDataStream in(&bytes, QIODevice::ReadOnly);
        qint32 number = -1;
        QString text;
        in >> number >> text;
        ok = number == variant && text == QStringLiteral("case-%1").arg(variant)
                && in.status() == QDataStream::Ok;
        break;
    }
    case 7: { // QStringList operations and stable ordering.
        QStringList values{QStringLiteral("a%1").arg(variant), QStringLiteral("b%1").arg(variant)};
        values.append(QStringLiteral("c%1").arg(variant));
        values.sort(Qt::CaseSensitive);
        ok = values.size() == 3 && values.first().startsWith(QLatin1Char('a'))
                && values.join(QLatin1Char(',')).count(QLatin1Char(',')) == 2;
        break;
    }
    case 8: { // QVariant type preservation.
        const QVariant integer(variant);
        const QVariant string(QStringLiteral("v%1").arg(variant));
        ok = integer.typeId() == QMetaType::Int && integer.toInt() == variant
                && string.typeId() == QMetaType::QString && string.toString().endsWith(QString::number(variant));
        break;
    }
    case 9: { // URL normalization and relative resolution.
        const QUrl base(QStringLiteral("https://example.invalid/a/b/"));
        const QUrl resolved = base.resolved(QUrl(QStringLiteral("../case-%1?q=x").arg(variant)));
        ok = resolved.scheme() == QStringLiteral("https")
                && resolved.path() == QStringLiteral("/a/case-%1").arg(variant)
                && resolved.query() == QStringLiteral("q=x");
        break;
    }
    default:
        break;
    }
    detail = ok ? QStringLiteral("bulk group %1 variant %2").arg(group).arg(variant)
                : QStringLiteral("bulk invariant failed: group %1 variant %2").arg(group).arg(variant);
    return ok;
}
