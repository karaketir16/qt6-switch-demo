# Build and Run

This document covers the full build flow for the patched Qt tree and the demo application.

## 1. Apply the Patch Series

If you want to patch a separate clean checkout instead of using the included submodule:

```bash
./scripts/apply-qtbase-patches.sh /path/to/qtbase ./patches
```

If you use the included submodule, this step is already represented by `third_party/qtbase`.

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
  bash -lc 'cmake --build "$(pwd)" --parallel 4'
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
  make clean nro
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

Because the font is now embedded, there is no longer a separate font file to deploy.
