# GitHub Publishing

This repository was prepared to be published as a private GitHub repository.

## Requirements

- GitHub CLI installed
- authenticated session:

```bash
gh auth status
```

## Create a Private Repository

From the repository root:

```bash
gh repo create qt6-switch-demo --private --source=. --remote=origin --push
```

If that name is already taken in your account, choose another private name:

```bash
gh repo create qt6-switch-widgets-demo --private --source=. --remote=origin --push
```

## Verify the Remote

```bash
git remote -v
git branch --show-current
gh repo view --web
```

