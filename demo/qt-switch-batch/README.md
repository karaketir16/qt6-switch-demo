# Switch QtTest batches

This directory uses `cmake/QtSwitchBatch.cmake`, not Qt's global
`QT_BUILD_TESTS_BATCHED` graph. Each manifest entry is compiled into one
translation unit with its own `BATCHED_TEST_NAME`, and each manifest produces
one executable.

The QtDeclarative batch embeds its static QtQml, QtQml.Models, and
QtQml.WorkerScript plugins plus their resources. It does **not** load a QML
import tree from `sdmc:`. This explicit wiring is only needed because this is
a custom QtTest executable which bypasses Qt's normal executable finalizer.

For a regular Switch application, use the normal Qt 6 CMake path:

```cmake
qt_add_executable(my_app main.cpp)
qt_add_qml_module(my_app URI My.App VERSION 1.0 QML_FILES Main.qml)
```

Qt's executable finalizer invokes the QML import scanner, links every static
plugin required by the application's actual imports, and generates the plugin
import code. Therefore the app author neither writes `Q_IMPORT_QML_PLUGIN`
nor deploys `QtQml` plugins to the SD card. Avoid setting
`QT_SKIP_AUTO_QML_PLUGIN_INCLUSION` or `QT_QML_MODULE_NO_IMPORT_SCAN` for a
normal application: those are explicit expert opt-outs from this guarantee.

Prerequisites are recorded build outputs, not ad-hoc commands:

- `build/qtbase-host`: native Qt host tools (`moc`, `rcc`, `uic`)
- `build/qtbase-test-host`: native QtTest library when QtDeclarative's
  `qmltestrunner` host tool is required
- `build/qtbase-switch-clean`: target QtBase build tree containing Core and Test;
  QtBase must be configured with `FEATURE_batch_test_support=ON`
- `build/qtbase-switch`: target QtBase CMake helper overlay
- `build/qtdeclarative-switch`: target QtDeclarative prefix, when the QML batch is enabled

The documented build entry point is:

```sh
./scripts/build-qt-switch-batches.sh
```

When using uninstalled build-tree packages, set `QTBASE_BUILD_DIR` to the
QtBase build tree and leave `QTBASE_PREFIX`/`QT_CMAKE_MODULE_PATH` pointing at
the helper overlay that contains `QtFindWrapHelper.cmake`.
The build script generates a minimal `Qt6QmlTools` package overlay because the
custom batch does not invoke QML host tools.

Set `BUILD_QTDECLARATIVE=ON` only after the QtDeclarative prefix exists.
The script configures and builds the batch project; it does not silently build
Qt modules or create one executable per test.

The runner keeps one NRO per batch manifest and invokes registered QtTest
entries in that NRO. Cases may be selected by the first command-line argument
or by the text file `sdmc:/qt-switch-batch-case.txt`; `--all` runs every entry
in the manifest in one launch. Each invocation appends `BEGIN`, `PASS`, or
`FAIL` to `sdmc:/qt-switch-batch.log`. The selector may contain additional
QtTest arguments after the case name.

The scripted form is:

```sh
./scripts/run-qt-switch-batch-case-ryubing.sh \
    build/qt-switch-batch/nro/qtbase_tests_batch.nro qbytearray
```

Add a third argument such as `55561` to enable the Ryubing GDB stub. The
script waits for the case result, has no wall-clock timeout, and can be
interrupted for GDB inspection.

Current executed results and exclusions are maintained in
`docs/batch-test-status.md`; that table, rather than this usage document, is
the authoritative test-status record.

The Switch QtDeclarative configuration sets `QT_SWITCH_SKIP_QMLTESTRUNNER=ON`.
That standalone host runner is unnecessary here: the QML batch uses the same
in-process Qt batch runner as the QtBase batch, avoiding the full upstream
Quick Controls test graph.

For GDB diagnosis, use the target emulator/device launch command documented in
the repository's Switch skill and attach GDB to the one batch executable. Do
not use a wall-clock timeout to decide that a test is stuck; capture a GDB
backtrace and record the command/output in the build log.

On macOS/Ryubing, the current GDB-stub result is a tooling limitation rather
than a test failure: the stub maps the NRO and permits memory inspection, but
its `continue` path remains at the NRO entry point while logging unsupported
`qTStatus`, `qOffsets`, and `qSymbol` packets plus AppleHv `GetX` errors. This
must be resolved before claiming an in-emulator pass; it is not treated as a
test hang.

Package all built batch ELFs with:

```sh
./scripts/package-qt-switch-batches.sh
```

The package script discovers all `*_tests_batch` targets, so extending the
manifests does not create a separate per-test NRO.
