#include <QGuiApplication>
#include <QFile>
#include <QObject>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlExpression>
#include <QQmlExtensionPlugin>
#include <QQuickView>
#include <QQuickWindow>
#include <QUrl>
#include <QtPlugin>

#include <cstdio>
#include <cstring>

#include <switch.h>

Q_IMPORT_PLUGIN(QSwitchIntegrationPlugin)
Q_IMPORT_QML_PLUGIN(QtQmlPlugin)
Q_IMPORT_QML_PLUGIN(QtQmlModelsPlugin)
Q_IMPORT_QML_PLUGIN(QtQmlWorkerScriptPlugin)
Q_IMPORT_QML_PLUGIN(QtQuick2Plugin)

namespace {

void traceProbe(const char *message)
{
    std::FILE *fp = std::fopen("sdmc:/qt6-switch-quick-probe.log", "a");
    if (fp) {
        std::fprintf(fp, "[quick-probe] %s\n", message);
        std::fclose(fp);
    }
    svcOutputDebugString(message, std::strlen(message));
}

void traceResourceExists(const char *label, const QString &path)
{
    const QByteArray message = QByteArray(label)
            + (QFile::exists(path) ? " exists " : " missing ")
            + path.toUtf8();
    traceProbe(message.constData());
}

constexpr auto qmlSource = R"QML(
import QtQuick

Rectangle {
    id: root
    width: 1280
    height: 720
    property int phase: 0
    color: Qt.hsla((phase % 360) / 360, 0.56, 0.18, 1)

    Timer {
        interval: 16
        running: true
        repeat: true
        onTriggered: root.phase += 1
    }

    Rectangle {
        anchors.centerIn: parent
        width: 760
        height: 360
        radius: 34
        color: "#f7c948"
        border.color: "#fff5cc"
        border.width: 6

        Text {
            anchors.centerIn: parent
            width: parent.width - 80
            horizontalAlignment: Text.AlignHCenter
            text: "QtQuick Switch Smoke Test\nQML Timer phase: " + root.phase + "\nPress B/Escape to quit"
            color: "#12332b"
            font.pixelSize: 42
            font.bold: true
            wrapMode: Text.WordWrap
        }
    }

    Rectangle {
        x: 80 + ((root.phase * 7) % 1040)
        y: 610
        width: 120
        height: 28
        radius: 14
        color: "#8bd3ff"
    }
}
)QML";

constexpr auto qmlEngineOnlySource = R"QML(
import QML 1.0

QtObject {
    property int answer: 42
}
)QML";

} // namespace

int main(int argc, char **argv)
{
    qputenv("QT_QPA_PLATFORM", "switch");
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qputenv("QSG_INFO", "1");
    qputenv("QT_QUICK_BACKEND", "software");
    QQuickWindow::setSceneGraphBackend(QStringLiteral("software"));

    Q_INIT_RESOURCE(qmake_QML);
    Q_INIT_RESOURCE(qmake_QtQml);
    Q_INIT_RESOURCE(qmake_QtQml_Models);
    Q_INIT_RESOURCE(qmake_QtQml_WorkerScript);
    Q_INIT_RESOURCE(qmake_QtQuick);

    traceProbe("main: starting");
    traceResourceExists("resource", QStringLiteral(":/qt-project.org/imports/QML/qmldir"));
    traceResourceExists("resource", QStringLiteral(":/qt-project.org/imports/QtQml/qmldir"));
    traceResourceExists("resource", QStringLiteral(":/qt-project.org/imports/QtQuick/qmldir"));
    QGuiApplication app(argc, argv);
    traceProbe("main: QGuiApplication constructed");

    traceProbe("main: constructing QQmlEngine");
    QQmlEngine engine;
    traceProbe("main: QQmlEngine constructed");

    traceProbe("main: evaluating QQmlExpression");
    QQmlExpression expression(engine.rootContext(), nullptr, QStringLiteral("1 + 41"));
    const QVariant expressionResult = expression.evaluate();
    if (expression.hasError()) {
        const QByteArray text = expression.error().toString().toUtf8();
        traceProbe(text.constData());
        return 3;
    }
    char expressionBuffer[96];
    std::snprintf(expressionBuffer, sizeof(expressionBuffer), "main: QQmlExpression result %d", expressionResult.toInt());
    traceProbe(expressionBuffer);

    traceProbe("main: constructing QQmlComponent");
    QQmlComponent component(&engine);
    traceProbe("main: QQmlComponent constructed");

    traceProbe("main: setting engine-only component data");
    component.setData(qmlEngineOnlySource, QUrl(QStringLiteral("qrc:/qt6-switch-engine-only.qml")));
    traceProbe("main: engine-only component data set");
    if (component.isError()) {
        for (const QQmlError &error : component.errors()) {
            const QByteArray text = error.toString().toUtf8();
            traceProbe(text.constData());
        }
        return 3;
    }

    traceProbe("main: setting QtQuick component data");
    component.setData(qmlSource, QUrl(QStringLiteral("qrc:/qt6-switch-quick-probe.qml")));
    traceProbe("main: QtQuick component data set");
    if (component.isError()) {
        for (const QQmlError &error : component.errors()) {
            const QByteArray text = error.toString().toUtf8();
            traceProbe(text.constData());
        }
        return 3;
    }

    traceProbe("main: creating QtQuick root object");
    QObject *rootObject = component.create();
    if (!rootObject) {
        traceProbe("main: QtQuick root object create failed");
        for (const QQmlError &error : component.errors()) {
            const QByteArray text = error.toString().toUtf8();
            traceProbe(text.constData());
        }
        return 3;
    }
    traceProbe("main: QtQuick root object created");

    traceProbe("main: constructing QQuickView");
    QQuickView view(&engine, nullptr);
    traceProbe("main: QQuickView constructed");
    traceProbe("main: setting resize mode");
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    traceProbe("main: resizing view");
    view.resize(1280, 720);

    QObject::connect(&view, &QQuickView::statusChanged, &view, [](QQuickView::Status status) {
        char buffer[96];
        std::snprintf(buffer, sizeof(buffer), "QQuickView: status %d", int(status));
        traceProbe(buffer);
    });
    QObject::connect(&view, &QQuickView::sceneGraphInitialized, &view, [] {
        traceProbe("QQuickView: sceneGraphInitialized");
    });
    QObject::connect(&view, &QQuickView::frameSwapped, &view, [] {
        static int frameCount = 0;
        ++frameCount;
        if ((frameCount % 60) == 0) {
            char buffer[96];
            std::snprintf(buffer, sizeof(buffer), "QQuickView: frameSwapped %d", frameCount);
            traceProbe(buffer);
        }
    });
    QObject::connect(&view, &QQuickView::sceneGraphError, &view, [](QQuickWindow::SceneGraphError error, const QString &message) {
        const QByteArray text = QByteArray("QQuickView: sceneGraphError ")
                + QByteArray::number(int(error))
                + " "
                + message.toUtf8();
        traceProbe(text.constData());
    });

    traceProbe("main: setting QQuickView content");
    view.setContent(QUrl(QStringLiteral("qrc:/qt6-switch-quick-probe.qml")), &component, rootObject);
    traceProbe("main: QQuickView content set");
    if (view.status() == QQuickView::Error) {
        traceProbe("QQuickView: load error");
        for (const QQmlError &error : view.errors()) {
            const QByteArray text = error.toString().toUtf8();
            traceProbe(text.constData());
        }
        return 3;
    }

    traceProbe("main: showing view");
    view.show();
    traceProbe("main: view shown");
    return app.exec();
}
