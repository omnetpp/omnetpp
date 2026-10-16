# doc-builder container images

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

A second, larger variant adds what the PDF documents need: TeX Live (Ubuntu's
`texlive-latex-recommended`, `-latex-extra`, `-fonts-recommended`, `tex-gyre`,
`latexmk`), ImageMagick (with its SVG coder) for `sphinx.ext.imgconverter`, and
`rsvg-convert` for the manual's figures. Both are built from the one
`Containerfile` (stages `base` and `pdf`); the HTML image stays free of TeX.

The web fonts that `sphinx_immaterial` would download at the start of every build
(Roboto and Roboto Mono, its defaults) are cached in the image, in
`/opt/sphinx-immaterial-cache` (`SPHINX_IMMATERIAL_EXTERNAL_RESOURCE_CACHE_DIR`), so
building the documentation needs no network access. A project that sets other
fonts in `html_theme_options` still downloads them, into a cache directory of its
own (the image's is not writable for non-root users).

## Naming and tags

The same references are used by CI and locally:

| Reference | Meaning |
|-----------|---------|
| `ghcr.io/omnetpp/doc-builder:latest` | HTML toolchain; what `deploy-docs.yml` pulls; moves with each rebuild |
| `ghcr.io/omnetpp/doc-builder:<short-sha>` | immutable; pin a deployment to an exact toolchain by editing the `image:` line |
| `ghcr.io/omnetpp/doc-builder:latest-pdf` | the same plus the PDF toolchain |
| `ghcr.io/omnetpp/doc-builder:<short-sha>-pdf` | immutable PDF variant, built together with `:<short-sha>` |

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
its version and the vscode commit in the image labels. It always builds both
variants (`--target base`, then `--target pdf`, which reuses the base layers), so
the two never drift apart:

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

PDFs need the PDF variant; pass any make target:

```bash
IMAGE=ghcr.io/omnetpp/doc-builder:latest-pdf doc/src/site/doc-builder/run.sh do-guides-pdf
IMAGE=ghcr.io/omnetpp/doc-builder:latest-pdf doc/src/site/doc-builder/run.sh do-manual-pdf
```

They land in `doc/` (`UserGuide.pdf`, `SimulationManual.pdf`, …).

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

Override one for a test build with `podman build --build-arg PANDOC_VERSION=…`.

The PDF variant's TeX Live, ImageMagick and librsvg are Ubuntu packages, fixed by
the Ubuntu release in the `FROM` line rather than by a build arg. Its build runs a
smoke check (the Simulation Manual's real preamble, and the tiny Sphinx project in
`pdf-smoke/` with GIF and SVG images, with both the OMNeT++ and Sphinx's default
fonts), so a missing LaTeX package fails the image build instead of a later PDF
build.

Exception: opp-tools is whatever tarball `build.sh` bundled; its version is in the
`org.omnetpp.opp-tools.version` (and `.commit`) image labels.

## Using it as a base image

Model libraries (e.g. INET) can build their documentation site on top of this
image: derive from it, add their own Python/Sphinx requirements, and generate the
NED reference with `opp_docgen` (`--format=rst` or `markdown` to embed it in a
Sphinx site, `html-material`/`html-readthedocs` for a standalone subtree).

```dockerfile
FROM ghcr.io/omnetpp/doc-builder:<short-sha>        # HTML only
FROM ghcr.io/omnetpp/doc-builder:<short-sha>-pdf    # also Sphinx latexpdf
```

A workflow in another repository can pull the image while the package is private
only if the package grants that repository read access (package settings →
Manage Actions access).
