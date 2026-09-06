# Build notes

The combined payload is intentionally PS5-only.

GitHub Actions:
- checks out ps5-payload-dev/sdk v0.42
- checks out earthonion/np-fake-signin v1.1 during the template preparation step
- generates the required embedded NP headers from the upstream binary templates
- builds the combined ELF
- verifies the ELF and integration strings
- publishes an artifact on normal builds
- publishes a GitHub Release when a `vX.Y.Z` tag is pushed

The binary `.dat` template files are not fabricated in this repository. They are retrieved from the upstream project at build time so the combined project uses the original template data.

Build robustness fix:
- `prepare_upstream.sh` is invoked through `bash` so the build does not depend on the executable permission bit preserved by GitHub's web upload.
