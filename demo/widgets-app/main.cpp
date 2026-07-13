#include <QApplication>
#include <QColor>
#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMap>
#include <QPushButton>
#include <QShowEvent>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtPlugin>

#include <cstdio>
#include <cstring>

#include <switch.h>

Q_IMPORT_PLUGIN(QSwitchIntegrationPlugin)

namespace {

bool probeLoggingEnabled()
{
    static const bool enabled = qEnvironmentVariableIntValue("QT_SWITCH_DEBUG_LOG") != 0;
    return enabled;
}

void traceProbe(const char *message)
{
    if (!probeLoggingEnabled())
        return;
    std::FILE *fp = std::fopen("sdmc:/qt6-switch-widgets-probe.log", "a");
    if (fp) {
        std::fprintf(fp, "[widgets-probe] %s\n", message);
        std::fclose(fp);
    }
    svcOutputDebugString(message, std::strlen(message));
}

QString buttonStyle()
{
    return QStringLiteral(
        "QPushButton { background: rgba(255,255,255,0.14); color: white; border: 2px solid rgba(255,255,255,0.25); "
        "border-radius: 18px; padding: 18px 24px; font-size: 22px; text-align: left; min-height: 92px; }"
        "QPushButton:focus { border-color: rgb(120, 220, 255); background: rgba(120,220,255,0.24); }"
        "QPushButton:pressed { background: rgba(255,255,255,0.28); }");
}

QWidget *makePageContainer(QWidget *parent, const QString &title, const QString &body, QVBoxLayout **outLayout)
{
    auto *page = new QWidget(parent);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(28, 28, 28, 28);
    layout->setSpacing(18);

    auto *titleLabel = new QLabel(title, page);
    titleLabel->setStyleSheet(QStringLiteral("font-size: 26px; font-weight: 700; color: white;"));
    layout->addWidget(titleLabel);

    auto *bodyLabel = new QLabel(body, page);
    bodyLabel->setWordWrap(true);
    bodyLabel->setStyleSheet(QStringLiteral("font-size: 18px; color: rgba(255,255,255,0.88);"));
    layout->addWidget(bodyLabel);

    *outLayout = layout;
    return page;
}

} // namespace

class ProbeWidget final : public QWidget
{
public:
    ProbeWidget()
    {
        traceProbe("ProbeWidget: ctor");
        setWindowTitle(QStringLiteral("Qt 6 Widgets Switch Probe"));
        resize(1280, 720);
        setFocusPolicy(Qt::NoFocus);

        auto *rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(40, 32, 40, 32);
        rootLayout->setSpacing(18);

        auto *headerLayout = new QHBoxLayout;
        headerLayout->setSpacing(24);

        auto *titleLayout = new QVBoxLayout;
        titleLayout->setSpacing(8);

        auto *title = new QLabel(QStringLiteral("Qt 6 Widgets Nintendo Switch Port"), this);
        title->setStyleSheet(QStringLiteral("font-size: 30px; font-weight: 700; color: white;"));
        titleLayout->addWidget(title);

        auto *subtitle = new QLabel(QStringLiteral("Simple menu flow test: D-pad move, A click, B back."), this);
        subtitle->setStyleSheet(QStringLiteral("font-size: 18px; color: rgba(255,255,255,0.92);"));
        titleLayout->addWidget(subtitle);
        headerLayout->addLayout(titleLayout, 1);

        m_hudLabel = new QLabel(this);
        m_hudLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_hudLabel->setMinimumWidth(220);
        m_hudLabel->setStyleSheet(QStringLiteral(
            "font-size: 20px; color: white; background: rgba(255,255,255,0.10); "
            "border: 2px solid rgba(255,255,255,0.30); border-radius: 18px; padding: 16px 20px;"));
        headerLayout->addWidget(m_hudLabel);
        rootLayout->addLayout(headerLayout);

        m_inputLabel = new QLabel(QStringLiteral("Last input: none"), this);
        m_inputLabel->setStyleSheet(QStringLiteral("font-size: 18px; color: rgba(255,255,255,0.96);"));
        rootLayout->addWidget(m_inputLabel);

        m_stack = new QStackedWidget(this);
        m_stack->setStyleSheet(QStringLiteral(
            "QStackedWidget { background: rgba(255,255,255,0.08); border: 3px solid rgba(255,255,255,0.28); border-radius: 28px; }"));
        rootLayout->addWidget(m_stack, 1);

        buildPages();

        setStyleSheet(QStringLiteral("background-color: rgb(33, 97, 72);"));

        m_timer.setInterval(16);
        connect(&m_timer, &QTimer::timeout, this, [this] {
            ++m_frameCounter;
            ++m_fpsFrameCounter;
            m_phase = (m_phase + 1) % 360;
            updateHudLabel();
            setStyleSheet(QStringLiteral("background-color: %1;").arg(QColor::fromHsv(m_phase, 145, 96).name()));
            const qint64 elapsedMs = m_fpsTimer.elapsed();
            if (elapsedMs >= 1000) {
                m_currentFps = (double(m_fpsFrameCounter) * 1000.0) / double(elapsedMs);
                m_fpsFrameCounter = 0;
                m_fpsTimer.restart();
                updateHudLabel();
            }
            if (!m_loggedFirstTimeout) {
                traceProbe("ProbeWidget: first timer timeout");
                m_loggedFirstTimeout = true;
            }
        });
        m_fpsTimer.start();
        m_timer.start();
        updateHudLabel();
    }

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->isAutoRepeat()) {
            QWidget::keyPressEvent(event);
            return;
        }

        char buffer[128];
        std::snprintf(buffer, sizeof(buffer), "ProbeWidget: key press %d", event->key());
        traceProbe(buffer);

        if (event->key() == Qt::Key_Escape) {
            if (goBack()) {
                setStatus(QStringLiteral("B pressed: back"));
                event->accept();
                return;
            }
            setStatus(QStringLiteral("B pressed: already at Menu 1"));
            event->accept();
            return;
        }

        if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down
            || event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
            setStatus(QStringLiteral("%1 pressed").arg(describeKey(event->key())));
        }

        QWidget::keyPressEvent(event);
    }

    void keyReleaseEvent(QKeyEvent *event) override
    {
        if (!event->isAutoRepeat()
            && event->key() != Qt::Key_Space
            && event->key() != Qt::Key_Escape) {
            setStatus(QStringLiteral("%1 released").arg(describeKey(event->key())));
        }
        QWidget::keyReleaseEvent(event);
    }

    void showEvent(QShowEvent *event) override
    {
        traceProbe("ProbeWidget: showEvent");
        QWidget::showEvent(event);
    }

private:
    void buildPages()
    {
        QVBoxLayout *layout = nullptr;
        m_menu1Page = makePageContainer(
            this,
            QStringLiteral("Menu 1"),
            QStringLiteral("This is the first page. Move focus with the D-pad and press A to activate a button."),
            &layout);

        auto *openMenu2Button = makeButton(QStringLiteral("Open Menu 2"));
        auto *menu1ActionButton = makeButton(QStringLiteral("Menu 1 Action"));
        layout->addWidget(openMenu2Button);
        layout->addWidget(menu1ActionButton);
        layout->addStretch(1);

        connect(openMenu2Button, &QPushButton::clicked, this, [this] {
            setStatus(QStringLiteral("Menu 1: Open Menu 2 clicked"));
            navigateToPage(m_menu2Page);
        });
        connect(menu1ActionButton, &QPushButton::clicked, this, [this] {
            setStatus(QStringLiteral("Menu 1: Action clicked"));
        });

        m_defaultFocus.insert(m_menu1Page, openMenu2Button);
        m_stack->addWidget(m_menu1Page);

        m_menu2Page = makePageContainer(
            this,
            QStringLiteral("Menu 2"),
            QStringLiteral("Press A on a focused button, or press B to go back to Menu 1."),
            &layout);

        auto *menu2ActionButton = makeButton(QStringLiteral("Menu 2 Action"));
        auto *backButton = makeButton(QStringLiteral("Back to Menu 1"));
        layout->addWidget(menu2ActionButton);
        layout->addWidget(backButton);
        layout->addStretch(1);

        connect(menu2ActionButton, &QPushButton::clicked, this, [this] {
            setStatus(QStringLiteral("Menu 2: Action clicked"));
        });
        connect(backButton, &QPushButton::clicked, this, [this] {
            setStatus(QStringLiteral("Menu 2: Back clicked"));
            goBack();
        });

        m_defaultFocus.insert(m_menu2Page, menu2ActionButton);
        m_stack->addWidget(m_menu2Page);

        navigateToPage(m_menu1Page, false);
    }

    QPushButton *makeButton(const QString &text)
    {
        auto *button = new QPushButton(text, this);
        button->setStyleSheet(buttonStyle());
        button->setFocusPolicy(Qt::StrongFocus);
        connect(button, &QPushButton::pressed, this, [this, text] {
            setStatus(QStringLiteral("%1 pressed").arg(text));
        });
        connect(button, &QPushButton::clicked, this, [this, text] {
            setStatus(QStringLiteral("%1 clicked").arg(text));
        });
        return button;
    }

    void navigateToPage(QWidget *page, bool pushHistory = true)
    {
        if (!page)
            return;

        QWidget *current = m_stack->currentWidget();
        if (pushHistory && current && current != page)
            m_history.append(current);

        m_stack->setCurrentWidget(page);
        if (QPushButton *focusButton = m_defaultFocus.value(page, nullptr))
            focusButton->setFocus();
    }

    bool goBack()
    {
        if (m_history.isEmpty())
            return false;

        QWidget *page = m_history.takeLast();
        navigateToPage(page, false);
        return true;
    }

    void setStatus(const QString &text)
    {
        m_inputLabel->setText(QStringLiteral("Last input: %1").arg(text));
    }

    void updateHudLabel()
    {
        m_hudLabel->setText(QStringLiteral("Frame %1\nFPS %2")
                                .arg(m_frameCounter)
                                .arg(m_currentFps, 0, 'f', 1));
    }

    static QString describeKey(int key)
    {
        switch (key) {
        case Qt::Key_Space:
            return QStringLiteral("A");
        case Qt::Key_Escape:
            return QStringLiteral("B");
        case Qt::Key_Up:
            return QStringLiteral("Up");
        case Qt::Key_Down:
            return QStringLiteral("Down");
        case Qt::Key_Left:
            return QStringLiteral("Left");
        case Qt::Key_Right:
            return QStringLiteral("Right");
        default:
            return QStringLiteral("Key %1").arg(key);
        }
    }

    QLabel *m_hudLabel = nullptr;
    QLabel *m_inputLabel = nullptr;
    QStackedWidget *m_stack = nullptr;
    QWidget *m_menu1Page = nullptr;
    QWidget *m_menu2Page = nullptr;
    QMap<QWidget *, QPushButton *> m_defaultFocus;
    QList<QWidget *> m_history;
    QElapsedTimer m_fpsTimer;
    QTimer m_timer;
    int m_phase = 0;
    int m_frameCounter = 0;
    int m_fpsFrameCounter = 0;
    double m_currentFps = 0.0;
    bool m_loggedFirstTimeout = false;
};

int main(int argc, char **argv)
{
    if (probeLoggingEnabled())
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
