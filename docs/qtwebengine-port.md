# QtWebEngine Switch Port

This document tracks the staged QtWebEngine port attempt for the existing Qt 6 Switch bring-up.

## Stage 1: Source and Consumer Demo

The repository now carries a `third_party/qtwebengine` submodule on the Qt `6.8.3` branch, matching the `qtbase` baseline.

Why:

- QtWebEngine must match the QtBase private API version.
- The existing Switch port is based on Qt `6.8.3`.

The first consumer is:

```bash
./scripts/build-webengine-probe.sh
```

That builds:

```text
demo/webengine-app/qt6-switch-webengine-probe.nro
```

The app intentionally uses `QWebEngineView` and loads local HTML with CSS and JavaScript. This proves the real WebEngine API is present; it is not a `QTextBrowser` fallback.

Test:

```bash
./scripts/build-webengine-probe.sh
```

Expected while QtWebEngine is not built yet:

```text
Missing .../libQt6WebEngineCore.a
```

That failure is intentional until the WebEngine module port produces the libraries.

## Stage 2: Configure QtWebEngine

QtWebEngineWidgets depends on Qt Quick internals, so the dependency order is:

```text
QtBase -> QtShaderTools/qsb -> QtDeclarative/Qml/Quick/QuickWidgets -> QtWebEngine -> webengine-app
```

Configure and build QtShaderTools first:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/build-host-qt-tools.sh

QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/build-host-qtshadertools-tools.sh

QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtshadertools-switch.sh

./scripts/build-qtshadertools-switch.sh
```

Why:

- QtDeclarative only builds Qt Quick when it can find the host `qsb` shader tool from QtShaderTools.
- QtWebEngineWidgets needs QuickWidgets, so Quick must be available before WebEngine configure.
- `scripts/build-host-qt-tools.sh` keeps a single host QtBase build and now leaves `QT_FEATURE_gui` enabled because QtShaderTools requires host `Qt::Gui`.
- The single host QtBase build must build both `Gui` and `host_tools`; `Qt6GuiConfig.cmake` alone is not enough because QtShaderTools links against `libQt6Gui.so`.

If `build/qtbase-host` was previously configured with GUI disabled, remove it before rebuilding:

```bash
rm -rf build/qtbase-host build/qtbase-host-install
./scripts/build-host-qt-tools.sh
```

Then configure and build QtDeclarative:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/build-host-qtdeclarative-tools.sh

QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtdeclarative-switch.sh

./scripts/build-qtdeclarative-switch.sh
```

Why:

- QtWebEngineCore links against Qt Quick private APIs in Qt 6.8.3.
- QtWebEngineWidgets pulls in the QtDeclarative/Quick stack even when the WebEngine Quick frontend is disabled.
- The target Switch build needs host-side QML tools such as `qmltyperegistrar`, `qmlcachegen`, `qmlimportscanner`, and `qmlaotstats`.

Host QML tool test:

```bash
find build/qtdeclarative-host/libexec build/qtdeclarative-host/bin \
  -maxdepth 1 -type f \
  \( -name qmltyperegistrar -o -name qmlcachegen -o -name qmlimportscanner -o -name qmlaotstats \)
```

Expected:

```text
build/qtdeclarative-host/libexec/qmltyperegistrar
build/qtdeclarative-host/libexec/qmlcachegen
build/qtdeclarative-host/libexec/qmlimportscanner
build/qtdeclarative-host/libexec/qmlaotstats
```

Switch dependency artifact test:

```bash
test -f build/qtbase-host/lib/libQt6Gui.so.6.8.3
test -x build/qtshadertools-host/bin/qsb
test -f build/qtshadertools-switch/lib/libQt6ShaderTools.a
test -f build/qtdeclarative-switch/lib/libQt6Qml.a
test -f build/qtdeclarative-switch/lib/libQt6Quick.a
```

Then run QtWebEngine configure after the existing host tools, Switch QtBase build, and Switch QtDeclarative build are available:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtwebengine-switch.sh
```

Then:

```bash
./scripts/build-qtwebengine-switch.sh
```

Expected first failures are in upstream QtWebEngine platform support checks and Chromium GN arguments. In Qt 6.8.3, the relevant starting points are:

- `third_party/qtwebengine/configure.cmake`
- `third_party/qtwebengine/src/core/CMakeLists.txt`

Known upstream assumptions to unwind for Switch:

- QtWebEngine rejects non-Linux, non-Windows and non-macOS targets.
- Static QtWebEngine builds are rejected upstream.
- Linux support checks assume glibc, fontconfig, NSS, DBus and desktop Khronos headers.
- Chromium GN setup is currently gated around desktop Linux Ozone, Windows, and macOS.

Current local patches:

- `scripts/configure-qtwebengine-switch.sh` mirrors the existing QtBase Switch configure script and passes `QT_QMAKE_TARGET_MKSPEC=devices/switch-aarch64-libnx-g++`.
- `scripts/configure-qtwebengine-switch.sh` sets `QT_MKSPECS_DIR` to the QtBase source mkspecs because the build tree only contains generated `.pri` files.
- `scripts/configure-qtwebengine-switch.sh` now points QtWebEngine at the separately built Switch QtDeclarative packages (`Qt6Qml`, `Qt6QmlModels`, and `Qt6Quick`).
- `third_party/qtwebengine/configure.cmake` allows the `SWITCH` platform through the first support gate.
- `third_party/qtwebengine/configure.cmake` allows static QtWebEngine only for `SWITCH`, matching the existing static QtBase Switch build.
- `third_party/qtwebengine/configure.cmake` relaxes the initial `Quick/Qml` support assertion for the staged widgets path, while the actual dependency is handled by building QtDeclarative.
- `scripts/build-qtbase-switch.sh` now builds `OpenGL` and `OpenGLWidgets` in addition to `Core`, `Gui`, `Widgets`, and the Switch platform plugin because QtDeclarative/Quick need their headers and static libraries.
- `third_party/qtbase/src/network/...` has an experimental Switch socket-backend patch set. It is not complete yet; full Network rebuild with `SWITCH=ON` currently exposes missing libnx/POSIX pieces such as local sockets and IPv6 packet info.
- `scripts/configure-qtdeclarative-switch.sh` disables QML debug/profiler/preview tooling for the Switch target because those tools are not needed for the WebEngine dependency path.
- `scripts/configure-qtdeclarative-switch.sh` sets `QT_SKIP_AUTO_PLUGIN_INCLUSION=ON` so unrelated QtGui plugins such as eglfs emulator are not imported during the Switch module configure step.
- `scripts/configure-qtdeclarative-switch.sh` creates a small build-tree `Qt6QmlTools` package shim so target builds can find the host QML tools produced by `build-host-qtdeclarative-tools.sh`.
- `scripts/configure-qtdeclarative-switch.sh` creates a small build-tree `Qt6ShaderToolsTools` package shim so target builds can find the host `qsb` produced by `build-host-qtshadertools-tools.sh`.
- `third_party/qtshadertools/src/glslang/CMakeLists.txt` treats `SWITCH` like Unix for glslang's OS-dependent source file.
- `third_party/qtdeclarative/CMakeLists.txt` conditionally includes QtShaderTools shader macros when the Switch configure script provides their path.
- `third_party/qtdeclarative/src/3rdparty/masm/...` stubs unavailable Switch VM/JIT memory-management helpers enough for the non-JIT QML build path.
- `third_party/qtdeclarative/src/3rdparty/masm/wtf/OSAllocatorSwitch.cpp` provides the Switch allocator path needed by the QML runtime.
- `third_party/qtdeclarative/src/qml/jsruntime/qv4compilationunitmapper_noop.cpp` avoids Unix mmap-backed QML cache mapping on Switch.
- `third_party/qtdeclarative/src/qml/memory/qv4stacklimits.cpp` uses a conservative Switch stack fallback instead of unavailable `pthread_getattr_np`.
- `scripts/configure-qtdeclarative-switch.sh` disables `qml_network` and `qml_xml_http_request` for the QtQuick smoke test so the test isolates QML/Quick rendering from the still-incomplete QtNetwork socket backend.

Validated so far:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/build-host-qtdeclarative-tools.sh
```

Result:

- completed successfully
- produced `qmlaotstats`, `qmlcachegen`, `qmlimportscanner`, and `qmltyperegistrar`

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/build-host-qtshadertools-tools.sh
```

Result:

- completed successfully
- produced `build/qtshadertools-host/bin/qsb`

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtshadertools-switch.sh
./scripts/build-qtshadertools-switch.sh
```

Result:

- completed successfully
- produced `build/qtshadertools-switch/lib/libQt6ShaderTools.a`

```bash
docker run --rm \
  -v "$(pwd):$(pwd)" \
  -w "$(pwd)/build/qtbase-switch" \
  devkitpro/devkita64 \
  bash -lc 'cmake --build "$(pwd)" --parallel "$(nproc)" --target OpenGL OpenGLWidgets'
```

Result:

- completed successfully
- produced `libQt6OpenGL.a` and `libQt6OpenGLWidgets.a`

Network note:

- an earlier `libQt6Network.a` artifact exists, but it was not a complete Switch socket backend validation
- after passing `SWITCH=ON` into QtBase configure, rebuilding `Network` reaches real porting gaps in Unix socket code
- keep QtQuick smoke testing independent of Network until those gaps are handled deliberately

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtdeclarative-switch.sh
```

Result:

- completed successfully

```bash
./scripts/build-qtdeclarative-switch.sh
```

Result:

- completed successfully for the current `Qml Quick` target set
- produced `build/qtdeclarative-switch/lib/libQt6Qml.a`
- produced `build/qtdeclarative-switch/lib/libQt6QmlModels.a`
- produced `build/qtdeclarative-switch/lib/libQt6Quick.a`
- `build/qtdeclarative-switch/config.summary` shows Qt Quick features enabled

Next useful gate:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtwebengine-switch.sh
```

Expected:

- QtWebEngine should now get past the earlier missing QML/Quick dependency gate.
- The next likely blocker is still the missing Chromium submodule or Chromium GN/platform assumptions.

Current external blocker:

- `third_party/qtwebengine/src/3rdparty` comes from `qtwebengine-chromium.git`.
- `code.qt.io` returned HTTP `503` while fetching that submodule.
- Continue with:

```bash
git submodule update --init --recursive --depth 1 --recommend-shallow third_party/qtwebengine
```

## Stage 2.5: QtQuick Smoke Test

Before continuing deep into QtWebEngine, test QtQuick directly.

Why:

- QtWebEngine depends on the same QtDeclarative/Quick stack.
- A tiny QtQuick app gives a cleaner failure signal than Chromium if the Switch scenegraph, QML runtime, or static plugin wiring is still incomplete.
- This should be a real runtime test, not just checking that `libQt6Quick.a` exists.

Recommended minimal test:

- build a tiny `QQuickView` or `QQmlApplicationEngine` app that loads an embedded QML rectangle/text scene
- link against the existing Switch QtBase, QtNetwork, QtShaderTools, QtQml, QtQmlModels, and QtQuick static libraries
- use the existing Astris launch pattern and close the previous app before launching
- add a short debug log similar to the WebEngine probe

Implemented probe:

```bash
./scripts/build-quick-probe.sh
./scripts/run-qt6-switch-quick-probe-astris.sh
```

Artifacts:

```text
demo/quick-app/qt6-switch-quick-probe.elf
demo/quick-app/qt6-switch-quick-probe.nro
```

The launcher copies the NRO to:

```text
/Volumes/T7/astrisData/homebrew/qt6-switch-quick-probe/qt6-switch-quick-probe.nro
```

and removes the previous guest trace before launch:

```text
/Volumes/T7/astrisData/sdcard/qt6-switch-quick-probe.log
```

Build fixes found during the smoke test:

- `scripts/build-widgets-probe.sh` now runs `make clean && make -j"$(nproc)" nro`; the previous single `make clean nro` could return successfully after deleting the build directory without producing a fresh NRO.
- `demo/quick-app/main.cpp` explicitly sets `QT_QPA_PLATFORM=switch`, matching the working Widgets probe.
- the Quick probe imports and links the minimal static QML plugins for `QtQml`, `QtQml.Models`, `QtQml.WorkerScript`, and `QtQuick`.
- the Switch QPA trace is gated behind `QT_SWITCH_DEBUG_LOG=1`; per-frame present logging is disabled by default because it can destabilize long Astris runs.

Validated build command:

```bash
./scripts/build-quick-probe.sh
```

Result:

```text
built ... qt6-switch-quick-probe.nro
```

Validated QtDeclarative plugin build command:

```bash
docker run --rm \
  -v "$(pwd):$(pwd)" \
  -w "$(pwd)" \
  devkitpro/devkita64 \
  bash -lc 'cmake --build build/qtdeclarative-switch --target qtquick2plugin qmlplugin modelsplugin workerscriptplugin -j"$(nproc)"'
```

Result:

- produced `build/qtdeclarative-switch/qml/QtQuick/libqtquick2plugin.a`
- produced `build/qtdeclarative-switch/qml/QtQml/libqmlplugin.a`
- produced `build/qtdeclarative-switch/qml/QtQml/Models/libmodelsplugin.a`
- produced `build/qtdeclarative-switch/qml/QtQml/WorkerScript/libworkerscriptplugin.a`

Runtime result on Astris:

- Astris launches the homebrew and creates the emulator GPU swapchain/layer.
- Switch QPA plugin loads once `QT_QPA_PLATFORM=switch` is set in the app.
- Switch runtime initializes input, touch, default `NWindow`, and linear framebuffer.
- `QGuiApplication` constructs successfully.
- static QML resources are linked into the probe; `QML`, `QtQml`, and `QtQuick` qmldirs exist under `:/qt-project.org/imports/...`.
- `QQmlEngine` constructs successfully.
- `QQmlExpression("1 + 41")` evaluates successfully after fixing the Switch stack-limit fallback.
- `QQmlComponent::setData()` initially blocked inside `QQmlTypeLoader::getType(data, url)`; the current Switch diagnostic path loads synchronous inline QML data on the main thread.
- engine-only `import QML 1.0` / `QtObject` component loading now succeeds.
- static `QtQuick` component data loads and resolves `QtQuick`, `QtQml`, `QML`, `QtQml.Models`, and `QtQml.WorkerScript` from QRC qmldirs.
- animated `Rectangle` + `Text` root object creation succeeds.
- `QQuickView::setContent()` reaches `QQuickView::Ready`.
- `QQuickView::show()` now returns after removing the explicit window-system flush from the Switch QPA `setVisible()` path.
- the probe forces the Qt Quick software scene graph with `QT_QUICK_BACKEND=software` and `QQuickWindow::setSceneGraphBackend("software")`.
- `QQuickView::sceneGraphInitialized` fires.
- `QSwitchBackingStore::flush` runs and presents a frame through the Switch framebuffer.
- Qt Quick declarative animations drive a changing background color and moving color bar.
- `QQuickView::frameSwapped` fires repeatedly; the latest stable run logged through `frameSwapped 3960` over roughly 66 seconds, and the Astris HUD reported `FPS 60` / `FIFO 0%`.

Switch QML runtime fixes found during the smoke test:

- `third_party/qtdeclarative/src/qml/memory/qv4stacklimits.cpp` now uses the current stack marker as the Switch stack base with an 8 MB fallback size. The previous fallback placed the soft limit on the wrong side of the current stack pointer and caused `RangeError: Maximum call stack size exceeded` for `QQmlExpression("1 + 41")`.
- `third_party/qtdeclarative/src/qml/qml/qqmltypeloader.cpp` has temporary Switch tracing around `QQmlTypeLoader::getType(data, url)` and `doLoad()`.
- `third_party/qtdeclarative/src/qml/qml/qqmlcomponent.cpp` has temporary Switch tracing around `QQmlComponent::setData()`.
- `QQmlTypeLoader::doLoad()` uses a Switch-only direct path for synchronous/prefer-synchronous static QML data. The normal `loader.load()` path still blocks before the app event loop starts; a broader `QQmlTypeLoaderThread` single-thread fallback also loaded imports but later crashed during QML binding finalization, so the current direct path is intentionally kept narrow.
- `QQmlTypeLoader::Blob::addLibraryImport()` lets the strongly locked `QML` module perform a real QRC qmldir lookup on Switch. Before that, `QML` used `QmldirCacheOnly`, missed the cache, and failed even though `:/qt-project.org/imports/QML/qmldir` existed.
- `QQmlComponent` uses a non-`thread_local` `creationDepth` on Switch. The devkitA64 TLS access path crashed during `QQmlComponent::beginCreate()` before object creation.
- `QUnifiedTimer` and `QAnimationTimer` use non-`thread_local` singleton storage on Switch. This fixes the QML `Timer` path that previously crashed through `QQmlTimer::update()` / `QUnifiedTimer::instance()`.
- per-frame Switch QPA present traces are disabled unless `QT_SWITCH_DEBUG_LOG=1` is set. The earlier unconditional `flush` / `framebufferBegin` / `framebufferEnd` debug strings created hundreds of host log events during animated Quick runs and coincided with Astris/Ryujinx memory-tracking failures.
- `QSwitchPlatformWindow::setVisible()` no longer calls `QWindowSystemInterface::flushWindowSystemEvents()` inline. With the flush, `QQuickView::show()` did not return; without it, the probe reaches `main: view shown`.
- `demo/quick-app/main.cpp` sets `QSG_INFO=1`, `QT_QUICK_BACKEND=software`, and `QQuickWindow::setSceneGraphBackend("software")` before creating the app so the Switch test uses the software scene graph rather than an unavailable OpenGL/RHI path.
- `scripts/run-qt6-switch-quick-probe-astris.sh` and the WebEngine launcher now try to dismiss Astris' macOS restore prompt by clicking `Don’t Reopen` / `Don't Reopen` if that prompt appears. This keeps test runs clean instead of restoring a crashed previous session.

Earlier guest trace before the stack and loader-thread fixes:

```text
[quick-probe] main: starting
[quick-probe] main: QGuiApplication constructed
[quick-probe] writeQmlFile: begin
[quick-probe] writeQmlFile: opened
[quick-probe] writeQmlFile: closed
[quick-probe] main: QML file written
[quick-probe] main: constructing QQuickView
[quick-probe] main: QQuickView constructed
[quick-probe] main: setting resize mode
[quick-probe] main: resizing view
[quick-probe] main: setting QML source
```

Current guest trace:

```text
[quick-probe] main: starting
[quick-probe] resource exists :/qt-project.org/imports/QML/qmldir
[quick-probe] resource exists :/qt-project.org/imports/QtQml/qmldir
[quick-probe] resource exists :/qt-project.org/imports/QtQuick/qmldir
[quick-probe] main: QGuiApplication constructed
[quick-probe] main: constructing QQmlEngine
[quick-probe] main: QQmlEngine constructed
[quick-probe] main: evaluating QQmlExpression
[quick-probe] main: QQmlExpression result 42
[quick-probe] main: constructing QQmlComponent
[quick-probe] main: QQmlComponent constructed
[quick-probe] main: setting engine-only component data
[qqmlcomponent] setData: begin
[qqmlcomponent] setData: cleared
[qqmlcomponent] setData: before getType
[qqmltypeloader] getType(data): begin
[qqmltypeloader] getType(data): locked
[qqmltypeloader] getType(data): typeData created
[qqmltypeloader] doLoad: begin
[qqmltypeloader] doLoad: startLoading done
[qqmltypeloader] doLoad: sync/prefer path
[qqmltypeloader] doLoad: switch direct loadThread
[qqmltypeloader] getType(data): loadWithStaticData returned
[qqmlcomponent] setData: after getType
[qqmlcomponent] setData: type complete/error
[quick-probe] main: engine-only component data set
[quick-probe] main: setting QtQuick component data
[qqmltypeloader] addLibraryImport: qmldir QtQuick path=:/qt-project.org/imports/QtQuick/qmldir url=qrc:/qt-project.org/imports/QtQuick/
[qqmltypeloader] addLibraryImport: qmldir QtQml path=:/qt-project.org/imports/QtQml/qmldir url=qrc:/qt-project.org/imports/QtQml/
[qqmltypeloader] addLibraryImport: qmldir QML path=:/qt-project.org/imports/QML/qmldir url=qrc:/qt-project.org/imports/QML/
[qqmltypeloader] addLibraryImport: qmldir QtQml.Models path=:/qt-project.org/imports/QtQml/Models/qmldir url=qrc:/qt-project.org/imports/QtQml/Models/
[qqmltypeloader] addLibraryImport: qmldir QtQml.WorkerScript path=:/qt-project.org/imports/QtQml/WorkerScript/qmldir url=qrc:/qt-project.org/imports/QtQml/WorkerScript/
[quick-probe] main: QtQuick component data set
[quick-probe] main: creating QtQuick root object
[qqmlcomponent] beginCreate: begin
[qqmlcomponent] beginCreate: state ready
[qqmlcomponent] beginCreate: compilation unit path
[qqmlcomponent] beginCreate: before initCreator
[qqmlcomponent] beginCreate: after initCreator
[qqmlcomponent] beginCreate: before creator create
[qqmlcomponent] beginCreate: after creator create
[quick-probe] main: QtQuick root object created
[quick-probe] main: constructing QQuickView
[quick-probe] main: QQuickView constructed
[quick-probe] main: setting resize mode
[quick-probe] main: resizing view
[quick-probe] main: setting QQuickView content
[quick-probe] QQuickView: status 1
[quick-probe] main: QQuickView content set
[quick-probe] main: showing view
[quick-probe] main: view shown
[quick-probe] QQuickView: sceneGraphInitialized
[switch-qpa] QSwitchBackingStore::flush
[switch-qpa] presentImage: framebufferBegin ok
[switch-qpa] presentImage: src=1280x720 bytesPerLine=5120 stride=5120 firstPixel=23,63,53,255
[switch-qpa] presentImage: framebufferEnd ok
[quick-probe] QQuickView: frameSwapped
```

Pass criteria:

- app launches under Astris
- text/rectangle scene loads
- Qt Quick animation updates visible state
- scene graph initializes
- Switch backing store flushes to the framebuffer
- frame-swap signal fires
- frame-swap count advances beyond the first frame

Current status:

- QtQuick passes the current animated visual smoke test on Astris using the software scene graph.
- The runtime reaches QPA + `QGuiApplication` + `QQmlEngine` + V4 expression + QRC QML imports + QtQuick root object creation + `QQuickView::Ready` + `view.show()` + `sceneGraphInitialized` + framebuffer present + `frameSwapped`.
- Qt Quick animation now works after the Switch `QUnifiedTimer` storage fix.
- latest screenshot captured the live Astris window with the QtQuick smoke scene and HUD `FPS 60` / `FIFO 0%` after the QPA trace gating change.

Next narrow debug gate:

- replace the narrow Switch static-data loader direct path with a real worker-thread/event-dispatcher fix
- reduce temporary Switch tracing once the port stabilizes

## Stage 3: Astris Test

Before launching a new run, close the previous app instance:

```bash
QT_SWITCH_DEBUG_LOG=1 \
./scripts/run-qt6-switch-webengine-probe-astris.sh
```

Good startup signals:

- `main: QApplication constructed`
- `WebEngineProbeWindow: ctor`
- `QWebEngineView: loadStarted`
- `QWebEngineView: loadFinished ok`

Visual pass criteria:

- the WebEngine page shows the `QtWebEngine on Switch` heading
- CSS styling is applied
- activating the button updates the page with JavaScript
