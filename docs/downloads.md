# Downloads

This document records what was downloaded or pulled during development, where it came from, and how it was used.

## Primary Sources

### Qt Base Source

- Upstream source: `https://code.qt.io/qt/qtbase.git`
- GitHub mirror used for the fork workflow: `https://github.com/qt/qtbase`
- Fork used by this repository: `https://github.com/karaketir16/qtbase`
- Branch used by the submodule: `qt6-switch-demo-v6.8.3`
- Base upstream tag used for the fork branch: `v6.8.3`

This repository includes the forked Qt source as a git submodule at `third_party/qtbase`.

### devkitPro Switch Build Container

- Source: [Docker Hub: `devkitpro/devkita64`](https://hub.docker.com/r/devkitpro/devkita64)
- Local image used during development: `devkitpro/devkita64:latest`
- Local image ID seen during packaging: `sha256:146025c8997f3effc12ad2e2acb8633b1a167d98bb6a551cbd6760f2aac3c892`

This container provides the Switch-targeting toolchain and libraries used for cross-compiling Qt and the demo application.

### Astris Emulator

- Used during verification, but not redistributed by this repository.
- Release page: [Astris.Binaries releases](https://github.com/V380-Ori/Astris.Binaries/releases)
- Runtime path is configured through `ASTRIS_APP`.

This repository documents the Astris workflow that was used, but it does not redistribute Astris itself.

### Embedded Demo Font

- Font file embedded into the Switch plugin: `DejaVuSans.ttf`
- Source location inside the Qt tree:
  `third_party/qtbase/src/3rdparty/wasm/DejaVuSans.ttf`

The final setup embeds the font into the Switch Qt runtime used by the demo.

## What This Repository Does Not Download Automatically

This repository does not automatically download:

- Astris
- a fresh Qt checkout
- Docker Desktop
- GitHub CLI

Those tools are expected to be installed separately.
