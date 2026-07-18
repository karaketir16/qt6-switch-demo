# Switch test strategy

The current on-device module test entry point is `demo/qt-module-test`. It supports
two modes:

- default mode shows module results on the Switch screen;
- `--batch` runs the same checks and exits `0` only when all enabled groups
  pass, making it suitable for Astris or a CI launcher.

Network checks live in `demo/qt-network-test`. Local checks are deterministic
and offline-safe: numeric/localhost resolution, TCP, UDP, and an HTTP request
against a local `QTcpServer`. Separate mandatory Google DNS and HTTPS checks
validate the live network path through both libcurl-mbedTLS and native Qt
OpenSSL. The separate runner logs each check and returns non-zero on failure;
emulator BSD-service gaps remain distinguishable from real-hardware results.

QtTest now builds for the Switch target (`libQt6Test.a`). The next runner step
is a small `QTEST_APPLESS_MAIN` executable; the current cross-build still needs
the host `moc` tool exposed to generate its meta-object source.
QtQuickTest should follow only after QML deployment is stable; its runner must
use the existing software scene-graph path and embedded QML resources. The
visible probe remains useful for QPA/input/rendering checks that are not
appropriate for an app-less test.
