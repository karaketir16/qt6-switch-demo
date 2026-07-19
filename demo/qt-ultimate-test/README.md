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

Coverage is split across `main.cpp`, `core_extended.cpp`, `gui_extended.cpp`,
and `qml_extended.cpp`/`quick_extended.cpp`; the Makefile discovers and links them into the same
NRO. The extended checks exercise buffers, hashing, temporary files, URL
queries, timers/signals, image scaling/color spaces, antialiasing, text
documents, model/view filtering and index mapping, QML bindings, context lookup, QQuickItem geometry/child trees, and
QML JavaScript evaluation.

The current dashboard contains 546 ordered steps: 14 focused checks, 500
individually logged category-labelled bulk cases. The bulk cases cycle through
ten deterministic Qt value/container/serialization families while retaining
their QtBase/QtDeclarative source-tree category labels; they are coverage
probes, not a claim that the upstream 1,142 test files have all been ported.
There are also 32 individually named upstream-adapter cases with source
provenance in the dashboard; these execute portable assertions adapted from
the corresponding QtBase/QtDeclarative test areas.
For Ryubing GDB, load the ELF
and use the relocated address reported by the guest (`0x8500000 + 0xdd4c` for
the current `runNextTest` build):

```gdb
set architecture aarch64
file qt6-switch-ultimate-test.elf
break *0x850dd4c
continue
```

At the breakpoint, inspect `m_currentIndex` and `m_currentTest`; the active
step is also written to `sdmc:/qt6-switch-ultimate-test.log`.

Features not built in this port are represented as `SKIP` results behind
compile-time gates. For example, enable `QT_ULTIMATE_ENABLE_QML_NETWORK` only
when the corresponding QtQml network feature and QtNetwork library are part
of the Switch build.
