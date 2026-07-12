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

## Outputs

When built through the repository wrapper script, the output files are written here:

- `demo/widgets-app/qt6-switch-widgets-probe.elf`
- `demo/widgets-app/qt6-switch-widgets-probe.nro`
- `demo/widgets-app/qt6-switch-widgets-probe.nacp`

If you `cd demo/widgets-app` and run `make` directly, the same files are written in that current directory.

## Runtime Notes

- The current plugin embeds a default font.
