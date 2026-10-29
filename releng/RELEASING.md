# Releasing a tested build

This file is the procedure for publishing an OMNeT++ release after its archives passed the tests in
`RELEASE-TESTING.md`. It is written for coding agents and for people. `misc/release-dist` does the
mechanical part. The agent prepares everything, but publishes nothing that the release manager did
not approve:

- The agent stops and asks before each of the three gates below: pushing to the git remotes,
  pushing the container images, and creating the draft release.
- The agent never publishes the GitHub release. The release manager does that on GitHub.
- The changes to omnetpp.org, models.omnetpp.org and opp_env are local commits. The agent does not
  push them.

In the commands, `X.Y.Z` is the version (e.g. `6.5.0`) and `X.Y` is its first two numbers.

## Preconditions

- The release commit is the tip of the `omnetpp-6.x` bookmark. It already contains the updated
  ChangeLogs, `doc/API-changes.txt`, `WHATSNEW.md` and the ChangeLog tagging done by
  `misc/tag-changelogs` (see the omnetpp-release-engineering skill).
- `releng/dist` contains the archives of one build of exactly that commit, made with
  `build-omnetpp-dist`, and `releng/dist/test-results/<buildid>/` contains its test results.
- The release check report accepts the results. Failed tests are allowed only if the report
  accepts them.
- `gh` is logged in with write access to `omnetpp/omnetpp`, and podman (or docker) can push to
  `ghcr.io/omnetpp`.
- The working copies of `../www/omnetpp.org`, `../www/models.omnetpp.org` and `../opp/opp_env` have
  no uncommitted changes, and their main bookmark or branch is up to date with origin.

## Procedure

1. **Build the container images**, which also writes `releng/dist/omnetpp-X.Y.Z-distrobox.ini`.
   First check the base images: both `dockerfiles/omnetpp/Dockerfile` (`ubuntu:<version>`) and
   `dockerfiles/omnetpp-gui/Dockerfile` (`quay.io/toolbx/ubuntu-toolbox:<version>`) must be based
   on the latest Ubuntu LTS release, the one that runs `test_quick` in `misc/test-dist.distros`. If
   one is older, update its `FROM` line and its tags (see below) before building, and commit the
   change. Then build:

        releng/build-omnetpp-docker-images

   This builds `ghcr.io/omnetpp/omnetpp:u26.04-X.Y.Z` (core, no GUI) and
   `ghcr.io/omnetpp/omnetpp-gui:u26.04-X.Y.Z` (for distrobox) from the archives in `releng/dist`.
   The Ubuntu version in the tags follows the `FROM` line of each Dockerfile. If a Dockerfile
   changed its base image, check that the tag in its `build` script, `opp_docker_shell` and
   `distrobox-template.ini` changed with it.

2. **Check the build.** Stop if it fails; do not work around a failed check.

        releng/misc/release-dist check

   It verifies that all archives come from one build, that the build is the tip of `omnetpp-6.x`,
   that `Version` and `WHATSNEW.md` match the version, that the distrobox.ini exists, that a tag of
   the version (if it exists) points to the build, and that every platform in
   `misc/test-dist.distros`, Windows and macOS passed. Add `--accept-failures` only if the release
   check report accepts the failures that it lists.

3. **Tag the release commit.** jj cannot create tags, so use git:

        git tag -a omnetpp-X.Y.Z -m "OMNeT++ X.Y.Z" omnetpp-6.x

4. **Gate 1: push the branch and the tag.** Show the release manager what will be published to the
   public repository, and wait for approval:

        jj log -r 'omnetpp-6.x@upstream..omnetpp-6.x'

   Then push to both remotes:

        jj git push --remote origin -b omnetpp-6.x
        jj git push --remote upstream -b omnetpp-6.x
        git push origin refs/tags/omnetpp-X.Y.Z
        git push upstream refs/tags/omnetpp-X.Y.Z

   `origin` is `omnetpp/omnetpp-dev`, `upstream` is `omnetpp/omnetpp`. jj does not push tags, so the
   tag goes with git.

5. **Gate 2: push the container images.** Ask, then:

        podman push ghcr.io/omnetpp/omnetpp:u26.04-X.Y.Z
        podman push ghcr.io/omnetpp/omnetpp-gui:u26.04-X.Y.Z

6. **Stage the assets and write the release notes:**

        releng/misc/release-dist stage
        releng/misc/release-dist body

   `stage` copies the release assets to `releng/dist/release/` under their published names (without
   the `<date>-<hash>-` prefix) and writes `SHA256SUMS`. The assets are the five platform archives,
   `omnetpp-X.Y.Z-core.tgz` and `omnetpp-X.Y.Z-distrobox.ini`. The full `omnetpp-X.Y.Z.tgz` is never
   published. `body` writes `releng/dist/release/release-notes.md` from the `OMNeT++ X.Y` section of
   `WHATSNEW.md`: the intro paragraph, then the rest of the section in a collapsed block. Read the
   notes; if they are wrong, fix `WHATSNEW.md`, not the generated file.

7. **Gate 3: create the draft release.** Ask, then:

        releng/misc/release-dist draft

   It creates a draft release titled `OMNeT++ X.Y.Z` on `omnetpp/omnetpp` from the pushed tag, with
   the release notes and the staged assets. If it fails halfway, delete the partial draft on GitHub
   before running it again. Do not publish the draft.

8. **Update omnetpp.org** (`../www/omnetpp.org`, Jekyll, jj). Use today's date as the release date
   in every file below.
   - Copy the download item of the current version, `download-items/omnetpp/omnetpp-<previous>.md`
     (for example `omnetpp-640.md`), to `omnetpp-XYZ.md` (the version digits concatenated).
   - In the new file, set `id: omnetpp-XYZ`, `version`, `date` and `sortkey`. The URLs use
     `@VERSION@`, so they need no change.
   - Fill in the asset sizes and checksums:

        releng/misc/release-dist website-data ../www/omnetpp.org/download-items/omnetpp/omnetpp-XYZ.md

   - Check the docker image tag in the `docker` tab against step 1.
   - Replace the text after the front matter with three paragraphs:
     - the intro paragraph of the WHATSNEW section;
     - ``Issues [fixed](https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3AX.Y) in version X.Y.Z.``
     - ``Read the [What's New](https://github.com/omnetpp/omnetpp/blob/omnetpp-X.Y.Z/WHATSNEW.md) page for more details.``
   - In the previous version's file, change `category: omnetpp-current` to `category: omnetpp`. This
     moves it to the previous versions page.
   - Add the announcement, `_posts/YYYY-MM-DD-omnet-XYZ-released.md`, with this front matter and the
     same three paragraphs:

        ---
        layout: post
        title: OMNeT++ X.Y.Z Available
        category: Software
        details-url: https://github.com/omnetpp/omnetpp/releases/tag/omnetpp-X.Y.Z
        ---

   - Optionally preview the site with `./start_local_server` (http://localhost:4000).
   - Commit: `jj describe -m "OMNeT++ X.Y.Z released"`, `jj bookmark set master -r @`, then `jj new`.
     Moving the bookmark is local; the release manager pushes it.

9. **Update models.omnetpp.org** (`../www/models.omnetpp.org`, git). Do this step only if
   `descriptors/omnetpp-X.Y/` does not exist yet, that is, for the first release of a new X.Y. The
   IDE's "Install Simulation Models" dialog reads that directory.
   - Copy the directory of the previous version to `descriptors/omnetpp-X.Y/`.
   - In each XML file, check that the model version supports OMNeT++ X.Y (the model's release
     notes, or opp_env's database). Keep the versions you can verify. List the others for the
     release manager to decide. Do not download or build the models.
   - Keep `descriptors.txt` in sync with the XML files. Keep `inet.xml`, because the IDE needs it.
   - Commit only that directory: `git add descriptors/omnetpp-X.Y` and
     `git commit -m "descriptors for omnetpp-X.Y"`.

10. **Update opp_env** (`../opp/opp_env`, jj).
    - Add `"X.Y.Z*",` as the first entry of `released_versions` in `opp_env/database/omnetpp.py`.
    - In `CHANGES.md`, add `- omnetpp: added version X.Y.Z` under `### Database (Frameworks and
      Models)` in the `## Unreleased` section. Add that subsection if it is missing.
    - Commit: `jj describe -m "omnetpp.py: added version X.Y.Z"`, `jj bookmark set main -r @`,
      then `jj new`.
    - opp_env downloads the GitHub release assets by name, so this entry works only after the draft
      release is published.
    - Adding the new version to the models in the database (inet, simu5g, veins, external) and the
      smoke test is separate work; see the oppenv-dev skill.

11. **Hand over.** Report what was done, with the draft release URL and the local commits, and list
    what the release manager has to do:
    - review and publish the draft release on GitHub;
    - push the omnetpp.org, models.omnetpp.org and opp_env commits;
    - release opp_env, so that users get the new version;
    - decide on the model versions that step 9 could not verify;
    - the issues still open on milestone X.Y, from
      `gh issue list -R omnetpp/omnetpp --milestone X.Y --state open`.

## Pre-releases

Pre-releases (versions such as `6.4.0pre1` or `6.4.0aipre2`) follow the same procedure, with these
differences:

- `release-dist draft` marks the release as a pre-release. `release-dist body` does not work for
  them; the release manager writes the notes, and `draft` takes them from
  `releng/dist/release/release-notes.md`.
- On omnetpp.org, the download item gets `category: preview`, and the current version stays
  current.
- In opp_env, the version goes under `# temporary low prority versions` in `omnetpp.py`.

## Traps

- **ssh:** on some machines git cannot use ssh, because a system ssh config file is rejected ("Bad
  owner or permissions"). Use `GIT_SSH_COMMAND='ssh -F ~/.ssh/config'` for the git and jj pushes.
- **jj workspaces:** a secondary jj workspace has no `.git` directory, so git commands and
  `release-dist` fail there. Run them in the main workspace, or set `GIT_DIR` to its `.git`.
- **Several builds in `releng/dist`:** `release-dist` refuses to choose. Remove the old archives
  first.
- **No `grep -q` at the end of a pipeline under `pipefail`:** see `RELEASE-TESTING.md`. This also
  applies to `release-dist`.
