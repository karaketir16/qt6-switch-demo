# Ryubing socket compatibility patches

`third_party/ryubing` is a source-only git submodule pinned to Ryubing commit
`a82350bb774f70fcbd41c9987bf67a3775409963`. It contains neither a Ryubing
binary nor Nintendo material.

## Problem and scope

libnx uses the Switch BSD `SendMMsg` IPC command for Qt UDP datagrams. Ryubing
rejects a message that contains a destination sockaddr (`BsdMsgHdr.Name`) with
`EOPNOTSUPP`, although it already has a working host `SendTo` path. The Ryubing
patch also accepts an omitted `recvfrom` address buffer and handles Qt's UDP
`MSG_PEEK | MSG_TRUNC` size probe.

`patches/ryubing-sendmmsg-udp-destination.patch` sends each addressed datagram
through the host `SendTo` API. It deliberately continues to reject ancillary
control data: silently dropping it would produce incorrect socket semantics.

The QtBase fallback-poll fix is committed and pushed in the QtBase submodule,
not kept as a local patch. The Ryubing patch remains outside its submodule so
the upstream revision is auditable and updates are explicit.

## Patch maintenance

Do not edit patch hunks by hand. Apply the intended source change in a clean
submodule worktree, then regenerate the patch with Git and validate it:

```sh
git -C third_party/ryubing diff --binary -- src/Ryujinx.HLE/HOS/Services/Sockets/Bsd > patches/ryubing-sendmmsg-udp-destination.patch
git -C third_party/ryubing apply --check ../../patches/ryubing-sendmmsg-udp-destination.patch
```

## Toolchain

Ryubing pins .NET SDK 10.0.301 in `global.json`. Scripts discover it through
`DOTNET`, `PATH`, or an optional `../tools/dotnet` sibling installation.

```sh
export DOTNET="/path/to/dotnet"
"$DOTNET" --version
```

## Build a patched Ryubing

```sh
./scripts/build-ryubing.sh
```

The script applies the patch only when necessary and reverses its own temporary
application on exit. Release output is written below
`third_party/ryubing/src/Ryujinx/bin/Release/net10.0/`.

## Verification

Run the current automated probes with:

```sh
./scripts/run-qt-module-test-ryubing.sh
./scripts/run-qt-network-test-ryubing.sh
```

The network probe has completed with `15/15` passes both in the locally built
emulator and on physical Switch. Physical hardware remains authoritative;
old device logs kept for reference are not a statement about the current build.

## Licensing and repository boundaries

Ryubing is MIT licensed. Retain its license and third-party notices when
redistributing source or binaries. Its distribution also lists third-party
components under licenses including LGPL; binary redistribution needs a
separate notice/compliance review.

Never add `prod.keys`, `title.keys`, firmware, game dumps, Nintendo assets,
or a locally configured Ryubing data directory to this repository. This
repository stores only upstream source metadata and the small interoperability
patch above.
