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
CA bundle path=sdmc:/qt6-switch-ca-bundle.pem exists=1 certificates=<positive count>
PASS Qt Google HTTPS ... status=204 error=none
```

The Google target is `https://www.google.com/generate_204`. The test separately exercises libcurl-mbedTLS and native `QNetworkAccessManager`/Qt OpenSSL; both are required checks.

## Known failure signatures

| Signal | Meaning | Next action |
| --- | --- | --- |
| `error retrieving entropy`, DRBG errors, `RAND_status=0` | OpenSSL provider cannot seed | Verify the OpenSSL RNG patch is in the actual built source/install; inspect seed-source log. |
| `SSL_CTX_new failed` | Context creation failed, often provider/RNG initialization | Inspect OpenSSL error queue, RAND/DRBG logs, symbols, and provider load result. |
| `issuer certificate ... could not be found` | TLS reached certificate verification but no trusted root is loaded | Deploy `qt6-switch-ca-bundle.pem` to SD root and verify positive certificate count. |
| Ryubing `Invalid socket descriptor` | Emulator BSD service limitation | Confirm setup logs, then test physical hardware. |

## Emulator operation

Ryubing binary: `/Volumes/T7/Ryubing/Ryujinx.app/Contents/MacOS/Ryujinx`.

Ryubing maps `sdmc:/` to `~/Library/Application Support/Ryujinx/sdcard/`. Before running a TLS test, stage a bundle as `qt6-switch-ca-bundle.pem`, create `qt6-switch-debug` to enable trace logs, and create `qt6-switch-emulator` only when an emulator marker is wanted. Clean-start Ryubing with the documented command in `docs/astris-testing.md`; starting via `open` can leave stale guest logs or fail to load the requested NRO.

## Real hardware baseline (2026-07-18)

`demo/qt-network-test/fromRealSwitch/` contains the verified run: 15/15 passed, native Qt Google HTTPS returned HTTP 204, 128 root certificates loaded, and OpenSSL provider entropy passed. Re-test after altering any of the above components; do not assume this result applies to a new build.
