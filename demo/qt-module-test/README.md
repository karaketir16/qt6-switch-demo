# Qt Module Test

This is a small, on-device Qt smoke test for Nintendo Switch. It shows a
pass/fail result on screen instead of silently relying on a build succeeding.

## Tests included

- **QtCore:** JSON, regular expressions, byte arrays and `QTimer`
- **QtThreads:** code execution on a worker `QThread`
- **QtGui:** `QImage`, `QPainter` and pixel readback
- **QtWidgets/QPA:** `QWidget`, layouts, native Switch window creation and input

Build and run it from the repository root:

```bash
./scripts/build-qt-module-test.sh
./scripts/run-qt6-switch-module-test-astris.sh
```

For repeatable automation, pass `--batch`: the probe exits with status 0 only
when every enabled module group passes. This keeps the same Switch-native
checks usable from a launcher or CI harness without relying on screen reading.

QtNetwork is tested separately by `demo/qt-network-test`; this keeps the
module smoke test focused and makes network failures unambiguous.
