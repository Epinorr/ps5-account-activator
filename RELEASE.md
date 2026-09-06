# Release

The repository publishes the combined PS5 payload from GitHub Actions.

## Build

Open **Actions → Build Combined PS5 Payload**.

A normal build produces an artifact named:

`EPINOR-NP-Fake-Signin`

containing:

- `EPINOR-NP-Fake-Signin.elf`
- `SHA256SUMS.txt`

## Release

Create a GitHub tag such as:

`v2.0.0`

The same workflow builds that exact tagged revision and creates the GitHub Release with the ELF and checksum attached.

## Device test matrix

Before publishing a new public release, verify:

1. Account inactive + signed out
2. Account active + signed out
3. Account active + already signed in
