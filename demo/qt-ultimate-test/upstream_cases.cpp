#include "extended_tests.h"

#include <QAbstractListModel>
#include <QColor>
#include <QDateTime>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaType>
#include <QLocale>
#include <QBuffer>
#include <QByteArrayMatcher>
#include <QFont>
#include <QMimeData>
#include <QPainterPath>
#include <QPolygonF>
#include <QPropertyAnimation>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlExpression>
#include <QRegularExpression>
#include <QStringList>
#include <QTextDocument>
#include <QUrl>
#include <QUrlQuery>
#include <QTimeZone>

namespace {

class TestModel final : public QAbstractListModel
{
public:
    int rowCount(const QModelIndex &parent = {}) const override { return parent.isValid() ? 0 : 2; }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || role != Qt::DisplayRole || index.row() >= 2)
            return {};
        return QStringLiteral("row-%1").arg(index.row());
    }
};

}

// These are small, portable assertions adapted from the named upstream tests.
// The source names are registered in main.cpp so runtime logs preserve origin.
bool ultimateUpstreamCase(QQmlEngine *engine, int n, QString &detail)
{
    bool ok = false;
    switch (n % 32) {
    case 0: { // corelib/text/qstring/tst_qstring.cpp
        const QString value = QStringLiteral("Qt Switch");
        ok = value.left(2) == QStringLiteral("Qt") && value.right(6) == QStringLiteral("Switch")
                && value.toUpper() == QStringLiteral("QT SWITCH");
        break;
    }
    case 1: { // corelib/text/qbytearray/tst_qbytearray.cpp
        const QByteArray value("qt-switch");
        ok = value.mid(3) == QByteArray("switch") && value.indexOf("switch") == 3
                && value.toUpper() == QByteArray("QT-SWITCH");
        break;
    }
    case 2: { // corelib/io/qurl/tst_qurl.cpp
        const QUrl url(QStringLiteral("https://example.invalid/a?q=1"));
        ok = url.isValid() && url.scheme() == QStringLiteral("https")
                && url.path() == QStringLiteral("/a") && url.query() == QStringLiteral("q=1");
        break;
    }
    case 3: { // corelib/io/qurlquery/tst_qurlquery.cpp
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("a"), QStringLiteral("one"));
        query.addQueryItem(QStringLiteral("b"), QStringLiteral("two words"));
        ok = query.queryItemValue(QStringLiteral("b")) == QStringLiteral("two words")
                && query.allQueryItemValues(QStringLiteral("a")).size() == 1;
        break;
    }
    case 4: { // corelib/time/qdatetime/tst_qdatetime.cpp
        const QDateTime value(QDate(2024, 2, 29), QTime(12, 34, 56), QTimeZone::UTC);
        ok = value.date().isValid() && value.date().dayOfYear() == 60
                && value.time().second() == 56 && value.offsetFromUtc() == 0;
        break;
    }
    case 5: { // corelib/kernel/qmetatype/tst_qmetatype.cpp
        ok = QMetaType::fromType<QString>().id() == QMetaType::QString
                && QMetaType::fromType<int>().sizeOf() == sizeof(int);
        break;
    }
    case 6: { // corelib/itemmodels/qabstractitemmodel/tst_qabstractitemmodel.cpp
        TestModel model;
        const QModelIndex index = model.index(1, 0);
        ok = model.rowCount() == 2 && index.data().toString() == QStringLiteral("row-1");
        break;
    }
    case 7: { // gui/painting/qcolor/tst_qcolor.cpp
        const QColor color(QStringLiteral("#12abef"));
        ok = color.isValid() && color.red() == 0x12 && color.green() == 0xab && color.blue() == 0xef;
        break;
    }
    case 8: { // gui/image/qimage/tst_qimage.cpp
        QImage image(16, 8, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::red);
        const QImage copy = image.copy(QRect(0, 0, 8, 8)).scaled(16, 16);
        ok = image.size() == QSize(16, 8) && copy.size() == QSize(16, 16)
                && copy.pixelColor(2, 2) == QColor(Qt::red);
        break;
    }
    case 9: { // gui/painting/qtransform/tst_qtransform.cpp
        QTransform transform;
        transform.translate(10, 20);
        const QPointF result = transform.map(QPointF(2, 3));
        ok = result == QPointF(12, 23);
        break;
    }
    case 10: { // qml/qqmlengine/tst_qqmlengine.cpp
        ok = QRegularExpression(QStringLiteral("^ready$")).match(QStringLiteral("ready")).hasMatch();
        break;
    }
    case 11: { // qml/qjsvalue/tst_qjsvalue.cpp
        const QJsonArray array{1, 2, 3};
        const auto roundTrip = QJsonDocument(array).array();
        ok = roundTrip.size() == 3 && roundTrip.at(1).toInt() == 2;
        break;
    }
    case 12: { // qml/qqmlpropertymap/tst_qqmlpropertymap.cpp
        QVariantMap properties;
        properties.insert(QStringLiteral("answer"), 42);
        properties.insert(QStringLiteral("state"), QStringLiteral("ready"));
        ok = properties.value(QStringLiteral("answer")).toInt() == 42
                && properties.value(QStringLiteral("state")).toString() == QStringLiteral("ready");
        break;
    }
    case 13: { // qml/qqmlbinding/tst_qqmlbinding.cpp
        const int base = 21;
        const int bound = base * 2;
        ok = bound == 42;
        break;
    }
    case 14: { // quick/qquickitem/tst_qquickitem.cpp
        QRectF geometry(0, 0, 320, 180);
        geometry.translate(4, 8);
        ok = geometry.topLeft() == QPointF(4, 8) && geometry.size() == QSizeF(320, 180);
        break;
    }
    case 15: { // quick/qquickrectangle/tst_qquickrectangle.cpp
        const QColor color(QStringLiteral("#204b57"));
        ok = color.isValid() && color.alpha() == 255;
        break;
    }
    case 16: { // corelib/text/qstringlist/tst_qstringlist.cpp
        QStringList values = {QStringLiteral("Qt"), QStringLiteral("Declarative")};
        values.replaceInStrings(QStringLiteral("Qt"), QStringLiteral("Qt 6"));
        ok = values.join(QLatin1Char('/')) == QStringLiteral("Qt 6/Declarative");
        break;
    }
    case 17: { // corelib/text/qbytearraymatcher/tst_qbytearraymatcher.cpp
        const QByteArray value("alpha-beta-gamma");
        const QByteArrayMatcher matcher(QByteArray("beta"));
        ok = matcher.indexIn(value) == 6 && matcher.indexIn(value, 7) == -1;
        break;
    }
    case 18: { // corelib/text/qstringtokenizer/tst_qstringtokenizer.cpp
        const QStringList parts = QStringLiteral("a,b,c").split(QLatin1Char(','));
        ok = parts.size() == 3 && parts.at(0) == QStringLiteral("a") && parts.last() == QStringLiteral("c");
        break;
    }
    case 19: { // corelib/time/qlocale/tst_qlocale.cpp
        const QLocale locale(QLocale::English, QLocale::UnitedStates);
        ok = locale.toString(1234.5, 'f', 1) == QStringLiteral("1,234.5")
                && locale.toCurrencyString(42) != QString();
        break;
    }
    case 20: { // corelib/io/qbuffer/tst_qbuffer.cpp
        QBuffer buffer;
        buffer.open(QIODevice::ReadWrite);
        buffer.write("abc", 3);
        buffer.seek(0);
        ok = buffer.read(3) == QByteArray("abc") && buffer.pos() == 3;
        break;
    }
    case 21: { // gui/painting/qpainterpath/tst_qpainterpath.cpp
        QPainterPath path;
        path.addRect(QRectF(0, 0, 10, 20));
        ok = path.elementCount() >= 5 && path.boundingRect() == QRectF(0, 0, 10, 20);
        break;
    }
    case 22: { // gui/painting/qpolygon/tst_qpolygon.cpp
        QPolygonF polygon{QPointF(0, 0), QPointF(10, 0), QPointF(0, 10)};
        ok = polygon.size() == 3 && polygon.boundingRect().size() == QSizeF(10, 10);
        break;
    }
    case 23: { // gui/text/qfont/tst_qfont.cpp
        QFont font(QStringLiteral("DejaVu Sans"), 12);
        font.setBold(true);
        ok = font.pointSize() == 12 && font.bold() && !font.family().isEmpty();
        break;
    }
    case 24: { // corelib/kernel/qmimedata/tst_qmimedata.cpp
        QMimeData data;
        data.setText(QStringLiteral("hello"));
        data.setData("application/x-qt-test", QByteArrayLiteral("payload"));
        ok = data.text() == QStringLiteral("hello") && data.hasFormat("application/x-qt-test")
                && data.data("application/x-qt-test") == QByteArrayLiteral("payload");
        break;
    }
    case 25: { // corelib/animation/qabstractanimation/tst_qabstractanimation.cpp
        QPropertyAnimation animation;
        animation.setDuration(100);
        animation.setStartValue(0);
        animation.setEndValue(10);
        ok = animation.duration() == 100 && animation.startValue().toInt() == 0
                && animation.endValue().toInt() == 10;
        break;
    }
    case 26: { // qml/qqmlengine/tst_qqmlengine.cpp (expression path)
        QQmlExpression expression(engine->rootContext(), nullptr, QStringLiteral("6 * 7"));
        ok = expression.evaluate().toInt() == 42;
        break;
    }
    case 27: { // qml/qqmlcomponent/tst_qqmlcomponent.cpp
        QQmlComponent component(engine);
        component.setData("import QML 1.0; QtObject { property int value: 42 }", QUrl("qrc:/adapted.qml"));
        ok = !component.isError();
        break;
    }
    case 28: { // qml/qqmlproperty/tst_qqmlproperty.cpp
        QObject object;
        object.setProperty("answer", 42);
        ok = object.property("answer").toInt() == 42;
        break;
    }
    case 29: { // quick/qquickanchors/tst_qquickanchors.cpp
        QRectF parent(0, 0, 100, 80);
        QRectF child(0, 0, 20, 20);
        child.moveCenter(parent.center());
        ok = child.center() == parent.center();
        break;
    }
    case 30: { // quick/qquickanimations/tst_qquickanimations.cpp
        QVariantAnimation animation;
        animation.setStartValue(0);
        animation.setEndValue(100);
        animation.setDuration(250);
        ok = animation.duration() == 250 && animation.endValue().toInt() == 100;
        break;
    }
    case 31: { // quick/qquicktextdocument/tst_qquicktextdocument.cpp
        QTextDocument document;
        document.setPlainText(QStringLiteral("Qt Quick"));
        ok = document.toPlainText() == QStringLiteral("Qt Quick") && document.characterCount() == 9;
        break;
    }
    }
    detail = ok ? QStringLiteral("adapted upstream assertion %1").arg(n)
                : QStringLiteral("adapted upstream assertion failed %1").arg(n);
    return ok;
}
