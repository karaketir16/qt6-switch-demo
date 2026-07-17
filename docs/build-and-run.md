# Build and Run

This document covers the current working build flow for the submodule-based Qt tree and the Switch demos.

Unless noted otherwise, run the wrapper scripts from the repository root.

## 1. Clone With the Patched Submodules

The repository tracks forked Qt submodules because the current Switch work depends on commits that do not exist in upstream `code.qt.io`.

Fresh clone:

```bash
git clone --recurse-submodules \
  -b codex/qtquick-main-ready \
  https://github.com/karaketir16/qt6-switch-demo.git
```

If the submodules were not initialized during clone:

```bash
git submodule update --init --recursive
```

To refresh them later:

```bash
git submodule update --remote --recursive
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
  devkitpro/devkita64 \
  bash -lc 'cmake --build "$(pwd)" --parallel "$(nproc)" --target Core Gui Widgets plugins/platforms/libqswitch.a'
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
  devkitpro/devkita64 \
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

The manual GitHub Actions workflow checks out only the three Qt submodules above.

## 8. Run in Astris

```bash
ASTRIS_APP="/path/to/Astris.app" \
ASTRIS_DATA="/path/to/astrisData" \
./scripts/run-qt6-switch-widgets-probe-astris.sh
```

For the Qt Quick demo:

```bash
ASTRIS_APP="/path/to/Astris.app" \
ASTRIS_DATA="/path/to/astrisData" \
./scripts/run-qt6-switch-quick-probe-astris.sh
```

## 9. Upload to Real Hardware Over FTP

```bash
./scripts/upload-widgets-probe-ftp.sh 192.168.1.6 5000
```

The current setup uploads:

- `/switch/qt6-switch-widgets-probe.nro`
