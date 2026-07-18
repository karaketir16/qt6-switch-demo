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
- Image digest used by the scripts:
  `devkitpro/devkita64@sha256:1fc388c3a0d34bd2045a6dadcb1020e069d5f876a187fd705de14b4440c00282`

This container provides the Switch-targeting toolchain and libraries used for cross-compiling Qt and the demo application.

### Ryubing

- Source: `third_party/ryubing`
- Pinned revision: `a82350bb774f70fcbd41c9987bf67a3775409963`
- Required SDK: .NET 10.0.301

Only this pinned source build is supported by the runners.

### OpenSSL and CA certificates

- OpenSSL 3.0.16 archive SHA-256:
  `57e03c50feab5d31b152af2b764f10379aecd8ee92f16c985983ce4a99f7ef86`
- The standalone network probe embeds the curl Mozilla CA extract documented in
  `demo/qt-network-test/assets/README.md`.

### Embedded Demo Font

- Font file embedded into the Switch plugin: `DejaVuSans.ttf`
- Source location inside the Qt tree:
  `third_party/qtbase/src/3rdparty/wasm/DejaVuSans.ttf`

The final setup embeds the font into the Switch Qt runtime used by the demo.

## What This Repository Does Not Download Automatically

This repository does not automatically download:

- a separate emulator binary
- Docker Desktop
- GitHub CLI

Those tools are expected to be installed separately.
