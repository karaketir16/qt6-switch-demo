# QtNetwork Test

Standalone QtNetwork smoke test for Nintendo Switch, with offline local checks
and mandatory live-network checks.

It displays the result on the Switch screen and covers address parsing, URL
parsing, interface enumeration, proxy defaults, numeric and `localhost`
resolution, TCP listen/accept/payload, UDP datagrams and bursts, local HTTP
success and HTTP 404 handling through `QNetworkAccessManager`, and
manager/thread construction. It also requires live `www.google.com` DNS and
HTTPS access as separate mandatory checks. Google HTTPS is tested both through
the Switch portlib `libcurl` mbedTLS backend and native Qt OpenSSL
`QNetworkAccessManager`. Native HTTPS uses an embedded Mozilla CA bundle;
certificate verification remains enabled. Its source, license and update
checksum are in `assets/README.md`.
The local checks are otherwise offline-safe.

Build and run:

```bash
./scripts/build-qt-network-test.sh
./scripts/run-qt-network-test-ryubing.sh
```

The result is printed to stdout and appended on hardware to
`sdmc:/qt6-switch-network-test.log`. The process exits non-zero if any check
fails. Emulator socket-service limitations should be treated separately from
the real Switch result.

The runner uses only the binary built from `third_party/ryubing`, enables Qt's
emulator socket-wait fallback, waits for the final summary, and exits non-zero
if any probe failed. Host output is kept under `build/test-results/`.
