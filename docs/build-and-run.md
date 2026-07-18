# Build and Run

This document covers the current working build flow for the submodule-based Qt tree and the Switch demos.

Unless noted otherwise, run the wrapper scripts from the repository root.

## 1. Clone With the Patched Submodules

The repository tracks forked Qt submodules because the current Switch work depends on commits that do not exist in upstream `code.qt.io`.

Fresh clone:

```bash
git clone --recurse-submodules \
  -b main \
  https://github.com/karaketir16/qt6-switch-demo.git
```

If the submodules were not initialized during clone:

```bash
git submodule update --init --recursive
```

To restore the exact submodule revisions tracked by the checked-out commit:

```bash
git submodule update --init --recursive
```

The active submodule remotes are expected to be:

- `third_party/qtbase -> https://github.com/karaketir16/qtbase.git`
- `third_party/qtdeclarative -> https://github.com/karaketir16/qtdeclarative.git`
- `third_party/qtshadertools -> https://github.com/karaketir16/qtshadertools.git`
For the currently validated `widgets + quick` flow, these submodules are required:

- `third_party/qtbase`
- `third_party/qtdeclarative`
- `third_party/qtshadertools`

## 2. Build Host QtBase Tools

The Switch cross-build requires host-side Qt tools first.

```bash
./scripts/build-host-qt-tools.sh
```

Output directory:

- `build/qtbase-host/`

## 3. Configure and Build the Switch QtBase Target

Configure:

```bash
./scripts/build-openssl-switch.sh

QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtbase-switch.sh
```

Configuration output directory:

- `build/qtbase-switch/`

Build:

```bash
./scripts/build-qtbase-switch.sh
```

Or build only the libraries needed by the demo inside the devkitPro container:

```bash
docker run --rm \
  -v "$(pwd):$(pwd)" \
  -w "$(pwd)/build/qtbase-switch" \
  devkitpro/devkita64@sha256:1fc388c3a0d34bd2045a6dadcb1020e069d5f876a187fd705de14b4440c00282 \
  bash -lc 'cmake --build "$(pwd)" --parallel "$(nproc)" --target Core Gui Network Widgets OpenGL OpenGLWidgets plugins/platforms/libqswitch.a plugins/tls/libqopensslbackend.a'
```

Do not use a plain `cmake --build` here on a fresh tree. That asks Qt to build extra modules such as `QtTest`, and the Switch toolchain does not provide everything those optional pieces expect.

At minimum, the following outputs should exist:

- `lib/libQt6Core.a`
- `lib/libQt6Gui.a`
- `lib/libQt6Network.a`
- `lib/libQt6Widgets.a`
- `lib/libQt6OpenGL.a`
- `lib/libQt6OpenGLWidgets.a`
- `plugins/platforms/libqswitch.a`
- `plugins/tls/libqopensslbackend.a`

Those files are generated under:

- `build/qtbase-switch/`

## 4. Build the Widgets Demo

Use the wrapper script:

```bash
./scripts/build-widgets-probe.sh
```

Or run the container directly:

```bash
docker run --rm \
  -v "$(pwd):$(pwd)" \
  -w "$(pwd)/demo/widgets-app" \
  devkitpro/devkita64@sha256:1fc388c3a0d34bd2045a6dadcb1020e069d5f876a187fd705de14b4440c00282 \
  bash -lc 'make -j"$(nproc)" clean nro'
```

Expected outputs:

- `demo/widgets-app/qt6-switch-widgets-probe.elf`
- `demo/widgets-app/qt6-switch-widgets-probe.nro`
- `demo/widgets-app/qt6-switch-widgets-probe.nacp`

## 5. Build the Qt Quick Dependency Chain

Build host `qsb`:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/build-host-qtshadertools-tools.sh
```

Configure and build Switch QtShaderTools:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtshadertools-switch.sh

./scripts/build-qtshadertools-switch.sh
```

Build host QML tools:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/build-host-qtdeclarative-tools.sh
```

Configure and build Switch QtDeclarative:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtdeclarative-switch.sh

./scripts/build-qtdeclarative-switch.sh
```

Expected dependency outputs:

- `build/qtshadertools-host/bin/qsb`
- `build/qtshadertools-switch/lib/libQt6ShaderTools.a`
- `build/qtdeclarative-switch/lib/libQt6Qml.a`
- `build/qtdeclarative-switch/lib/libQt6QmlModels.a`
- `build/qtdeclarative-switch/lib/libQt6Quick.a`

## 6. Build the Qt Quick Demo

```bash
./scripts/build-quick-probe.sh
```

Expected outputs:

- `demo/quick-app/qt6-switch-quick-probe.elf`
- `demo/quick-app/qt6-switch-quick-probe.nro`
- `demo/quick-app/qt6-switch-quick-probe.nacp`

## 7. Current Qt Quick Scope

The validated path today is:

```text
QtBase -> Widgets demo
QtBase -> QtShaderTools -> QtDeclarative/Quick -> Qt Quick demo
```

GitHub Actions checks out the three Qt submodules above plus the pinned Ryubing
submodule. QtDeclarative's nested `test262` testsuite is intentionally omitted
because the workflow builds products but does not run Qt's upstream test suite.

## 8. Build the pinned Ryubing source

```bash
./scripts/build-ryubing.sh
```

Scripts find .NET through `DOTNET`, `PATH`, or the optional sibling
`../tools/dotnet` installation. The build script temporarily applies the
tracked socket compatibility patch when the submodule is clean and restores
the clean source after the build.

Launch an interactive demo with the locally built emulator:

```bash
./scripts/launch-qt-demo-ryubing.sh demo/widgets-app/qt6-switch-widgets-probe.nro
```

## 9. Run the Qt Module Test

Build the compact module smoke-test application:

```bash
./scripts/build-qt-module-test.sh
```

Expected output:

- `demo/qt-module-test/qt6-switch-module-test.elf`
- `demo/qt-module-test/qt6-switch-module-test.nro`

Run it in the locally built Ryubing:

```bash
./scripts/run-qt-module-test-ryubing.sh
```

The application tests QtCore, QtThreads, QtGui and QtWidgets/QPA on-screen.
QtNetwork has its own standalone probe so module-test failures remain isolated;
see `demo/qt-network-test/README.md`.

The network probe is run the same way:

```bash
./scripts/run-qt-network-test-ryubing.sh
```

## 10. Upload to Real Hardware Over FTP

```bash
./scripts/upload-widgets-probe-ftp.sh 192.168.1.6 5000
```

The current setup uploads:

- `/switch/qt6-switch-widgets-probe.nro`
