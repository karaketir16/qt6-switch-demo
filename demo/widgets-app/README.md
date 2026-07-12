# Widgets Demo

This directory contains the standalone Qt Widgets demo homebrew application used by this repository.

## Purpose

The demo is intentionally simple and is meant to prove that the current Switch Qt port can:

- start `QApplication`
- create and show a top-level `QWidget`
- render labels and buttons
- update the screen through a timer
- receive input events through the custom Switch QPA path

## Build

Use the wrapper script from the repository root:

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

## Outputs

- `qt6-switch-widgets-probe.elf`
- `qt6-switch-widgets-probe.nro`

## Runtime Notes

- The current plugin embeds a default font.
- No separate `qt6-switch-font.ttf` file is required for this demo anymore.
