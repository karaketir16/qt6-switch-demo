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
`QNetworkAccessManager`. Native HTTPS requires an updateable PEM root bundle
at `sdmc:/qt6-switch-ca-bundle.pem`; certificate verification remains enabled.
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

The Ryubing runner waits for the final summary, then exits non-zero if any
probe failed. It stages the CA bundle and writes host output to
`/tmp/ryubing-network.stdout` and `/tmp/ryubing-network.stderr`; see
`docs/astris-testing.md` for the SD-card mapping and TLS log locations.
