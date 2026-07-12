# Downloads

This document records what was downloaded or pulled during development, where it came from, and how it was used.

## Primary Sources

### Qt Base Source

- Source: `https://code.qt.io/qt/qtbase.git`
- Local checkout used during development: `/Volumes/T7/sdk/qt6-switch-src/qtbase`
- Base version used for the patch series: `v6.8.3`

This repository does not vendor the full Qt source tree. Instead, it ships a patch series that can be applied to a clean `qtbase` checkout.

### devkitPro Switch Build Container

- Source: [Docker Hub: `devkitpro/devkita64`](https://hub.docker.com/r/devkitpro/devkita64)
- Local image used during development: `devkitpro/devkita64:latest`
- Local image ID seen during packaging: `sha256:146025c8997f3effc12ad2e2acb8633b1a167d98bb6a551cbd6760f2aac3c892`

This container provides the Switch-targeting toolchain and libraries used for cross-compiling Qt and the demo application.

### Astris Emulator

- Local app path used during verification: `/Volumes/T7/Applications/Astris/Astris.app`

This repository documents the Astris workflow that was used, but it does not redistribute Astris itself.

### Embedded Demo Font

- Font file embedded into the Switch plugin: `DejaVuSans.ttf`
- Source location inside the Qt tree:
  `/Volumes/T7/sdk/qt6-switch-src/qtbase/src/3rdparty/wasm/DejaVuSans.ttf`

The final setup embeds the font into the Switch Qt runtime, so there is no longer a separate SD card font deployment step for the demo.

## What This Repository Does Not Download Automatically

This repository does not automatically download:

- Astris
- a fresh Qt checkout
- Docker Desktop
- GitHub CLI

Those tools are expected to be installed separately.

