# Development Environment

This document describes the environment that was used to bring up the demo and what must exist before you start building.

## Host Machine

The verified workflow was run from macOS. Repository scripts derive paths from
the checkout and do not depend on a machine-specific mount point.

Recommended prerequisites:

- `git`
- `docker`
- `cmake`
- `ninja`
- .NET SDK 10.0.301

## Directory Layout

The repository now assumes this internal layout:

```text
qt6-switch-demo/
  third_party/
    qtbase/
    qtdeclarative/
    qtshadertools/
    ryubing/
  build/
    qtbase-host/
    qtbase-switch/
  demo/
  scripts/
  docs/
```

## Required Tools

### Qt Base Source Checkout

Use the included git submodule:

```bash
git submodule update --init --recursive
```

### Docker Container for Switch Cross-Building

Pull the build image:

```bash
docker pull devkitpro/devkita64@sha256:1fc388c3a0d34bd2045a6dadcb1020e069d5f876a187fd705de14b4440c00282
```

### Ryubing

Ryubing is built from the pinned source submodule. Scripts check `DOTNET`,
`PATH`, and the optional sibling installation at `../tools/dotnet`. Set
`DOTNET` explicitly when the SDK is elsewhere:

```bash
export DOTNET="/path/to/dotnet"
./scripts/build-ryubing.sh
```

`RYUBING_SDCARD` is optional and defaults to Ryubing's normal macOS SD-card
directory.

## Why the Workflow Uses Docker

The Switch-targeting Qt build runs in the digest-pinned devkitA64 container
rather than on the host directly. This keeps the Switch toolchain and portlibs
consistent and avoids host-specific package drift.
