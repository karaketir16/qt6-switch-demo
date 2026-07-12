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

- `verification/astris-widgets-demo-embedded-font.png`

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

1. Read `docs/downloads.md`.
2. Prepare the environment from `docs/development-environment.md`.
3. Initialize the submodule or apply the patch series as described in `docs/build-and-run.md`.
4. Build host tools and configure Qt as described in `docs/build-and-run.md`.
5. Verify the result in Astris using `docs/astris-testing.md`.

## Verified Local Paths Used During Development

Default repo-relative locations:

- Qt source submodule: `third_party/qtbase`
- host tools build: `build/qtbase-host`
- Switch target build: `build/qtbase-switch`
- verification artifacts: `verification/`

Astris paths are intentionally environment-driven:

- `ASTRIS_APP`
- `ASTRIS_DATA`

## Patch Series

The exported patch series in `patches/` currently contains:

1. `0001-feat-add-switch-qtbase-port-patches.patch`
2. `0002-feat-bring-up-switch-qt-runtime.patch`
3. `0003-feat-support-qtwidgets-on-switch.patch`
4. `0004-feat-embed-switch-default-font.patch`

See `docs/what-changed.md` for the higher-level summary.
