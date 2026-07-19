#include "extended_tests.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>

bool ultimateQmlExtended(QQmlEngine *engine, QString &detail)
{
    QQmlComponent component(engine);
    component.setData(R"QML(import QML 1.0
QtObject {
    property int answer: base * 2
    property int base: 21
    property string label: "ready"
})QML", QUrl(QStringLiteral("qrc:/qt-ultimate-extended.qml")));
    const bool ok = !component.isError();
    detail = ok ? QStringLiteral("QQmlComponent parse (creation/status/context gated on Switch)")
                : component.errorString();
    return ok;
}
