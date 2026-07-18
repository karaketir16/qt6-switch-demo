# Qt 6 Switch Ultimate Test

This is one GUI diagnostic binary covering QtCore, QtGui, QtWidgets/QPA,
QtQml, and QtQuick. QtTest is not required: this diagnostic intentionally uses
its own GUI harness so results remain visible and GDB can inspect the active
step directly. QtTest may still be used separately for focused regression
tests. The diagnostic does not use wall-clock test timeouts.
The current step is shown on screen and logged to
`sdmc:/qt6-switch-ultimate-test.log`; the symbols `m_currentTest`,
`m_currentIndex`, and `UltimateWindow::runNextTest` are kept easy to inspect
with GDB.

Features not built in this port are represented as `SKIP` results behind
compile-time gates. For example, enable `QT_ULTIMATE_ENABLE_QML_NETWORK` only
when the corresponding QtQml network feature and QtNetwork library are part
of the Switch build.
