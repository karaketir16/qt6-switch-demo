# What Changed

This document summarizes the key changes made to Qt and the demo during the bring-up.

## High-Level Milestones

1. Add a Nintendo Switch device target and Switch platform plugin scaffolding for `qtbase`.
2. Bring up the runtime enough for framebuffer presentation and event dispatching.
3. Support a Qt Widgets demo build and fix a Switch-specific link issue in `QFileDialog`.
4. Embed a default font into the Switch plugin.

## Qt Runtime Changes

The Switch plugin work includes:

- custom `switch` QPA plugin
- Switch screen definition
- raster backing store path
- `libnx` framebuffer presentation
- button-to-Qt-key mapping
- event pumping during backing store flush

## Qt Widgets Changes

The Qt Widgets bring-up required a Switch-specific adjustment in:

- `src/widgets/dialogs/qfiledialog.cpp`

This avoids relying on `getpwnam` for `~user` expansion in a Switch/libnx environment where that symbol is not available.

## Font Handling Change

The initial working version loaded a TTF file from:

```text
sdmc:/qt6-switch-font.ttf
```

The final version embeds `DejaVuSans.ttf` directly into the Switch plugin resources and initializes those resources from the static plugin entry point.

That change removes a manual deployment step and makes the demo easier to share and test.
