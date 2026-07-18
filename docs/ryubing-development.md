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

Ryubing pins the .NET SDK in `global.json`. The required SDK is installed
outside this repository at `/Volumes/T7/tools/dotnet`:

```sh
curl --fail --location --silent --show-error https://dot.net/v1/dotnet-install.sh -o /tmp/ryubing-dotnet-install.sh
bash /tmp/ryubing-dotnet-install.sh --version 10.0.301 --install-dir /Volumes/T7/tools/dotnet --no-path
```

Use it without modifying the system-wide SDK installation:

```sh
export DOTNET_ROOT=/Volumes/T7/tools/dotnet
export PATH="$DOTNET_ROOT:$PATH"
```

## Build a patched Ryubing

```sh
git -C third_party/ryubing apply ../../patches/ryubing-sendmmsg-udp-destination.patch
DOTNET_ROOT=/Volumes/T7/tools/dotnet /Volumes/T7/tools/dotnet/dotnet build Ryujinx.sln --configuration Release
git -C third_party/ryubing apply -R ../../patches/ryubing-sendmmsg-udp-destination.patch
```

The Release executable is written below
`third_party/ryubing/src/Ryujinx/bin/Release/net10.0/`. Do not commit that
output or the applied submodule worktree.

## Verification

Ryubing was compiled with .NET SDK 10.0.301 on macOS arm64 with zero errors
and warnings. The patched emulator completed the network NRO with `15/15`
passes: TCP, UDP, local HTTP, libcurl HTTPS, and Qt OpenSSL HTTPS. Its log had
no `Invalid socket descriptor` entry.

## Licensing and repository boundaries

Ryubing is MIT licensed. Retain its license and third-party notices when
redistributing source or binaries. Its distribution also lists third-party
components under licenses including LGPL; binary redistribution needs a
separate notice/compliance review.

Never add `prod.keys`, `title.keys`, firmware, game dumps, Nintendo assets,
or a locally configured Ryubing data directory to this repository. This
repository stores only upstream source metadata and the small interoperability
patch above.
