#include <QApplication>
#include <QByteArray>
#include <QElapsedTimer>
#include <QFile>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QRegularExpression>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtPlugin>

#include <cstdio>

Q_IMPORT_PLUGIN(QSwitchIntegrationPlugin)

namespace {

struct Result {
    const char *module;
    bool ok;
    QString detail;
};

class ProbeThread final : public QThread
{
public:
    bool ranOnWorker = false;

protected:
    void run() override
    {
        ranOnWorker = QThread::currentThread() == this;
    }
};

class ProbeWindow final : public QWidget
{
public:
    ProbeWindow()
    {
        setWindowTitle(QStringLiteral("Qt 6 Switch Module Test"));
        setStyleSheet(QStringLiteral("background: #173f35; color: white;"));
        setMinimumSize(1280, 720);

        auto *layout = new QVBoxLayout(this);
        auto *title = new QLabel(QStringLiteral("Qt 6 Switch module test"), this);
        title->setStyleSheet(QStringLiteral("font-size: 30px; font-weight: 700; color: #f7c948;"));
        layout->addWidget(title);

        m_summary = new QLabel(this);
        m_summary->setStyleSheet(QStringLiteral("font-size: 20px; color: white;"));
        layout->addWidget(m_summary);

        m_results = new QLabel(this);
        m_results->setWordWrap(true);
        m_results->setStyleSheet(QStringLiteral("font-size: 19px; color: white;"));
        layout->addWidget(m_results, 1);

        auto *rerun = new QPushButton(QStringLiteral("Run tests again (A / Space)"), this);
        layout->addWidget(rerun);
        connect(rerun, &QPushButton::clicked, this, [this] { runTests(); });

        m_timer = new QTimer(this);
        m_timer->setInterval(1000);
        connect(m_timer, &QTimer::timeout, this, [this] {
            ++m_ticks;
            m_timerStatus->setText(QStringLiteral("QtCore/QTimer: PASS (%1 ticks)").arg(m_ticks));
        });

        m_timerStatus = new QLabel(this);
        m_timerStatus->setStyleSheet(QStringLiteral("font-size: 19px; color: #8bd3ff;"));
        layout->addWidget(m_timerStatus);
        m_timer->start();

    }

public:
    bool runTests()
    {
        QList<Result> results;
        results << testCore() << testGui() << testWidgets() << testThreads();

        int passed = 0;
        QString text;
        for (const Result &result : results) {
            passed += result.ok;
            text += QStringLiteral("%1  %2  %3\n")
                    .arg(result.ok ? QStringLiteral("PASS") : QStringLiteral("FAIL"),
                         QString::fromLatin1(result.module), result.detail);
        }
        m_summary->setText(QStringLiteral("%1/%2 module groups passed | Press B / Escape to exit")
                           .arg(passed).arg(results.size()));
        m_results->setText(text);
        if (std::FILE *log = std::fopen("sdmc:/qt6-switch-module-test.log", "a")) {
            std::fprintf(log, "module-test: %d/%lld passed\n", passed,
                         static_cast<long long>(results.size()));
            for (const Result &result : results)
                std::fprintf(log, "%s %s %s\n", result.ok ? "PASS" : "FAIL",
                             result.module, result.detail.toUtf8().constData());
            std::fclose(log);
        }
        return passed == results.size();
    }

    Result testCore() const
    {
        const QByteArray data("qt-switch");
        const QJsonDocument json = QJsonDocument::fromJson(QByteArrayLiteral("{\"answer\":42}"));
        const bool ok = data == QByteArrayLiteral("qt-switch")
                && QRegularExpression(QStringLiteral("switch")).match(QStringLiteral("qt-switch")).hasMatch()
                && json.object().value(QStringLiteral("answer")).toInt() == 42;
        return {"QtCore", ok, QStringLiteral("QByteArray + QRegularExpression + QJsonDocument")};
    }

    Result testGui() const
    {
        QImage image(96, 64, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::black);
        QPainter painter(&image);
        painter.fillRect(8, 8, 80, 48, QColor("#f7c948"));
        painter.end();
        const bool ok = image.pixelColor(20, 20) == QColor("#f7c948");
        return {"QtGui", ok, QStringLiteral("QImage + QPainter + pixel readback")};
    }

    Result testWidgets() const
    {
        const bool ok = windowHandle() != nullptr || isVisible();
        return {"QtWidgets/QPA", ok, QStringLiteral("QWidget + layout + native Switch window")};
    }

    Result testThreads()
    {
        ProbeThread worker;
        worker.start();
        if (!worker.wait(3000)) {
            worker.requestInterruption();
            worker.quit();
            worker.wait();
        }
        return {"QtThreads", worker.ranOnWorker, QStringLiteral("QThread start + worker-thread execution")};
    }

    QLabel *m_summary = nullptr;
    QLabel *m_results = nullptr;
    QLabel *m_timerStatus = nullptr;
    QTimer *m_timer = nullptr;
    int m_ticks = 0;
};

} // namespace

int main(int argc, char **argv)
{
    qputenv("QT_QPA_PLATFORM", "switch");
    QApplication app(argc, argv);
    ProbeWindow window;
    const bool batch = app.arguments().contains(QStringLiteral("--batch"));
    window.show();
    QTimer::singleShot(0, &window, [&window, batch, &app] {
        const bool passed = window.runTests();
        if (batch)
            QTimer::singleShot(0, &app, [passed] { QCoreApplication::exit(passed ? 0 : 1); });
    });
    const int result = app.exec();
    return result;
}
