# Documentation site pipeline (`doc/src/site`)

This directory holds everything needed to **automatically build and deploy** the
OMNeT++ documentation to <https://doc.omnetpp.org>.

On every push of the dedicated **`doc` branch**, a GitHub Actions workflow builds
the documentation for the versions listed in [`versions.txt`](versions.txt) and
publishes the result to GitHub Pages **straight from the workflow run** — this
repository is the only one involved, and no deployment credential is stored
anywhere.

```
push doc branch ──► .github/workflows/deploy-docs.yml
    job build (in the doc-builder image)
      ├─ deploy/build-all.sh  _site        # build each version → _site/<label>/
      ├─ deploy/assemble-site.sh _site     # default version also at the root, + 404.html
      ├─ deploy/check-legacy-urls.py _site # pre-versioning URLs still lead to pages
      └─ actions/upload-pages-artifact     # pack _site as the 'github-pages' artifact
    job deploy (plain runner, only on refs/heads/doc)
      └─ actions/deploy-pages              # OIDC → Pages → https://doc.omnetpp.org
```

## What's here

| Path | Purpose |
|------|---------|
| `versions.txt` | Version manifest: `"<label>  <git-ref>"` per line; the first non-comment line is the **default**, also published at the site root. |
| `deploy/build-all.sh` | Reads `versions.txt` and builds every listed version. |
| `deploy/build-version.sh` | Checks a ref out into a git worktree and runs `make ci-html` for it, staging `_site/<label>/`. |
| `deploy/assemble-site.sh` | Copies the default version to the site root and renders `404.html`. |
| `deploy/check-legacy-urls.py` | Checks that the URLs of the pre-versioning site lead to pages, and that the pages exist which `404.html` sends the old `/omnetpp/` URLs to; prints the Cloudflare redirect rules they need. |
| `templates/404.html` | Template for the not-found page, which also leads the old `/omnetpp/` URLs and the indexes of the API references on to their pages. |
| `run.sh` | Builds the docs of the current checkout in the doc-builder image (see [Testing locally](#testing-locally-before-pushing)). |
| `check-conversion.py` | The TeX→RST conversion audit that gates the build. |
| `../../../.github/workflows/deploy-docs.yml` | Builds and deploys the site. |

---

## First-time setup, step by step

You need admin rights on the repository that hosts this pipeline — below called
the **pipeline repository** (`omnetpp/omnetpp-dev`). Do this once; after that,
publishing is just moving the `doc` branch (see [Publishing](#publishing-an-update)).

### Step 1 — Create the `doc` branch

The deploy workflow triggers on pushes to a branch literally named **`doc`**.
Create it from the commit whose documentation pipeline you want to use (typically
the tip of the branch that contains this `doc/src` build system):

```bash
git branch doc <commit-with-the-doc-pipeline>
git push origin doc
```

### Step 2 — Publish the `doc-builder` image

The build job runs inside `ghcr.io/omnetpp/doc-builder:latest`, so that image must
exist and be pullable. It is built and published from the
[omnetpp/doc-builder](https://github.com/omnetpp/doc-builder) repository, which also
holds the OMNeT++ Sphinx extensions in it (see its `image/README.md`). Republishing
`:latest` retunes the toolchain under every subsequent deploy, so it is a deliberate
act, never a side effect of a push here.

The package is public, so the build job pulls it without credentials. (A private
package can only be pulled with the `GITHUB_TOKEN` of a repository that its
**Package settings → Manage Actions access** lists.)

### Step 3 — Point DNS at GitHub Pages

```
doc.omnetpp.org.   CNAME   omnetpp.github.io.
```

Do this before step 4: GitHub verifies the record when the custom domain is set.

The zone is proxied by Cloudflare. The URLs of the site before it was versioned
(MkDocs, e.g. `/tutorials/tictoc/part2/`) end in a slash, which GitHub Pages only
serves for directories, so they need two **Single Redirect** rules in the zone
(**Rules → Redirect Rules**). Print them with:

```bash
doc/src/site/deploy/check-legacy-urls.py --cloudflare
```

They list the old site's pages explicitly: that set never grows, so the rules
never change. The deploy job checks every old URL against them on each run.

Before this site, doc.omnetpp.org served the documentation under `/omnetpp/`
(e.g. `/omnetpp/manual/#sec:sim-lib:cobject`, `/omnetpp/api/classomnetpp_1_1cMessage.html`,
`/omnetpp/InstallGuide.pdf`). Those URLs need no rule: `404.html`, which GitHub
Pages serves for them, leads on to the matching page in the browser, the section
of the manual included. Crawlers see a 404 there, not a redirect.

### Step 4 — Turn on Pages

In the pipeline repository, **Settings → Pages**:

1. *Source* = **GitHub Actions**. (Not "Deploy from a branch" — there is no
   publishing branch; the workflow run is the source.)
2. *Custom domain* = **`doc.omnetpp.org`**, then enable **Enforce HTTPS** once
   the certificate has been issued, which can take a few minutes.

The custom domain lives here and only here. Nothing in the built tree configures
it, which is why there is no `CNAME` file in this directory.

### Step 5 — Allow the `doc` branch to deploy

Enabling Pages auto-creates a **`github-pages` environment** whose deployment
branch policy permits only the repository's **default branch**. Since this
pipeline deploys from `doc`, the deploy job would otherwise fail with:

```
Branch "doc" is not allowed to deploy to github-pages due to environment protection rules.
```

In **Settings → Environments → github-pages → Deployment branches and tags**,
keep *Selected branches and tags* and **add a rule for `doc`**. Leaving the policy
narrow — `doc` only, rather than switching to *No restriction* — is what replaces
the blast-radius control the old deployment token provided: only that one branch
can publish to the live site.

### Step 6 — Run the first deploy

Trigger **Actions → Deploy documentation → Run workflow** on the `doc` branch (or
push the branch again). The run should build each version in `versions.txt`,
upload the `github-pages` artifact, and then deploy it; the deploy job's summary
links the published URL.

### Step 7 — Verify

```bash
curl -sI https://doc.omnetpp.org/                          # 200 - the default version
curl -sI https://doc.omnetpp.org/6.5/                      # 200 - the same, under its label
curl -sI https://doc.omnetpp.org/api/api-index             # 200 - the C++ API
curl -sI https://doc.omnetpp.org/tutorials/tictoc/part2/   # 301 to .../part2 (Cloudflare)
curl -sI https://doc.omnetpp.org/no-such-page              # 404, with the site's 404.html
```

Open <https://doc.omnetpp.org/omnetpp/manual/#sec:sim-lib:cobject> in a browser:
the not-found page should lead on to that section of the manual.

Check that a page under `_static/` loads and that math renders, which confirms the
bundled KaTeX assets were published.

### Migrating from the old two-repository setup

Earlier revisions of this pipeline built the site here and pushed it to a separate
`omnetpp/docs.omnetpp.org` repository. If that setup is still live:

1. A custom domain can only be claimed by one repository — clear `docs.omnetpp.org`
   from that repository's Pages settings **before** step 4 above.
2. Delete the `DOCS_PUSH_TOKEN` repository secret, and **revoke the token itself**;
   nothing uses it any more.
3. Archive or delete `omnetpp/docs.omnetpp.org`.

---

## Publishing an update

Whenever you want to refresh the live site, **move the `doc` branch** to the commit
you want built and push it:

```bash
git branch -f doc <new-commit>
git push --force-with-lease origin doc
```

You can also trigger it manually from the **Actions** tab (it has
`workflow_dispatch`) — note that the deploy job is gated on `refs/heads/doc`, so a
dispatch from any other branch builds the site and uploads the artifact for
inspection, but does not publish it.

Remember that the `doc` branch only supplies *the pipeline*; the documentation
**content** comes from the refs named in `versions.txt`. Moving `doc` without
moving those refs republishes the same content with a possibly different pipeline.

## Managing versions

`versions.txt` controls what gets built and how it appears in the URL:

```
# <label>   <git-ref>
6.x         omnetpp-6.x
# 7.x       omnetpp-7.x      # future: uncomment when a 7.x release exists
```

- `<label>` is the URL path segment: `doc.omnetpp.org/<label>/…`. It is also
  passed to the build as `VERSION`. It must not be the name of a top-level entry
  of the documentation (`manual`, `api`, …), with which it shares the root;
  `assemble-site.sh` refuses such a label.
- `<git-ref>` is the branch or tag to build. It **must already contain this Sphinx
  build system** (`doc/src/site/`); `build-version.sh` refuses anything else with
  exit code 3. Old, LaTeX-only release tags cannot be built.
- The **first** non-comment line is the default. It is published twice: at the
  site root, which carries the URLs of the site from before it was versioned and
  always shows the newest documentation, and under its `<label>/`, which keeps
  links to that version working after the next release. Both copies name the root
  page as canonical (`build-all.sh` builds it with `BASEURL` set to the root);
  every other version is canonical under its own `<label>/`.
- A release is a `versions.txt` edit: put the new version first. Nothing outside
  this repository changes.
- **Every listed version is rebuilt on every deploy.** A Pages deployment replaces
  the entire site, so there is no way to refresh one version and leave the others
  standing — and deleting a line removes that version from the live site on the
  next deploy.

## Testing locally before pushing

Build the docs exactly as CI does, using the same image:

```bash
doc/src/site/run.sh                        # -> doc/src/out for the current checkout
```

`IMAGE=<ref>` runs it in another image, e.g. one built from a doc-builder checkout
with its `image/build.sh`. Note that `run.sh` builds **your current checkout**. To exercise the full pipeline
instead — which builds the *refs listed in `versions.txt`*, fetching them from
origin if the local clone lacks them — stage it into a scratch directory:

```bash
doc/src/site/deploy/build-all.sh    /tmp/_site   # run from the repo root
doc/src/site/deploy/assemble-site.sh /tmp/_site
doc/src/site/deploy/check-legacy-urls.py /tmp/_site
python3 -m http.server -d /tmp/_site 8000   # open http://localhost:8000/
```

`/tmp/_site` is then byte-for-byte what the workflow uploads to Pages. This needs
no GitHub Actions or Pages, but it does need read access to origin.

## Notes / troubleshooting

- **`doc` branch vs. what gets built**: the workflow file and `versions.txt` are
  read from the pushed `doc` branch, but the *documentation content* comes from the
  refs listed in `versions.txt` (each built in its own worktree). This separates
  "how we deploy" from "what we build".
- **`build-version: ref '<x>' does not contain the Sphinx site build system`
  (exit 3)**: the ref in `versions.txt` predates this pipeline. Merge the doc build
  system into it, or point the line at a ref that has it.
- **Build fails on conversion defects**: `check-conversion.py` fails the build on
  leftover raw TeX / undefined roles. In CI only its per-file summary is available,
  in the job log — the full `conversion-report.md` is written inside the throwaway
  worktree and dies with it. To see the report, reproduce the failure locally with
  `make -C doc/src/manual rst && make -C doc/src ci-audit`, which leaves it in
  `doc/src/out/conversion-report.md`.
- **The job cannot pull the image**: confirm the package exists at
  `ghcr.io/omnetpp/doc-builder:latest`, and that it is public (see step 2).
- **The deploy job is skipped**: it only runs on `refs/heads/doc`. Download the
  `github-pages` artifact from the run to inspect what would have been published.
- **`Branch "doc" is not allowed to deploy to github-pages due to environment
  protection rules`**: the auto-created `github-pages` environment only permits the
  default branch. Add a deployment branch rule for `doc` — see step 5.
- **The run is green but the site is unchanged**: check the **github-pages**
  environment's deployment history in the Actions tab, and confirm
  **Settings → Pages → Source** is still *GitHub Actions* — switching it back to a
  branch source silently strips the workflow of its publishing target, and would
  also require restoring the `.nojekyll` file this pipeline no longer emits.
- **Math/pages look wrong offline**: math is KaTeX from bundled local assets; the
  toolchain is pinned in the `doc-builder` image (pandoc, doxygen, Sphinx).
