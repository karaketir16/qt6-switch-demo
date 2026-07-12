# Astris Testing

This document records the emulator workflow that was actually used to validate the demo.

## Verified Emulator Path

```text
/Volumes/T7/Applications/Astris/Astris.app
```

## Verified Data Directory

```text
/Volumes/T7/astrisData
```

## Run Command

```bash
/Volumes/T7/qt6-switch-demo/scripts/run-qt6-switch-widgets-probe-astris.sh
```

That script:

- copies the latest `.nro` into Astris homebrew storage
- removes the previous guest trace log
- launches Astris
- prints the latest Ryujinx log tail
- prints the guest-side trace file if present

## Logs to Check

### Host Log

Astris writes Ryujinx logs here:

```text
~/Library/Containers/V380-Ori.Astris/Data/Library/Logs/Ryujinx/
```

### Guest Trace

The demo writes a guest-side trace file here:

```text
/Volumes/T7/astrisData/sdcard/qt6-switch-widgets-probe.log
```

## What a Good Run Looks Like

The following signals were observed during successful emulator runs:

- `main: QApplication constructed`
- `ProbeWidget: showEvent`
- `ProbeWidget: first timer timeout`
- repeated `QSwitchBackingStore::flush`
- repeated `presentImage: framebufferBegin ok`
- repeated `presentImage: framebufferEnd ok`

## Visual Verification

A successful run should show:

- the Qt Widgets title text
- explanatory body text
- three buttons
- a large frame counter area
- a changing background color

Latest local verification screenshot:

- `/Volumes/T7/qt6-switch-demo/verification/astris-widgets-demo-embedded-font.png`

## Input Verification

Input was verified in Astris by observing:

- button press and release behavior
- `clicked` signal behavior
- guest trace lines from `ProbeWidget::keyPressEvent`
