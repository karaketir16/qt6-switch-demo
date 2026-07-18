# Embedded Mozilla CA bundle

`mozilla-ca-bundle.pem` is the Mozilla CA store converted to PEM by curl.
It was downloaded from `https://curl.se/ca/cacert.pem` on 2026-07-18.

- Source: https://curl.se/docs/caextract.html
- License: Mozilla Public License 2.0
- SHA-256: `3ff344e30b9b1ed2971044eabb438a08f2e2245ddb5f8ab1a3ad8b63ab4eaf91`

Update the bundle intentionally: download it again from the source, verify the
published SHA-256, update this file, and rebuild the NRO. It contains public CA
certificates only; it must never be replaced with a private key or a
device-specific certificate.
