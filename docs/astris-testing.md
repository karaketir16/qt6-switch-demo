# Astris Testing

This document records the emulator workflow that was actually used to validate the demo.

## Verified Emulator Path

Set through `ASTRIS_APP`.

## Verified Data Directory

Set through `ASTRIS_DATA`.

## Run Command

```bash
ASTRIS_APP="/path/to/Astris.app" \
ASTRIS_DATA="/path/to/astrisData" \
./scripts/run-qt6-switch-widgets-probe-astris.sh
```

To enable guest-side Switch trace logs for debugging:

```bash
QT_SWITCH_DEBUG_LOG=1 \
ASTRIS_APP="/path/to/Astris.app" \
ASTRIS_DATA="/path/to/astrisData" \
./scripts/run-qt6-switch-widgets-probe-astris.sh
```

That script:

- reads `demo/widgets-app/qt6-switch-widgets-probe.nro`
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

When `QT_SWITCH_DEBUG_LOG=1` is set, the widgets demo writes a guest-side trace file here:

```text
$ASTRIS_DATA/sdcard/qt6-switch-widgets-probe.log
```

The Quick probe writes the following file when trace logging is enabled:

```text
$ASTRIS_DATA/sdcard/qt6-switch-quick-probe.log
```

The Switch Qt platform plugin also writes here when debug logging is enabled:

```text
$ASTRIS_DATA/sdcard/qt6-switch-probe.log
```

The Qt libraries use the same runtime toggle. The launcher creates
`$ASTRIS_DATA/sdcard/qt6-switch-debug` when `QT_SWITCH_DEBUG_LOG=1` is set.
Remove that marker, or run without the variable, to keep tracing disabled.
The marker is checked once per process, so changing it takes effect on the
next launch and does not require rebuilding Qt.

Low-cost startup breadcrumbs are always written to
`$ASTRIS_DATA/sdcard/qt6-switch-startup.log`; verbose trace output remains
marker-gated.

## What a Good Run Looks Like

With `QT_SWITCH_DEBUG_LOG=1`, the following signals were observed during successful emulator runs:

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

- `verification/astris-widgets-demo-menu.png`

## Input Verification

Input was verified in Astris by observing:

- button press and release behavior
- `clicked` signal behavior
- guest trace lines from `ProbeWidget::keyPressEvent`

## Ryubing / Ryujinx: QtNetwork Test

For the network test, use the direct Ryubing runner rather than Astris. It
stops a prior Ryubing process, stages the CA bundle, waits for the final test
summary, and returns non-zero for a failed probe.

```bash
./scripts/run-qt-network-test-ryubing.sh
```

Ryubing maps `sdmc:/` to:

```text
~/Library/Application Support/Ryujinx/sdcard/
```

For the native Qt HTTPS test, place a PEM root bundle at:

```text
~/Library/Application Support/Ryujinx/sdcard/qt6-switch-ca-bundle.pem
```

Then inspect these files after the run:

```text
~/Library/Application Support/Ryujinx/sdcard/qt6-switch-network-test.log
~/Library/Application Support/Ryujinx/sdcard/qt6-switch-probe.log
~/Library/Logs/Ryujinx/Ryujinx_*.log
```

Expected TLS setup evidence in `qt6-switch-probe.log` is:

```text
[tls-ossl] RAND_status=1
[tls-ossl] CA bundle path=sdmc:/qt6-switch-ca-bundle.pem exists=1 certificates=<positive count>
```

Ryubing can still report `Invalid socket descriptor` for Qt TCP/UDP/HTTP(S)
despite correct TLS initialization and a loaded CA bundle. Treat that as an
emulator BSD socket-service limitation, not as a native Switch result.
