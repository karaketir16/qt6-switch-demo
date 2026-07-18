# Repository instructions

## Patch files

Never hand-author a `.patch` file or its hunk headers. Make the intended
change in a clean working tree, then generate the patch with Git:

```sh
git diff --binary -- <paths> > patches/<name>.patch
```

Use `git apply --check patches/<name>.patch` before committing it. Prefer
`git format-patch` when the patch is a committed upstream-ready change.

## Third-party ownership

Push QtBase fixes to the `third_party/qtbase` submodule and update this
repository's submodule pointer. Do not keep a QtBase patch here. Keep local,
Git-generated patch files only for Ryubing and OpenSSL.
