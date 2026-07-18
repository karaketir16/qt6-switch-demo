# Verified QtNetwork and TLS facts

## Build locations

- Qt source: `third_party/qtbase` (submodule).
- Qt Switch build: `build/qtbase-switch`.
- OpenSSL Switch install: `build/openssl-switch/install`.
- Network demo: `demo/qt-network-test`.
- Builds: `scripts/build-openssl-switch.sh`, `scripts/configure-qtbase-switch.sh`, `scripts/build-qt-network-test.sh`.

When changing a Qt OpenSSL backend source file, build `qopensslbackend` in the Qt build tree, then run the network-test build script.

## Current Switch adaptations

- `src/network/kernel/qnetworkinterface_unix.cpp`: enumerate the active interface through libnx NIFM.
- `src/plugins/tls/openssl/qtlsbackend_openssl.cpp`: preserve CSPRNG enforcement, log gated TLS diagnostics, and load the Switch PEM bundle.
- `src/plugins/tls/openssl/qsslcontext_openssl.cpp`: log `SSL_CTX_new` failure while the debug marker is enabled.
- `patches/openssl-3.0.16-switch-rng.patch`: provider seed source invokes `randomGet(out, outlen)` under `__SWITCH__`.

## Required TLS evidence

Healthy real hardware shows all of:

```text
RAND_status=1
rng ... RAND_bytes=1
seed-src randomGet outlen=...
PASS Qt Google HTTPS ... embeddedCa=<positive count> ... error=none
PASS Qt Google HTTPS ... status=204 error=none
```

The Google target is `https://www.google.com/generate_204`. The test separately exercises libcurl-mbedTLS and native `QNetworkAccessManager`/Qt OpenSSL; both are required checks.

## Known failure signatures

| Signal | Meaning | Next action |
| --- | --- | --- |
| `error retrieving entropy`, DRBG errors, `RAND_status=0` | OpenSSL provider cannot seed | Verify the OpenSSL RNG patch is in the actual built source/install; inspect seed-source log. |
| `SSL_CTX_new failed` | Context creation failed, often provider/RNG initialization | Inspect OpenSSL error queue, RAND/DRBG logs, symbols, and provider load result. |
| `issuer certificate ... could not be found` | TLS reached certificate verification but no trusted root is loaded | For the network test, verify its embedded Mozilla bundle has a positive certificate count; generic apps can deploy `qt6-switch-ca-bundle.pem` to SD root. |
| Ryubing `Invalid socket descriptor` | Emulator BSD service limitation | Confirm setup logs, then test physical hardware. |

## Emulator operation

Ryubing binary: `/Volumes/T7/Ryubing/Ryujinx.app/Contents/MacOS/Ryujinx`.

Ryubing maps `sdmc:/` to `~/Library/Application Support/Ryujinx/sdcard/`. Use `scripts/run-qt-network-test-ryubing.sh` to create its emulator/debug markers, clean-start the emulator, and wait for the completed test log. The emulator marker opts into Qt's `select` socket-wait fallback; hardware keeps `poll` unless `sdmc:/qt6-switch-use-select` is created. Starting via `open` can leave stale guest logs or fail to load the requested NRO.

## Real hardware evidence

Historical real-Switch logs are deliberately not tracked. Treat native Qt HTTPS
as requiring a fresh hardware run after changing the network, TLS, OpenSSL, or
build path; retain the run's result outside the repository when needed.
