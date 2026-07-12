# Development Environment

This document describes the environment that was used to bring up the demo and what must exist before you start building.

## Host Machine

The verified workflow was run from macOS, but the repository is now structured so it does not depend on a machine-specific mount point.

Recommended prerequisites:

- `git`
- `docker`
- `cmake`
- `ninja`
- `gh` (optional, only needed for GitHub publishing)

## Directory Layout

The repository now assumes this internal layout:

```text
qt6-switch-demo/
  third_party/
    qtbase/
  build/
    qtbase-host/
    qtbase-switch/
  demo/
  scripts/
  docs/
```

Astris paths are provided via environment variables instead of hardcoded machine-local paths.

## Required Tools

### Qt Base Source Checkout

Use the included git submodule:

```bash
git submodule update --init --recursive
```

### Docker Container for Switch Cross-Building

Pull the build image:

```bash
docker pull devkitpro/devkita64:latest
```

### Astris

Install Astris separately, then provide the path at runtime:

```bash
export ASTRIS_APP="/path/to/Astris.app"
export ASTRIS_DATA="/path/to/astrisData"
```

### GitHub CLI

Only required if you want to publish this repo from the command line:

```bash
gh auth status
```

## Why the Workflow Uses Docker

The Switch-targeting Qt build was verified inside the `devkitpro/devkita64` container rather than on the host directly. This keeps the Switch toolchain and portlibs consistent and avoids host-specific package drift.
