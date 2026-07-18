# Repository instructions

## Patch files

Never hand-author a `.patch` file or its hunk headers. Make the intended
change in a clean working tree, then generate the patch with Git:

```sh
git diff --binary -- <paths> > patches/<name>.patch
```

Use `git apply --check patches/<name>.patch` before committing it. Prefer
`git format-patch` when the patch is a committed upstream-ready change.
