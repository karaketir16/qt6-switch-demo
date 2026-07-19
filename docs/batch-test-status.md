# Switch Qt batch test status

This table records guest-runtime evidence from the repository-built Ryubing
binary. A batch is one NRO containing several QtTest sources; its runner calls
the registered test entries sequentially in a single guest process.

The latest clean regression run rebuilt the batch project, then reran every
listed QtBase batch except the known `qmutex` failure path and every current
QtDeclarative batch. Every selected batch completed with only `PASS` markers;
the launcher closed its own Ryubing process after each NRO.

## QML plugin deployment invariant

Normal Switch applications built with `qt_add_executable()` and
`qt_add_qml_module()` use Qt's executable finalizer and QML import scanner.
For every detected QML import, that finalizer links the corresponding static
plugin and generates its import code into the NRO. No `sdmc:` QML import tree,
`QML2_IMPORT_PATH`, or application-authored `Q_IMPORT_QML_PLUGIN` call is
required. `QT_SKIP_AUTO_QML_PLUGIN_INCLUSION` and
`QT_QML_MODULE_NO_IMPORT_SCAN` are explicit advanced opt-outs and must not be
set by normal Switch application templates.

The custom QtTest batch runner bypasses that regular CMake finalizer, so it
explicitly embeds the QtQml, QtQml.Models, and QtQml.WorkerScript static
plugins and their resources. The five-test QtDeclarative batch passed after
that complete embedding was added.

| Suite | Test source | Status | Evidence / current action |
| --- | --- | --- | --- |
| QtBase | `qhash` | PASS | Passed in the eight-source QtBase batch. |
| QtBase | `qdatetime` | PASS | Passed after QtCore Switch was changed from the UTC-only backend to the TZ-file backend and given the default `sdmc:/qt6-switch-zoneinfo` bundle location. |
| QtBase | `qbytearray` | PASS | Passed in the eight-source QtBase batch. |
| QtBase | `qstring` | PASS | Passed in the eight-source QtBase batch. |
| QtBase | `qchar` | PASS | Passed after its public `NormalizationTest.txt` test data was staged on the SD card. |
| QtBase | `qdate` | PASS | Passed in the eight-source QtBase batch. |
| QtBase | `qtime` | PASS | Passed in the eight-source QtBase batch. |
| QtBase | `qstringlist` | PASS | Passed in the eight-source QtBase batch. |
| QtBase | `qcontiguouscache` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qmath` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qstringtokenizer` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qbytearraylist` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qtyperevision` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qqueue` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qbitarray` | PASS | Passed in the 20-source QtBase Core batch after enabling exceptions only for this upstream test source, which catches `std::bad_alloc`. |
| QtBase | `qvarlengtharray` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qscopeguard` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qset` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qpair` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qelapsedtimer` | PASS | Passed in the 20-source QtBase Core batch. |
| QtBase | `qmap` | PASS | Passed in a separate one-source QtBase batch because its global `MyClass::count` test helper collides with `qhash` when both are linked into one executable. |
| QtBase | `qcache` | PASS | Passed in the extra QtBase batch; its global `Foo::count` helper collides with `qhash` in the primary executable. |
| QtBase | `qsharedpointer` | PASS | The isolated selector run now completes. The Switch QThread path creates joinable threads and joins a finished native thread once, closing its libnx handle; Ryubing records both the close and KThread destruction and no resource-limit event. |
| QtBase | `qmutex` | FAIL (Ryubing mutex scheduling) | The same-NRO verbose selector run passes 15 functions, including `stressTest()` (388,117 locks), then `moreStress()` times out waiting for its first worker at `tst_qmutex.cpp:1187`; QtTest reports 15 passed, 1 failed and then its expected running-QThread fatal. The earlier thread-resource exhaustion is absent. A second run with the Ryubing hypervisor disabled has the same applet exit, so it is in the shared guest mutex/scheduler path. |
| QtBase | `qreadwritelock` | PASS | A clean, isolated selector run in the existing thread-batch NRO logged `BEGIN`, `PASS`, and `COMPLETE`; Ryubing also logged matched close/destroy events across its stress loop. |
| QtBase | `qsemaphore` | PASS | Passed in the clean serialized thread batch before `qmutex`. |
| QtBase | `qwaitcondition` | PASS | Passed in the clean serialized thread batch before `qmutex`. |
| QtBase | `qdeadlinetimer` | PASS | Passed in the clean serialized thread batch before `qmutex`. |
| QtBase | `qscopedpointer` | PASS | Passed in the 28-source QtBase Core batch. |
| QtBase | `qsignalblocker` | PASS | Passed in the 28-source QtBase Core batch. |
| QtBase | `qcommandlineparser` | PASS | Passed in the 28-source QtBase Core batch. |
| QtBase | `qflatmap` | PASS | Passed in the 28-source QtBase Core batch. |
| QtBase | `qrect` | PASS | Passed in the 28-source QtBase Core batch. |
| QtBase | `qpoint` | PASS | Passed in the 28-source QtBase Core batch. |
| QtBase | `qline` | PASS | Passed in the 28-source QtBase Core batch. |
| QtBase | `qsize` | PASS | Passed in the 28-source QtBase Core batch. |
| QtBase | `qarraydata` | PASS | Passed in the 32-source QtBase Core batch; exceptions are enabled only for this upstream test source. |
| QtBase | `qflags` | PASS | Passed in the 32-source QtBase Core batch. |
| QtBase | `qoperatingsystemversion` | PASS | Passed in the 32-source QtBase Core batch. |
| QtBase | `qversionnumber` | PASS | Passed in the 32-source QtBase Core batch. |
| QtBase | `qrandomgenerator` | PASS | Passed in the 35-source QtBase Core batch. |
| QtBase | `qloggingcategory` | PASS | Passed in the 35-source QtBase Core batch. |
| QtBase | `qcollator` | PASS | Passed in the 35-source QtBase Core batch. |
| QtBase | `qnumeric` | PASS | Passed in the four-source Core misc batch. |
| QtBase | `qbytearraymatcher` | PASS | Passed in the four-source Core misc batch; exceptions are enabled only for this test source because it checks the >4GiB `bad_alloc` path. |
| QtBase | `qringbuffer` | PASS | Passed in the four-source Core misc batch. |
| QtBase | `qoffsetstringarray` | PASS | Passed in the four-source Core misc batch. |
| QtBase | `qglobalstatic` | PASS | Passed in the ten-source broad Core batch; exceptions are enabled only for this upstream test source. |
| QtBase | `qmetamethod` | PASS | Passed in the ten-source broad Core batch. |
| QtBase | `qsignalmapper` | PASS | Passed in the ten-source broad Core batch. |
| QtBase | `qatomicscopedvaluerollback` | PASS | Passed in the ten-source broad Core batch. |
| QtBase | `qalgorithms` | PASS | Passed in the ten-source broad Core batch. |
| QtBase | `qmargins` | PASS | Passed in the ten-source broad Core batch. |
| QtBase | `qtendian` | PASS | Passed in the ten-source broad Core batch. |
| QtBase | `qfloat16` | PASS | Passed in the ten-source broad Core batch. |
| QtBase | `qbuffer` | PASS | Passed in the ten-source broad Core batch. |
| QtBase | `qipaddress` | PASS | Passed in the ten-source broad Core batch. |
| QtBase | `qexplicitlyshareddatapointer` | PASS | Passed in the eleven-source value/Core utility batch. |
| QtBase | `qexplicitlyshareddatapointerv2` | PASS | Passed in the eleven-source value/Core utility batch. |
| QtBase | `qmakearray` | PASS | Passed in the eleven-source value/Core utility batch. |
| QtBase | `qscopedvaluerollback` | PASS | Passed in the eleven-source value/Core utility batch. |
| QtBase | `qtaggedpointer` | PASS | Passed in the eleven-source value/Core utility batch. |
| QtBase | `qduplicatetracker` | PASS | Passed in the eleven-source value/Core utility batch. |
| QtBase | `qstl` | PASS | Passed in the eleven-source value/Core utility batch. |
| QtBase | `q20_memory` | PASS | Passed in the eleven-source value/Core utility batch; exceptions are enabled only for this upstream test source. |
| QtBase | `qxp_function_ref` | PASS | Passed in the eleven-source value/Core utility batch; exceptions are enabled only for this upstream test source. |
| QtBase | `qxp_is_virtual_base_of` | PASS | Passed in the eleven-source value/Core utility batch; exceptions are enabled only for this upstream test source. |
| QtBase | `qfreelist` | PASS | Passed in the eleven-source value/Core utility batch. |
| QtBase | `qcborvalue_json` | PASS | Passed in the six-source serialization/URL batch. |
| QtBase | `qmessageauthenticationcode` | PASS | Passed in the six-source serialization/URL batch. |
| QtBase | `qurlquery` | PASS | Passed in the six-source serialization/URL batch. |
| QtBase | `qurlinternal` | PASS | Passed in the six-source serialization/URL batch with its upstream `utf8data.cpp` helper. |
| QtBase | `qdataurl` | PASS | Passed in the six-source serialization/URL batch. |
| QtBase | `qcompare` | PASS | Passed in the six-source serialization/URL batch. |
| QtBase | `qstringview` | PASS | Passed with its upstream `arrays_of_unknown_bounds.cpp` helper in the two-source text batch. |
| QtBase | `qbytearrayview` | PASS | Passed in the two-source text batch. |
| QtBase | `qtimeline` | PASS | Passed in a batch-selector run: 32 passed, 0 failed, in 96.8 seconds. The longer timer/event-loop coverage needs more guest time than the short smoke batch. |
| QtDeclarative | `qjsmanagedvalue` | PASS | Passed in the three-source QtDeclarative batch: 89 passed, 0 failed. `import QtQml` is served by the NRO-embedded static `QtQmlPlugin`, with no SD-card QML deployment. |
| QtDeclarative | `qjsprimitivevalue` | PASS | Passed in the same three-source QtDeclarative batch. |
| QtDeclarative | `qjsvalueiterator` | PASS | Passed in the same three-source QtDeclarative batch. |
| QtDeclarative | `qabstractanimationjob` | PASS | Passed in the five-source QtDeclarative batch. |
| QtDeclarative | `qsequentialanimationgroupjob` | PASS | Passed in the five-source QtDeclarative batch. |
| QtDeclarative | `qanimationgroupjob` | PASS | Passed in a separate one-source QtDeclarative batch because its global `UncontrolledAnimation` helper collides with `qsequentialanimationgroupjob` in one executable. |
| QtDeclarative | `qv4identifiertable` | PASS | Passed in the five-source QV4/QML-model batch. |
| QtDeclarative | `qv4estable` | PASS | Passed in the five-source QV4/QML-model batch. |
| QtDeclarative | `qv4urlobject` | PASS | Passed in the five-source QV4/QML-model batch. |
| QtDeclarative | `qv4regexp` | PASS | Passed in the five-source QV4/QML-model batch. |
| QtDeclarative | `qqmlchangeset` | PASS | Passed in the five-source QV4/QML-model batch. |
| QtDeclarative | `qqmlopenmetaobject` | PASS | Passed in the two-source QML metadata batch. |
| QtDeclarative | `qqmlcpputils` | PASS | Passed in the two-source QML metadata batch. |

## Not yet tested

The current manifests contain 76 of 680 QtBase `tst_*.cpp` sources and 13 of
323 QtDeclarative sources. The remaining sources are not claimed as tested.

Primary reasons:

- the current Switch build does not include all required Qt modules, plugins,
  host tools, or test fixtures;
- many upstream tests require data files, helper executables, network/docker
  fixtures, GUI/window-system behavior, or a QtTest/QML runner not yet
  deployed to Switch;
- GUI-linked QML tests such as `qv4mm` additionally need the Switch prefix to
  expose `Qt6QuickTestUtilsPrivate` (and, for wider groups, Quick) as usable
  CMake package targets. The current prefix has incomplete package integration,
  so they are not added to the batch merely to produce configuration failures;
- only the selected static QtQml plugin set is embedded in the current batch.
  Tests needing additional QML modules must name and link those plugins, as a
  normal static Qt application does; no application is expected to deploy the
  Qt runtime tree onto the SD card.
- timer-heavy tests such as `qtimeline` are kept in a separate batch selector
  during diagnosis so their longer guest runtime does not delay the core smoke
  batch. `qtimeline` itself is now verified as passing.
- `qmutex` is now a precise Ryubing scheduling failure rather than a
  resource-handle exhaustion; `qreadwritelock` is separately proven in the
  existing batch NRO, including its high-thread-count stress loop. GDB
  attachment is useful for mapping and registers, but guest continuation stalls
  on this host configuration.

## Runtime fix verified by QtBase batch

Switch QtCore previously compiled `qtimezoneprivate_tz.cpp` but selected
`QUtcTimeZonePrivate` in `qtimezone.cpp`. It now selects `QTzTimeZonePrivate`
and searches `sdmc:/qt6-switch-zoneinfo` when `TZDIR` is unset. Applications
can deploy a standard IANA zoneinfo tree, including `zone.tab`, at that path.

## GDB and Ryubing evidence

The repository-built Ryubing binary has been used with its guest GDB stub
(`--no-gui --enable-gdb-stub --suspend-on-start`). GDB attached as AArch64,
queried the loaded NRO mapping with `monitor get info`, and used relocated NRO
addresses for breakpoints. The stub can provide register/memory and mapping
evidence, but its `continue` path currently stalls on this macOS/AppleHv setup;
normal batch validation therefore uses the batch log and guest QtTest output.
The latest direct attachment used `qtbase_thread_tests_batch` with selector
`qmutex`: GDB reported the NRO at `0x8500000-0x87c9fff`, with
`pc=0x8500000` and `sp=0x8109000` before guest execution. A subsequent GDB
`continue` remained stalled for 20 seconds, then was terminated; the exact
Ryubing process was then stopped and a process-list check confirmed that no
Ryujinx instance remained.

For the QtDeclarative allocator failure, guest instruction inspection located
the malformed QV4 persistent-value page pointer path; that evidence led to the
page-aligned allocation fix recorded below. For the previous QtBase thread
failure, the Ryubing guest log identified `CreateThread(handle: 0x00000000) =
LimitReached`. The QThread join fix removes that condition for
`qsharedpointer`.
The local, Git-generated Ryubing patch
`patches/ryubing-thread-limit-diagnostics.patch` records the thread resource
current/limit/remaining values at the exact failing SVC; `build-ryubing.sh`
applies it only for a Ryubing build and restores the submodule source afterward.

## Runtime fixes verified by the QtDeclarative batch

- The Switch JavaScript allocator now reserves page-aligned storage. QV4's
  persistent-value pages recover their header by page masking, which is invalid
  for ordinary `calloc()` alignment and previously produced a null engine
  pointer at the guest instruction that stores a QJSValue property.
- The Switch QV4 fallback now uses the portable bounded call-depth check rather
  than deriving physical stack bounds from a temporary frame; ordinary JS calls
  no longer throw a false stack-overflow error.
- The batch runner links and imports `QtQmlPlugin`, `QtQmlModelsPlugin`, and
  `QtQmlWorkerScriptPlugin`, then initializes their embedded resources. This
  matches the QML demos' static deployment model and fixes `import QtQml`; the
  temporary SD-card QML import path was removed.
