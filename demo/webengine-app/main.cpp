#include <QApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QtPlugin>

#include <cstdio>
#include <cstring>

#include <switch.h>

Q_IMPORT_PLUGIN(QSwitchIntegrationPlugin)

namespace {

bool webLoggingEnabled()
{
    static const bool enabled = qEnvironmentVariableIntValue("QT_SWITCH_DEBUG_LOG") != 0;
    return enabled;
}

void traceWeb(const char *message)
{
    if (!webLoggingEnabled())
        return;

    std::FILE *fp = std::fopen("sdmc:/qt6-switch-webengine-probe.log", "a");
    if (fp) {
        std::fprintf(fp, "[webengine-probe] %s\n", message);
        std::fclose(fp);
    }
    svcOutputDebugString(message, std::strlen(message));
}

QString demoHtml()
{
    return QStringLiteral(R"HTML(
<!doctype html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html, body { margin: 0; height: 100%; font-family: sans-serif; color: #f8fbff; background: #14382f; }
    body { display: grid; place-items: center; }
    main { width: min(1040px, 88vw); }
    h1 { font-size: 48px; margin: 0 0 20px; }
    p { font-size: 24px; line-height: 1.35; margin: 0 0 24px; }
    button { font-size: 24px; padding: 18px 24px; border: 0; border-radius: 8px; background: #79e0bf; color: #06241d; }
    #status { margin-top: 24px; font-size: 22px; color: #f1d46b; }
  </style>
</head>
<body>
  <main>
    <h1>QtWebEngine on Switch</h1>
    <p>This local page proves WebEngine HTML, CSS and JavaScript are running inside the Switch Qt platform plugin.</p>
    <button onclick="document.getElementById('status').textContent = 'JavaScript click handled: ' + new Date().toISOString()">Run JavaScript</button>
    <div id="status">Waiting for input.</div>
  </main>
</body>
</html>
)HTML");
}

} // namespace

class WebEngineProbeWindow final : public QMainWindow
{
public:
    WebEngineProbeWindow()
    {
        traceWeb("WebEngineProbeWindow: ctor");
        setWindowTitle(QStringLiteral("Qt 6 Switch WebEngine Probe"));
        resize(1280, 720);

        auto *container = new QWidget(this);
        auto *layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        m_status = new QLabel(QStringLiteral("Loading local WebEngine page..."), container);
        m_status->setStyleSheet(QStringLiteral(
            "font-size: 20px; color: white; background: #0b201a; padding: 10px 18px;"));
        layout->addWidget(m_status);

        m_view = new QWebEngineView(container);
        layout->addWidget(m_view, 1);

        setCentralWidget(container);

        connect(m_view, &QWebEngineView::loadStarted, this, [this] {
            traceWeb("QWebEngineView: loadStarted");
            m_status->setText(QStringLiteral("Load started"));
        });
        connect(m_view, &QWebEngineView::loadFinished, this, [this](bool ok) {
            traceWeb(ok ? "QWebEngineView: loadFinished ok" : "QWebEngineView: loadFinished failed");
            m_status->setText(ok ? QStringLiteral("Load finished") : QStringLiteral("Load failed"));
        });

        m_view->setHtml(demoHtml(), QUrl(QStringLiteral("https://switch.local/")));
        QTimer::singleShot(1000, this, [] {
            traceWeb("WebEngineProbeWindow: one second alive");
        });
    }

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Escape) {
            traceWeb("WebEngineProbeWindow: exit requested");
            QApplication::quit();
            event->accept();
            return;
        }
        QMainWindow::keyPressEvent(event);
    }

private:
    QLabel *m_status = nullptr;
    QWebEngineView *m_view = nullptr;
};

int main(int argc, char **argv)
{
    traceWeb("main: start");
    QApplication app(argc, argv);
    traceWeb("main: QApplication constructed");

    WebEngineProbeWindow window;
    window.show();
    traceWeb("main: window shown");

    const int result = app.exec();
    traceWeb("main: app exited");
    return result;
}
