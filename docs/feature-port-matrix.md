# Qt 6 Nintendo Switch Port Status

This is a practical status guide for developers deciding what to use today and
what to port next. It describes the current repository and the tested demo
paths; it does **not** claim that every API in a `Supported` module is complete.

## At a glance

| Area | Current answer |
|---|---|
| Can I build a Qt Widgets application? | Yes. The Widgets probe builds and runs in Astris. |
| Can I build a Qt Quick/QML application? | Yes, with the software renderer and statically embedded QML resources. |
| Are QtCore threads tested? | Yes. The module test executes code on a worker `QThread`. |
| Is hardware-accelerated Qt Quick available? | No. OpenGL/GPU scene graph support is missing. |
| Is QtNetwork usable? | Yes on real Switch hardware, including native Qt HTTPS; emulator socket-service coverage is incomplete. |
| Is QtWebEngine available? | No. It is outside the current branch scope. |
| Is this a complete Qt port? | No. It is a working QtCore/Gui/Widgets + QML/Quick bring-up. |

## Status legend

- **Supported** — tested in a Switch application and usable on the current path.
- **Workaround** — usable only through a Switch-specific fallback, restriction,
  or build-time workaround.
- **Not tested** — code may build or exist, but there is no meaningful runtime
  validation yet.
- **Missing** — no Switch implementation or usable build path is present.

## Module and feature matrix

| Module / feature | Status | What is actually demonstrated | What this means for the next step |
|---|---|---|---|
| QtCore | Supported | Event loop, timers, JSON, regular expressions, byte arrays and application runtime. | Expand from smoke tests to QtCore stress tests and failure cases. |
| QtThreads | Supported | A worker `QThread` executes code off the main thread. | Add queued signals, `QObject::moveToThread`, mutexes and thread shutdown tests. |
| QtGui | Supported | `QImage`, `QPainter`, font loading and raster presentation. | Test image formats, text rendering, scaling and memory pressure. |
| QtWidgets | Supported | Widgets, layouts, labels, buttons, timers and controller input. | Add dialogs, item views, text editing, focus, resize and widget interaction tests. |
| Qt Quick | Supported with workaround | QML objects, animations, input and frame updates run in the Quick probe. | Keep software path stable, then implement and test a Switch GPU backend. |
| QtQml | Supported with workaround | `QQmlEngine`, `QQmlExpression`, `QQmlComponent` and embedded QML modules. | Test imports, bindings, errors, garbage collection and larger QML applications. |
| QtQmlModels | Build/link validated | Included in the Quick dependency chain and statically linked. | Add a model/view test with list, model updates and delegates. |
| QtQmlWorkerScript | Build/link validated | Included in the Quick dependency chain and plugin set. | Add an actual WorkerScript runtime test. |
| QtShaderTools | Workaround | Host `qsb` is used to prepare the Quick dependency chain. | Validate shader compilation variants and document supported shader formats. |
| QtNetwork | Supported on hardware | A standalone probe passes addressing, interfaces, DNS, TCP/UDP, local HTTP, `QNetworkAccessManager`, and Google DNS/HTTPS on real Switch hardware. Native `QNetworkAccessManager` HTTPS uses Qt OpenSSL, libnx CSPRNG seeding, and an SD-deployed PEM root bundle. Ryubing may lack BSD socket-service paths. | Expand protocol and failure-path coverage; keep real hardware as the authoritative network test target. |
| QtOpenGL | Missing | QPA reports OpenGL and GL surface capabilities as unavailable. | Implement a Switch graphics context and platform GL integration. |
| Qt Quick GPU scene graph | Missing | Quick is forced to the software backend. | This is the highest-impact performance task for real Quick applications. |
| QtWebEngine | Missing | No WebEngine module, demo or current build path. | Treat as a separate large project after QtNetwork and graphics foundations. |
| QtMultimedia | Missing | No Switch audio/video backend or runtime probe. | Decide whether audio, video, or both are required; then port libnx backends. |
| QtSql | Not tested | No Switch database driver or application probe. | Start with SQLite only if a product requirement exists. |
| QtQuickControls2 | Not tested | No Controls2 runtime probe on Switch. | Add it after basic Quick and text/input behavior is stable. |
| QtQuickLayouts | Not tested | No dedicated Layouts runtime probe. | Add a small QML layout test if Controls2 is needed. |
| QtTest | Missing for Switch runtime | No supported Switch QtTest runner or validated `Qt6Test` build is tracked in this repository. | Revisit only when a real runner and host-tool flow are in place. |
| QtQuickTest | Missing for Switch runtime | No QML test runner is available on Switch. | Revisit after QML deployment is reliable. |
| QtDBus | Missing | D-Bus is disabled and has no Switch service equivalent. | Low priority unless a specific service integration is required. |
| QtPrintSupport | Missing | No printer/system print backend. | Not relevant for the current console target unless requirements change. |
| QtBluetooth | Missing | No Switch Bluetooth backend. | Requires a product-specific libnx/system integration. |
| QtSensors | Missing | No Switch sensor backend. | Add only with a concrete controller/sensor use case. |
| QtLocation / QtPositioning | Missing | No location backend. | Low priority for the current target. |
| QtNfc | Missing | No NFC backend. | Requires a concrete hardware integration before porting. |
| QtWebSockets / QtHttpServer / QtGrpc / QtCoAP | Not tested | No runtime network protocol probes. | Revisit after the QtNetwork foundation works. |
| QtPdf / QtTextToSpeech / QtHelp | Missing | No Switch backend or validated runtime path. | Separate feature projects; not part of the current bring-up. |

## Platform limitations that affect all modules

| Area | Status | Current behavior |
|---|---|---|
| Controller input | Supported | A/B, D-pad, left stick, Plus/Minus and basic modifier mappings generate Qt key events. |
| Touch input | Workaround | The first touch point is synthesized as mouse press/move/release; multi-touch is not implemented. |
| Rendering | Workaround | CPU raster framebuffer presentation is used; the screen is fixed at 1280x720. |
| Window management | Workaround | A single default `NWindow` and framebuffer are used; desktop-style multi-window behavior is not validated. |
| Fonts | Workaround | FreeType uses an embedded DejaVu Sans default font; system font discovery is unavailable. |
| QML deployment | Workaround | QML plugins and `qmldir` data are statically embedded; disk cache is disabled. |
| QML loading/threading | Workaround | Switch-specific synchronous loading and single-thread assumptions are used. |
| Debugging | Workaround | Runtime traces are disabled by default and enabled with `QT_SWITCH_DEBUG_LOG` or the marker file. |

## Recommended development order

1. **QtNetwork compatibility** — expand the standalone offline-safe DNS, TCP,
   UDP and HTTP smoke tests beyond the current basic hardware pass.
2. **Repeatable test strategy** — add a supported automated harness instead of
   relying only on visual probes.
3. **Qt Quick GPU backend** — replace the software-only path and measure frame
   time, memory, and shader compatibility.
4. **Input and window completeness** — add real touch events, focus/keyboard
   coverage, resize behavior and multi-window decisions.
5. **Product-driven modules** — port Multimedia, Bluetooth, Sensors, SQLite or
   WebEngine only when a concrete application requires them.

## How to validate the current state

```bash
./scripts/build-qt-module-test.sh
./scripts/run-qt6-switch-module-test-astris.sh
./scripts/build-qt-network-test.sh
./scripts/run-qt-network-test-astris.sh
```

The module test is a focused runtime smoke test, not a replacement for Qt's
full `QtTest` suite. Its on-screen result is the strongest current evidence for
QtCore, QtThreads, QtGui and QtWidgets/QPA.
