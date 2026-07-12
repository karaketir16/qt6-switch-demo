#include <QApplication>
#include <QColor>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QShowEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtPlugin>

#include <cstdio>
#include <cstring>

#include <switch.h>

Q_IMPORT_PLUGIN(QSwitchIntegrationPlugin)

namespace {

void traceProbe(const char *message)
{
    std::FILE *fp = std::fopen("sdmc:/qt6-switch-widgets-probe.log", "a");
    if (fp) {
        std::fprintf(fp, "[widgets-probe] %s\n", message);
        std::fclose(fp);
    }
    svcOutputDebugString(message, std::strlen(message));
}

}

class ProbeWidget final : public QWidget
{
public:
    ProbeWidget()
    {
        traceProbe("ProbeWidget: ctor");
        setWindowTitle(QStringLiteral("Qt 6 Widgets Switch Probe"));
        resize(1280, 720);

        auto *rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(48, 48, 48, 48);
        rootLayout->setSpacing(24);

        auto *title = new QLabel(QStringLiteral("Qt 6 Widgets Nintendo Switch Port"), this);
        title->setStyleSheet(QStringLiteral("font-size: 28px; font-weight: 700; color: white;"));
        rootLayout->addWidget(title);

        auto *body = new QLabel(QStringLiteral("If this screen is visible, QApplication, QWidget, layouts, painting, and QPushButton are all running on the current Switch QPA backend."), this);
        body->setWordWrap(true);
        body->setStyleSheet(QStringLiteral("font-size: 18px; color: rgba(255,255,255,0.92);"));
        rootLayout->addWidget(body);

        m_inputLabel = new QLabel(QStringLiteral("Last input: none"), this);
        m_inputLabel->setStyleSheet(QStringLiteral("font-size: 18px; color: rgba(255,255,255,0.96);"));
        rootLayout->addWidget(m_inputLabel);

        auto *buttonRow = new QHBoxLayout;
        buttonRow->setSpacing(16);

        auto *acceptButton = new QPushButton(QStringLiteral("A / Enter"), this);
        auto *cancelButton = new QPushButton(QStringLiteral("B / Escape"), this);
        auto *spaceButton = new QPushButton(QStringLiteral("X / Space"), this);

        const QString buttonStyle = QStringLiteral(
            "QPushButton { background: rgba(255,255,255,0.14); color: white; border: 2px solid rgba(255,255,255,0.25); "
            "border-radius: 18px; padding: 18px 24px; font-size: 18px; }"
            "QPushButton:focus { border-color: rgb(120, 220, 255); background: rgba(120,220,255,0.24); }"
            "QPushButton:pressed { background: rgba(255,255,255,0.28); }");
        acceptButton->setStyleSheet(buttonStyle);
        cancelButton->setStyleSheet(buttonStyle);
        spaceButton->setStyleSheet(buttonStyle);

        connect(acceptButton, &QPushButton::clicked, this, [this] {
            setStatus(QStringLiteral("A / Enter clicked"));
        });
        connect(cancelButton, &QPushButton::clicked, this, [this] {
            setStatus(QStringLiteral("B / Escape clicked"));
        });
        connect(spaceButton, &QPushButton::clicked, this, [this] {
            setStatus(QStringLiteral("X / Space clicked"));
        });

        buttonRow->addWidget(acceptButton);
        buttonRow->addWidget(cancelButton);
        buttonRow->addWidget(spaceButton);
        rootLayout->addLayout(buttonRow);

        m_frameLabel = new QLabel(QStringLiteral("Frame 0"), this);
        m_frameLabel->setAlignment(Qt::AlignCenter);
        m_frameLabel->setMinimumHeight(220);
        m_frameLabel->setStyleSheet(QStringLiteral(
            "font-size: 24px; color: white; background: rgba(255,255,255,0.10); "
            "border: 3px solid rgba(255,255,255,0.60); border-radius: 28px;"));
        rootLayout->addWidget(m_frameLabel, 1);

        setStyleSheet(QStringLiteral("background-color: rgb(33, 97, 72);"));

        m_timer.setInterval(16);
        connect(&m_timer, &QTimer::timeout, this, [this] {
            ++m_frameCounter;
            m_phase = (m_phase + 1) % 360;
            m_frameLabel->setText(QStringLiteral("Frame %1").arg(m_frameCounter));
            setStyleSheet(QStringLiteral("background-color: %1;").arg(QColor::fromHsv(m_phase, 145, 96).name()));
            if (!m_loggedFirstTimeout) {
                traceProbe("ProbeWidget: first timer timeout");
                m_loggedFirstTimeout = true;
            }
        });
        m_timer.start();
        traceProbe("ProbeWidget: timer started");
    }

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        setStatus(QStringLiteral("%1 pressed").arg(describeKey(event->key())));
        char buffer[128];
        std::snprintf(buffer, sizeof(buffer), "ProbeWidget: key press %d", event->key());
        traceProbe(buffer);
        QWidget::keyPressEvent(event);
    }

    void keyReleaseEvent(QKeyEvent *event) override
    {
        setStatus(QStringLiteral("%1 released").arg(describeKey(event->key())));
        QWidget::keyReleaseEvent(event);
    }

    void showEvent(QShowEvent *event) override
    {
        traceProbe("ProbeWidget: showEvent");
        QWidget::showEvent(event);
    }

private:
    void setStatus(const QString &text)
    {
        m_inputLabel->setText(QStringLiteral("Last input: %1").arg(text));
    }

    static QString describeKey(int key)
    {
        switch (key) {
        case Qt::Key_Return:
            return QStringLiteral("Return");
        case Qt::Key_Escape:
            return QStringLiteral("Escape");
        case Qt::Key_Space:
            return QStringLiteral("Space");
        case Qt::Key_Backspace:
            return QStringLiteral("Backspace");
        case Qt::Key_Up:
            return QStringLiteral("Up");
        case Qt::Key_Down:
            return QStringLiteral("Down");
        case Qt::Key_Left:
            return QStringLiteral("Left");
        case Qt::Key_Right:
            return QStringLiteral("Right");
        case Qt::Key_Plus:
            return QStringLiteral("Plus");
        default:
            return QStringLiteral("Key %1").arg(key);
        }
    }

    QLabel *m_inputLabel = nullptr;
    QLabel *m_frameLabel = nullptr;
    QTimer m_timer;
    int m_phase = 0;
    int m_frameCounter = 0;
    bool m_loggedFirstTimeout = false;
};

int main(int argc, char **argv)
{
    std::remove("sdmc:/qt6-switch-widgets-probe.log");
    traceProbe("main: start");
    qputenv("QT_QPA_PLATFORM", "switch");
    traceProbe("main: env set");

    QApplication app(argc, argv);
    traceProbe("main: QApplication constructed");

    ProbeWidget widget;
    widget.show();
    traceProbe("main: widget shown");
    traceProbe("main: before app.exec");

    return app.exec();
}
