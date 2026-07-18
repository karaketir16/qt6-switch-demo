# Switch test strategy

The current on-device validation entry points are the module test, the Widgets
probe, the Quick probe, and the standalone network probe.

- `demo/qt-module-test` validates QtCore, QtThreads, QtGui, and
  QtWidgets/QPA on-screen.
- `demo/widgets-app` validates the QtCore/Gui/Widgets/QPA runtime path on the
  Switch screen.
- `demo/quick-app` validates the current software-rendered Qt Quick/QML path.
- `demo/qt-network-test` validates QtNetwork separately so emulator limitations
  and live-network failures stay isolated.

Network checks are deterministic where possible: numeric/localhost resolution,
TCP, UDP, and an HTTP request against a local `QTcpServer` are offline-safe.
Separate mandatory Google DNS and HTTPS checks validate the live network path
through both libcurl-mbedTLS and native Qt OpenSSL. The network runner logs
each check and returns non-zero on failure; emulator BSD-service gaps remain
distinguishable from real-hardware results.

QtTest and QtQuickTest do not currently have a supported Switch runner in this
repository. Visible probes remain the practical validation path for QPA, input,
rendering, and module integration until a repeatable test harness exists.
