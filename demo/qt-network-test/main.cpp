#include <QApplication>
#include <QEventLoop>
#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkInterface>
#include <QNetworkProxy>
#include <QHostAddress>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QLabel>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QThread>
#include <QUrl>
#include <QUdpSocket>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <QtPlugin>

#include <cstdio>
#include <cstdarg>
#include <curl/curl.h>
#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/params.h>
#include <openssl/ssl.h>
#include <openssl/provider.h>

#ifdef __SWITCH__
#include <switch.h>

extern "C" gid_t getgid() { return 0; }
extern "C" gid_t getegid() { return 0; }
#endif

Q_IMPORT_PLUGIN(QSwitchIntegrationPlugin)
Q_IMPORT_PLUGIN(QTlsBackendOpenSSL)

#ifdef __SWITCH__
static int gOpenSslCryptoInit = 0;
static int gOpenSslSslInit = 0;
static int gOpenSslRandStatus = 0;
static int gOpenSslRandBytes = 0;
static int gOpenSslSeedSource = 0;
static int gOpenSslDefaultProvider = 0;
static bool gNifmInitialized = false;

static bool switchTraceEnabled()
{
    static const bool enabled = [] {
        if (qEnvironmentVariableIntValue("QT_SWITCH_DEBUG_LOG") != 0)
            return true;
        if (std::FILE *marker = std::fopen("sdmc:/qt6-switch-debug", "rb")) {
            std::fclose(marker);
            return true;
        }
        return false;
    }();
    return enabled;
}

static bool runningInEmulator()
{
    if (std::FILE *marker = std::fopen("sdmc:/qt6-switch-emulator", "rb")) {
        std::fclose(marker);
        return true;
    }
    return false;
}

static void switchTrace(const char *format, ...)
{
    if (!switchTraceEnabled())
        return;
    char message[768];
    va_list args;
    va_start(args, format);
    std::vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    if (std::FILE *log = std::fopen("sdmc:/qt6-switch-probe.log", "a")) {
        std::fprintf(log, "[network-test] %s\n", message);
        std::fclose(log);
    }
}

static void traceOpenSslErrors(const char *stage)
{
    unsigned long error = 0;
    bool hadError = false;
    while ((error = ERR_get_error()) != 0) {
        char text[256];
        ERR_error_string_n(error, text, sizeof(text));
        switchTrace("openssl stage=%s error=%s", stage, text);
        hadError = true;
    }
    if (!hadError)
        switchTrace("openssl stage=%s error=none", stage);
}

static void traceDrbgState(const char *stage)
{
    EVP_RAND_CTX *primary = RAND_get0_primary(nullptr);
    if (!primary) {
        switchTrace("drbg stage=%s primary=null", stage);
        traceOpenSslErrors(stage);
        return;
    }
    int state = -1;
    unsigned int strength = 0;
    OSSL_PARAM params[] = {
        OSSL_PARAM_construct_int(OSSL_RAND_PARAM_STATE, &state),
        OSSL_PARAM_construct_uint(OSSL_RAND_PARAM_STRENGTH, &strength),
        OSSL_PARAM_construct_end()
    };
    const int paramsOk = EVP_RAND_CTX_get_params(primary, params);
    switchTrace("drbg stage=%s state=%d getParams=%d strength=%u", stage,
                EVP_RAND_get_state(primary), paramsOk, strength);
    traceOpenSslErrors(stage);
}

static void addSwitchEntropy(const char *stage)
{
    unsigned char seed[64] = {};
    randomGet(seed, sizeof(seed));
    int nonZero = 0;
    int transitions = 0;
    for (int i = 0; i < int(sizeof(seed)); ++i) {
        nonZero += seed[i] != 0;
        if (i && seed[i] != seed[i - 1])
            ++transitions;
    }
    switchTrace("rng stage=%s emulator=%d bytes=%zu nonZero=%d transitions=%d", stage,
                runningInEmulator(), sizeof(seed), nonZero, transitions);
    ERR_clear_error();
    RAND_add(seed, sizeof(seed), sizeof(seed));
    traceOpenSslErrors("RAND_add");
    RAND_seed(seed, sizeof(seed));
    traceOpenSslErrors("RAND_seed");
    gOpenSslRandStatus = RAND_status();
    traceOpenSslErrors("RAND_status");
    unsigned char probe[32] = {};
    gOpenSslRandBytes = RAND_bytes(probe, sizeof(probe));
    traceOpenSslErrors("RAND_bytes");
    switchTrace("rng stage=%s RAND_status=%d RAND_bytes=%d", stage,
                gOpenSslRandStatus, gOpenSslRandBytes);
    traceDrbgState(stage);
}

static void seedOpenSslFromSwitchRng()
{
    switchTrace("openssl startup emulator=%d", runningInEmulator());
    ERR_clear_error();
    gOpenSslCryptoInit = OPENSSL_init_crypto(OPENSSL_INIT_NO_LOAD_CONFIG, nullptr);
    traceOpenSslErrors("OPENSSL_init_crypto");
    gOpenSslSeedSource = RAND_set_seed_source_type(nullptr, "SEED-SRC", "provider=default");
    traceOpenSslErrors("RAND_set_seed_source_type");
    gOpenSslDefaultProvider = OSSL_PROVIDER_load(nullptr, "default") != nullptr;
    traceOpenSslErrors("OSSL_PROVIDER_load(default)");
    gOpenSslSslInit = OPENSSL_init_ssl(OPENSSL_INIT_NO_LOAD_CONFIG, nullptr);
    traceOpenSslErrors("OPENSSL_init_ssl");
    switchTrace("openssl startup cryptoInit=%d sslInit=%d provider=%d seedSource=%d",
                gOpenSslCryptoInit, gOpenSslSslInit, gOpenSslDefaultProvider,
                gOpenSslSeedSource);
    addSwitchEntropy("startup");
}
#endif

namespace {

struct ProbeResult { const char *name; bool ok; QString detail; };

ProbeResult testAddressing()
{
    const QHostAddress ipv4(QStringLiteral("192.0.2.10"));
    const QHostAddress ipv6(QStringLiteral("::1"));
    const bool ok = ipv4.protocol() == QAbstractSocket::IPv4Protocol
            && ipv6.protocol() == QAbstractSocket::IPv6Protocol
            && QHostAddress::LocalHost == QHostAddress(QStringLiteral("127.0.0.1"));
    return {"Addressing", ok,
            QStringLiteral("ipv4=%1 ipv6=%2 localhost=%3")
            .arg(ipv4.toString()).arg(ipv6.toString()).arg(ok)};
}

ProbeResult testUrlParsing()
{
    const QUrl url(QStringLiteral("https://example.test:8443/api?q=qt#top"));
    const bool ok = url.isValid() && url.scheme() == QStringLiteral("https")
            && url.host() == QStringLiteral("example.test") && url.port() == 8443
            && url.query(QUrl::FullyDecoded) == QStringLiteral("q=qt")
            && url.fragment() == QStringLiteral("top");
    return {"QUrl", ok, QStringLiteral("valid=%1 scheme=%2 host=%3 port=%4")
            .arg(url.isValid()).arg(url.scheme()).arg(url.host()).arg(url.port())};
}

ProbeResult testInterfaces()
{
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    int up = 0;
    for (const QNetworkInterface &interface : interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp))
            ++up;
    }
    return {"Interfaces", !interfaces.isEmpty(),
            QStringLiteral("count=%1 up=%2").arg(interfaces.size()).arg(up)};
}

ProbeResult testProxyDefaults()
{
    const QNetworkProxy proxy = QNetworkProxy::applicationProxy();
    return {"Proxy", proxy.type() == QNetworkProxy::NoProxy,
            QStringLiteral("type=%1 host=%2 port=%3")
            .arg(static_cast<int>(proxy.type())).arg(proxy.hostName()).arg(proxy.port())};
}

ProbeResult testDns()
{
    QHostAddress numeric;
    const bool numericOk = numeric.setAddress(QStringLiteral("127.0.0.1"));
    const QHostInfo info = QHostInfo::fromName(QStringLiteral("localhost"));
    const bool hostnameOk = !info.addresses().isEmpty();
    QStringList resolvedAddresses;
    for (const QHostAddress &address : info.addresses())
        resolvedAddresses.append(address.toString());
    const QString addresses = resolvedAddresses.join(QLatin1Char(','));
    const QString error = hostnameOk ? QStringLiteral("none") : info.errorString();
    return {"DNS", numericOk && hostnameOk,
            QStringLiteral("numeric=%1 localhost=%2 addresses=%3 error=%4")
            .arg(numericOk).arg(hostnameOk).arg(addresses).arg(error)};
}

ProbeResult testGoogleDns()
{
    const QHostInfo info = QHostInfo::fromName(QStringLiteral("www.google.com"));
    QStringList addresses;
    for (const QHostAddress &address : info.addresses())
        addresses.append(address.toString());
    const bool ok = !info.addresses().isEmpty();
    return {"Google DNS", ok,
            QStringLiteral("resolved=%1 addresses=%2 error=%3")
            .arg(ok).arg(addresses.join(QLatin1Char(',')))
            .arg(ok ? QStringLiteral("none") : info.errorString())};
}

ProbeResult testTcpLifecycle()
{
    QTcpServer server;
    const bool listened = server.listen(QHostAddress::LocalHost, 0);
    const quint16 port = server.serverPort();
    const bool portOk = listened && port != 0;
    server.close();
    return {"TCP lifecycle", portOk,
            QStringLiteral("listened=%1 port=%2 closed=%3")
            .arg(listened).arg(port).arg(!server.isListening())};
}

ProbeResult testTcp()
{
    QTcpServer server;
    if (!server.listen(QHostAddress::AnyIPv4))
        return {"TCP", false, server.errorString()};
    QTcpSocket client;
    const QHostAddress loopback(QStringLiteral("127.0.0.1"));
    bool accepted = false;
    bool payload = false;
    QEventLoop loop;
    QTimer::singleShot(2000, &loop, &QEventLoop::quit);
    QObject::connect(&client, &QTcpSocket::errorOccurred, &loop, &QEventLoop::quit);
    QObject::connect(&server, &QTcpServer::newConnection, &loop, [&] {
        accepted = true;
        auto *peer = server.nextPendingConnection();
        QObject::connect(peer, &QTcpSocket::readyRead, &loop, [&, peer] {
            payload = peer->readAll() == QByteArrayLiteral("qt-network");
            loop.quit();
        });
        client.write(QByteArrayLiteral("qt-network"));
    });
    client.connectToHost(loopback, server.serverPort());
    loop.exec();
    return {"TCP", accepted && payload,
            QStringLiteral("accepted=%1 payload=%2").arg(accepted).arg(payload)};
}

ProbeResult testUdp()
{
    QUdpSocket server;
    if (!server.bind(QHostAddress::AnyIPv4, 0))
        return {"UDP", false, server.errorString()};
    QUdpSocket client;
    const QByteArray expected("qt-network");
    const bool sent = client.writeDatagram(expected, QHostAddress(QStringLiteral("127.0.0.1")),
                                            server.localPort()) == expected.size();
    QByteArray received(32, Qt::Uninitialized);
    bool ready = false;
    QEventLoop loop;
    QTimer::singleShot(2000, &loop, &QEventLoop::quit);
    QObject::connect(&server, &QUdpSocket::readyRead, &loop, [&] {
        received.resize(server.readDatagram(received.data(), received.size()));
        ready = true;
        loop.quit();
    });
    loop.exec();
    return {"UDP", sent && ready && received == expected,
            QStringLiteral("sent=%1 ready=%2 payload=%3")
            .arg(sent).arg(ready).arg(received == expected)};
}

ProbeResult testUdpBurst()
{
    QUdpSocket server;
    if (!server.bind(QHostAddress::AnyIPv4, 0))
        return {"UDP burst", false, server.errorString()};
    QUdpSocket client;
    const QByteArray first("one");
    const QByteArray second("two");
    const bool sent = client.writeDatagram(first, QHostAddress::LocalHost, server.localPort())
            == first.size()
            && client.writeDatagram(second, QHostAddress::LocalHost, server.localPort())
            == second.size();
    QList<QByteArray> received;
    QEventLoop loop;
    QTimer::singleShot(2000, &loop, &QEventLoop::quit);
    QObject::connect(&server, &QUdpSocket::readyRead, &loop, [&] {
        while (server.hasPendingDatagrams()) {
            QByteArray datagram(int(server.pendingDatagramSize()), Qt::Uninitialized);
            const qint64 size = server.readDatagram(datagram.data(), datagram.size());
            if (size >= 0) {
                datagram.resize(int(size));
                received.append(datagram);
            }
        }
        if (received.size() >= 2)
            loop.quit();
    });
    loop.exec();
    const bool payload = received.size() == 2 && received.contains(first) && received.contains(second);
    const bool ok = sent && payload;
    const QString receivedText = QStringList{
        QString::fromLatin1(received.value(0).toHex()),
        QString::fromLatin1(received.value(1).toHex())
    }.join(u',');
    return {"UDP burst", ok,
            QStringLiteral("sent=%1 datagrams=%2 payload=%3 received=%4")
            .arg(sent).arg(received.size()).arg(payload).arg(receivedText)};
}

ProbeResult testHttp()
{
    QTcpServer server;
    if (!server.listen(QHostAddress::AnyIPv4))
        return {"HTTP", false, server.errorString()};
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(
        QUrl(QStringLiteral("http://127.0.0.1:%1/health").arg(server.serverPort()))));
    QEventLoop loop;
    QTimer::singleShot(2500, &loop, &QEventLoop::quit);
    QObject::connect(&server, &QTcpServer::newConnection, &loop, [&] {
        auto *peer = server.nextPendingConnection();
        QObject::connect(peer, &QTcpSocket::readyRead, &loop, [peer] {
            peer->write(QByteArrayLiteral("HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK"));
            peer->flush();
        });
    });
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    const bool ok = reply->isFinished() && reply->error() == QNetworkReply::NoError
            && reply->readAll() == QByteArrayLiteral("OK");
    const QString error = reply->error() == QNetworkReply::NoError
            ? QStringLiteral("none") : reply->errorString();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QString detail = QStringLiteral("finished=%1 status=%2 error=%3 body=%4")
            .arg(reply->isFinished()).arg(status).arg(error).arg(ok);
    reply->deleteLater();
    return {"HTTP", ok, detail};
}

ProbeResult testHttpError()
{
    QTcpServer server;
    if (!server.listen(QHostAddress::AnyIPv4))
        return {"HTTP error", false, server.errorString()};
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(
        QUrl(QStringLiteral("http://127.0.0.1:%1/missing").arg(server.serverPort()))));
    QEventLoop loop;
    QTimer::singleShot(2500, &loop, &QEventLoop::quit);
    QObject::connect(&server, &QTcpServer::newConnection, &loop, [&] {
        auto *peer = server.nextPendingConnection();
        peer->write(QByteArrayLiteral("HTTP/1.1 404 Not Found\r\nContent-Length: 7\r\n\r\nmissing"));
        peer->flush();
        peer->waitForBytesWritten(1000);
    });
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const bool ok = reply->isFinished() && status == 404
            && reply->error() == QNetworkReply::ContentNotFoundError;
    const QString error = ok ? QStringLiteral("none")
            : (reply->isFinished() ? reply->errorString() : QStringLiteral("timeout"));
    reply->deleteLater();
    return {"HTTP error", ok,
            QStringLiteral("finished=%1 status=%2 error=%3")
            .arg(reply->isFinished()).arg(status).arg(error)};
}

ProbeResult testGoogleHttps()
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return {"Google HTTPS", false, QStringLiteral("curl init failed")};
    long status = 0;
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.com/generate_204");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "QtSwitchNetworkTest/1.0");
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    const CURLcode result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    const bool ok = result == CURLE_OK && (status == 204 || status == 200);
    const QString error = ok ? QStringLiteral("none")
            : QString::fromUtf8(curl_easy_strerror(result));
    curl_easy_cleanup(curl);
    return {"Google HTTPS", ok,
            QStringLiteral("status=%1 error=%2 backend=libcurl-mbedTLS")
            .arg(status).arg(error)};
}

ProbeResult testQtGoogleHttps()
{
#ifdef __SWITCH__
    // Re-seed immediately before Qt asks OpenSSL to create its SSL_CTX. This
    // also proves whether the provider accepts libnx entropy after startup.
    addSwitchEntropy("before-qnam-https");
#endif
    const bool sslSupported = QSslSocket::supportsSsl();
    const QList<QSslCertificate> embeddedCaCertificates = QSslCertificate::fromPath(
            QStringLiteral(":/qt-switch/mozilla-ca-bundle.pem"), QSsl::Pem);
    QSslConfiguration configuration = QSslConfiguration::defaultConfiguration();
    configuration.setCaCertificates(embeddedCaCertificates);
    QSslConfiguration::setDefaultConfiguration(configuration);
    const qsizetype systemCaCertificates = QSslConfiguration::systemCaCertificates().size();
    const qsizetype defaultCaCertificates = QSslConfiguration::defaultConfiguration()
            .caCertificates().size();
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(
        QUrl(QStringLiteral("https://www.google.com/generate_204"))));
    QEventLoop loop;
    QTimer::singleShot(10000, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const bool ok = !embeddedCaCertificates.isEmpty() && reply->isFinished()
            && reply->error() == QNetworkReply::NoError
            && (status == 204 || status == 200);
    const QString error = ok ? QStringLiteral("none")
            : (reply->isFinished() ? reply->errorString() : QStringLiteral("timeout"));
    reply->deleteLater();
    return {"Qt Google HTTPS", ok,
            QStringLiteral("finished=%1 status=%2 error=%3 sslSupported=%4 embeddedCa=%5 systemCa=%6 defaultCa=%7 version=%8 cryptoInit=%9 sslInit=%10 provider=%11 seedSource=%12 rand=%13 randBytes=%14 backend=QtOpenSSL")
            .arg(reply->isFinished()).arg(status).arg(error).arg(sslSupported)
            .arg(embeddedCaCertificates.size()).arg(systemCaCertificates).arg(defaultCaCertificates)
            .arg(QSslSocket::sslLibraryVersionString()).arg(gOpenSslCryptoInit)
            .arg(gOpenSslSslInit).arg(gOpenSslDefaultProvider).arg(gOpenSslSeedSource)
            .arg(gOpenSslRandStatus).arg(gOpenSslRandBytes)};
}

ProbeResult testNetworkManager()
{
    QNetworkAccessManager manager;
    return {"QNetworkAccessManager", manager.thread() == QThread::currentThread(),
            QStringLiteral("constructed on application thread")};
}

class NetworkWindow final : public QWidget
{
public:
    NetworkWindow()
    {
        setWindowTitle(QStringLiteral("QtNetwork Switch Test"));
        setStyleSheet(QStringLiteral("background: #173f35; color: white;"));
        setMinimumSize(1280, 720);

        auto *layout = new QVBoxLayout(this);
        auto *title = new QLabel(QStringLiteral("QtNetwork Switch test"), this);
        title->setStyleSheet(QStringLiteral("font-size: 30px; font-weight: 700; color: #f7c948;"));
        layout->addWidget(title);
        m_summary = new QLabel(QStringLiteral("Running tests..."), this);
        m_summary->setStyleSheet(QStringLiteral("font-size: 20px; color: white;"));
        layout->addWidget(m_summary);
        m_results = new QLabel(this);
        m_results->setStyleSheet(QStringLiteral("font-size: 19px; color: white;"));
        m_results->setWordWrap(true);
        layout->addWidget(m_results, 1);
        auto *rerun = new QPushButton(QStringLiteral("Run tests again (A / Space)"), this);
        layout->addWidget(rerun);
        connect(rerun, &QPushButton::clicked, this, [this] { runTests(); });
    }

    bool runTests()
    {
        const QList<ProbeResult> results{testAddressing(), testUrlParsing(), testInterfaces(),
                                         testProxyDefaults(), testDns(), testTcpLifecycle(),
                                         testTcp(), testUdp(), testUdpBurst(), testHttp(),
                                         testHttpError(), testGoogleDns(), testGoogleHttps(),
                                         testQtGoogleHttps(),
                                         testNetworkManager()};
        int passed = 0;
        QString text;
        std::FILE *log = std::fopen("sdmc:/qt6-switch-network-test.log", "w");
        for (const ProbeResult &result : results) {
            passed += result.ok;
            const QString line = QStringLiteral("%1  %2  %3")
                    .arg(result.ok ? QStringLiteral("PASS") : QStringLiteral("FAIL"),
                         QString::fromUtf8(result.name), result.detail);
            text += line + QLatin1Char('\n');
            const QByteArray detail = result.detail.toUtf8();
            std::fprintf(stdout, "%s %s %s\n", result.ok ? "PASS" : "FAIL", result.name,
                         detail.constData());
            if (log)
                std::fprintf(log, "%s %s %s\n", result.ok ? "PASS" : "FAIL", result.name,
                             detail.constData());
        }
        if (log) {
            std::fprintf(log, "network-test: %d/%lld passed\n", passed,
                         static_cast<long long>(results.size()));
            std::fclose(log);
        }
        m_summary->setText(QStringLiteral("%1/%2 test groups passed")
                           .arg(passed).arg(results.size()));
        m_results->setText(text);
        return passed == results.size();
    }

private:
    QLabel *m_summary = nullptr;
    QLabel *m_results = nullptr;
};

} // namespace

int main(int argc, char **argv)
{
#ifdef __SWITCH__
    socketInitializeDefault();
    gNifmInitialized = R_SUCCEEDED(nifmInitialize(NifmServiceType_User));
    seedOpenSslFromSwitchRng();
#endif
    qputenv("QT_QPA_PLATFORM", "switch");
    qputenv("QT_LOGGING_RULES", "qt.network.ssl.debug=true;qt.tlsbackend.ossl.debug=true");
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(network_test);
    NetworkWindow window;
    const bool batch = app.arguments().contains(QStringLiteral("--batch"));
    window.show();
    QTimer::singleShot(0, &window, [&window, batch, &app] {
        const bool passed = window.runTests();
        if (batch)
            QTimer::singleShot(0, &app, [passed] { QCoreApplication::exit(passed ? 0 : 1); });
    });
    const int result = app.exec();
#ifdef __SWITCH__
    if (gNifmInitialized)
        nifmExit();
    socketExit();
#endif
    return result;
}
