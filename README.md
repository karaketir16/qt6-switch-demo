# Qt 6 Nintendo Switch Demo

This repository packages a reproducible Qt 6 Nintendo Switch bring-up demo based on a patched `qtbase` tree, a small Qt Widgets homebrew application, and the scripts and documentation needed to build, test, and run it.

## Status

Current verified state:

- A patched Qt 6 `qtbase` can be cross-built for Nintendo Switch.
- The custom `switch` QPA plugin renders a Qt Widgets application.
- The demo application runs in Astris.
- Input works in the Astris demo.
- The default font is embedded in the Switch Qt runtime, so the demo no longer requires a separate `qt6-switch-font.ttf` file.

Latest local Astris verification artifact:

- `/Volumes/T7/qt6-switch-demo/verification/astris-widgets-demo-embedded-font.png`

This is still a bring-up demo, not a full upstream-ready Qt for Switch port.

## Repository Layout

- `demo/widgets-app/`
  Qt Widgets demo homebrew app.
- `scripts/`
  Helper scripts for patching Qt, building host tools, configuring the Switch target, running in Astris, and uploading to real hardware over FTP.
- `patches/`
  `git format-patch` output exported from the patched `qtbase` branch.
- `extras/`
  Supporting files such as the Switch CMake toolchain file.
- `docs/`
  Detailed setup, build, test, emulator, and change documentation.

## Quick Start

1. Read [downloads.md](/Volumes/T7/qt6-switch-demo/docs/downloads.md).
2. Prepare the environment from [development-environment.md](/Volumes/T7/qt6-switch-demo/docs/development-environment.md).
3. Apply the Qt patch series with [apply-qtbase-patches.sh](/Volumes/T7/qt6-switch-demo/scripts/apply-qtbase-patches.sh).
4. Build host tools and configure Qt as described in [build-and-run.md](/Volumes/T7/qt6-switch-demo/docs/build-and-run.md).
5. Verify the result in Astris using [astris-testing.md](/Volumes/T7/qt6-switch-demo/docs/astris-testing.md).

## Verified Local Paths Used During Development

These are the local paths that were actually used during bring-up:

- Qt source checkout: `/Volumes/T7/sdk/qt6-switch-src/qtbase`
- Qt host tools build: `/Volumes/T7/sdk/qt6-host-build-linux/qtbase`
- Qt Switch target build: `/Volumes/T7/sdk/qt6-switch-build/qtbase-widgets-test`
- Astris app: `/Volumes/T7/Applications/Astris/Astris.app`
- Astris data directory: `/Volumes/T7/astrisData`

## Patch Series

The exported patch series in `patches/` currently contains:

1. `0001-feat-add-switch-qtbase-port-patches.patch`
2. `0002-feat-bring-up-switch-qt-runtime.patch`
3. `0003-feat-support-qtwidgets-on-switch.patch`
4. `0004-feat-embed-switch-default-font.patch`

See [what-changed.md](/Volumes/T7/qt6-switch-demo/docs/what-changed.md) for the higher-level summary.
