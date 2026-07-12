# Build and Run

This document covers the full build flow for the submodule-based Qt tree and the demo application.

Unless noted otherwise, run the wrapper scripts from the repository root.

## 1. Pull the Submodule

After cloning the repository:

```bash
git submodule update --init --recursive
```

To update the submodule later:

```bash
git submodule update --remote --recursive
```

## 2. Build Host Tools

The Switch cross-build requires host-side Qt tools first.

```bash
./scripts/build-host-qt-tools.sh
```

Output directory:

- `build/qtbase-host/`

## 3. Configure the Switch Qt Target

For the Widgets demo, configure with widgets enabled:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtbase-switch.sh
```

Configuration output directory:

- `build/qtbase-switch/`

## 4. Build the Switch Qt Pieces

Use the wrapper script:

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
- `lib/libQt6Widgets.a`
- `plugins/platforms/libqswitch.a`

Those files are generated under:

- `build/qtbase-switch/`

## 5. Build the Demo Application

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

## 6. Run in Astris

```bash
ASTRIS_APP="/path/to/Astris.app" \
ASTRIS_DATA="/path/to/astrisData" \
./scripts/run-qt6-switch-widgets-probe-astris.sh
```

## 7. Upload to Real Hardware Over FTP

```bash
./scripts/upload-widgets-probe-ftp.sh 192.168.1.6 5000
```

The current setup uploads:

- `/switch/qt6-switch-widgets-probe.nro`
