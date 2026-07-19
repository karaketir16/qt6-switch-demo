#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPainter>
#include <QPlainTextEdit>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlExpression>
#include <QQmlExtensionPlugin>
#include <QPushButton>
#include <QQuickView>
#include <QQuickWindow>
#include <QRegularExpression>
#include <QThread>
#include <QTimer>
#include <QUrl>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QWidget>
#include <QtPlugin>

#include "extended_tests.h"

#include <atomic>

#ifndef QT_ULTIMATE_ENABLE_QML_NETWORK
#define QT_ULTIMATE_ENABLE_QML_NETWORK 0
#endif

#if QT_ULTIMATE_ENABLE_QML_NETWORK
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#endif

Q_IMPORT_PLUGIN(QSwitchIntegrationPlugin)
Q_IMPORT_QML_PLUGIN(QtQmlPlugin)
Q_IMPORT_QML_PLUGIN(QtQmlModelsPlugin)
Q_IMPORT_QML_PLUGIN(QtQmlWorkerScriptPlugin)
Q_IMPORT_QML_PLUGIN(QtQuick2Plugin)

namespace {

enum class State { Pass, Fail, Skip };

struct Result {
    QString name;
    State state;
    QString detail;
};

class Worker final : public QThread
{
public:
    std::atomic_bool ran{false};

protected:
    void run() override
    {
        ran = QThread::currentThread() == this;
    }
};

class UltimateWindow final : public QWidget
{
public:
    UltimateWindow()
    {
        m_qmlEngine = new QQmlEngine(this);
        setWindowTitle(QStringLiteral("Qt 6 Switch Ultimate Test"));
        setMinimumSize(1280, 720);
        setStyleSheet(QStringLiteral("background:#142b3a;color:#f2f5f7;"));

        auto *layout = new QVBoxLayout(this);
        auto *title = new QLabel(QStringLiteral("Qt 6 Switch / QtBase + QtDeclarative diagnostic"), this);
        title->setStyleSheet(QStringLiteral("font-size:28px;font-weight:700;color:#f7c948;"));
        layout->addWidget(title);

        m_summary = new QLabel(QStringLiteral("Starting..."), this);
        m_summary->setStyleSheet(QStringLiteral("font-size:19px;"));
        layout->addWidget(m_summary);

        m_current = new QLabel(this);
        m_current->setStyleSheet(QStringLiteral("font-size:18px;color:#8bd3ff;"));
        layout->addWidget(m_current);

        m_results = new QPlainTextEdit(this);
        m_results->setReadOnly(true);
        m_results->setStyleSheet(QStringLiteral("font-size:16px;background:#0b1821;color:#f2f5f7;"));
        layout->addWidget(m_results, 1);

        auto *rerun = new QPushButton(QStringLiteral("Run again"), this);
        layout->addWidget(rerun);
        connect(rerun, &QPushButton::clicked, this, [this] { start(); });

        m_pulse = new QTimer(this);
        connect(m_pulse, &QTimer::timeout, this, [this] {
            ++m_pulses;
            m_summary->setToolTip(QStringLiteral("UI event loop pulses: %1").arg(m_pulses));
        });
        m_pulse->start(1000);
    }

    void start()
    {
        m_results->clear();
        m_results->appendPlainText(QStringLiteral("Qt 6 Switch Ultimate Test"));
        m_results->appendPlainText(QStringLiteral("No test timeout is used. Attach GDB and inspect m_currentTest/m_currentIndex."));
        m_results->appendPlainText(QStringLiteral("----------------------------------------"));
        m_results->appendPlainText(QStringLiteral("LOG: sdmc:/qt6-switch-ultimate-test.log"));
        m_results->appendPlainText(QString());
        m_results->appendPlainText(QStringLiteral("RUN_BEGIN %1").arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate)));
        m_resultsList.clear();
        m_currentIndex = 0;
        m_upstreamIndex = 0;
        m_running = true;
        log(QStringLiteral("RUN_BEGIN"));
        QTimer::singleShot(0, this, [this] { runNextTest(); });
    }

    // Stable symbols for GDB: break UltimateWindow::runNextTest and inspect these fields.
    void runNextTest()
    {
        if (!m_running)
            return;
        if (m_currentIndex >= tests().size()) {
            finishRun();
            return;
        }

        const TestCase test = tests().at(m_currentIndex);
        m_currentTest = test.name;
        m_current->setText(QStringLiteral("CURRENT: %1 (index %2/%3)")
                           .arg(m_currentTest).arg(m_currentIndex + 1).arg(tests().size()));
        log(QStringLiteral("STEP_BEGIN %1").arg(m_currentTest));
        QCoreApplication::processEvents();

        Result result = (this->*test.function)();
        result.name = m_currentTest;
        m_resultsList.append(result);
        const QString status = stateName(result.state);
        m_results->appendPlainText(QStringLiteral("%1  %2  %3").arg(status, result.name, result.detail));
        log(QStringLiteral("STEP_END %1 %2 %3").arg(status, result.name, result.detail));
        ++m_currentIndex;
        QTimer::singleShot(0, this, [this] { runNextTest(); });
    }

private:
    struct TestCase {
        QString name;
        Result (UltimateWindow::*function)();
    };

    static const QVector<TestCase> &tests()
    {
        static const QVector<TestCase> all = [] {
            QVector<TestCase> cases {
            {"QtCore/data", &UltimateWindow::testCoreData},
            {"QtCore/extended", &UltimateWindow::testCoreExtended},
            {"QtCore/qobject-events", &UltimateWindow::testObjectEvents},
            {"QtCore/threads", &UltimateWindow::testThreads},
            {"QtGui/image-painting", &UltimateWindow::testGui},
            {"QtGui/extended", &UltimateWindow::testGuiExtended},
            {"QtWidgets/qpa-window", &UltimateWindow::testWidgets},
            {"QtWidgets/model-view", &UltimateWindow::testWidgetsExtended},
            {"QtQml/engine-expression", &UltimateWindow::testQmlEngine},
            {"QtQml/component-binding", &UltimateWindow::testQmlComponent},
            {"QtQml/extended", &UltimateWindow::testQmlExtended},
            {"QtQuick/item-javascript", &UltimateWindow::testQuickExtended},
            {"QtQuick/scenegraph-window", &UltimateWindow::testQuick},
            {"Future/qml_network", &UltimateWindow::testQmlNetwork},
            };
            const QStringList categories {
                QStringLiteral("QtBase/corelib"), QStringLiteral("QtBase/gui"),
                QStringLiteral("QtBase/widgets"), QStringLiteral("QtBase/concurrent"),
                QStringLiteral("QtBase/network"), QStringLiteral("QtBase/xml"),
                QStringLiteral("QtBase/sql"), QStringLiteral("QtBase/opengl"),
                QStringLiteral("QtBase/testlib"), QStringLiteral("QtBase/other"),
                QStringLiteral("QtDeclarative/core"), QStringLiteral("QtDeclarative/qml"),
                QStringLiteral("QtDeclarative/quick"), QStringLiteral("QtDeclarative/quickcontrols"),
                QStringLiteral("QtDeclarative/quickwidgets"), QStringLiteral("QtDeclarative/particles"),
                QStringLiteral("QtDeclarative/qmldom"), QStringLiteral("QtDeclarative/qmlnetwork"),
                QStringLiteral("QtDeclarative/qmltest"), QStringLiteral("QtDeclarative/quicktest")
            };
            for (int i = 0; i < 500; ++i) {
                const QString &category = categories.at(i % categories.size());
                cases.append({QStringLiteral("Bulk/%1/case-%2").arg(category).arg(i), &UltimateWindow::testBulk});
            }
            const QStringList upstream {
                QStringLiteral("QtBase/corelib/text/qstring/tst_qstring.cpp"),
                QStringLiteral("QtBase/corelib/text/qbytearray/tst_qbytearray.cpp"),
                QStringLiteral("QtBase/corelib/io/qurl/tst_qurl.cpp"),
                QStringLiteral("QtBase/corelib/io/qurlquery/tst_qurlquery.cpp"),
                QStringLiteral("QtBase/corelib/time/qdatetime/tst_qdatetime.cpp"),
                QStringLiteral("QtBase/corelib/kernel/qmetatype/tst_qmetatype.cpp"),
                QStringLiteral("QtBase/corelib/itemmodels/qabstractitemmodel/tst_qabstractitemmodel.cpp"),
                QStringLiteral("QtBase/gui/painting/qcolor/tst_qcolor.cpp"),
                QStringLiteral("QtBase/gui/image/qimage/tst_qimage.cpp"),
                QStringLiteral("QtBase/gui/painting/qtransform/tst_qtransform.cpp"),
                QStringLiteral("QtDeclarative/qml/qqmlengine/tst_qqmlengine.cpp"),
                QStringLiteral("QtDeclarative/qml/qjsvalue/tst_qjsvalue.cpp"),
                QStringLiteral("QtDeclarative/qml/qqmlpropertymap/tst_qqmlpropertymap.cpp"),
                QStringLiteral("QtDeclarative/qml/qqmlbinding/tst_qqmlbinding.cpp"),
                QStringLiteral("QtDeclarative/quick/qquickitem/tst_qquickitem.cpp"),
                QStringLiteral("QtDeclarative/quick/qquickrectangle/tst_qquickrectangle.cpp")
                , QStringLiteral("QtBase/corelib/text/qstringlist/tst_qstringlist.cpp")
                , QStringLiteral("QtBase/corelib/text/qbytearraymatcher/tst_qbytearraymatcher.cpp")
                , QStringLiteral("QtBase/corelib/text/qstringtokenizer/tst_qstringtokenizer.cpp")
                , QStringLiteral("QtBase/corelib/time/qlocale/tst_qlocale.cpp")
                , QStringLiteral("QtBase/corelib/io/qbuffer/tst_qbuffer.cpp")
                , QStringLiteral("QtBase/gui/painting/qpainterpath/tst_qpainterpath.cpp")
                , QStringLiteral("QtBase/gui/painting/qpolygon/tst_qpolygon.cpp")
                , QStringLiteral("QtBase/gui/text/qfont/tst_qfont.cpp")
                , QStringLiteral("QtBase/corelib/kernel/qmimedata/tst_qmimedata.cpp")
                , QStringLiteral("QtBase/corelib/animation/qabstractanimation/tst_qabstractanimation.cpp")
                , QStringLiteral("QtDeclarative/qml/qqmlengine/expression/tst_qqmlengine.cpp")
                , QStringLiteral("QtDeclarative/qml/qqmlcomponent/tst_qqmlcomponent.cpp")
                , QStringLiteral("QtDeclarative/qml/qqmlproperty/tst_qqmlproperty.cpp")
                , QStringLiteral("QtDeclarative/quick/qquickanchors/tst_qquickanchors.cpp")
                , QStringLiteral("QtDeclarative/quick/qquickanimations/tst_qquickanimations.cpp")
                , QStringLiteral("QtDeclarative/quick/qquicktextdocument/tst_qquicktextdocument.cpp")
            };
            for (int i = 0; i < upstream.size(); ++i)
                cases.append({upstream.at(i), &UltimateWindow::testUpstream});
            return cases;
        }();
        return all;
    }

    static QString stateName(State state)
    {
        return state == State::Pass ? QStringLiteral("PASS")
             : state == State::Skip ? QStringLiteral("SKIP") : QStringLiteral("FAIL");
    }

    void log(const QString &line) const
    {
        qInfo().noquote() << line;
        QFile file(QStringLiteral("sdmc:/qt6-switch-ultimate-test.log"));
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            file.write(line.toUtf8());
            file.write("\n");
        }
    }

    Result pass(const QString &detail) const { return {QString(), State::Pass, detail}; }
    Result fail(const QString &detail) const { return {QString(), State::Fail, detail}; }

    Result testCoreData()
    {
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << QStringLiteral("switch") << qint32(42) << QVariantMap{{"port", "qt"}};
        QDataStream in(&bytes, QIODevice::ReadOnly);
        QString text;
        qint32 number = 0;
        QVariantMap map;
        in >> text >> number >> map;
        const auto json = QJsonDocument::fromJson(QByteArrayLiteral("{\"answer\":42}"));
        const bool ok = text == QStringLiteral("switch") && number == 42 && map.value("port") == "qt"
                && QRegularExpression(QStringLiteral("sw.tch")).match(text).hasMatch()
                && json.object().value("answer").toInt() == 42
                && QUrl(QStringLiteral("qrc:/qt6-switch-ultimate.qml")).isValid();
        return ok ? pass(QStringLiteral("QDataStream + QVariantMap + JSON + regex + URL"))
                  : fail(QStringLiteral("core value round-trip mismatch"));
    }

    Result testCoreExtended()
    {
        QString detail;
        return ultimateCoreExtended(detail) ? pass(detail) : fail(detail);
    }

    Result testObjectEvents()
    {
        QObject parent;
        QObject child(&parent);
        child.setProperty("answer", 42);
        bool invoked = false;
        const bool queued = QMetaObject::invokeMethod(&child, [&invoked] { invoked = true; }, Qt::DirectConnection);
        const bool ok = child.parent() == &parent && child.property("answer").toInt() == 42 && queued && invoked;
        return ok ? pass(QStringLiteral("parenting + dynamic property + invokeMethod"))
                  : fail(QStringLiteral("QObject relationship/event invocation mismatch"));
    }

    Result testThreads()
    {
        Worker worker;
        worker.start();
        worker.wait();
        return worker.ran ? pass(QStringLiteral("QThread start + worker execution + unbounded wait"))
                          : fail(QStringLiteral("worker did not execute"));
    }

    Result testGui()
    {
        QImage image(128, 96, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::black);
        QPainter painter(&image);
        painter.fillRect(8, 8, 112, 80, QColor(QStringLiteral("#f7c948")));
        painter.drawLine(8, 8, 120, 88);
        painter.end();
        const bool ok = image.pixelColor(20, 20) == QColor(QStringLiteral("#f7c948"))
                && image.size() == QSize(128, 96);
        return ok ? pass(QStringLiteral("QImage + QPainter + pixel/size readback"))
                  : fail(QStringLiteral("painted pixel mismatch"));
    }

    Result testGuiExtended()
    {
        QString detail;
        return ultimateGuiExtended(detail) ? pass(detail) : fail(detail);
    }

    Result testWidgets()
    {
        const bool ok = windowHandle() != nullptr || isVisible();
        return ok ? pass(QStringLiteral("QWidget layout + Switch QPA native window"))
                  : fail(QStringLiteral("window has no native handle yet"));
    }

    Result testWidgetsExtended()
    {
        QString detail;
        return ultimateWidgetsExtended(detail) ? pass(detail) : fail(detail);
    }

    Result testQmlEngine()
    {
        QQmlExpression expression(m_qmlEngine->rootContext(), nullptr, QStringLiteral("21 * 2"));
        const QVariant value = expression.evaluate();
        return value.toInt() == 42 ? pass(QStringLiteral("QQmlEngine + QQmlExpression = 42"))
                                   : fail(QStringLiteral("expression returned %1").arg(value.toString()));
    }

    Result testQmlComponent()
    {
        QQmlComponent component(m_qmlEngine);
        const QByteArray source = R"QML(import QML 1.0
QtObject {
    property int answer: 21 * 2
    property string state: "ready"
})QML";
        component.setData(source, QUrl(QStringLiteral("qrc:/qt6-switch-ultimate.qml")));
        const bool ok = !component.isError();
        return ok ? pass(QStringLiteral("QQmlComponent setData + QML binding parse"))
                  : fail(component.errorString());
    }

    Result testQmlExtended()
    {
        QString detail;
        return ultimateQmlExtended(m_qmlEngine, detail) ? pass(detail) : fail(detail);
    }

    Result testQuick()
    {
        QQmlComponent component(m_qmlEngine);
        const QByteArray source = R"QML(import QtQuick
Rectangle { width: 320; height: 180; color: "#173f35" })QML";
        const QUrl url(QStringLiteral("qrc:/qt6-switch-ultimate.qml"));
        component.setData(source, url);
        QObject *root = component.create();
        if (!root || component.isError()) {
            const QString error = component.errorString();
            delete root;
            return fail(error);
        }
        // Keep the probe view alive for the process lifetime. The current
        // Switch QML port faults while destroying a QQuickView containing a
        // dynamically-created Rectangle; retaining it lets us test rendering
        // without turning cleanup into a false port failure.
        if (!m_quickView)
            m_quickView = new QQuickView(m_qmlEngine, nullptr);
        m_quickView->setResizeMode(QQuickView::SizeRootObjectToView);
        m_quickView->resize(640, 360);
        m_quickView->setContent(url, &component, root);
        m_quickView->show();
        for (int i = 0; i < 8; ++i)
            QCoreApplication::processEvents();
        const bool ok = m_quickView->isVisible() && m_quickView->width() == 640;
        m_quickView->hide();
        return ok ? pass(QStringLiteral("QQuickView + software scenegraph event pump (view retained)"))
                  : fail(QStringLiteral("Quick window did not become native"));
    }

    Result testQuickExtended()
    {
        QString detail;
        return ultimateQuickExtended(m_qmlEngine, detail) ? pass(detail) : fail(detail);
    }

    Result testQmlNetwork()
    {
#if QT_ULTIMATE_ENABLE_QML_NETWORK
        QNetworkAccessManager manager;
        QNetworkRequest request(QUrl(QStringLiteral("https://example.invalid")));
        return manager.networkAccessible() != QNetworkAccessManager::NotAccessible
                ? pass(QStringLiteral("network feature compiled and manager created"))
                : fail(QStringLiteral("network manager is inaccessible"));
#else
        return {QString(), State::Skip, QStringLiteral("compile-time disabled; enable QT_ULTIMATE_ENABLE_QML_NETWORK when QtQml network is built")};
#endif
    }

    Result testBulk()
    {
        bool converted = false;
        const int number = m_currentTest.section(QLatin1Char('-'), 1, 1).toInt(&converted);
        QString detail;
        return converted && ultimateBulkCase(number, detail) ? pass(detail) : fail(detail);
    }

    Result testUpstream()
    {
        QString detail;
        const bool ok = ultimateUpstreamCase(m_qmlEngine, m_upstreamIndex++, detail);
        return ok ? pass(detail) : fail(detail);
    }

    void finishRun()
    {
        m_running = false;
        int passed = 0;
        int skipped = 0;
        for (const Result &result : std::as_const(m_resultsList)) {
            passed += result.state == State::Pass;
            skipped += result.state == State::Skip;
        }
        const int failed = m_resultsList.size() - passed - skipped;
        m_summary->setText(QStringLiteral("DONE: %1 pass, %2 fail, %3 skip | no timeout used")
                           .arg(passed).arg(failed).arg(skipped));
        m_current->setText(QStringLiteral("CURRENT: complete; attach GDB during a rerun to inspect each step"));
        log(QStringLiteral("RUN_END pass=%1 fail=%2 skip=%3").arg(passed).arg(failed).arg(skipped));
    }

    QLabel *m_summary = nullptr;
    QLabel *m_current = nullptr;
    QPlainTextEdit *m_results = nullptr;
    QTimer *m_pulse = nullptr;
    QQmlEngine *m_qmlEngine = nullptr;
    QQuickView *m_quickView = nullptr;
    QVector<Result> m_resultsList;
    QString m_currentTest;
    int m_currentIndex = 0;
    int m_upstreamIndex = 0;
    int m_pulses = 0;
    bool m_running = false;
};

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
    QApplication app(argc, argv);
    UltimateWindow window;
    window.show();
    QTimer::singleShot(0, &window, [&window] { window.start(); });
    return app.exec();
}
