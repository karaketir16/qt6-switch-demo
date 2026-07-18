---
name: qt6-switch-port
description: Build, modify, diagnose, and validate the Qt 6 Nintendo Switch port in this workspace. Use for Qt base/module builds, libnx platform work, QtNetwork/TLS/OpenSSL issues, real-Switch testing, Ryubing/Ryujinx emulator tests, Switch debug logs, or feature-port-matrix updates in /Volumes/T7/qt6-switch-demo.
---

# Qt 6 Switch Port

Work from the repository root. Preserve unrelated dirty-worktree changes.

## Orient

1. Read `docs/feature-port-matrix.md` and the test README relevant to the requested module.
2. Inspect existing patches, build scripts, and the current log artifacts before changing code.
3. Use `rg` for source/log searches. Treat real-Switch logs as authoritative over emulator output.

## Build and change workflow

- Use the existing `scripts/build-*.sh` and `scripts/configure-*.sh`; they encapsulate the Docker/devkitA64 toolchain.
- Keep `.github/workflows/build-demos.yml` manual-only. Its full cross-build is intentionally triggered with `workflow_dispatch`, not automatically by pushes or pull requests.
- When changing Qt source in `third_party/qtbase`, rebuild the affected static target before rebuilding the NRO. Rebuilding only the demo does not absorb Qt library changes.
- Keep Switch-specific code behind `Q_OS_SWITCH` or `__SWITCH__`; retain the upstream behavior for other platforms.
- Prefer a small upstream-style platform implementation over an application-only workaround when the defect affects Qt generally.
- Validate with the narrowest relevant test, then update the feature matrix only after evidence supports the status.

## Logging and evidence

The shared runtime debug toggle is `QT_SWITCH_DEBUG_LOG=1` or the marker file `sdmc:/qt6-switch-debug`. It is read once per process. Write verbose probe data to `sdmc:/qt6-switch-probe.log` through the existing Qt Switch debug mechanism, not a parallel logging scheme.

Always collect:

- `sdmc:/qt6-switch-network-test.log` for pass/fail lines;
- `sdmc:/qt6-switch-probe.log` for internal stages;
- `sdmc:/qt6-switch-openssl-seed.log` when debugging provider entropy;
- `sdmc:/qt6-switch-startup.log` for pre-main/early startup breadcrumbs.

For native TLS, inspect the complete chain: entropy source, `RAND_status`, DRBG state, `SSL_CTX` creation, CA bundle loading, handshake error, and HTTP status. Do not call HTTPS working merely because DNS or libcurl succeeds.

## Network/TLS rules

Read `references/network-tls.md` before altering QtNetwork, OpenSSL, the network test, or the related build patches.

Do not remove `patches/openssl-3.0.16-switch-rng.patch`: it connects OpenSSL 3's provider `SEED-SRC` to libnx `randomGet()` and is required for general Qt OpenSSL safety. Application `RAND_add()` is diagnostic/defensive only, not a port-wide entropy solution.

Keep certificate verification enabled. Generic native Qt public HTTPS can load a PEM root bundle from `sdmc:/qt6-switch-ca-bundle.pem`. The standalone network test instead embeds a Mozilla CA bundle so its emulator test is self-contained; update it intentionally from the documented source and never embed a private key.

## Validation hierarchy

1. Compile the changed Qt target and NRO.
2. Build `third_party/ryubing` with `scripts/build-ryubing.sh`, then use that
   build to confirm loadability, entropy initialization, and CA-bundle discovery.
3. Run the same NRO on a real Switch for network/TLS truth.
4. Report exact logs, counts, status codes, and the NRO SHA-256.

The supported emulator is exclusively the binary built from the pinned
`third_party/ryubing` source plus the tracked compatibility patch. Do not use a
packaged or external Ryubing/Ryujinx binary as validation evidence.
Automated `run-qt-*-ryubing.sh` runners close the process after capturing the
summary; use `scripts/launch-qt-demo-ryubing.sh <nro>` for an interactive run.
