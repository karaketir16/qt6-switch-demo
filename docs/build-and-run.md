# Build and Run

This document covers the full build flow for the submodule-based Qt tree and the demo application.

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

## 3. Configure the Switch Qt Target

For the Widgets demo, configure with widgets enabled:

```bash
QT_HOST_PATH="$(pwd)/build/qtbase-host" \
./scripts/configure-qtbase-switch.sh
```

## 4. Build the Switch Qt Pieces

Build inside the devkitPro container:

```bash
docker run --rm \
  -v "$(pwd):$(pwd)" \
  -w "$(pwd)/build/qtbase-switch" \
  devkitpro/devkita64 \
  bash -lc 'cmake --build "$(pwd)" --parallel "$(nproc)"'
```

At minimum, the following outputs should exist:

- `lib/libQt6Core.a`
- `lib/libQt6Gui.a`
- `lib/libQt6Widgets.a`
- `plugins/platforms/libqswitch.a`

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

- `qt6-switch-widgets-probe.elf`
- `qt6-switch-widgets-probe.nro`

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
