#include "extended_tests.h"

#include <QUrl>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>

bool ultimateQuickExtended(QQmlEngine *engine, QString &detail)
{
    QQmlComponent component(engine);
    component.setData(R"QML(import QtQuick
Item {
    objectName: "coverageRoot"
    width: 320; height: 180
    Rectangle { objectName: "child"; anchors.fill: parent; color: "#204b57" }
})QML", QUrl(QStringLiteral("qrc:/qt-ultimate-quick.qml")));
    const bool ok = !component.isError();
    detail = ok ? QStringLiteral("QQuickItem source parse on shared QQmlEngine (creation covered by scenegraph step)")
                : component.errorString();
    return ok;
}
