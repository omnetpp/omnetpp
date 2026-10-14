# doc-builder container image

A self-contained image with the full toolchain to build the OMNeT++ Sphinx
documentation site (manual, guides, tutorials, articles, the doxygen APIs and the
`omnetpp.scave` Python API): doxygen, graphviz, client-side KaTeX math (no LaTeX),
a version-pinned `pandoc` for the manual LaTeX→RST conversion, and the Python
Sphinx stack from `doc/src/uv.lock`.

It also carries the OMNeT++ command-line tools of the VS Code extension
(`@omnetpp/opp-tools`: `opp_docgen`, `opp_lint`, `opp_lsp`, `opp_ide_mcp`, run by
the distribution's `nodejs`), so it can generate NED documentation, e.g.
`opp_docgen --format=rst -o <out> <project>`.

The Python packages are installed with uv into a venv on `PATH`, so the build runs
without uv inside the container by passing `PIPRUN=` and `VENVDEP=`.

## Naming and tags

There is exactly one image reference, used identically by CI and locally:

| Reference | Meaning |
|-----------|---------|
| `ghcr.io/omnetpp/doc-builder:latest` | what `deploy-docs.yml` pulls; moves with each rebuild |
| `ghcr.io/omnetpp/doc-builder:<short-sha>` | immutable; pin a deployment to an exact toolchain by editing the `image:` line |

The image is `linux/amd64` only — every tool in it is fetched as an x86_64 binary,
so the `Containerfile` and `build.sh` pin the platform explicitly.

## Build locally

`@omnetpp/opp-tools` is not published to npm yet, so the image bundles a locally
built npm tarball. Build it first in the omnetpp-vscode checkout (hosted in-tree as
`vscode/`):

```bash
(cd vscode && bun run package:cli)         # -> vscode/build/omnetpp-opp-tools-<ver>.tgz
doc/src/site/doc-builder/build.sh          # -> ghcr.io/omnetpp/doc-builder:latest
```

`build.sh` takes the single tarball in `vscode/build/` (set `OPP_TOOLS_TGZ=<path>`
to use another one), stages it as `doc/src/site/doc-builder/opp-tools.tgz`
(gitignored, removed afterwards) for the `Containerfile` to bind-mount, and records
its version and the vscode commit in the image labels:

```bash
podman inspect -f '{{json .Labels}}' ghcr.io/omnetpp/doc-builder:latest
```

The build context is `doc/src` (it holds `pyproject.toml`/`uv.lock`); building
without `build.sh` requires staging the tarball by hand.

## Build the docs with it

```bash
doc/src/site/doc-builder/run.sh            # builds ci-html -> doc/src/out
VERSION=6.x doc/src/site/doc-builder/run.sh
```

`run.sh` bind-mounts the repository and runs
`make -C doc/src ci-html PIPRUN= VENVDEP=` inside the container.

## CI usage

`.github/workflows/deploy-docs.yml` already runs its build job inside this image
(its deploy job is a plain runner — it only calls the Pages API):

```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    permissions:
      contents: read
      packages: read
    container:
      image: ghcr.io/omnetpp/doc-builder:latest
      credentials:                    # needed only while the package is private
        username: ${{ github.actor }}
        password: ${{ secrets.GITHUB_TOKEN }}
    steps:
      - uses: actions/checkout@v4
        with: { fetch-depth: 0 }
      - run: doc/src/site/deploy/build-all.sh _site
      ...
```

`build-version.sh` auto-detects the image's global Sphinx toolchain and runs
`make ci-html` with uv bypassed (`PIPRUN= VENVDEP=`), so no extra configuration
is needed.

## Publishing

**Currently from a workstation only** (see below): `doc-builder-image.yml` has no
access to the locally built opp-tools tarball, so it deliberately fails in its
first step until the `Containerfile` fetches a published opp-tools version.

Once it is re-enabled, use `.github/workflows/doc-builder-image.yml`: **Actions →
Build doc-builder image → Run workflow**. It publishes both the `:latest` and `:<short-sha>` tags and
needs no personal access token. It is manual on purpose — republishing `:latest`
changes the toolchain under every subsequent deploy — so run it after changing the
`Containerfile`, `pyproject.toml` or `uv.lock`, and not otherwise.

To publish from a workstation instead:

```bash
echo "$GHCR_TOKEN" | podman login ghcr.io -u <your-user> --password-stdin
doc/src/site/doc-builder/build.sh --push     # pushes :latest and :<short-sha>
```

Both paths stamp an `org.opencontainers.image.source` label naming the publishing
repository — the workflow from `github.repository`, `build.sh` from the `origin`
remote. That label is what makes GHCR link the package to the repository, which in
turn lets the build job's `GITHUB_TOKEN` pull it while the package is private.

## Toolchain pins

The versions baked into the image are the `Containerfile`'s build args — that file
is the single source of truth, nothing outside it pins these:

| Build arg | Default | Note |
|-----------|---------|------|
| `PANDOC_VERSION` | 3.10 | drives the manual LaTeX→RST conversion; the converter is version-sensitive |
| `DOXYGEN_VERSION` | 1.14.0 | newest release doxysphinx's frozen LibSass scoper can still parse |
| `UV_VERSION` | 0.11.8 | build-time only; installs the Sphinx stack from `uv.lock` |

Exception: opp-tools is whatever tarball `build.sh` bundled; its version is in the
`org.omnetpp.opp-tools.version` (and `.commit`) image labels.

Override one for a test build with `podman build --build-arg PANDOC_VERSION=…`.

## Using it as a base image

Model libraries (e.g. INET) can build their documentation site on top of this
image: derive from it, add their own Python/Sphinx requirements, and generate the
NED reference with `opp_docgen` (`--format=rst` or `markdown` to embed it in a
Sphinx site, `html-material`/`html-readthedocs` for a standalone subtree).

```dockerfile
FROM ghcr.io/omnetpp/doc-builder:<short-sha>
```

A workflow in another repository can pull the image while the package is private
only if the package grants that repository read access (package settings →
Manage Actions access).
