# Development Environment

This document describes the environment that was used to bring up the demo and what must exist before you start building.

## Host Machine

The verified workflow was run from macOS with a writable `/Volumes/T7` external volume used for source checkouts, build directories, emulator data, and helper tools.

Recommended prerequisites:

- `git`
- `docker`
- `cmake`
- `ninja`
- `gh` (optional, only needed for GitHub publishing)

## Directory Layout

The following layout was used during development:

```text
/Volumes/T7/
  Applications/
    Astris/
      Astris.app
  astrisData/
  sdk/
    qt6-switch-src/
      qtbase/
    qt6-host-build-linux/
      qtbase/
    qt6-switch-build/
      qtbase-widgets-test/
  qt6-switch-demo/
```

You can change these paths, but the provided scripts assume this layout by default.

## Required Tools

### Qt Base Source Checkout

Clone the official repository:

```bash
git clone https://code.qt.io/qt/qtbase.git /Volumes/T7/sdk/qt6-switch-src/qtbase
cd /Volumes/T7/sdk/qt6-switch-src/qtbase
git checkout v6.8.3
```

### Docker Container for Switch Cross-Building

Pull the build image:

```bash
docker pull devkitpro/devkita64:latest
```

### Astris

Install Astris separately, then make sure the verified app path exists:

```bash
/Volumes/T7/Applications/Astris/Astris.app
```

### GitHub CLI

Only required if you want to publish this repo from the command line:

```bash
gh auth status
```

## Why the Workflow Uses Docker

The Switch-targeting Qt build was verified inside the `devkitpro/devkita64` container rather than on the host directly. This keeps the Switch toolchain and portlibs consistent and avoids host-specific package drift.

