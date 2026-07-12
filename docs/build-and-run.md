# Build and Run

This document covers the full build flow for the patched Qt tree and the demo application.

## 1. Apply the Patch Series

From a clean `qtbase` checkout at `v6.8.3`:

```bash
/Volumes/T7/qt6-switch-demo/scripts/apply-qtbase-patches.sh \
  /Volumes/T7/sdk/qt6-switch-src/qtbase \
  /Volumes/T7/qt6-switch-demo/patches
```

## 2. Build Host Tools

The Switch cross-build requires host-side Qt tools first.

```bash
/Volumes/T7/qt6-switch-demo/scripts/build-host-qt-tools.sh \
  /Volumes/T7/sdk/qt6-switch-src/qtbase \
  /Volumes/T7/sdk/qt6-host-build-linux/qtbase \
  /Volumes/T7/sdk/qt6-host-build-linux/install
```

## 3. Configure the Switch Qt Target

For the Widgets demo, configure with widgets enabled:

```bash
QT_HOST_PATH=/Volumes/T7/sdk/qt6-host-build-linux/qtbase \
/Volumes/T7/qt6-switch-demo/scripts/configure-qtbase-switch.sh \
  /Volumes/T7/sdk/qt6-switch-src/qtbase \
  /Volumes/T7/sdk/qt6-switch-build/qtbase-widgets-test \
  /Volumes/T7/qt6-switch-demo/extras/toolchain-switch.cmake \
  /Volumes/T7/sdk/qt6-host-build-linux/qtbase \
  ON
```

## 4. Build the Switch Qt Pieces

Build inside the devkitPro container:

```bash
docker run --rm \
  -v /Volumes/T7:/Volumes/T7 \
  -w /Volumes/T7/sdk/qt6-switch-build/qtbase-widgets-test \
  devkitpro/devkita64 \
  bash -lc 'cmake --build /Volumes/T7/sdk/qt6-switch-build/qtbase-widgets-test --parallel 4'
```

At minimum, the following outputs should exist:

- `lib/libQt6Core.a`
- `lib/libQt6Gui.a`
- `lib/libQt6Widgets.a`
- `plugins/platforms/libqswitch.a`

## 5. Build the Demo Application

Use the wrapper script:

```bash
/Volumes/T7/qt6-switch-demo/scripts/build-widgets-probe.sh
```

Or run the container directly:

```bash
docker run --rm \
  -v /Volumes/T7:/Volumes/T7 \
  -w /Volumes/T7/qt6-switch-demo/demo/widgets-app \
  devkitpro/devkita64 \
  make clean nro
```

Expected outputs:

- `qt6-switch-widgets-probe.elf`
- `qt6-switch-widgets-probe.nro`

## 6. Run in Astris

```bash
/Volumes/T7/qt6-switch-demo/scripts/run-qt6-switch-widgets-probe-astris.sh
```

## 7. Upload to Real Hardware Over FTP

```bash
/Volumes/T7/qt6-switch-demo/scripts/upload-widgets-probe-ftp.sh \
  192.168.1.6 5000
```

The current setup uploads:

- `/switch/qt6-switch-widgets-probe.nro`

Because the font is now embedded, there is no longer a separate font file to deploy.

